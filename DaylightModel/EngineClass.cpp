#include "stdafx.h"
#include "EngineClass.h"
#include <thread>
#include <iomanip>
#include <ctime>
#include "SWEngineFiles/PerformanceCheck.h"



extern unsigned int windowX;
extern unsigned int windowY;

USE_PERFORMANCE_CHECK( SKYBOX_RENDERING )

void skyThreadFunction( EngineClass* engine )
{
	engine->updateSky();
}


EngineClass::EngineClass()
	: config_file("config/config.txt")
{
	config_needs_reload = false;

	models_manager = new ModelsManager;
	sky_dome = new HosekSkyDome( models_manager );
	sun_position = new SunPosition;

	vertical_angle = 0.0;
	horizontal_angle = 0.0;
	mouse_button = false;
	horizontal_multiplier = 3.0f;
	vertical_multiplier = 3.0f;


	latitude = 53.0f;
	longitude = 0.0f;
	time = 0.0f;

	albedo[0] = 0.8;
	albedo[1] = 0.8;
	albedo[2] = 0.8;
	turbidity = 4.0;

	sky_intensity = 5.0f;
	sun_intensity = 5.0f;

	endThread = false;

	rate = 1;		// W ka�d� sekund� czasu rzeczywistego mija 60 sekund.


	time_manager.initTimer();
	sky_time_manager.initTimer();

	ReloadConfigurationFile();
	FPScounter.init(time_manager.getTimeSec());
}

void EngineClass::release()
{
	delete sun_position;
	delete sky_dome;
	delete models_manager;
	//guiManager.ReleaseGUI();
	release_DirectX();
}

EngineClass::~EngineClass()
{

}

DX11_INIT_RESULT EngineClass::init_all( HWND window, unsigned int width, unsigned int height )
{
	DX11_INIT_RESULT result;

	REGISTER_PERFORMANCE_TASK( SKYBOX_RENDERING )

	std::wstring shader_file = L"shaders/HosekSkyDome.fx";
	std::string pixel_shader_name = "pixel_shader";
	std::string vertex_shader_name = "vertex_shader";

	set_vertex_layout( VERTEX_COLOR );

	result =  init_DX11( width, height, window, false, shader_file, pixel_shader_name,
			   shader_file, vertex_shader_name, false );

	if ( result != DX11_INIT_RESULT::DX11_INIT_OK )
		return result;

	init_buffers( sizeof( ConstantPerFrame ), sizeof( ConstantPerMesh ) );
	set_projection_matrix( XMConvertToRadians( 45 ),
						   (float)width / (float)height, 1, 100000 );
	device_context->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );
	//D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST

	////
	//ID3D11RasterizerState* rasterizer_state;
	//D3D11_RASTERIZER_DESC disable_backfaceculling = get_rasterizer_desc();
	//disable_backfaceculling.CullMode = D3D11_CULL_MODE::D3D11_CULL_NONE;
	////disable_backfaceculling.FillMode = D3D11_FILL_MODE::D3D11_FILL_WIREFRAME;
	//device->CreateRasterizerState( &disable_backfaceculling, &rasterizer_state );
	//device_context->RSSetState( rasterizer_state );
	////

	sun_position->setSunConditions( time );
	DirectX::XMVECTOR sun_dir = sun_position->computeSunDirection();

	int domeVerticalTess = config_file.GetValue<int>( "verticalTesselation" );
	int domeHorizontalTess = config_file.GetValue<int>( "horizontalTesselation" );

	sky_dome->init_sky_dome( sun_dir, turbidity, albedo, domeVerticalTess, domeHorizontalTess, 10, sky_intensity );

	skyThread = std::thread( skyThreadFunction, this );

	//guiManager.initGUI( device, device_context );

	return result;
}

void EngineClass::render_frame()
{
	START_PERFORMANCE_CHECK( SKYBOX_RENDERING )

	begin_scene();

	if ( !sky_dome )
		return;

	float timeElapsed = (float)time_manager.onStartRenderFrame();
	double seconds = time_manager.getTimeSec();
	FPScounter.count( seconds );

	// Ustawiamy format wierzcho�k�w i bufory sta�ych
	device_context->IASetInputLayout( sky_dome->get_vertex_layout() );
	device_context->UpdateSubresource( const_per_frame, 0, nullptr, &shader_data_per_frame, 0, 0 );
	device_context->VSSetConstantBuffers( 0, 1, &const_per_frame );
	device_context->PSSetConstantBuffers( 0, 1, &const_per_frame );	// Aktualizuje bufor wierzcho�k�w. Wstawiane s� nowe kolory.


	// Powinna by� to raczej rzadka czynno��, poniewa� aktualizacja jest kosztowna czasowo
	if ( sky_dome->update_ready() )
		sky_dome->update_buffers();

	// Ustawiamy bufor wierzcho�k�w
	BufferObject* buffer = sky_dome->get_vertex_buffer();
	if ( set_vertex_buffer( buffer ) )
		return;	// Je�eli nie ma bufora wierzcho�k�w, to idziemy do nast�pnego mesha
	// Ustawiamy bufor indeks�w, je�eli istnieje
	set_index_buffer( sky_dome->get_index_buffer() );


	ModelPart* model = sky_dome->get_model_part();


	XMMATRIX rotation_matrix = getRotationMatrix();
	//XMMATRIX proj_matrix = DirectX::XMMatrixTranspose( XMLoadFloat4x4( &shader_data_per_frame.projection_matrix ) );
	//rotation_matrix = XMMatrixMultiply( rotation_matrix, proj_matrix );

	// Wype�niamy bufor sta�ych
	ConstantPerMesh shader_data_per_mesh;
	shader_data_per_mesh.world_matrix = XMMatrixTranspose( rotation_matrix );	// Transformacja wierzcho�k�w
	// Przepisujemy materia�
	copy_material( &shader_data_per_mesh, model );

	// Ustawiamy shadery
	device_context->VSSetShader( model->vertex_shader->get(), nullptr, 0 );
	device_context->PSSetShader( model->pixel_shader->get(), nullptr, 0 );

	// Aktualizujemy bufor sta�ych
	device_context->UpdateSubresource( const_per_mesh, 0, nullptr, &shader_data_per_mesh, 0, 0 );
	device_context->VSSetConstantBuffers( 1, 1, &const_per_mesh );
	device_context->PSSetConstantBuffers( 1, 1, &const_per_mesh );

	ID3D11Buffer* const_buffer = sky_dome->get_constant_buffer();
	device_context->VSSetConstantBuffers( 2, 1, &const_buffer );
	device_context->PSSetConstantBuffers( 2, 1, &const_buffer );

	// Ustawiamy tekstury
	set_textures( *model );

	//depth_buffer_enable( false );		// Wy��czamy z-bufor

	// Teraz renderujemy. Wybieramy albo tryb indeksowany, albo bezpo�redni.
	const MeshPartObject* part = model->mesh;
	if ( part->use_index_buf )
		device_context->DrawIndexed( part->vertices_count, part->buffer_offset, part->base_vertex );
	else // Tryb bezpo�redni
		device_context->Draw( part->vertices_count, part->buffer_offset );

	//depth_buffer_enable( true );		// W��czamy z-bufor spowrotem

	//guiManager.RenderGUI( timeElapsed );

	END_PERFORMANCE_CHECK( SKYBOX_RENDERING )

	end_scene_and_present();
}


void EngineClass::HandleMessages( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	//if( guiManager.HandleMessages( hWnd, uMsg, wParam, lParam ) )
		//return;

	if ( uMsg == WM_LBUTTONDOWN )
	{
		last_mouseX = (float)LOWORD( lParam );
		last_mouseY = (float)HIWORD( lParam );
		mouse_button = true;
	}
	else if ( (uMsg == WM_LBUTTONUP) )
	{
		mouse_button = false;
	}
	else if ( uMsg == WM_MOUSEMOVE )
	{
		if ( !mouse_button ) return;

		float iMouseX = (float)LOWORD( lParam );
		float iMouseY = (float)HIWORD( lParam );

		float delta_hor_angle = horizontal_multiplier * (iMouseX - last_mouseX) / windowX;
		float delta_vert_angle = vertical_multiplier * (iMouseY - last_mouseY) / windowY;

		horizontal_angle += delta_hor_angle;
		vertical_angle += delta_vert_angle;

		last_mouseX = iMouseX;
		last_mouseY = iMouseY;
	}
	else if( uMsg == WM_KEYUP )
	{
		if( wParam == VK_SPACE )
			config_needs_reload = true;
		else if( wParam == VK_F1 )
			PRINT_STATISTICS( "Performance.txt" )
	}
	else if( uMsg == WM_QUIT )
		endThread = true;
}


void EngineClass::ReloadConfigurationFile()
{
	config_file.ReloadFile();

	latitude = config_file.GetValue< float >( "latitude" );
	longitude = config_file.GetValue< float >( "longitude" );
	turbidity = config_file.GetValue< double >( "turbidity" );

	albedo[ 0 ] = config_file.GetValue< float >( "albedoR" );
	albedo[ 1 ] = config_file.GetValue< float >( "albedoG" );
	albedo[ 2 ] = config_file.GetValue< float >( "albedoB" );

	sky_intensity = config_file.GetValue<float>( "skyIntensity" );
	sun_intensity = config_file.GetValue<float>( "sunIntensity" );

	rate = config_file.GetValue< float >( "rate" );

	// Je�eli ta zmienna jest ustawiona na false to czas nie jest nadpisywany czasem z pliku, ale kontynuowane jest
	// obecne odliczanie.
	bool updateDate = config_file.GetValue< bool >( "updatedate" );

	if( updateDate )
	{
		std::string newTime = config_file.GetValue< std::string >( "datetime" );

		std::tm convertedTime;
		std::stringstream converter( newTime );

		converter >> std::get_time( &convertedTime, "%d.%m.%Y %H:%M:%S" );
		//time_t secondsTime = mktime( &convertedTime );
		//time = (double)secondsTime / 3600;
		//time = fmod( time, 24.0 );
		//dateTime = convertedTime;

		time = sun_position->setSunConditions( latitude, longitude, &convertedTime );
	}
}

void EngineClass::updateSky()
{
	while ( !endThread )
	{
		sky_dome->wait_for_update();
		if( endThread ) return;		//Na wszelki wypadek, �eby nie czeka� za d�ugo na zako�czenie.

		float currentTime = sky_time_manager.onStartRenderFrame();
		currentTime *= rate;		// Przeliczamy na czas jaki up�yn�� dla nieba.
		time += currentTime;		// Dodajemy do aktualnego czasu.

		if( config_needs_reload )
		{
			ReloadConfigurationFile();		// Tylko ten w�tek ma prawo to wywo�a�. Nie ma innej synchronizacji.
			config_needs_reload = false;	// Zaznaczamy, �e obs�u�yli�my.
		}

		sun_position->setSunConditions( time );
		DirectX::XMVECTOR sun_dir = sun_position->computeSunDirection();
		sky_dome->update_sky_dome( sun_dir, turbidity, albedo, sky_intensity );
	}
}


DirectX::XMMATRIX EngineClass::getRotationMatrix()
{
	DirectX::XMVECTOR zenith = DirectX::XMVectorSet( 0.0, 1.0, 0.0, 0.0 );
	DirectX::XMVECTOR right = DirectX::XMVectorSet( 1.0, 0.0, 0.0, 0.0 );
	DirectX::XMVECTOR y_rotation = DirectX::XMQuaternionRotationNormal( zenith, horizontal_angle );
	//DirectX::XMMATRIX y_rot_matrix = DirectX::XMMatrixRotationQuaternion( y_rotation );

	//right = DirectX::XMVector4Transform( right, y_rot_matrix );
	DirectX::XMVECTOR x_rotation = DirectX::XMQuaternionRotationAxis( right, vertical_angle );
	//DirectX::XMMATRIX x_rot_matrix = DirectX::XMMatrixRotationQuaternion( x_rotation );
	
	DirectX::XMMATRIX translation = DirectX::XMMatrixTranslation( 0.0, 0.0, 30.0 );
	
	//DirectX::XMMATRIX rotation = x_rot_matrix;
	//DirectX::XMMATRIX rotation = y_rot_matrix;
	//DirectX::XMMATRIX rotation = DirectX::XMMatrixMultiply( y_rot_matrix, x_rot_matrix );
	DirectX::XMMATRIX rotation = DirectX::XMMatrixRotationQuaternion( DirectX::XMQuaternionMultiply( y_rotation, x_rotation ) );
	//DirectX::XMMATRIX rotation = DirectX::XMMatrixRotationQuaternion( DirectX::XMQuaternionRotationRollPitchYaw( vertical_angle, horizontal_angle, 0.0 ) );

	//return DirectX::XMMatrixMultiply( rotation, translation );
	return rotation;
}






/**@brief Funkcja ustawia tesktury z danego ModelParta w DirectXie.

@param[in] model ModelPart z kt�rego pochodz� tekstury do ustawienia.
@todo SetShaderResource mo�na u�y� do ustawienia od razu ca�ej tablicy. Trzeba umo�liwi� ustawianie
do VS i innych.*/
void EngineClass::set_textures( const ModelPart& model )
{
	for ( int i = 0; i < ENGINE_MAX_TEXTURES; ++i )
		if ( model.texture[i] )		// Nie ka�da tekstura w tablicy istnieje
		{
			ID3D11ShaderResourceView* tex = model.texture[i]->get();
			device_context->PSSetShaderResources( i, 1, &tex );
		}
}

/**@brief Ustawia w kontek�cie urz�dzenia bufor indeks�w.

@param[in] buffer Bufor do ustawienia.*/
void EngineClass::set_index_buffer( BufferObject* buffer )
{
	// Ustawiamy bufor indeks�w, je�eli istnieje
	ID3D11Buffer* index_buffer = nullptr;
	if ( buffer )
	{
		index_buffer = buffer->get();
		unsigned int offset = 0;
		device_context->IASetIndexBuffer( index_buffer, INDEX_BUFFER_FORMAT, offset );
	}
}

/**@brief Ustawia w kontek�cie urz�dzenia bufor wierzcho�k�w.

@param[in] buffer Bufor do ustawienia.
@return Je�eli bufor nie istnieje to zwraca warto�� true. Inaczej false.
Wywo�anie if( set_vertex_buffer() ) ma zwr�ci� tak� warto��, �eby w ifie mo�na by�o
wywo�a� return lub continue, w przypadku braku bufora.*/
bool EngineClass::set_vertex_buffer( BufferObject* buffer )
{
	ID3D11Buffer* vertex_buffer = nullptr;
	if ( buffer )
	{
		vertex_buffer = buffer->get();
		unsigned int stride = buffer->get_stride();
		unsigned int offset = 0;
		device_context->IASetVertexBuffers( 0, 1, &vertex_buffer, &stride, &offset );

		return false;
	}
	return true;
}

/**@brief kopiuje materia� do struktury, kt�ra pos�u�y do zaktualizowania bufora sta�ych.

@param[in] shader_data_per_mesh Struktura docelowa.
@param[in] model ModelPart z kt�rego pobieramy dane.*/
void EngineClass::copy_material( ConstantPerMesh* shader_data_per_mesh, const ModelPart* model )
{
	MaterialObject* material = model->material;
	shader_data_per_mesh->Diffuse = material->Diffuse;
	shader_data_per_mesh->Ambient = material->Ambient;
	shader_data_per_mesh->Specular = material->Specular;
	shader_data_per_mesh->Emissive = material->Emissive;
	shader_data_per_mesh->Power = material->Power;
}


/**@brief Tworzy macierz projekcji i zapami�tuje j� w polu projection_matrix klasy. W ka�dym wywo�aniu funkcji
display_scene ustawiana jest macierz zapisana w tym polu.
@param[in] angle K�t widzenia w pionie
@param[in] X_to_Y Stosunek Szeroko�ci do wysoko�ci ekranu
@param[in] near_plane Bli�sza p�aszczyzna obcinania
@param[in] far_plane Dalsza p�aszczyzna obcinania*/
void EngineClass::set_projection_matrix( float angle, float X_to_Y, float near_plane, float far_plane )
{
	XMMATRIX proj_matrix = XMMatrixPerspectiveFovLH( angle, X_to_Y, near_plane, far_plane );
	proj_matrix = XMMatrixTranspose( proj_matrix );
	XMStoreFloat4x4( &shader_data_per_frame.projection_matrix, proj_matrix );
}

