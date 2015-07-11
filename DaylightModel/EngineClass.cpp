#include "stdafx.h"
#include "EngineClass.h"
#include <thread>


extern unsigned int windowX;
extern unsigned int windowY;

ID3D11Device* WINAPI DXUTGetD3D11Device() { return DX11_constant_buffers_container::device; }
ID3D11DeviceContext* WINAPI DXUTGetD3D11DeviceContext() { return DX11_constant_buffers_container::device_context; }
ID3D11RenderTargetView* WINAPI DXUTGetD3D11RenderTargetView() { return DX11_constant_buffers_container::render_target; }
ID3D11DepthStencilView* WINAPI DXUTGetD3D11DepthStencilView() { return DX11_constant_buffers_container::z_buffer_view; }
IDXGISwapChain* WINAPI DXUTGetDXGISwapChain() { return DX11_constant_buffers_container::swap_chain; }


void skyThreadFunction( EngineClass* engine )
{
	engine->updateSky();
}


EngineClass::EngineClass()
{
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
	time = 21420.0f;

	albedo[0] = 0.8;
	albedo[1] = 0.8;
	albedo[2] = 0.8;
	turbidity = 4.0;

	sky_intensity = 5.0f;
	sun_intensity = 5.0f;

	endThread = false;

	rate = 360;		// W ka¿d¹ sekundê czasu rzeczywistego mija 60 sekund.
	time_manager.initTimer();
	sky_time_manager.initTimer();
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

	//DirectX::XMVECTOR sun_dir = DirectX::XMVectorSet( -0.2f, 0.6f, 0.6f, 1.0f );
	sun_position->setSunConditions( DirectX::XMConvertToRadians( latitude ), DirectX::XMConvertToRadians( longitude ), time );
	DirectX::XMVECTOR sun_dir = sun_position->computeSunDirection();
	sky_dome->init_sky_dome( sun_dir, turbidity, albedo, 101, 101, 1000, sky_intensity );

	skyThread = std::thread( skyThreadFunction, this );

	//guiManager.initGUI( device, device_context );

	return result;
}

void EngineClass::render_frame()
{
	begin_scene();

	if ( !sky_dome )
		return;

	float timeElapsed = (float)time_manager.queryTimeFromBegin();

	// Ustawiamy format wierzcho³ków i bufory sta³ych
	device_context->IASetInputLayout( sky_dome->get_vertex_layout() );
	device_context->UpdateSubresource( const_per_frame, 0, nullptr, &shader_data_per_frame, 0, 0 );
	device_context->VSSetConstantBuffers( 0, 1, &const_per_frame );
	device_context->PSSetConstantBuffers( 0, 1, &const_per_frame );	// Aktualizuje bufor wierzcho³ków. Wstawiane s¹ nowe kolory.


	// Powinna byæ to raczej rzadka czynnoœæ, poniewa¿ aktualizacja jest kosztowna czasowo
	if ( sky_dome->update_vertex_buffer )
		sky_dome->update_buffers();

	// Ustawiamy bufor wierzcho³ków
	BufferObject* buffer = sky_dome->get_vertex_buffer();
	if ( set_vertex_buffer( buffer ) )
		return;	// Je¿eli nie ma bufora wierzcho³ków, to idziemy do nastêpnego mesha
	// Ustawiamy bufor indeksów, je¿eli istnieje
	set_index_buffer( sky_dome->get_index_buffer() );


	ModelPart* model = sky_dome->get_model_part();


	XMMATRIX rotation_matrix = getRotationMatrix();

	// Wype³niamy bufor sta³ych
	ConstantPerMesh shader_data_per_mesh;
	shader_data_per_mesh.world_matrix = rotation_matrix;// XMMatrixTranspose( rotation_matrix );	// Transformacja wierzcho³ków
	// Przepisujemy materia³
	copy_material( &shader_data_per_mesh, model );

	// Ustawiamy shadery
	device_context->VSSetShader( model->vertex_shader->get(), nullptr, 0 );
	device_context->PSSetShader( model->pixel_shader->get(), nullptr, 0 );

	// Aktualizujemy bufor sta³ych
	device_context->UpdateSubresource( const_per_mesh, 0, nullptr, &shader_data_per_mesh, 0, 0 );
	device_context->VSSetConstantBuffers( 1, 1, &const_per_mesh );
	device_context->PSSetConstantBuffers( 1, 1, &const_per_mesh );

	ID3D11Buffer* const_buffer = sky_dome->get_constant_buffer();
	device_context->VSSetConstantBuffers( 2, 1, &const_buffer );
	device_context->PSSetConstantBuffers( 2, 1, &const_buffer );

	// Ustawiamy tekstury
	set_textures( *model );

	//depth_buffer_enable( false );		// Wy³¹czamy z-bufor

	// Teraz renderujemy. Wybieramy albo tryb indeksowany, albo bezpoœredni.
	const MeshPartObject* part = model->mesh;
	if ( part->use_index_buf )
		device_context->DrawIndexed( part->vertices_count, part->buffer_offset, part->base_vertex );
	else // Tryb bezpoœredni
		device_context->Draw( part->vertices_count, part->buffer_offset );

	//depth_buffer_enable( true );		// W³¹czamy z-bufor spowrotem

	//guiManager.RenderGUI( timeElapsed );

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
	else if ( uMsg == WM_QUIT )
		endThread = true;
}


void EngineClass::updateSky()
{
	while ( !endThread )
	{
		float currentTime = sky_time_manager.onStartRenderFrame();
		currentTime *= rate;		// Przeliczamy na czas jaki up³yn¹³ dla nieba.
		time += currentTime;		// Dodajemy do aktualnego czasu.

		sun_position->setSunConditions( latitude, longitude, time );
		DirectX::XMVECTOR sun_dir = sun_position->computeSunDirection();
		sky_dome->update_sky_dome( sun_dir, turbidity, albedo, sky_intensity );
	}
}


DirectX::XMMATRIX EngineClass::getRotationMatrix()
{
	DirectX::XMVECTOR zenith = DirectX::XMVectorSet( 0.0, 1.0, 0.0, 0.0 );
	DirectX::XMVECTOR right = DirectX::XMVectorSet( 1.0, 0.0, 0.0, 0.0 );
	DirectX::XMVECTOR y_rotation = DirectX::XMQuaternionRotationNormal( zenith, horizontal_angle );
	//right = DirectX::XMVector3Rotate( right, y_rotation );
	DirectX::XMVECTOR x_rotation = DirectX::XMQuaternionRotationAxis( right, vertical_angle );
	return DirectX::XMMatrixRotationQuaternion( DirectX::XMQuaternionMultiply( x_rotation, y_rotation ) );
	//return DirectX::XMMatrixRotationRollPitchYaw( vertical_angle, horizontal_angle, 0.0f );
}






/**@brief Funkcja ustawia tesktury z danego ModelParta w DirectXie.

@param[in] model ModelPart z którego pochodz¹ tekstury do ustawienia.
@todo SetShaderResource mo¿na u¿yæ do ustawienia od razu ca³ej tablicy. Trzeba umo¿liwiæ ustawianie
do VS i innych.*/
void EngineClass::set_textures( const ModelPart& model )
{
	for ( int i = 0; i < ENGINE_MAX_TEXTURES; ++i )
		if ( model.texture[i] )		// Nie ka¿da tekstura w tablicy istnieje
		{
			ID3D11ShaderResourceView* tex = model.texture[i]->get();
			device_context->PSSetShaderResources( i, 1, &tex );
		}
}

/**@brief Ustawia w kontekœcie urz¹dzenia bufor indeksów.

@param[in] buffer Bufor do ustawienia.*/
void EngineClass::set_index_buffer( BufferObject* buffer )
{
	// Ustawiamy bufor indeksów, je¿eli istnieje
	ID3D11Buffer* index_buffer = nullptr;
	if ( buffer )
	{
		index_buffer = buffer->get();
		unsigned int offset = 0;
		device_context->IASetIndexBuffer( index_buffer, INDEX_BUFFER_FORMAT, offset );
	}
}

/**@brief Ustawia w kontekœcie urz¹dzenia bufor wierzcho³ków.

@param[in] buffer Bufor do ustawienia.
@return Je¿eli bufor nie istnieje to zwraca wartoœæ true. Inaczej false.
Wywo³anie if( set_vertex_buffer() ) ma zwróciæ tak¹ wartoœæ, ¿eby w ifie mo¿na by³o
wywo³aæ return lub continue, w przypadku braku bufora.*/
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

/**@brief kopiuje materia³ do struktury, która pos³u¿y do zaktualizowania bufora sta³ych.

@param[in] shader_data_per_mesh Struktura docelowa.
@param[in] model ModelPart z którego pobieramy dane.*/
void EngineClass::copy_material( ConstantPerMesh* shader_data_per_mesh, const ModelPart* model )
{
	MaterialObject* material = model->material;
	shader_data_per_mesh->Diffuse = material->Diffuse;
	shader_data_per_mesh->Ambient = material->Ambient;
	shader_data_per_mesh->Specular = material->Specular;
	shader_data_per_mesh->Emissive = material->Emissive;
	shader_data_per_mesh->Power = material->Power;
}


/**@brief Tworzy macierz projekcji i zapamiêtuje j¹ w polu projection_matrix klasy. W ka¿dym wywo³aniu funkcji
display_scene ustawiana jest macierz zapisana w tym polu.
@param[in] angle K¹t widzenia w pionie
@param[in] X_to_Y Stosunek Szerokoœci do wysokoœci ekranu
@param[in] near_plane Bli¿sza p³aszczyzna obcinania
@param[in] far_plane Dalsza p³aszczyzna obcinania*/
void EngineClass::set_projection_matrix( float angle, float X_to_Y, float near_plane, float far_plane )
{
	XMMATRIX proj_matrix = XMMatrixPerspectiveFovLH( angle, X_to_Y, near_plane, far_plane );
	proj_matrix = XMMatrixTranspose( proj_matrix );
	XMStoreFloat4x4( &shader_data_per_frame.projection_matrix, proj_matrix );
}

