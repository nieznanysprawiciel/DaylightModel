#include "DX11_interfaces_container.h"


//#include "..\memory_leaks.h"

//----------------------------------------------------------------------------------------------//
//								Zmienne statyczne klasy											//
//----------------------------------------------------------------------------------------------//

/**@brief Wska�nik na klas� lub nullptr je�eli nie zosta� stworzony jeszcze obiekt.

Ta klasa nie zawiera �adnych zmiennych niestatycznych, ale chcemy wiedzie� czy zosta� stworzony chocia� jeden
obiekt. Dzi�ki temu mo�emy zainicjowa� deskryptory domy�lnymi warto�ciami w wygodny spos�b w konstruktorze.*/
DX11_interfaces_container* DX11_interfaces_container::this_ptr = nullptr;

//Zmienne globalne dla funkcji
D3D11_VIEWPORT DX11_interfaces_container::_view_port_desc;			//Domy�lny viewport. Je�eli uzytkownik poda w�asny to zostanie on nadpisany.
DXGI_SWAP_CHAIN_DESC DX11_interfaces_container::_swap_chain_desc;
D3D_FEATURE_LEVEL* DX11_interfaces_container::_feature_levels = nullptr;
unsigned int DX11_interfaces_container::_num_feature_levels = 0;
D3D_FEATURE_LEVEL DX11_interfaces_container::_current_feature_level;
D3D11_TEXTURE2D_DESC DX11_interfaces_container::_z_buffer_desc;					///<Deskryptor z i stencil bufora.
D3D11_DEPTH_STENCIL_VIEW_DESC DX11_interfaces_container::_z_buffer_view_desc;
std::string DX11_interfaces_container::_pixel_shader_model = "ps_4_0";
std::string DX11_interfaces_container::_vertex_shader_model = "vs_4_0";
unsigned int DX11_interfaces_container::_window_width = 1024;
unsigned int DX11_interfaces_container::_window_height = 768;
D3D11_INPUT_ELEMENT_DESC* DX11_interfaces_container::_vertex_layout_desc = nullptr;
unsigned int DX11_interfaces_container::_layout_elements_count = 0;
D3D11_SAMPLER_DESC DX11_interfaces_container::_sampler_desc;

/*Inicjalizacja zmiennych statycznych.*/
ID3D11Device* DX11_interfaces_container::device = nullptr;
ID3D11DeviceContext* DX11_interfaces_container::device_context = nullptr;
IDXGISwapChain* DX11_interfaces_container::swap_chain = nullptr;
ID3D11RenderTargetView* DX11_interfaces_container::render_target = nullptr;
ID3D11DepthStencilView*	DX11_interfaces_container::z_buffer_view = nullptr;


ID3D11InputLayout* DX11_interfaces_container::default_vertex_layout = nullptr;
ID3D11VertexShader* DX11_interfaces_container::default_vertex_shader = nullptr;
ID3D11PixelShader* DX11_interfaces_container::default_pixel_shader = nullptr;
ID3D11SamplerState*	DX11_interfaces_container::default_sampler = nullptr;

ID3D11Buffer* DX11_constant_buffers_container::const_per_frame = nullptr;
ID3D11Buffer* DX11_constant_buffers_container::const_per_mesh = nullptr;
ID3D11DepthStencilState* DX11_constant_buffers_container::depth_enabled = nullptr;		///<Do w��czania z-bufora
ID3D11DepthStencilState* DX11_constant_buffers_container::depth_disabled = nullptr;		///<D0 wy��czania z-bufora

//----------------------------------------------------------------------------------------------//
//								Layouty dla bufora wierzcho�k�w									//
//----------------------------------------------------------------------------------------------//
D3D11_INPUT_ELEMENT_DESC vertex_normal_texture[] =
{
	{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 }
};

D3D11_INPUT_ELEMENT_DESC vertex_texture[] =
{
	{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 }
};

D3D11_INPUT_ELEMENT_DESC vertex_color[] =
{
	{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "COLOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
};

D3D11_INPUT_ELEMENT_DESC vertex_normal_color[] =
{
	{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "COLOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 },
};

D3D11_INPUT_ELEMENT_DESC* layouts[] = { vertex_normal_texture,
										vertex_texture,
										vertex_color,
										vertex_normal_color
										};
unsigned int layouts_elements[] = { ARRAYSIZE( vertex_normal_texture ),
									ARRAYSIZE( vertex_texture ),
									ARRAYSIZE( vertex_color ),
									ARRAYSIZE( vertex_normal_color ) };

//----------------------------------------------------------------------------------------------//
//								Inicjalizacja deskryptor�w										//
//----------------------------------------------------------------------------------------------//

DX11_interfaces_container::DX11_interfaces_container()
{
	// Inicjalizujemy tylko wtedy, kiedy wcze�niej si� to nie sta�o.
	if ( this_ptr == nullptr )
	{
		this_ptr = this;

		_window_width = 1024;
		_window_height = 768;

		_vertex_layout_desc = layouts[0];
		_layout_elements_count = layouts_elements[0];

		// DXGI_SWAP_CHAIN_DESC
		ZeroMemory( &_swap_chain_desc, sizeof( _swap_chain_desc ) );
		_swap_chain_desc.BufferCount = 1;
		_swap_chain_desc.BufferDesc.Width = _window_width;
		_swap_chain_desc.BufferDesc.Height = _window_height;
		_swap_chain_desc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		_swap_chain_desc.BufferDesc.RefreshRate.Numerator = 60;
		_swap_chain_desc.BufferDesc.RefreshRate.Denominator = 1;
		_swap_chain_desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		_swap_chain_desc.OutputWindow = 0;		//HWND niestety nie wiem co wpisa� w tym momencie
		_swap_chain_desc.SampleDesc.Count = 1;
		_swap_chain_desc.SampleDesc.Quality = 0;
		_swap_chain_desc.Windowed = TRUE;
		_swap_chain_desc.SwapEffect = DXGI_SWAP_EFFECT::DXGI_SWAP_EFFECT_DISCARD;
		_swap_chain_desc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

		// D3D11_VIEWPORT Deklarujemy przekszta�cenie wsp�rz�dnych ekranowych, na wsp�rzedne znormalizowane
		_view_port_desc.Width = (FLOAT)_window_width;
		_view_port_desc.Height = (FLOAT)_window_height;
		_view_port_desc.MinDepth = 0.0f;
		_view_port_desc.MaxDepth = 1.0f;
		_view_port_desc.TopLeftX = 0;
		_view_port_desc.TopLeftY = 0;

		// D3D11_TEXTURE2D_DESC Deskryptor z-stencil_buffer
		ZeroMemory( &_z_buffer_desc, sizeof( _z_buffer_desc ) );
		_z_buffer_desc.Width = _window_width;
		_z_buffer_desc.Height = _window_height;
		_z_buffer_desc.MipLevels = 1;
		_z_buffer_desc.ArraySize = 1;
		_z_buffer_desc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		_z_buffer_desc.SampleDesc.Count = 1;
		_z_buffer_desc.SampleDesc.Quality = 0;
		_z_buffer_desc.Usage = D3D11_USAGE_DEFAULT;
		_z_buffer_desc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
		_z_buffer_desc.CPUAccessFlags = 0;
		_z_buffer_desc.MiscFlags = 0;

		ZeroMemory( &_z_buffer_view_desc, sizeof( _z_buffer_view_desc ) );
		_z_buffer_view_desc.Format = _z_buffer_desc.Format;
		_z_buffer_view_desc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		_z_buffer_view_desc.Texture2D.MipSlice = 0;

		// D3D11_SAMPLER_DESC
		ZeroMemory( &_sampler_desc, sizeof( _sampler_desc ) );
		_sampler_desc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		_sampler_desc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
		_sampler_desc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
		_sampler_desc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
		_sampler_desc.ComparisonFunc = D3D11_COMPARISON_NEVER;
		_sampler_desc.MinLOD = 0;
		_sampler_desc.MaxLOD = D3D11_FLOAT32_MAX;
	}
}

//----------------------------------------------------------------------------------------------//
//								Zwalnianie obiekt�w DirectXa									//
//----------------------------------------------------------------------------------------------//

/**@brief Zwalnia wszystkie stworzone obiekty DirectXa.
*/
void DX11_interfaces_container::release_DirectX( )
{
	// Bardzo wa�ne jest ustawienie zmiennych na nullptr w razie, gdyby jaka� inna klasa wywo�ywa�a destruktor

	//Zmienne pomocnicze
	if ( default_vertex_layout )
		default_vertex_layout->Release(), default_vertex_layout = nullptr;
	if ( default_vertex_shader )
		default_vertex_shader->Release( ), default_vertex_shader = nullptr;
	if ( default_pixel_shader )
		default_pixel_shader->Release( ), default_pixel_shader = nullptr;
	if ( default_sampler )
		default_sampler->Release( ), default_sampler = nullptr;

	if ( swap_chain )
		//DirectX nie potrafi si� zamkn�� w trybie pe�noekranowym, wi�c musimy go zmieni�
		swap_chain->SetFullscreenState( FALSE, NULL );

	//Zmienne s�u��ce do wy�wietlania
	if ( z_buffer_view )
		z_buffer_view->Release( ), z_buffer_view = nullptr;
	if ( swap_chain )
		swap_chain->Release( ), swap_chain = nullptr;
	if ( render_target )
		render_target->Release( ), render_target = nullptr;
	if ( device )
		device->Release( ), device = nullptr;
	if ( device_context )
		device_context->Release( ), device_context = nullptr;
}


//----------------------------------------------------------------------------------------------//
//					Funkcje ustawiaj�ce parametry inicjalizacji obiekt�w DX11					//
//----------------------------------------------------------------------------------------------//

/**@brief Funkcja ustawia deskryptor dla obiekt�w Device, DeviceContext i SwapChain DirectXa.

Warto�ci takich p�l jak:
- BufferDesc.Width
- BufferDesc.Height
s� pobierane z p�l klasy i trzeba je modyfikowa� dedykowan� do tego funkcj�.*/
void DX11_interfaces_container::set_swapchain_desc( const DXGI_SWAP_CHAIN_DESC& swap_chain_desc )
{
	_swap_chain_desc = swap_chain_desc;

	_swap_chain_desc.BufferDesc.Width = _window_width;
	_swap_chain_desc.BufferDesc.Height = _window_height;
}

/**@brief Funkcja ustawia deskryptor dla Viewportu.
Nie jest on ustawiany jako aktywny w DirectX, trzeba wywo�a� funkcj� init_viewport.

Warto�ci takich p�l jak:
- BufferDesc.Width
- BufferDesc.Height
s� pobierane z p�l klasy i trzeba je modyfikowa� dedykowan� do tego funkcj�.*/
void DX11_interfaces_container::set_viewport_desc( const D3D11_VIEWPORT& view_port_desc )
{
	_view_port_desc = view_port_desc;

	_view_port_desc.Height = static_cast<float>( _window_height );
	_view_port_desc.Width = static_cast<float>( _window_width );
}

/**@brief ustawia podan� w parametrze tablic� z levelami.
Tablica nie powinna zosta� usuni�ta, poniewa� nie jest kopiowana.

Je�eli nic nie zostanie ustawione to domy�lne ustawiana jest warto�� nullptr, co jest 
r�wnowa�ne podaniu nast�puj�cej tablicy:
D3D_FEATURE_LEVEL_11_0,
D3D_FEATURE_LEVEL_10_1,
D3D_FEATURE_LEVEL_10_0,
D3D_FEATURE_LEVEL_9_3,
D3D_FEATURE_LEVEL_9_2,
D3D_FEATURE_LEVEL_9_1,

Zasadniczo lepiej nie modyfikowa� tego parametru, jezeli nie ma takiej konieczno�ci.

@param[in] elements Liczba element�w w tablicy.*/
void DX11_interfaces_container::set_feature_levels( D3D_FEATURE_LEVEL* feature_levels, unsigned int elements )
{
	_feature_levels = feature_levels;
	_num_feature_levels = elements;
}

void DX11_interfaces_container::set_window_resolution( unsigned int window_width, unsigned int window_height )
{
	_window_width = window_width;
	_window_height = window_height;

// Uzupe�niamy dane w odpowiednich deskryptorach
	// DXGI_SWAP_CHAIN_DESC
	_swap_chain_desc.BufferDesc.Width = _window_width;
	_swap_chain_desc.BufferDesc.Height = _window_height;

	// D3D11_VIEWPORT
	_view_port_desc.Height = static_cast<float>( _window_height );
	_view_port_desc.Width = static_cast<float>( _window_width );

	// D3D11_TEXTURE2D_DESC Z-Buffer
	_z_buffer_desc.Height = _window_height;
	_z_buffer_desc.Width = _window_width;
}

void DX11_interfaces_container::set_depth_stencil_format( DXGI_FORMAT depth_stencil_format )
{
	_z_buffer_desc.Format = depth_stencil_format;
	_z_buffer_view_desc.Format = depth_stencil_format;
}

/**@brief Ustawia jeden z domy�lnych deskryptor�w layout�w zdefiniowanych w klasie.
Funkcja nie ustawia �adnego layoutu w directX.

@attention Funkcja nie zwalnia pami�ci po starym layoucie. Layouty powinny by� alokowane na stosie
i by� zmiennymi globalnymi w przeciwnym razie moga byc wycieki pam�ci.*/
void DX11_interfaces_container::set_vertex_layout( DX11_DEFAULT_VERTEX_LAYOUT layout )
{
	_vertex_layout_desc = layouts[static_cast<unsigned int>(layout)];
	_layout_elements_count = layouts_elements[static_cast<int>(layout)];
}

/**@brief Ustawia podany deskryptor layoutu.
Funkcja nie ustawia �adnego layoutu w directX.

@attention Funkcja nie zwalnia pami�ci po starym layoucie. Layouty powinny by� alokowane na stosie
i by� zmiennymi globalnymi w przeciwnym razie moga byc wycieki pam�ci.*/
void DX11_interfaces_container::set_vertex_layout( D3D11_INPUT_ELEMENT_DESC* layout, unsigned int array_size )
{
	_vertex_layout_desc = layout;
	_layout_elements_count = array_size;
}

void DX11_interfaces_container::set_sampler_desc( D3D11_SAMPLER_DESC sampler_desc )
{
	_sampler_desc = sampler_desc;
}

//----------------------------------------------------------------------------------------------//
//					Jedna du�a funkcja, kt�ra za�atwia jak najwi�cej							//
//----------------------------------------------------------------------------------------------//
DX11_INIT_RESULT DX11_interfaces_container::init_DX11(
							int width, int height, HWND window, bool fullscreen,
							const std::wstring& pix_shader_file, const std::string& pix_shader_name,
							const std::wstring& vert_shader_file, const std::string& vert_shader_name,
							bool single_thread )
{
	DX11_INIT_RESULT result;

	set_window_resolution( width, height );
	_swap_chain_desc.OutputWindow = window;

	result = init_devices( window, fullscreen, single_thread );	// Funkcja sama sprz�ta po sobie
	if ( result != DX11_INIT_OK )
		return result;

	result = init_z_buffer_and_render_target();		// Funkcja sama sprz�ta po sobie
	if ( result != DX11_INIT_OK )
		return result;

	result = init_viewport();		// Funkcja nie sprz�ta, bo by� mo�e nie ma czego, je�eli si� nie uda�o.
	if ( result != DX11_INIT_OK )
	{
		release_DirectX();	// Sprz�tamy na wszelki wypadek, ale w gruncie rzeczy najprawdopodobniej nie ma czego.
		return result;
	}

	result = init_vertex_shader( vert_shader_file, vert_shader_name );
	if ( result != DX11_INIT_OK )
	{
		release_DirectX();	// By� mo�e nie by�o pliku, ale i tak nic z tym nie zrobimy wi�c sprz�tamy.
		return result;
	}

	result = init_pixel_shader( pix_shader_file, pix_shader_name );
	if ( result != DX11_INIT_OK )
	{
		release_DirectX();	// By� mo�e nie by�o pliku, ale i tak nic z tym nie zrobimy wi�c sprz�tamy.
		return result;
	}

	result = init_sampler();
	if ( result != DX11_INIT_OK )
	{
		release_DirectX();	// Jak tu co� si� nie uda�o, to znaczy, �e deskryptor by� niepoprawny.
		return result;
	}

	return DX11_INIT_OK;
}

//----------------------------------------------------------------------------------------------//
//								Tworzenie obiekt�w DirectXa										//
//----------------------------------------------------------------------------------------------//

/**@brief Tworzy obiekty swap_chain, device i device_context.

@param[in] width Szeroko�� ekranu w pikselach
@param[in] height Wysoko�� ekranu w pikselach
@param[in] window Uchwyt okna, w kt�rym renderujemy
@param[in] fullscreen Tryb pe�noekranowy lub w oknie.
@param[in] single_thread Ustawi� na true, je�eli mamy tylko jeden w�tek, kt�ry tworzy wszystkie obiekty
DirectXa typu bufory i shadery. Domy�lnie false.
@return Zwraca jedn� z warto�ci DX11_INIT_RESULT.*/
DX11_INIT_RESULT DX11_interfaces_container::init_devices( HWND window, bool fullscreen, bool single_thread )
{
	if ( !this_ptr )
		return NO_CLASS_INHERTIS_THIS_INTERFACE;

	HRESULT result = S_OK;
	UINT createDeviceFlags = 0;

#ifdef _DEBUG
	createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif
	if ( single_thread )
		// Domy�lnie obiekt ID3D11Device jest synchronizowany, ale mo�na to wy��czy�
		createDeviceFlags |= D3D11_CREATE_DEVICE_SINGLETHREADED;


	result = D3D11CreateDeviceAndSwapChain( nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr,
											createDeviceFlags, _feature_levels, _num_feature_levels,
											D3D11_SDK_VERSION, &_swap_chain_desc, &swap_chain,
											&device, &_current_feature_level, &device_context );
	if ( FAILED(result) )
		return COULD_NOT_INIT_DEVICES_AND_SWAPCHAIN;

	if ( fullscreen )
		swap_chain->SetFullscreenState( TRUE, nullptr );


	return DX11_INIT_OK;
}

/**@brief Ustawia viewport zgodny z aktualnie ustawionym deskryptorem.*/
DX11_INIT_RESULT DX11_interfaces_container::init_viewport()
{
	if ( !device )
		return DX11_DEVICE_NOT_INITIALIZED;
	device_context->RSSetViewports( 1, &_view_port_desc );	//tworzymy tylko jeden viewport, ale noramlnie mo�na wiecej, tylko po co
	return DX11_INIT_OK;
}

/**@brief Funkcja tworzy z-bufffer oraz pobiera tylny bufor i tworzy z niego render target.
Nast�pnie widok z bufora i widok tylnego bufora s� ustawione jako cel dla funkcji renderuj�cych renderowania.

@return Zwraca jedn� ze sta�ych DX11_INIT_RESULT.
*/
DX11_INIT_RESULT DX11_interfaces_container::init_z_buffer_and_render_target()
{
	HRESULT result = S_OK;
	if ( !device )
		return DX11_DEVICE_NOT_INITIALIZED;

// RenderTargetView
	// Tworzymy RenderTargetView. W tym celu pobieramy wska�nik na obiekt tylniego bufora
	// i tworzymy z niego widok.
	ID3D11Texture2D* back_buffer = nullptr;
	result = swap_chain->GetBuffer( 0, __uuidof(ID3D11Texture2D), (LPVOID*)&back_buffer );
	if ( FAILED( result ) )
	{
		release_DirectX();
		return COULD_NOT_CREATE_BACKBUFFER;
	}

	result = device->CreateRenderTargetView( back_buffer, nullptr, &render_target );
	if ( FAILED( result ) )
	{
		release_DirectX();
		return COULD_NOT_CREATE_RENDERTARGET;
	}
	//Zwracam uwag�, �e tutaj nie zwalniamy bufora. Zwalniamy tylko odwo�anie do niego, a bufor
	//zostanie zwolniony dopiero jak wszystkie istniej�ce obiekty przestan� go u�ywa�.
	back_buffer->Release();

// z-buffer

	ID3D11Texture2D* zBuffer;

	result = device->CreateTexture2D( &_z_buffer_desc, nullptr, &zBuffer );

	if ( FAILED(result) )
	{
		release_DirectX( );
		return COULD_NOT_CREATE_DEPTHSTENCIL;
	}

	// Create the depth stencil view
	result = device->CreateDepthStencilView( zBuffer, &_z_buffer_view_desc, &z_buffer_view );

	zBuffer->Release();

	if ( FAILED(result) )
	{
		release_DirectX( );
		return COULD_NOT_CREATE_DEPTHSTENCIL_VIEW;
	}

	device_context->OMSetRenderTargets( 1, &render_target, z_buffer_view );

	return DX11_INIT_OK;
}



//----------------------------------------------------------------------------------------------//
//								Tworzenie domy�lnych shader�w									//
//----------------------------------------------------------------------------------------------//

/**@brief Tworzy obiekt VertexShaderObject na podstawie pliku.

W przypadku b��d�w kompilacji w trybie debug s� one przekierowane do okna Output.

Na razie obs�uguje tylko nieskompilowane pliki.
@param[in] file_name Nazwa pliku, z kt�rego zostanie wczytany shader.
@param[in] shader_name Nazwa funkcji, kt�ra jest punktem poczatkowym wykonania shadera.
@param[in] shader_model �a�cuch znak�w opisuj�cy shader model.
@return Zwraca wska�nik na obiekt vertex shadera lub nullptr w przypadku niepowodzenia.*/
ID3D11VertexShader* DX11_interfaces_container::load_vertex_shader( const std::wstring& file_name, const std::string& shader_name, const char* shader_model = "vs_4_0" )
{
	if ( !device )
		return nullptr;

	HRESULT result;
	ID3DBlob* compiled_shader;
	ID3D11VertexShader* vertex_shader;
	// Troszk� zamieszania, ale w trybie debug warto wiedzie� co jest nie tak przy kompilacji shadera
#ifdef _DEBUG
	ID3D10Blob* error_blob = nullptr;	// Tu znajdzie si� komunikat o b��dzie
#endif

	// Kompilujemy shader znaleziony w pliku
	D3DX11CompileFromFile( file_name.c_str(), 0, 0, shader_name.c_str(), shader_model,
						   0, 0, 0, &compiled_shader,
#ifdef _DEBUG
						   &error_blob,	// Funkcja wsadzi informacje o b��dzie w to miejsce
#else
						   0,	// W trybie Release nie chcemy dostawa� b��d�w
#endif
						   &result );

	if ( FAILED( result ) )
	{
#ifdef _DEBUG
		if ( error_blob )
		{
			// B��d zostanie wypisany na standardowe wyj�cie
			OutputDebugStringA( (char*)error_blob->GetBufferPointer() );
			error_blob->Release();
		}
#endif
		return nullptr;
	}

	// Tworzymy obiekt shadera na podstawie tego co sie skompilowa�o
	result = device->CreateVertexShader( compiled_shader->GetBufferPointer(),
										 compiled_shader->GetBufferSize(),
										 nullptr, &vertex_shader );

	if ( FAILED( result ) )
	{
		compiled_shader->Release();
		return nullptr;
	}

	return vertex_shader;
}

/**@brief Tworzy obiekt vertex shadera na podstawie pliku. Zwraca r�wnie� layout dla podanej struktury wierzcho�ka.
Nie nale�y u�ywa� tej funkcji, je�eli layout nie jest rzeczywi�cie potrzebny. Trzeba pamieta� o zwolnieniu
go, kiedy przestanie by� potrzebny.

W przypadku b��d�w kompilacji w trybie debug s� one przekierowane do okna Output.

Na razie obs�uguje tylko nieskompilowane pliki.
@param[in] file_name Nazwa pliku, z kt�rego zostanie wczytany shader
@param[in] shader_name Nazwa funkcji, kt�ra jest punktem poczatkowym wykonania shadera
@param[out] layout W zmiennej umieszczany jest wska�nik na layout wierzcho�ka. Nale�y pami�ta� o zwolnieniu go kiedy b�dzie niepotrzebny.
@param[in] layout_desc Deskryptor opisujacy tworzony layout.
@param[in] array_size Liczba element�w tablicy layout_desc.
@param[in] shader_model �a�cuch znak�w opisuj�cy shader model.
@return Zwraca wska�nik na obiekt vertex shadera lub nullptr w przypadku niepowodzenia.*/
ID3D11VertexShader* DX11_interfaces_container::load_vertex_shader( const std::wstring& file_name, const std::string& shader_name,
										ID3D11InputLayout** layout, D3D11_INPUT_ELEMENT_DESC* layout_desc,
										unsigned int array_size, const char* shader_model = "vs_4_0" )
{
	if ( !device )
		return nullptr;

	HRESULT result;
	ID3DBlob* compiled_shader;
	ID3D11VertexShader* vertex_shader;
	// Troszk� zamieszania, ale w trybie debug warto wiedzie� co jest nie tak przy kompilacji shadera
#ifdef _DEBUG
	ID3D10Blob* error_blob = nullptr;	// Tu znajdzie si� komunikat o b��dzie
#endif

	// Kompilujemy shader znaleziony w pliku
	D3DX11CompileFromFile( file_name.c_str(), 0, 0, shader_name.c_str(), shader_model,
						   0, 0, 0, &compiled_shader,
#ifdef _DEBUG
						   &error_blob,	// Funkcja wsadzi informacje o b��dzie w to miejsce
#else
						   0,	// W trybie Release nie chcemy dostawa� b��d�w
#endif
						   &result );

	if ( FAILED( result ) )
	{
#ifdef _DEBUG
		if ( error_blob )
		{
			// B��d zostanie wypisany na standardowe wyj�cie
			OutputDebugStringA( (char*)error_blob->GetBufferPointer() );
			error_blob->Release();
		}
#endif
		layout = nullptr;
		return nullptr;
	}

	// Tworzymy obiekt shadera na podstawie tego co sie skompilowa�o
	result = device->CreateVertexShader( compiled_shader->GetBufferPointer(),
										 compiled_shader->GetBufferSize(),
										 nullptr, &vertex_shader );

	if ( FAILED( result ) )
	{
		compiled_shader->Release();
		layout = nullptr;
		return nullptr;
	}

	// Tworzymy layout
	result = device->CreateInputLayout( layout_desc, array_size, compiled_shader->GetBufferPointer(),
										compiled_shader->GetBufferSize(), layout );
	compiled_shader->Release();
	if ( FAILED( result ) )
	{
		layout = nullptr;
		return nullptr;
	}

	return vertex_shader;
}

/**@bref Tworzy pixel shader na podstawie pliku.

W przypadku b��d�w kompilacji w trybie debug s� one przekierowane do okna Output.

Na razie obs�uguje tylko nieskompilowane pliki.
@param[in] file_name Nazwa pliku, z kt�rego zostanie wczytany shader
@param[in] shader_name Nazwa funkcji, kt�ra jest punktem poczatkowym wykonania shadera
@param[in] shader_model �a�cuch znak�w opisuj�cy shader model.
*/
ID3D11PixelShader* DX11_interfaces_container::load_pixel_shader( const std::wstring& file_name, const std::string& shader_name, const char* shader_model = "ps_4_0" )
{
	if ( !device )
		return nullptr;

	HRESULT result;
	ID3DBlob* compiled_shader;
	ID3D11PixelShader* pixel_shader;
	// Troszk� zamieszania, ale w trybie debug warto wiedzie� co jest nie tak przy kompilacji shadera
#ifdef _DEBUG
	ID3D10Blob* error_blob = nullptr;	// Tu znajdzie si� komunikat o b��dzie
#endif

	// Kompilujemy shader znaleziony w pliku
	D3DX11CompileFromFile( file_name.c_str(), 0, 0, shader_name.c_str(), shader_model,
						   0, 0, 0, &compiled_shader,
#ifdef _DEBUG
						   &error_blob,	// Funkcja wsadzi informacje o b��dzie w to miejsce
#else
						   0,	// W trybie Release nie chcemy dostawa� b��d�w
#endif
						   &result );

	if ( FAILED( result ) )
	{
#ifdef _DEBUG
		if ( error_blob )
		{
			// B��d zostanie wypisany na standardowe wyj�cie
			OutputDebugStringA( (char*)error_blob->GetBufferPointer() );
			error_blob->Release();
		}
#endif
		return nullptr;
	}

	// Tworzymy obiekt shadera na podstawie tego co sie skompilowa�o
	result = device->CreatePixelShader( compiled_shader->GetBufferPointer(),
										compiled_shader->GetBufferSize(),
										nullptr, &pixel_shader );

	if ( FAILED( result ) )
	{
		compiled_shader->Release();
		return nullptr;
	}

	return pixel_shader;
}


/**@brief Funkcja tworzy shader na podstawie podanego pliku, kompiluje go, a potem obudowuje klas�
b�d�c� interfejsem shadera. Parametr shader_name oznacza nazw� funkcji, od kt�rej zaczyna
si� wykonanie kodu shadera.

Funkcja ustawia shader oraz layout wierzcho�k�w w kontek�cie urz�dzenia jako aktywny.
W trybie debug komunikaty kompilacji shadera s� przekazywane do okna output.

@param[in] file_name Nazwa pliku zawieraj�cego shader.
@param[in] shader_name Nazwa shadera.
@return Zwraca jedn� ze sta�ych, jak� zwraca DirectX.*/
DX11_INIT_RESULT DX11_interfaces_container::init_vertex_shader( const std::wstring& file_name, const std::string& shader_name )
{
	if ( !device )
		return DX11_DEVICE_NOT_INITIALIZED;
	if ( !device_context )
		return DX11_DEVICECONTEXT_NOT_INITIALIZED;
	
	default_vertex_shader = load_vertex_shader( file_name, shader_name, &default_vertex_layout, _vertex_layout_desc, _layout_elements_count, _vertex_shader_model.c_str() );
	if ( !default_vertex_shader )
		return COULD_NOT_CREATE_VERTEX_SHADER;

	device_context->VSSetShader( default_vertex_shader, nullptr, 0 );
	device_context->IASetInputLayout( default_vertex_layout );

	return DX11_INIT_OK;
}

/**@brief Funkcja tworzy shader na podstawie podanego pliku, kompiluje go, a potem obudowuje klas�
b�d�c� interfejsem shadera. Parametr shader_name oznacza nazw� funkcji, od kt�rej zaczyna
si� wykonanie kodu shadera.

Poza tworzeniem domy�lnego shadera, funkcja tworzy domy�lny sampler.
W trybie debug komunikaty kompilacji shadera s� przekazywane do okna output.

Funkcja ustawia ten shader i sampler w kontek�cie urz�dzenia jako aktywny.
@param[in] file_name Nazwa pliku zawieraj�cego shader.
@param[in] shader_name Nazwa shadera.
@return Zwraca jedn� ze sta�ych, jak� zwraca DirectX.*/
DX11_INIT_RESULT DX11_interfaces_container::init_pixel_shader( const std::wstring& file_name, const std::string& shader_name )
{
	if ( !device )
		return DX11_DEVICE_NOT_INITIALIZED;
	if ( !device_context )
		return DX11_DEVICECONTEXT_NOT_INITIALIZED;

	default_pixel_shader = load_pixel_shader( file_name, shader_name, _pixel_shader_model.c_str() );

	if ( !default_pixel_shader )
		return COULD_NOT_CREATE_PIXEL_SHADER;

	device_context->PSSetShader( default_pixel_shader, nullptr, 0 );
	return DX11_INIT_OK;
}

/**@brief Tworzy domy�lny smapler.
@return Zwraca jedn� ze sta�ych DX11_INIT_RESULT.*/
DX11_INIT_RESULT DX11_interfaces_container::init_sampler()
{
	HRESULT result = device->CreateSamplerState( &_sampler_desc, &default_sampler );

	if ( FAILED( result ) )
		return COULD_NOT_CREATE_SAMPLER;
	device_context->PSSetSamplers( 0, 1, &default_sampler );

	return DX11_INIT_OK;
}

//----------------------------------------------------------------------------------------------//
//								Funkcje pomocne przy wy�wietlaniu								//
//----------------------------------------------------------------------------------------------//


/**@brief Czy�cimy tylny bufor oraz z-bufor i wywo�ujemy funcj� inicjuj�c� renderowanie.

@attention Funkcja zak�ada, �e device_context, render target i depthstencil s� utworzone.
Poniewa� funkcja jest wywo�ywana w p�tli renderingu, to nie jest to sprawdzane, wi�c
programista jest odpowiedzialny za poprawne wywo�anie.*/
void DX11_interfaces_container::begin_scene( )
{
	//Bufor tylny
	float ClearColor[4] = { 0.0f, 0.0f, 0.0f, 1.0f };	// red, green, blue, alpha
	device_context->ClearRenderTargetView( render_target, ClearColor );

	//Z-bufor
	device_context->ClearDepthStencilView( z_buffer_view, D3D11_CLEAR_DEPTH, 1.0f, 0 );
}


//----------------------------------------------------------------------------------------------//
//								Funkcje dla klasy DX11_constant_buffers_container				//
//----------------------------------------------------------------------------------------------//

/**@brief Tworzy bufory sta�ych dla shadera.

Bufory musz� mie� rozmiar b�d�cy wielokrotno�ci� 16 bajt�w ze wzgl�du na rozmiar rejestr�w GPU.
Je�eli podane warto�ci takie nie s�, to s� rozszerzane do tej wielokrotno�ci, ale taka sytuacja mo�e to oznacza�,
�e jest jaki� b��d w programie.

@param[in] size_per_frame Rozmiar bufora const_per_frame.
@param[in] size_per_mesh Rozmiar bufora const_per_mesh.*/
void DX11_constant_buffers_container::init_buffers( unsigned int size_per_frame, unsigned int size_per_mesh )
{
	HRESULT result;

	// Bufory sta�ych musz� mie� rozmiar b�d�cy wielokrotno�ci� 16
	// Dobrze �e DirectX wypluwa jakie� debugowe informacje, bo nie wiem, jakbym na to wpad�
	if ( size_per_frame % 16 )
	{
		size_per_frame = size_per_frame >> 4;		// Dzielimy na 16 (dzielenie ca�kowite)
		size_per_frame = (size_per_frame + 1) << 4;	// Najbli�sza wielokrotno�� 16
	}

	if ( size_per_mesh % 16 )
	{
		size_per_mesh = size_per_mesh >> 4;			// Dzielimy na 16 (dzielenie ca�kowite)
		size_per_mesh = (size_per_mesh + 1) << 4;	// Najbli�sza wielokrotno�� 16
	}

	// Tworzymy bufor sta�ych w ka�dej ramce
	D3D11_BUFFER_DESC buffer_desc;
	ZeroMemory( &buffer_desc, sizeof(buffer_desc) );
	buffer_desc.Usage = D3D11_USAGE_DEFAULT;
	buffer_desc.ByteWidth = size_per_frame;
	buffer_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	buffer_desc.CPUAccessFlags = 0;

	//Tworzymy bufor sta�ych, kt�ry jest niezmienny podczas wy�wietlania ramki
	result = device->CreateBuffer( &buffer_desc, nullptr, &const_per_frame );

	//Drugi bufor r�ni si� tylko wielko�ci�
	buffer_desc.ByteWidth = size_per_mesh;

	//Tworzymy bufor sta�ych, do kt�rego b�dziemy wpisywa� warto�ci sta�e dla ka�dego mesha (albo cz�ci mesha)
	result = device->CreateBuffer( &buffer_desc, nullptr, &const_per_mesh );
}


void  DX11_constant_buffers_container::init_depth_states( )
{
	D3D11_DEPTH_STENCIL_DESC dsDesc;

	// Depth test parameters
	dsDesc.DepthEnable = true;
	dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	dsDesc.DepthFunc = D3D11_COMPARISON_LESS;

	// Stencil test parameters
	dsDesc.StencilEnable = false;
	dsDesc.StencilReadMask = 0xFF;
	dsDesc.StencilWriteMask = 0xFF;

	// Stencil operations if pixel is front-facing
	dsDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	dsDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	dsDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	dsDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	// Stencil operations if pixel is back-facing
	dsDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	dsDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	dsDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	dsDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	// Create depth stencil state
	device->CreateDepthStencilState( &dsDesc, &depth_enabled );

	device_context->OMSetDepthStencilState( depth_enabled, 1 );

	dsDesc.DepthEnable = false;
	device->CreateDepthStencilState( &dsDesc, &depth_disabled );
}

/**@brief Zwalania obiekty DirectXa. Funkcja wywo�uje t� sam� funkcj� z obiektu potomnego, �eby
zwolni� wszystkie obiekty, kt�re istniej�.*/
void DX11_constant_buffers_container::release_DirectX()
{
	if ( const_per_frame )
		const_per_frame->Release( ), const_per_frame = nullptr;
	if ( const_per_mesh )
		const_per_mesh->Release( ), const_per_mesh = nullptr;
	if ( depth_enabled )
		depth_enabled->Release();
	if ( depth_disabled )
		depth_disabled->Release();

	// Zwalniamy te� wszystkie obiekty, kt�re zwalnia�a klasa bazowa
	DX11_interfaces_container::release_DirectX();
}
