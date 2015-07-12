#pragma once

#include "SWEngineFiles\ModelsManager.h"
#include "SWEngineFiles\DX11_interfaces_container.h"
#include "HosekSkylightModel\HosekSkyDome.h"
#include "SWEngineFiles\ConstantBuffersFormat.h"
#include "SunPosition.h"
#include "TimeManager.h"
#include <thread>
//#include "GUIManager.h"
#include "ConfigurationFile.h"

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
	ModelsManager*			models_manager;
	HosekSkyDome*			sky_dome;
	SunPosition*			sun_position;
	//GUIManager				guiManager;

	TimeManager				time_manager;
	TimeManager				sky_time_manager;


	// Modyfikuje jedynie w¹tek przeliczaj¹cy niebo
	ConfigurationFile		config_file;
	bool					config_needs_reload;
	float					rate;

	float		latitude;					///< Szerokoœæ geograficzna na jakiej znajduje siê obserwator.
	float		longitude;					///< D³ugoœæ geograficzna obserwatora.
	double		time;
	double		albedo[3];
	double		turbidity;
	float		sky_intensity;
	float		sun_intensity;
	
	void		ReloadConfigurationFile();
	//

	bool		endThread;
	std::thread	skyThread;
	

	ConstantPerFrame				shader_data_per_frame;		///<Bufor sta³ych zmiennych co ramkê animacji

	// Funkcje pomocnicze do renderingu
	void		set_textures				( const ModelPart& model );
	void		set_index_buffer			( BufferObject* buffer );
	bool		set_vertex_buffer			( BufferObject* buffer );
	void		copy_material				( ConstantPerMesh* shader_data_per_mesh, const ModelPart* model );
	void		set_projection_matrix		( float angle, float X_to_Y, float near_plane, float far_plane );

	DirectX::XMMATRIX getRotationMatrix();
public:
	EngineClass();
	~EngineClass();

	DX11_INIT_RESULT init_all( HWND window, unsigned int width, unsigned int height );
	void render_frame();
	void updateSky();
	void release();

	void HandleMessages( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
	inline void engineEnd()
	{
		endThread = true;
		sky_dome->signal();		// Puszczamy w¹tek, ¿eby siê mog³ skoñczyæ.
		skyThread.join();
	}
};

