#pragma once

#include "ModelsManager.h"
#include "DX11_interfaces_container.h"
#include "HosekSkylightModel\HosekSkyDome.h"
#include "ConstantBuffersFormat.h"
#include "SunPosition.h"
#include "TimeManager.h"
#include <thread>

class EngineClass : public DX11_constant_buffers_container
{
private:
	// Widok
	float vertical_angle;
	float horizontal_angle;
	float last_mouseX;
	float last_mouseY;
	float vertical_multiplier;
	float horizontal_multiplier;
	bool mouse_button;

	//
	ModelsManager* models_manager;
	HosekSkyDome* sky_dome;
	SunPosition* sun_position;

	TimeManager time_manager;

	float		latitude;					///< Szerokoœæ geograficzna na jakiej znajduje siê obserwator.
	float		longitude;					///< D³ugoœæ geograficzna obserwatora.
	float		time;
	double		albedo[3];
	double		turbidity;
	float		sky_intensity;
	float		sun_intensity;

	bool		endThread;
	std::thread	skyThread;
	float		rate;

	ConstantPerFrame				shader_data_per_frame;		///<Bufor sta³ych zmiennych co ramkê animacji

	// Funkcje pomocnicze do renderingu
	void set_textures( const ModelPart& model );
	void set_index_buffer( BufferObject* buffer );
	bool set_vertex_buffer( BufferObject* buffer );
	void copy_material( ConstantPerMesh* shader_data_per_mesh, const ModelPart* model );
	void set_projection_matrix( float angle, float X_to_Y, float near_plane, float far_plane );

	DirectX::XMMATRIX getRotationMatrix();
public:
	EngineClass();
	~EngineClass();

	DX11_INIT_RESULT init_all( HWND window, unsigned int width, unsigned int height );
	void render_frame();
	void updateSky();

	void HandleMessages( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
	inline void engineEnd()
	{
		endThread = true;
		skyThread.join();
	}
};

