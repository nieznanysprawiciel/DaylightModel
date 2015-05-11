#pragma once

#include "ModelsManager.h"
#include "DX11_interfaces_container.h"
#include "HosekSkylightModel\HosekSkyDome.h"
#include "ConstantBuffersFormat.h"

class EngineClass : public DX11_constant_buffers_container
{
private:
	ModelsManager* models_manager;
	HosekSkyDome* sky_dome;

	ConstantPerFrame				shader_data_per_frame;		///<Bufor sta³ych zmiennych co ramkê animacji

	// Funkcje pomocnicze do renderingu
	void set_textures( const ModelPart& model );
	void set_index_buffer( BufferObject* buffer );
	bool set_vertex_buffer( BufferObject* buffer );
	void copy_material( ConstantPerMesh* shader_data_per_mesh, const ModelPart* model );
	void set_projection_matrix( float angle, float X_to_Y, float near_plane, float far_plane );
public:
	EngineClass();
	~EngineClass();

	DX11_INIT_RESULT init_all( HWND window, unsigned int width, unsigned int height );
	void render_frame();


};

