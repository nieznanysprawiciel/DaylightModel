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

	//Loader plików FBX jest domyœlnym narzêdziem do wczytywania
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
	
	//stworzymy sobie prosty obiekt do wyœwietlania
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






/**@brief Dopasowuje najlepszy z domyœlnych shaderów, który pasuje do podanej tablicy
tekstur. Tablica ma tyle elementów ile zmienna @ref ENGINE_MAX_TEXTURES.

Ka¿da pozycja w tablicy ma przypisane domyœlne znaczenie zgodnie z enumeracj¹ 
@ref TEXTURES_TYPES. Najlepszy shader jest wybierany na podstawie obecnoœci
lub nieobecnoœci tekstury w tablicy.

@param[in] textures Tablica tekstur z obiektu ModelPart.
@return Zwraca obiekt vertex shadera.*/
VertexShaderObject* ModelsManager::find_best_vertex_shader( TextureObject** textures )
{
	// Na razie nie mamy innych domyœlnych shaderów
	return vertex_shader.get( DEFAULT_VERTEX_SHADER_STRING );
}


/**@brief Dopasowuje najlepszy z domyœlnych shaderów, który pasuje do podanej tablicy
tekstur. Tablica ma tyle elementów ile zmienna @ref ENGINE_MAX_TEXTURES.

Ka¿da pozycja w tablicy ma przypisane domyœlne znaczenie zgodnie z enumeracj¹
@ref TEXTURES_TYPES. Najlepszy shader jest wybierany na podstawie obecnoœci
lub nieobecnoœci tekstury w tablicy.

@param[in] textures Tablica tekstur z obiektu ModelPart.
@return Zwraca obiekt pixel shadera.*/
PixelShaderObject* ModelsManager::find_best_pixel_shader( TextureObject** textures )
{
	PixelShaderObject* return_shader = nullptr;

	// Na razie nie ma innych tekstur ni¿ diffuse, wiêc algorytm nie jest skomplikowany
	if ( textures[TEXTURES_TYPES::TEX_DIFFUSE] )
		return_shader = pixel_shader.get( DEFAULT_TEX_DIFFUSE_PIXEL_SHADER_PATH );
	
	
	if ( !return_shader )	// Je¿eli nadal jest nullptrem to dajemy mu domyœlny shader
		return_shader = pixel_shader.get( DEFAULT_PIXEL_SHADER_STRING );

	return return_shader;
}


/** @brief Znajduje Loader pasuj¹cy do pliku podanego w parametrze.
@param[in] path Œcie¿ka do pliku, dla której szukamy loadera.
@return WskaŸnik na odpowiedni loader lub nullptr, je¿eli nie znaleziono pasuj¹cego.*/
Loader* ModelsManager::find_loader( const std::wstring& path )
{
	for ( unsigned int i = 0; i < loader.size( ); ++i )
	if ( loader[i]->can_load( path ) )
		return loader[i];
	return nullptr;
}


/** @brief Tworzy i wstawia domyœlne wartoœci assetów do swoich tablic.

Wstawiane s¹ g³ównie vertex i pixel shader oraz domyœlny materia³.
W tej funkcji mo¿na dopisaæ tworzenie wszystkich pozosta³ych domyœlnych obiektów.
@param[in] vert_shader Stworzony ju¿ obiekt vertex shadera.
@param[in] pix_shader Stworzony ju¿ obiekt pixel shadera.
*/
void ModelsManager::set_default_assets( ID3D11VertexShader* vert_shader, ID3D11PixelShader* pix_shader )
{
	// Podane w parametrach shadery nie maj¹ jeszcze swojego obiektu-opakowania, wiêc trzeba go stworzyæ
	VertexShaderObject* new_vertex_shader = new VertexShaderObject( vert_shader );
	PixelShaderObject* new_pixel_shader = new PixelShaderObject( pix_shader );

	// Dodajemy shadery. Takich nazwa na pewno nie ma w tablicach i nie bêdzie
	vertex_shader.unsafe_add( DEFAULT_VERTEX_SHADER_STRING, new_vertex_shader );
	pixel_shader.unsafe_add( DEFAULT_PIXEL_SHADER_STRING, new_pixel_shader );

	// Tworzymy defaultowy materai³
	MaterialObject* new_material = new MaterialObject();
	new_material->set_null_material();
	material.unsafe_add( DEFAULT_MATERIAL_STRING, new_material );

	// Teraz tworzymy shadery, których jeszcze nie skompilowaliœmy wczeœniej
	// Dla tekstury diffuse vertex shader jest taki sam, wiêc nie ma po co go kompilowaæ jeszcze raz
	new_pixel_shader = PixelShaderObject::create_from_file( DEFAULT_TEX_DIFFUSE_PIXEL_SHADER_PATH, DEFAULT_PIXEL_SHADER_ENTRY );
	pixel_shader.unsafe_add( DEFAULT_TEX_DIFFUSE_PIXEL_SHADER_PATH, new_pixel_shader );

}

//-------------------------------------------------------------------------------//
//							funkcje do zarzadzania assetami
//-------------------------------------------------------------------------------//



/**@brief Wczytuje model z podanego pliku.
@param[in] file Plik do wczytania
@return Jedna z wartoœci @ref MODELS_MANAGER_RESULT. Funkcja mo¿e zwróciæ @ref MODELS_MANAGER_RESULT::MODELS_MANAGER_OK,
@ref MODELS_MANAGER_RESULT::MODELS_MANAGER_LOADER_NOT_FOUND lub @ref MODELS_MANAGER_RESULT::MODELS_MANAGER_CANNOT_LOAD.*/
MODELS_MANAGER_RESULT ModelsManager::load_model_from_file( const std::wstring& file )
{
	// Sprawdzamy czy plik nie zosta³ ju¿ wczytany
	Model3DFromFile* new_model = file_model.get( file );
	if ( new_model != nullptr )
		return MODELS_MANAGER_RESULT::MODELS_MANAGER_OK;	// Udajemy, ¿e wszystko posz³o dobrze

	// Sprawdzamy, który loader potrafi otworzyæ plik
	Loader* loader = find_loader( file );
	if ( loader == nullptr )
		return MODELS_MANAGER_RESULT::MODELS_MANAGER_LOADER_NOT_FOUND;		// ¯aden nie potrafi

	// Tworzymy obiekt Model3DFromFile, do którego loader wpisze zawartoœæ pliku
	new_model = new Model3DFromFile( file );

	// Wszystkie operacje wpisywania musz¹ byæ zamkniête tymi wywo³aniami
	new_model->BeginEdit();
	LOADER_RESULT result = loader->load_mesh( new_model, file );
	new_model->EndEdit();

	if ( result != LOADER_RESULT::MESH_LOADING_OK )
	{	// load_mesh powinno zwróciæ 0
		// Destruktor jest prywatny, wiêc nie mo¿emy kasowaæ obiektu bezpoœrednio.
		ObjectDeleterKey<Model3DFromFile> key;					// Tworzymy klucz.
		ObjectDeleter<Model3DFromFile> model_deleter( key );	// Tworzymy obiekt kasuj¹cy i podajemy mu nasz klucz.
		model_deleter.delete_object( new_model );				// Kasujemy obiekt za poœrednictwem klucza.
		return MODELS_MANAGER_RESULT::MODELS_MANAGER_CANNOT_LOAD;
	}

	// Dodajemy model do tablic
	file_model.unsafe_add( file, new_model );

	return MODELS_MANAGER_RESULT::MODELS_MANAGER_OK;
}



/**@brief Dodaje materia³ do ModelsManagera, je¿eli jeszcze nie istnia³.
@note Funkcja nie dodaje odwo³ania do obiektu, bo nie zak³ada, ¿e ktoœ go od razu u¿yje.
W ka¿dym miejscu, gdzie zostanie przypisany zwrócony obiekt, nale¿y pamiêtaæ o dodaniu odwo³ania oraz
skasowaniu go, gdy obiekt przestanie byæ u¿ywany.

@param[in] material Materia³, który ma zostaæ dodany
@param[in] material_name Nazwa materia³u. Do materia³u bêdzie mo¿na siê odwo³aæ podaj¹c ci¹g znaków
[nazwa_pliku]::[nazwa_materia³u]. Oznacza to, ¿e mog¹ istnieæ dwa takie same materia³y, poniewa¿ nie jest sprawdzana
zawartoœæ, a jedynie nazwy.
@return Zwraca wskaŸnik na dodany materia³.*/
MaterialObject* ModelsManager::add_material( const MaterialObject* add_material, const std::wstring& material_name )
{
	MaterialObject* new_material = material.get( material_name );
	if ( !new_material )
	{
		// Nie by³o materia³u, trzeba j¹ stworzyæ i dodaæ
		new_material = new MaterialObject( add_material );

		material.unsafe_add( material_name, new_material );	// Dodaliœmy teksturê
	}

	return new_material;
}


/**@brief Dodaje vertex shader do ModelsManagera. Je¿eli obiekt ju¿ istnia³, to nie jest tworzony nowy.
@note Funkcja nie dodaje odwo³ania do obiektu, bo nie zak³ada, ¿e ktoœ go od razu u¿yje.
W ka¿dym miejscu, gdzie zostanie przypisany zwrócony obiekt, nale¿y pamiêtaæ o dodaniu odwo³ania oraz
skasowaniu go, gdy obiekt przestanie byæ u¿ywany.

@param[in] file_name Nazwa pliku, w którym znajduje siê vertex shader.
@param[in] shader_entry Nazwa funkcji od której ma siê zacz¹æ wykonywanie shadera.
@return Zwraca obiekt dodanego shadera. Zwraca nullptr, je¿eli shadera nie uda³o siê skompilowaæ.*/
VertexShaderObject* ModelsManager::add_vertex_shader( const std::wstring& file_name, const std::string& shader_entry )
{
	VertexShaderObject* shader = vertex_shader.get( file_name );
	if ( !shader )
	{
		// Nie by³o shadera, trzeba go stworzyæ i dodaæ
		shader = VertexShaderObject::create_from_file( file_name, shader_entry );
		if ( !shader )		// shader móg³ mieæ z³y format, a nie chcemy dodawaæ nullptra do ModelsManagera
			return nullptr;

		vertex_shader.unsafe_add( file_name, shader );	// Dodaliœmy teksturê
	}

	return shader;
}

/**@brief Dodaje vertex shader do ModelsManagera. Je¿eli obiekt ju¿ istnia³, to nie jest tworzony nowy.
Tworzy te¿ layout wierzcho³ka zwi¹zany z tym shaderem i zwraca go w zmiennej layout. Layout nie jest
dodawany do ModelsManagera ( bo i nie ma gdzie go dodaæ ).

Je¿eli vertex shader wczeœniej istnia³, to stworzenie layoutu wymaga ponownego skompilowania shadera. Shader taki jest potem
kasowany i nie zostaje zdublowany w ModelsManagerze, ale niepotrzebna praca zostaje w³o¿ona. Jest wiêc zadaniem programisty, ¿eby
do takich rzeczy dochodzi³o jak najrzadziej.

@note Funkcja nie dodaje odwo³ania do obiektu, bo nie zak³ada, ¿e ktoœ go od razu u¿yje.
W ka¿dym miejscu, gdzie zostanie przypisany zwrócony obiekt, nale¿y pamiêtaæ o dodaniu odwo³ania oraz
skasowaniu go, gdy obiekt przestanie byæ u¿ywany.

@param[in] file_name Nazwa pliku, w którym znajduje siê vertex shader.
@param[in] shader_entry Nazwa funkcji od której ma siê zacz¹æ wykonywanie shadera.
@param[out] layout W zmiennej umieszczany jest wskaŸnik na layout wierzcho³ka. Nale¿y pamiêtaæ o zwolnieniu go kiedy bêdzie niepotrzebny.
@attention Je¿eli vertex shader wczeœniej istnia³, to stworzenie layoutu wymaga ponownego skompilowania shadera. Shader taki jest potem 
kasowany i nie zostaje zdublowany w ModelsManagerze, ale niepotrzebna praca zostaje w³o¿ona. Jest wiêc zadaniem programisty, ¿eby
do takich rzeczy dochodzi³o jak najrzadziej.
@param[in] layout_desc Deskryptor opisujacy tworzony layout.
@param[in] array_size Liczba elementów tablicy layout_desc.
@return Zwraca obiekt dodanego shadera. Zwraca nullptr, je¿eli shadera nie uda³o siê skompilowaæ.*/
VertexShaderObject* ModelsManager::add_vertex_shader( const std::wstring& file_name,
									   const std::string& shader_entry,
									   ID3D11InputLayout** layout,
									   D3D11_INPUT_ELEMENT_DESC* layout_desc,
									   unsigned int array_size )
{
	*layout = nullptr;
	VertexShaderObject* shader = vertex_shader.get( file_name );
	
	// Tworzymy shader bo i tak usimy, potem go najwy¿ej skasujemy
	VertexShaderObject* new_shader = VertexShaderObject::create_from_file( file_name, shader_entry, layout, layout_desc, array_size );
	if ( !new_shader )		// shader móg³ mieæ z³y format, a nie chcemy dodawaæ nullptra do ModelsManagera
		return nullptr;
	
	if ( !shader )
	{
		// Nie by³o shadera, trzeba go dodaæ
		vertex_shader.unsafe_add( file_name, new_shader );	// Dodaliœmy teksturê
		shader = new_shader;
	}
	else
	{	// Shader ju¿ by³, wiêc kasujemy nowy
		// Destruktor jest prywatny, wiêc nie mo¿emy kasowaæ obiektu bezpoœrednio.
		ObjectDeleterKey<VertexShaderObject> key;					// Tworzymy klucz.
		ObjectDeleter<VertexShaderObject> model_deleter( key );	// Tworzymy obiekt kasuj¹cy i podajemy mu nasz klucz.
		model_deleter.delete_object( new_shader );				// Kasujemy obiekt za poœrednictwem klucza.
	}

	return shader;
}


/**@brief Dodaje pixel shader do ModelsManagera. Je¿eli obiekt ju¿ istnia³, to nie jest tworzony nowy.
@note Funkcja nie dodaje odwo³ania do obiektu, bo nie zak³ada, ¿e ktoœ go od razu u¿yje.
W ka¿dym miejscu, gdzie zostanie przypisany zwrócony obiekt, nale¿y pamiêtaæ o dodaniu odwo³ania oraz
skasowaniu go, gdy obiekt przestanie byæ u¿ywany.

@param[in] file_name Nazwa pliku, w którym znajduje siê pixel shader.
@param[in] shader_entry Nazwa funkcji od której ma siê zacz¹æ wykonywanie shadera.
@return Zwraca obiekt dodanego shadera. Zwraca nullptr, je¿eli shadera nie uda³o siê skompilowaæ.*/
PixelShaderObject* ModelsManager::add_pixel_shader( const std::wstring& file_name, const std::string& shader_entry )
{
	PixelShaderObject* shader = pixel_shader.get( file_name );
	if ( !shader )
	{
		// Nie by³o shadera, trzeba go stworzyæ i dodaæ
		shader = PixelShaderObject::create_from_file( file_name, shader_entry );
		if ( !shader )		// shader móg³ mieæ z³y format, a nie chcemy dodawaæ nullptra do ModelsManagera
			return nullptr;

		pixel_shader.unsafe_add( file_name, shader );	// Dodaliœmy teksturê
	}

	return shader;
}

/**@brief Dodaje teksturê do ModelManagera, je¿eli jeszcze nie istnia³a.
@note Funkcja nie dodaje odwo³ania do obiektu, bo nie zak³ada, ¿e ktoœ go od razu u¿yje.
W ka¿dym miejscu, gdzie zostanie przypisany zwrócony obiekt, nale¿y pamiêtaæ o dodaniu odwo³ania oraz
skasowaniu go, gdy obiekt przestanie byæ u¿ywany.

@param[in] file_name Œcie¿ka do tekstury

@return Zwraca wskaŸnik na dodan¹ teksturê lub nullptr, je¿eli nie da³o siê wczytaæ.*/
TextureObject* ModelsManager::add_texture( const std::wstring& file_name )
{

	TextureObject* tex = texture.get( file_name );
	if ( !tex )
	{
		// Nie by³o tekstury, trzeba j¹ stworzyæ i dodaæ
		tex = TextureObject::create_from_file( file_name );
		if ( !tex )		// Tekstura mog³a mieæ z³y format, a nie chcemy dodawaæ nullptra do ModelsManagera
			return nullptr;

		texture.unsafe_add( file_name, tex );	// Dodaliœmy teksturê
	}

	return tex;
}

/**@brief Dodaje do ModelsManagera bufor wierzcho³ków.
Je¿eli pod tak¹ nazw¹ istnieje jakiœ bufor, to zostanie zwrócony wskaŸnik na niego.
@note Funkcja nie dodaje odwo³ania do obiektu, bo nie zak³ada, ¿e ktoœ go od razu u¿yje.
W ka¿dym miejscu, gdzie zostanie przypisany zwrócony obiekt, nale¿y pamiêtaæ o dodaniu odwo³ania oraz
skasowaniu go, gdy obiekt przestanie byæ u¿ywany.

@param[in] name Nazwa bufora, po której mo¿na siê bêdzie odwo³aæ.
@param[in] buffer WskaŸnik na bufor z danym, które maj¹ byæ przeniesione do bufora DirectXowego.
@param[in] element_size Rozmiar pojedynczego elementu w buforze.
@param[in] vert_count Liczba wierzcho³ków/indeksów w buforze.
@return Dodany bufor wierzcho³ków. Zwraca nullptr, je¿eli nie uda³o siê stworzyæ bufora.*/
BufferObject* ModelsManager::add_vertex_buffer( const std::wstring& name,
												const void* buffer,
												unsigned int element_size,
												unsigned int vert_count )
{
	BufferObject* vertex_buff = vertex_buffer.get( name );
	if ( vertex_buff )	// Je¿eli znaleŸliœmy bufor, to go zwracamy
		return vertex_buff;

	// Tworzymy obiekt bufora indeksów i go zapisujemy
	vertex_buff = BufferObject::create_from_memory( buffer,
													element_size,
													vert_count,
													D3D11_BIND_VERTEX_BUFFER );
	if ( !vertex_buff )		// Bufor móg³ siê nie stworzyæ, a nie chcemy dodawaæ nullptra do ModelsManagera
		return nullptr;

	vertex_buffer.unsafe_add( name, vertex_buff );	// Dodaliœmy bufor

	return vertex_buff;
}

/**@brief Dodaje do ModelsManagera bufor indeksów.
Je¿eli pod tak¹ nazw¹ istnieje jakiœ bufor, to zostanie zwrócony wskaŸnik na niego.
@note Funkcja nie dodaje odwo³ania do obiektu, bo nie zak³ada, ¿e ktoœ go od razu u¿yje.
W ka¿dym miejscu, gdzie zostanie przypisany zwrócony obiekt, nale¿y pamiêtaæ o dodaniu odwo³ania oraz
skasowaniu go, gdy obiekt przestanie byæ u¿ywany.

@param[in] name Nazwa bufora, po której mo¿na siê bêdzie odwo³aæ.
@param[in] buffer WskaŸnik na bufor z danym, które maj¹ byæ przeniesione do bufora DirectXowego.
@param[in] element_size Rozmiar pojedynczego elementu w buforze.
@param[in] vert_count Liczba wierzcho³ków/indeksów w buforze.
@return Dodany bufor indeksów. Zwraca nullptr, je¿eli nie uda³o siê stworzyæ bufora.*/
BufferObject* ModelsManager::add_index_buffer( const std::wstring& name,
											   const void* buffer,
											   unsigned int element_size,
											   unsigned int vert_count )
{
	BufferObject* index_buff = index_buffer.get( name );
	if ( index_buff )	// Je¿eli znaleŸliœmy bufor, to go zwracamy
		return index_buff;

	// Tworzymy obiekt bufora indeksów i go zapisujemy
	index_buff = BufferObject::create_from_memory( buffer,
												   element_size,
												   vert_count,
												   D3D11_BIND_INDEX_BUFFER );
	if ( !index_buff )		// Bufor móg³ siê nie stworzyæ, a nie chcemy dodawaæ nullptra do ModelsManagera
		return nullptr;

	index_buffer.unsafe_add( name, index_buff );	// Dodaliœmy bufor

	return index_buff;
}

