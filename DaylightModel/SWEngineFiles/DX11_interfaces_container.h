#pragma once

/**@file DX11_interfaces_container.h
@brief Zaiwiera deklaracj� klasy DX11_interfaces_container i DX11_constant_buffers_container s�u��cych do
Inicjowania i przechowywania obiekt�w DirectXa.*/

#include <string>

#pragma warning( disable : 4005 )
#include "d3d11.h"
#include "d3dx11.h"
#pragma warning( default : 4005 )




enum DX11_INIT_RESULT
{
	DX11_INIT_OK,
	NO_CLASS_INHERTIS_THIS_INTERFACE,
	COULD_NOT_INIT_DEVICES_AND_SWAPCHAIN,
	COULD_NOT_CREATE_BACKBUFFER,
	COULD_NOT_CREATE_RENDERTARGET,
	COULD_NOT_CREATE_DEPTHSTENCIL,
	COULD_NOT_CREATE_DEPTHSTENCIL_VIEW,
	COULD_NOT_CREATE_PIXEL_SHADER,
	COULD_NOT_CREATE_VERTEX_SHADER,
	COULD_NOT_CREATE_SAMPLER,
	DX11_DEVICE_NOT_INITIALIZED,
	DX11_DEVICECONTEXT_NOT_INITIALIZED
};

enum DX11_DEFAULT_VERTEX_LAYOUT
{
	VERTEX_NORMAL_TEXTURE = 0,
	VERTEX_TEXTURE = 1,
	VERTEX_COLOR = 2,
	VERTEX_NORMAL_COLOR = 3
};

/**@brief Klasa przechowuje w zmiennych statycznych najwa�niejsze interfejsy
DirectX11.

Obiekty, kt�re ich potrzebuj�, powinny odziedziczy� po tej klasie,
dzi�ki czemu maj� bezpo�redni dost�p.

Poza tym zawiera troszk� funkcji u�atwiaj�cych tworzenie projektu DirectX.

Za zainicjowanie zmiennych odpowiednimi warto�ciami odpowiada tylko i wy��cznie
klasa Engine. Formalna inicjacja zmiennych statycznych odbywa si� w pliku 
DX11_interfaces_container.cpp (formalna, czyli mam na my�li, �e inicjacja nullptrami).

Klasa zawiera te� metody do zainicjowania odpowiednich komponent�w.
Oczywi�cie wywo�a� je ma prawo te� tylko i wy��cznie klasa Engine.

W przypadku b��d�w innych ni� zwr�conych przez DirectXa funkcje zwracaj� makro
DXGI_ERROR_CANNOT_PROTECT_CONTENT. Mam nadziej�, �e to jest tak rzadko zwracana warto��,
�e b�dzie do�� jednoznacznie m�wi�, �e b��d jest innego typu ni� DirectXowy. Niestety nie wiem wed�ug
jakiego klucza s� tworzone te kody b��d�w, wi�c nie mog� stworzy� swojego.*/

class DX11_interfaces_container
{
private:
	static DX11_interfaces_container*	this_ptr;

	static unsigned int				_window_width;
	static unsigned int				_window_height;
	static std::string				_pixel_shader_model;
	static std::string				_vertex_shader_model;

	static DXGI_SWAP_CHAIN_DESC		_swap_chain_desc;	///<Domy�lny deskryptor g��wnych obiekt�w DirectX11
	static D3D11_VIEWPORT			_view_port_desc;	///<Domy�lny deskryptor viewportu. Je�eli u�ytkownik poda w�asny to zostanie on nadpisany.
	static D3D_FEATURE_LEVEL*		_feature_levels;	///<Tablica z preferowanymi poziomami DirectX. Domy�lnie nullptr, uzytkownik mo�e zdefiniowa�.
	static unsigned int				_num_feature_levels;///<Liczba element�w powy�szej tablicy.
	static D3D_FEATURE_LEVEL		_current_feature_level;		///<Przechowuje poziom DirectXa.
	static D3D11_TEXTURE2D_DESC		_z_buffer_desc;				///<Deskryptor z i stencil bufora.
	static D3D11_DEPTH_STENCIL_VIEW_DESC	_z_buffer_view_desc;	///<Deskryptor widoku z i stencil bufora.
	static D3D11_INPUT_ELEMENT_DESC*		_vertex_layout_desc;	///<Aktualny deskryptor layoutu wierzcho�ka.
	static unsigned int				_layout_elements_count;			///<Liczba element�w w talicy deskryptora layoutu.
	static D3D11_SAMPLER_DESC		_sampler_desc;			///<Deskryptor samplera.
protected:
	static ID3D11Device*			device;					///<Zmienna s�u�y do tworzenia obiekt�, bufor�w tekstur i zasob�w
	static ID3D11DeviceContext*		device_context;			///<Zmienna u�ywana do renderowania sceny, ustawiania bufor�w, shader�w itp.
	static IDXGISwapChain*			swap_chain;				///<S�u�y do prezentowania ramki na ekranie
	static ID3D11RenderTargetView*	render_target;			///<Widok bufora docelowego renderowania, czyli bufora tylnego
	static ID3D11DepthStencilView*	z_buffer_view;			///<Widok z-buffora
	
	static ID3D11SamplerState*		default_sampler;		///<Obiekt domy�lnego samplera
	static ID3D11InputLayout*		default_vertex_layout;	///<Layout formatu wierzcho�ka u�ywanego dla meshy
	static ID3D11VertexShader*		default_vertex_shader;	///<Obiekt domy�lnego vertex shadera
	static ID3D11PixelShader*		default_pixel_shader;	///<Obiekt domy�lnego piksel shadera
public:
	// Funkcje do ustawiania deskryptor�w i innych parametr�w
	void set_swapchain_desc( const DXGI_SWAP_CHAIN_DESC& swap_chain_desc );
	void set_viewport_desc( const D3D11_VIEWPORT& view_port_desc );
	void set_feature_levels( D3D_FEATURE_LEVEL* feature_levels, unsigned int elements );
	void set_window_resolution( unsigned int window_width, unsigned int window_height );
	void set_depth_stencil_format( DXGI_FORMAT depth_stencil_format );
	void set_vertex_layout( DX11_DEFAULT_VERTEX_LAYOUT layout );
	void set_vertex_layout( D3D11_INPUT_ELEMENT_DESC* layout, unsigned int array_size );
	void set_sampler_desc( D3D11_SAMPLER_DESC sampler_desc );

	// Pobieranie deskryptor�w
	DXGI_SWAP_CHAIN_DESC get_swap_chain_desc() { return _swap_chain_desc; }
	D3D11_VIEWPORT get_viewport_desc() { return _view_port_desc; }
	D3D_FEATURE_LEVEL get_current_feature_level() { return _current_feature_level; }
	D3D11_TEXTURE2D_DESC get_z_buffer_desc() { return _z_buffer_desc; }
	D3D11_DEPTH_STENCIL_VIEW_DESC get_z_buffer_view_desc() { return _z_buffer_view_desc; }
	D3D11_SAMPLER_DESC get_sampler_desc() { return _sampler_desc; }

	ID3D11VertexShader* load_vertex_shader( const std::wstring& file_name, const std::string& shader_name, const char* shader_model );
	ID3D11VertexShader* load_vertex_shader( const std::wstring& file_name, const std::string& shader_name,
											ID3D11InputLayout** layout, D3D11_INPUT_ELEMENT_DESC* layout_desc,
											unsigned int array_size, const char* shader_model );
	ID3D11PixelShader* load_pixel_shader( const std::wstring& file_name, const std::string& shader_name, const char* shader_model );


	// Funkcje inicjuj�ce
	DX11_INIT_RESULT init_devices( HWND window, bool fullscreen, bool single_thread = true );
	DX11_INIT_RESULT init_viewport();
	DX11_INIT_RESULT init_z_buffer_and_render_target();
	DX11_INIT_RESULT init_vertex_shader( const std::wstring& file_name, const std::string& shader_name );
	DX11_INIT_RESULT init_pixel_shader( const std::wstring& file_name, const std::string& shader_name );
	DX11_INIT_RESULT init_sampler();

protected:
	DX11_interfaces_container();
	~DX11_interfaces_container() = default;

	DX11_INIT_RESULT init_DX11( int width, int height, HWND window, bool fullscreen,
								const std::wstring& pix_shader_file, const std::string& pix_shader_name,
								const std::wstring& vert_shader_file, const std::string& vert_shader_name,
								bool single_thread = true );



	virtual void release_DirectX();

	void begin_scene( );
	inline void end_scene_and_present() { swap_chain->Present( 1, 0 ); }	///<Wywo�uje funkcj� swap_chain->Present w celu wy�wietlenia narysowanej sceny na monitorze
};



/**@brief Klasa zawiera wska�niki na bufory sta�ych zawieraj�ce podstawowe zestawy
element�w przekazywanych do shader�w. S� to miedzy innymi macierze transformacji.

@note Je�eli w jakimkolwiek miejscu s� u�ywane obiekty z tej klasy (DX11_constant_buffers_container), a nie tylko z klasy bazowej,
to przy zwalnianiu nale�y koniecznie wywo�a� funkcj� release_DirectX tego obiektu, a nie klasy bazowej. Czyli innymi s�owy klasa
odpowiedzialna za zwalnianie musi odziedziczy� po tym obiekcie nawet, je�eli nie u�ywa jego zmiennych.
W przeciwnym razie zostanie wywo�any funkcja wirtualna dla klasy bazowej.*/
class DX11_constant_buffers_container : public DX11_interfaces_container
{
protected:
	static ID3D11Buffer*				const_per_frame;	///<Bufor sta�ych zmieniaj�cych si� nie cz�ciej ni� co ramk�
	static ID3D11Buffer*				const_per_mesh;		///<Bufor sta�ych zmieniaj�cy si� dla ka�dej cz�ci mesha
	static ID3D11DepthStencilState*		depth_enabled;		///<Do w��czania z-bufora
	static ID3D11DepthStencilState*		depth_disabled;		///<Do wy��czania z-bufora

	void init_buffers(unsigned int size_per_frame, unsigned int size_per_mesh);
	void init_depth_states();

	void release_DirectX() override;
};

