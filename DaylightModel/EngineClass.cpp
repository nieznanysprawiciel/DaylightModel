#include "stdafx.h"
#include "EngineClass.h"


EngineClass::EngineClass()
{
	models_manager = new ModelsManager;
	sky_dome = new HosekSkyDome( models_manager );
}


EngineClass::~EngineClass()
{
	delete sky_dome;
	delete models_manager;
	release_DirectX();
}

DX11_INIT_RESULT EngineClass::init_all( HWND window, unsigned int width, unsigned int height )
{
	DX11_INIT_RESULT result;

	std::wstring shader_file = L"HosekSkyDome.fx";
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

	double albedo[3] = { 0.8, 0.8, 0.8 };
	double turbidity = 5;
	DirectX::XMVECTOR sun_dir = DirectX::XMVectorSet( -0.2f, 0.6f, 0.6f, 1.0f );
	sky_dome->init_sky_dome( sun_dir, turbidity, albedo, 101, 101, 100, 5.0 );

	return result;
}

void EngineClass::render_frame()
{
	begin_scene();

	if ( !sky_dome )
		return;

	// Ustawiamy format wierzcho�k�w i bufory sta�ych
	device_context->IASetInputLayout( sky_dome->get_vertex_layout() );
	device_context->UpdateSubresource( const_per_frame, 0, nullptr, &shader_data_per_frame, 0, 0 );
	device_context->VSSetConstantBuffers( 0, 1, &const_per_frame );
	device_context->PSSetConstantBuffers( 0, 1, &const_per_frame );	// Aktualizuje bufor wierzcho�k�w. Wstawiane s� nowe kolory.


	// Powinna by� to raczej rzadka czynno��, poniewa� aktualizacja jest kosztowna czasowo
	if ( sky_dome->update_vertex_buffer )
		sky_dome->update_buffers();

	// Ustawiamy bufor wierzcho�k�w
	BufferObject* buffer = sky_dome->get_vertex_buffer();
	if ( set_vertex_buffer( buffer ) )
		return;	// Je�eli nie ma bufora wierzcho�k�w, to idziemy do nast�pnego mesha
	// Ustawiamy bufor indeks�w, je�eli istnieje
	set_index_buffer( sky_dome->get_index_buffer() );


	ModelPart* model = sky_dome->get_model_part();

	//// Wyliczamy macierz transformacji
	//XMVECTOR quaternion = current_camera->get_interpolated_orientation( time_lag );
	//inverse_camera_orientation( quaternion );

	XMVECTOR quaternion = XMQuaternionIdentity();

	XMMATRIX rotation_matrix = XMMatrixRotationQuaternion( quaternion );

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

	end_scene_and_present();
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

