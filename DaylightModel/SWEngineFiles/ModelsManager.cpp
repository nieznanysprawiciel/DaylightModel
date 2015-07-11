#include "stdafx.h"
#include "ModelsManager.h"
//#include "..\Engine.h"
#include "loader_interface.h"
//#include "Loaders\FBX_files_loader\FBX_loader.h"
#include "ObjectDeleter.h"


//#include "..\..\memory_leaks.h"

//-------------------------------------------------------------------------------//
//							wersja DirectX11
//-------------------------------------------------------------------------------//

ModelsManager::ModelsManager( /*Engine* engine*/ )
//: engine( engine )
{
	Model3DFromFile::models_manager = this;

	//Loader plik�w FBX jest domy�lnym narz�dziem do wczytywania
	//FBX_loader* fbx_loader = new FBX_loader( this );
	//loader.push_back( fbx_loader );
}



ModelsManager::~ModelsManager( )
{
	// Kasujemy obiekty do wczytywania danych
	for ( unsigned int i = 0; i < loader.size(); ++i )
		delete loader[i];
}



#ifdef __TEST
void ModelsManager::test( )
{
	
	//stworzymy sobie prosty obiekt do wy�wietlania
	VertexNormalTexCord1 g_Vertices[] =
	{
	// front
	{ XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT3(0.0f, 0.0f, -1.0f), XMFLOAT2(0.0f, 3.0f) },
	{ XMFLOAT3(-1.0f, 1.0f, -1.0f), XMFLOAT3(0.0f, 0.0f, -1.0f), XMFLOAT2(0.0f, 0.0f) },
	{ XMFLOAT3(1.0f, -1.0f, -1.0f), XMFLOAT3(0.0f, 0.0f, -1.0f), XMFLOAT2(3.0f, 3.0f) },

	{ XMFLOAT3(-1.0f, 1.0f, -1.0f), XMFLOAT3(0.0f, 0.0f, -1.0f), XMFLOAT2(0.0f, 0.0f) },
	{ XMFLOAT3(1.0f, 1.0f, -1.0f), XMFLOAT3(0.0f, 0.0f, -1.0f), XMFLOAT2(3.0f, 0.0f) },
	{ XMFLOAT3( 1.0f, -1.0f, -1.0f ), XMFLOAT3( 0.0f, 0.0f, -1.0f ), XMFLOAT2( 3.0f, 3.0f ) },

	// right
	{ XMFLOAT3(1.0f, -1.0f, -1.0f), XMFLOAT3(1.0f, 0.0f, 0.0f), XMFLOAT2(0.0f, 1.0f) },
	{ XMFLOAT3(1.0f, 1.0f, -1.0f), XMFLOAT3(1.0f, 0.0f, 0.0f), XMFLOAT2(0.0f, 0.0f) },
	{ XMFLOAT3(1.0f, -1.0f, 1.0f), XMFLOAT3(1.0f, 0.0f, 0.0f), XMFLOAT2(1.0f, 1.0f) },

	{ XMFLOAT3(1.0f, 1.0f, -1.0f), XMFLOAT3(1.0f, 0.0f, 0.0f), XMFLOAT2(0.0f, 0.0f) },
	{ XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT3(1.0f, 0.0f, 0.0f), XMFLOAT2(1.0f, 0.0f) },
	{ XMFLOAT3( 1.0f, -1.0f, 1.0f ), XMFLOAT3( 1.0f, 0.0f, 0.0f ), XMFLOAT2( 1.0f, 1.0f ) },

	// back
	{ XMFLOAT3(1.0f, -1.0f, 1.0f), XMFLOAT3(0.0f, 0.0f, 1.0f), XMFLOAT2(0.0f, 1.0f) },
	{ XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT3(0.0f, 0.0f, 1.0f), XMFLOAT2(0.0f, 0.0f) },
	{ XMFLOAT3(-1.0f, -1.0f, 1.0f), XMFLOAT3(0.0f, 0.0f, 1.0f), XMFLOAT2(1.0f, 1.0f) },

	{ XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT3(0.0f, 0.0f, 1.0f), XMFLOAT2(0.0f, 0.0f) },
	{ XMFLOAT3(-1.0f, 1.0f, 1.0f), XMFLOAT3(0.0f, 0.0f, 1.0f), XMFLOAT2(1.0f, 0.0f) },
	{ XMFLOAT3( -1.0f, -1.0f, 1.0f ), XMFLOAT3( 0.0f, 0.0f, 1.0f ), XMFLOAT2( 1.0f, 1.0f ) },

	// left/ left
	{ XMFLOAT3(-1.0f, -1.0f, 1.0f), XMFLOAT3(-1.0f, 0.0f, 0.0f), XMFLOAT2(0.0f, 1.0f) },
	{ XMFLOAT3(-1.0f, 1.0f, 1.0f), XMFLOAT3(-1.0f, 0.0f, 0.0f), XMFLOAT2(0.0f, 0.0f) },
	{ XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT3(-1.0f, 0.0f, 0.0f), XMFLOAT2(1.0f, 1.0f) },

	{ XMFLOAT3(-1.0f, 1.0f, 1.0f), XMFLOAT3(-1.0f, 0.0f, 0.0f), XMFLOAT2(0.0f, 0.0f) },
	{ XMFLOAT3(-1.0f, 1.0f, -1.0f), XMFLOAT3(-1.0f, 0.0f, 0.0f), XMFLOAT2(1.0f, 0.0f) },
	{ XMFLOAT3( -1.0f, -1.0f, -1.0f ), XMFLOAT3( -1.0f, 0.0f, 0.0f ), XMFLOAT2( 1.0f, 1.0f ) },

	// top/ top
	{ XMFLOAT3(-1.0f, 1.0f, -1.0f), XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT2(0.0f, 1.0f) },
	{ XMFLOAT3(-1.0f, 1.0f, 1.0f), XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT2(0.0f, 0.0f) },
	{ XMFLOAT3(1.0f, 1.0f, -1.0f), XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT2(1.0f, 1.0f) },

	{ XMFLOAT3(-1.0f, 1.0f, 1.0f), XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT2(0.0f, 0.0f) },
	{ XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT2(1.0f, 0.0f) },
	{ XMFLOAT3( 1.0f, 1.0f, -1.0f ), XMFLOAT3( 0.0f, 1.0f, 0.0f ), XMFLOAT2( 1.0f, 1.0f ) },

	// bootom/ bootom
	{ XMFLOAT3(-1.0f, -1.0f, 1.0f), XMFLOAT3(0.0f, -1.0f, 0.0f), XMFLOAT2(0.0f, 1.0f) },
	{ XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT3(0.0f, -1.0f, 0.0f), XMFLOAT2(0.0f, 0.0f) },
	{ XMFLOAT3(1.0f, -1.0f, 1.0f), XMFLOAT3(0.0f, -1.0f, 0.0f), XMFLOAT2(1.0f, 1.0f) },

	{ XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT3(0.0f, -1.0f, 0.0f), XMFLOAT2(0.0f, 0.0f) },
	{ XMFLOAT3(1.0f, -1.0f, -1.0f), XMFLOAT3(0.0f, -1.0f, 0.0f), XMFLOAT2(1.0f, 0.0f) },
	{ XMFLOAT3( 1.0f, -1.0f, 1.0f ), XMFLOAT3( 0.0f, -1.0f, 0.0f ), XMFLOAT2( 1.0f, 1.0f ) },
	};

	
	MaterialObject material;

//#define mat (*material)
#define mat material

	// Set the RGBA for diffuse reflection.
	mat.Diffuse.x = 0.5f;
	mat.Diffuse.y = 0.5f;
	mat.Diffuse.z = 0.67f;
	mat.Diffuse.w = 1.0f;

	// Set the RGBA for ambient reflection.
	mat.Ambient.x = 0.3f;
	mat.Ambient.y = 0.5f;
	mat.Ambient.z = 0.3f;

	// Set the color and sharpness of specular highlights.
	mat.Specular.x = 1.0f;
	mat.Specular.y = 1.0f;
	mat.Specular.z = 1.0f;
	mat.Power = 50.0f;

	// Set the RGBA for emissive color.
	mat.Emissive.x = 0.0f;
	mat.Emissive.y = 0.0f;
	mat.Emissive.z = 0.0f;


	Model3DFromFile* new_model = new Model3DFromFile(L"skrzynia");
	new_model->BeginEdit();
	new_model->BeginPart();
	
	new_model->add_vertex_buffer( g_Vertices, 36 );
	new_model->add_material( &mat, L"::skrzynia_material" );
	//L"..\tylko_do_testow\tex.bmp"
	new_model->add_texture( L"..\tylko_do_testow\tex.bmp" );

	new_model->EndPart();
	new_model->EndEdit();
	/*
	new_model->vertex_buffer = BufferObject::create_from_memory( g_Vertices,
																 sizeof(VertexNormalTexCord1),
																 36,
																 D3D11_BIND_VERTEX_BUFFER );
	vertex_buffer.unsafe_add( L"skrzynia", new_model->vertex_buffer );


	ModelPart part;
	part.mesh = new MeshPartObject;
	part.mesh->vertices_count = 36;
	part.material = material;
	part.pixel_shader = pixel_shader.get( L"default_pixel_shader" );
	part.vertex_shader = vertex_shader.get( L"default_vertex_shader" );

	new_model->model_parts.push_back(part);*/



	file_model.unsafe_add( L"skrzynia", new_model );


	load_model_from_file( L"tylko_do_testow/ARC.FBX" );
	load_model_from_file( L"tylko_do_testow/moon/moon.FBX" );
	load_model_from_file( L"tylko_do_testow/Nebulon/Nebulon.FBX" );
	load_model_from_file( L"tylko_do_testow/VadersTIE.FBX" );
	load_model_from_file( L"tylko_do_testow/TIE_Fighter/TIE_Fighter.FBX" );
}
#endif






/**@brief Dopasowuje najlepszy z domy�lnych shader�w, kt�ry pasuje do podanej tablicy
tekstur. Tablica ma tyle element�w ile zmienna @ref ENGINE_MAX_TEXTURES.

Ka�da pozycja w tablicy ma przypisane domy�lne znaczenie zgodnie z enumeracj� 
@ref TEXTURES_TYPES. Najlepszy shader jest wybierany na podstawie obecno�ci
lub nieobecno�ci tekstury w tablicy.

@param[in] textures Tablica tekstur z obiektu ModelPart.
@return Zwraca obiekt vertex shadera.*/
VertexShaderObject* ModelsManager::find_best_vertex_shader( TextureObject** textures )
{
	// Na razie nie mamy innych domy�lnych shader�w
	return vertex_shader.get( DEFAULT_VERTEX_SHADER_STRING );
}


/**@brief Dopasowuje najlepszy z domy�lnych shader�w, kt�ry pasuje do podanej tablicy
tekstur. Tablica ma tyle element�w ile zmienna @ref ENGINE_MAX_TEXTURES.

Ka�da pozycja w tablicy ma przypisane domy�lne znaczenie zgodnie z enumeracj�
@ref TEXTURES_TYPES. Najlepszy shader jest wybierany na podstawie obecno�ci
lub nieobecno�ci tekstury w tablicy.

@param[in] textures Tablica tekstur z obiektu ModelPart.
@return Zwraca obiekt pixel shadera.*/
PixelShaderObject* ModelsManager::find_best_pixel_shader( TextureObject** textures )
{
	PixelShaderObject* return_shader = nullptr;

	// Na razie nie ma innych tekstur ni� diffuse, wi�c algorytm nie jest skomplikowany
	if ( textures[TEXTURES_TYPES::TEX_DIFFUSE] )
		return_shader = pixel_shader.get( DEFAULT_TEX_DIFFUSE_PIXEL_SHADER_PATH );
	
	
	if ( !return_shader )	// Je�eli nadal jest nullptrem to dajemy mu domy�lny shader
		return_shader = pixel_shader.get( DEFAULT_PIXEL_SHADER_STRING );

	return return_shader;
}


/** @brief Znajduje Loader pasuj�cy do pliku podanego w parametrze.
@param[in] path �cie�ka do pliku, dla kt�rej szukamy loadera.
@return Wska�nik na odpowiedni loader lub nullptr, je�eli nie znaleziono pasuj�cego.*/
Loader* ModelsManager::find_loader( const std::wstring& path )
{
	for ( unsigned int i = 0; i < loader.size( ); ++i )
	if ( loader[i]->can_load( path ) )
		return loader[i];
	return nullptr;
}


/** @brief Tworzy i wstawia domy�lne warto�ci asset�w do swoich tablic.

Wstawiane s� g��wnie vertex i pixel shader oraz domy�lny materia�.
W tej funkcji mo�na dopisa� tworzenie wszystkich pozosta�ych domy�lnych obiekt�w.
@param[in] vert_shader Stworzony ju� obiekt vertex shadera.
@param[in] pix_shader Stworzony ju� obiekt pixel shadera.
*/
void ModelsManager::set_default_assets( ID3D11VertexShader* vert_shader, ID3D11PixelShader* pix_shader )
{
	// Podane w parametrach shadery nie maj� jeszcze swojego obiektu-opakowania, wi�c trzeba go stworzy�
	VertexShaderObject* new_vertex_shader = new VertexShaderObject( vert_shader );
	PixelShaderObject* new_pixel_shader = new PixelShaderObject( pix_shader );

	// Dodajemy shadery. Takich nazwa na pewno nie ma w tablicach i nie b�dzie
	vertex_shader.unsafe_add( DEFAULT_VERTEX_SHADER_STRING, new_vertex_shader );
	pixel_shader.unsafe_add( DEFAULT_PIXEL_SHADER_STRING, new_pixel_shader );

	// Tworzymy defaultowy materai�
	MaterialObject* new_material = new MaterialObject();
	new_material->set_null_material();
	material.unsafe_add( DEFAULT_MATERIAL_STRING, new_material );

	// Teraz tworzymy shadery, kt�rych jeszcze nie skompilowali�my wcze�niej
	// Dla tekstury diffuse vertex shader jest taki sam, wi�c nie ma po co go kompilowa� jeszcze raz
	new_pixel_shader = PixelShaderObject::create_from_file( DEFAULT_TEX_DIFFUSE_PIXEL_SHADER_PATH, DEFAULT_PIXEL_SHADER_ENTRY );
	pixel_shader.unsafe_add( DEFAULT_TEX_DIFFUSE_PIXEL_SHADER_PATH, new_pixel_shader );

}

//-------------------------------------------------------------------------------//
//							funkcje do zarzadzania assetami
//-------------------------------------------------------------------------------//



/**@brief Wczytuje model z podanego pliku.
@param[in] file Plik do wczytania
@return Jedna z warto�ci @ref MODELS_MANAGER_RESULT. Funkcja mo�e zwr�ci� @ref MODELS_MANAGER_RESULT::MODELS_MANAGER_OK,
@ref MODELS_MANAGER_RESULT::MODELS_MANAGER_LOADER_NOT_FOUND lub @ref MODELS_MANAGER_RESULT::MODELS_MANAGER_CANNOT_LOAD.*/
MODELS_MANAGER_RESULT ModelsManager::load_model_from_file( const std::wstring& file )
{
	// Sprawdzamy czy plik nie zosta� ju� wczytany
	Model3DFromFile* new_model = file_model.get( file );
	if ( new_model != nullptr )
		return MODELS_MANAGER_RESULT::MODELS_MANAGER_OK;	// Udajemy, �e wszystko posz�o dobrze

	// Sprawdzamy, kt�ry loader potrafi otworzy� plik
	Loader* loader = find_loader( file );
	if ( loader == nullptr )
		return MODELS_MANAGER_RESULT::MODELS_MANAGER_LOADER_NOT_FOUND;		// �aden nie potrafi

	// Tworzymy obiekt Model3DFromFile, do kt�rego loader wpisze zawarto�� pliku
	new_model = new Model3DFromFile( file );

	// Wszystkie operacje wpisywania musz� by� zamkni�te tymi wywo�aniami
	new_model->BeginEdit();
	LOADER_RESULT result = loader->load_mesh( new_model, file );
	new_model->EndEdit();

	if ( result != LOADER_RESULT::MESH_LOADING_OK )
	{	// load_mesh powinno zwr�ci� 0
		// Destruktor jest prywatny, wi�c nie mo�emy kasowa� obiektu bezpo�rednio.
		ObjectDeleterKey<Model3DFromFile> key;					// Tworzymy klucz.
		ObjectDeleter<Model3DFromFile> model_deleter( key );	// Tworzymy obiekt kasuj�cy i podajemy mu nasz klucz.
		model_deleter.delete_object( new_model );				// Kasujemy obiekt za po�rednictwem klucza.
		return MODELS_MANAGER_RESULT::MODELS_MANAGER_CANNOT_LOAD;
	}

	// Dodajemy model do tablic
	file_model.unsafe_add( file, new_model );

	return MODELS_MANAGER_RESULT::MODELS_MANAGER_OK;
}



/**@brief Dodaje materia� do ModelsManagera, je�eli jeszcze nie istnia�.
@note Funkcja nie dodaje odwo�ania do obiektu, bo nie zak�ada, �e kto� go od razu u�yje.
W ka�dym miejscu, gdzie zostanie przypisany zwr�cony obiekt, nale�y pami�ta� o dodaniu odwo�ania oraz
skasowaniu go, gdy obiekt przestanie by� u�ywany.

@param[in] material Materia�, kt�ry ma zosta� dodany
@param[in] material_name Nazwa materia�u. Do materia�u b�dzie mo�na si� odwo�a� podaj�c ci�g znak�w
[nazwa_pliku]::[nazwa_materia�u]. Oznacza to, �e mog� istnie� dwa takie same materia�y, poniewa� nie jest sprawdzana
zawarto��, a jedynie nazwy.
@return Zwraca wska�nik na dodany materia�.*/
MaterialObject* ModelsManager::add_material( const MaterialObject* add_material, const std::wstring& material_name )
{
	MaterialObject* new_material = material.get( material_name );
	if ( !new_material )
	{
		// Nie by�o materia�u, trzeba j� stworzy� i doda�
		new_material = new MaterialObject( add_material );

		material.unsafe_add( material_name, new_material );	// Dodali�my tekstur�
	}

	return new_material;
}


/**@brief Dodaje vertex shader do ModelsManagera. Je�eli obiekt ju� istnia�, to nie jest tworzony nowy.
@note Funkcja nie dodaje odwo�ania do obiektu, bo nie zak�ada, �e kto� go od razu u�yje.
W ka�dym miejscu, gdzie zostanie przypisany zwr�cony obiekt, nale�y pami�ta� o dodaniu odwo�ania oraz
skasowaniu go, gdy obiekt przestanie by� u�ywany.

@param[in] file_name Nazwa pliku, w kt�rym znajduje si� vertex shader.
@param[in] shader_entry Nazwa funkcji od kt�rej ma si� zacz�� wykonywanie shadera.
@return Zwraca obiekt dodanego shadera. Zwraca nullptr, je�eli shadera nie uda�o si� skompilowa�.*/
VertexShaderObject* ModelsManager::add_vertex_shader( const std::wstring& file_name, const std::string& shader_entry )
{
	VertexShaderObject* shader = vertex_shader.get( file_name );
	if ( !shader )
	{
		// Nie by�o shadera, trzeba go stworzy� i doda�
		shader = VertexShaderObject::create_from_file( file_name, shader_entry );
		if ( !shader )		// shader m�g� mie� z�y format, a nie chcemy dodawa� nullptra do ModelsManagera
			return nullptr;

		vertex_shader.unsafe_add( file_name, shader );	// Dodali�my tekstur�
	}

	return shader;
}

/**@brief Dodaje vertex shader do ModelsManagera. Je�eli obiekt ju� istnia�, to nie jest tworzony nowy.
Tworzy te� layout wierzcho�ka zwi�zany z tym shaderem i zwraca go w zmiennej layout. Layout nie jest
dodawany do ModelsManagera ( bo i nie ma gdzie go doda� ).

Je�eli vertex shader wcze�niej istnia�, to stworzenie layoutu wymaga ponownego skompilowania shadera. Shader taki jest potem
kasowany i nie zostaje zdublowany w ModelsManagerze, ale niepotrzebna praca zostaje w�o�ona. Jest wi�c zadaniem programisty, �eby
do takich rzeczy dochodzi�o jak najrzadziej.

@note Funkcja nie dodaje odwo�ania do obiektu, bo nie zak�ada, �e kto� go od razu u�yje.
W ka�dym miejscu, gdzie zostanie przypisany zwr�cony obiekt, nale�y pami�ta� o dodaniu odwo�ania oraz
skasowaniu go, gdy obiekt przestanie by� u�ywany.

@param[in] file_name Nazwa pliku, w kt�rym znajduje si� vertex shader.
@param[in] shader_entry Nazwa funkcji od kt�rej ma si� zacz�� wykonywanie shadera.
@param[out] layout W zmiennej umieszczany jest wska�nik na layout wierzcho�ka. Nale�y pami�ta� o zwolnieniu go kiedy b�dzie niepotrzebny.
@attention Je�eli vertex shader wcze�niej istnia�, to stworzenie layoutu wymaga ponownego skompilowania shadera. Shader taki jest potem 
kasowany i nie zostaje zdublowany w ModelsManagerze, ale niepotrzebna praca zostaje w�o�ona. Jest wi�c zadaniem programisty, �eby
do takich rzeczy dochodzi�o jak najrzadziej.
@param[in] layout_desc Deskryptor opisujacy tworzony layout.
@param[in] array_size Liczba element�w tablicy layout_desc.
@return Zwraca obiekt dodanego shadera. Zwraca nullptr, je�eli shadera nie uda�o si� skompilowa�.*/
VertexShaderObject* ModelsManager::add_vertex_shader( const std::wstring& file_name,
									   const std::string& shader_entry,
									   ID3D11InputLayout** layout,
									   D3D11_INPUT_ELEMENT_DESC* layout_desc,
									   unsigned int array_size )
{
	*layout = nullptr;
	VertexShaderObject* shader = vertex_shader.get( file_name );
	
	// Tworzymy shader bo i tak usimy, potem go najwy�ej skasujemy
	VertexShaderObject* new_shader = VertexShaderObject::create_from_file( file_name, shader_entry, layout, layout_desc, array_size );
	if ( !new_shader )		// shader m�g� mie� z�y format, a nie chcemy dodawa� nullptra do ModelsManagera
		return nullptr;
	
	if ( !shader )
	{
		// Nie by�o shadera, trzeba go doda�
		vertex_shader.unsafe_add( file_name, new_shader );	// Dodali�my tekstur�
		shader = new_shader;
	}
	else
	{	// Shader ju� by�, wi�c kasujemy nowy
		// Destruktor jest prywatny, wi�c nie mo�emy kasowa� obiektu bezpo�rednio.
		ObjectDeleterKey<VertexShaderObject> key;					// Tworzymy klucz.
		ObjectDeleter<VertexShaderObject> model_deleter( key );	// Tworzymy obiekt kasuj�cy i podajemy mu nasz klucz.
		model_deleter.delete_object( new_shader );				// Kasujemy obiekt za po�rednictwem klucza.
	}

	return shader;
}


/**@brief Dodaje pixel shader do ModelsManagera. Je�eli obiekt ju� istnia�, to nie jest tworzony nowy.
@note Funkcja nie dodaje odwo�ania do obiektu, bo nie zak�ada, �e kto� go od razu u�yje.
W ka�dym miejscu, gdzie zostanie przypisany zwr�cony obiekt, nale�y pami�ta� o dodaniu odwo�ania oraz
skasowaniu go, gdy obiekt przestanie by� u�ywany.

@param[in] file_name Nazwa pliku, w kt�rym znajduje si� pixel shader.
@param[in] shader_entry Nazwa funkcji od kt�rej ma si� zacz�� wykonywanie shadera.
@return Zwraca obiekt dodanego shadera. Zwraca nullptr, je�eli shadera nie uda�o si� skompilowa�.*/
PixelShaderObject* ModelsManager::add_pixel_shader( const std::wstring& file_name, const std::string& shader_entry )
{
	PixelShaderObject* shader = pixel_shader.get( file_name );
	if ( !shader )
	{
		// Nie by�o shadera, trzeba go stworzy� i doda�
		shader = PixelShaderObject::create_from_file( file_name, shader_entry );
		if ( !shader )		// shader m�g� mie� z�y format, a nie chcemy dodawa� nullptra do ModelsManagera
			return nullptr;

		pixel_shader.unsafe_add( file_name, shader );	// Dodali�my tekstur�
	}

	return shader;
}

/**@brief Dodaje tekstur� do ModelManagera, je�eli jeszcze nie istnia�a.
@note Funkcja nie dodaje odwo�ania do obiektu, bo nie zak�ada, �e kto� go od razu u�yje.
W ka�dym miejscu, gdzie zostanie przypisany zwr�cony obiekt, nale�y pami�ta� o dodaniu odwo�ania oraz
skasowaniu go, gdy obiekt przestanie by� u�ywany.

@param[in] file_name �cie�ka do tekstury

@return Zwraca wska�nik na dodan� tekstur� lub nullptr, je�eli nie da�o si� wczyta�.*/
TextureObject* ModelsManager::add_texture( const std::wstring& file_name )
{

	TextureObject* tex = texture.get( file_name );
	if ( !tex )
	{
		// Nie by�o tekstury, trzeba j� stworzy� i doda�
		tex = TextureObject::create_from_file( file_name );
		if ( !tex )		// Tekstura mog�a mie� z�y format, a nie chcemy dodawa� nullptra do ModelsManagera
			return nullptr;

		texture.unsafe_add( file_name, tex );	// Dodali�my tekstur�
	}

	return tex;
}

/**@brief Dodaje do ModelsManagera bufor wierzcho�k�w.
Je�eli pod tak� nazw� istnieje jaki� bufor, to zostanie zwr�cony wska�nik na niego.
@note Funkcja nie dodaje odwo�ania do obiektu, bo nie zak�ada, �e kto� go od razu u�yje.
W ka�dym miejscu, gdzie zostanie przypisany zwr�cony obiekt, nale�y pami�ta� o dodaniu odwo�ania oraz
skasowaniu go, gdy obiekt przestanie by� u�ywany.

@param[in] name Nazwa bufora, po kt�rej mo�na si� b�dzie odwo�a�.
@param[in] buffer Wska�nik na bufor z danym, kt�re maj� by� przeniesione do bufora DirectXowego.
@param[in] element_size Rozmiar pojedynczego elementu w buforze.
@param[in] vert_count Liczba wierzcho�k�w/indeks�w w buforze.
@return Dodany bufor wierzcho�k�w. Zwraca nullptr, je�eli nie uda�o si� stworzy� bufora.*/
BufferObject* ModelsManager::add_vertex_buffer( const std::wstring& name,
												const void* buffer,
												unsigned int element_size,
												unsigned int vert_count )
{
	BufferObject* vertex_buff = vertex_buffer.get( name );
	if ( vertex_buff )	// Je�eli znale�li�my bufor, to go zwracamy
		return vertex_buff;

	// Tworzymy obiekt bufora indeks�w i go zapisujemy
	vertex_buff = BufferObject::create_from_memory( buffer,
													element_size,
													vert_count,
													D3D11_BIND_VERTEX_BUFFER );
	if ( !vertex_buff )		// Bufor m�g� si� nie stworzy�, a nie chcemy dodawa� nullptra do ModelsManagera
		return nullptr;

	vertex_buffer.unsafe_add( name, vertex_buff );	// Dodali�my bufor

	return vertex_buff;
}

/**@brief Dodaje do ModelsManagera bufor indeks�w.
Je�eli pod tak� nazw� istnieje jaki� bufor, to zostanie zwr�cony wska�nik na niego.
@note Funkcja nie dodaje odwo�ania do obiektu, bo nie zak�ada, �e kto� go od razu u�yje.
W ka�dym miejscu, gdzie zostanie przypisany zwr�cony obiekt, nale�y pami�ta� o dodaniu odwo�ania oraz
skasowaniu go, gdy obiekt przestanie by� u�ywany.

@param[in] name Nazwa bufora, po kt�rej mo�na si� b�dzie odwo�a�.
@param[in] buffer Wska�nik na bufor z danym, kt�re maj� by� przeniesione do bufora DirectXowego.
@param[in] element_size Rozmiar pojedynczego elementu w buforze.
@param[in] vert_count Liczba wierzcho�k�w/indeks�w w buforze.
@return Dodany bufor indeks�w. Zwraca nullptr, je�eli nie uda�o si� stworzy� bufora.*/
BufferObject* ModelsManager::add_index_buffer( const std::wstring& name,
											   const void* buffer,
											   unsigned int element_size,
											   unsigned int vert_count )
{
	BufferObject* index_buff = index_buffer.get( name );
	if ( index_buff )	// Je�eli znale�li�my bufor, to go zwracamy
		return index_buff;

	// Tworzymy obiekt bufora indeks�w i go zapisujemy
	index_buff = BufferObject::create_from_memory( buffer,
												   element_size,
												   vert_count,
												   D3D11_BIND_INDEX_BUFFER );
	if ( !index_buff )		// Bufor m�g� si� nie stworzy�, a nie chcemy dodawa� nullptra do ModelsManagera
		return nullptr;

	index_buffer.unsafe_add( name, index_buff );	// Dodali�my bufor

	return index_buff;
}

