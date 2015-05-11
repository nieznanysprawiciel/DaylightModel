#pragma once

/**@file meshes_textures_materials.h
@brief plik zawieta deklaracje foramt�w wierzcho�k�w, klas zawieraj�cych assety oraz Model3DFromFile.*/


#include "DX11_interfaces_container.h"
#include "ObjectDeleter.h"
#include <DirectXMath.h>

/**@def ENGINE_MAX_TEXTURES
@brief Maksymalna liczba tekstur w silniku
*/
#define ENGINE_MAX_TEXTURES		8

//definicje
#define WRONG_ID						0
#define WRONG_MODEL_FILE_ID				WRONG_ID
#define WRONG_MESH_ID					WRONG_ID
#define WRONG_TEXTURE_ID				WRONG_ID
#define WRONG_MATERIAL_ID				WRONG_ID


#define INDEX_BUFFER_UINT16		// Definiujemy czy chcemy mie� bufor indeks�w 16-bitowy czy 32-bitowy
//#define INDEX_BUFFER_UINT32			// Wybra� tylko jedno, inaczej b�dzie b��d kompilacji

///@def VERTICIES_MAX_COUNT
///@brief VERTICIES_MAX_COUNT zawiera sta��, kt�ra jest maksymaln� liczb� wierzcho�k�w w buforze
#if defined(INDEX_BUFFER_UINT16)
#define VERTICIES_MAX_COUNT 0xFFFF
#elif defined(INDEX_BUFFER_UINT32)
#define VERTICIES_MAX_COUNT 0xFFFFFFFF
#else
#define VERTICIES_MAX_COUNT	0xFFFFFFFF
#endif

///@typedef VERT_INDEX
///@brief Definiujemy typ w buforze indeks�w.

///W zale�no�ci od tego czy wybierzemy bufor 16 bitowy czy 32 bitowy VERT_INDEX jest ustawiany jako UINT16 lub UINT32.
#if defined(INDEX_BUFFER_UINT16)
typedef UINT16 VERT_INDEX;
#define INDEX_BUFFER_FORMAT DXGI_FORMAT_R16_UINT
#elif defined(INDEX_BUFFER_UINT32)
typedef UINT32 VERT_INDEX;
#define INDEX_BUFFER_FORMAT DXGI_FORMAT_R32_UINT
#else
typedef UINT32 VERT_INDEX;
#define INDEX_BUFFER_FORMAT DXGI_FORMAT_R32_UINT
#endif


/// \def WRONG_ID B��dny identyfikator assetu w klasie referenced_obbject

//-------------------------------------------------------------------------------//
//							wersja DirectX11
//-------------------------------------------------------------------------------//


class ModelsManager;
struct ModelPart;
class BufferObject;
class TextureObject;
class VertexShaderObject;
class PixelShaderObject;
struct MeshPartObject;
struct MaterialObject;

/*
// W buforze wierzcho�k�w znajduj� si� elementy typu VERT_INDEX.
// Definicja typu VERT_INDEX znajduje si� w pliku macros_switches.h i wygl�da tak:

#if defined(INDEX_BUFFER_UINT16)
typedef UINT16 VERT_INDEX;
#elif defined(INDEX_BUFFER_UINT32)
typedef UINT32 VERT_INDEX;
#else
typedef UINT32 VERT_INDEX;
#endif

*/


//-------------------------------------------------------------------------------//
//	definicje wierzcho�k�w



/// @brief Strutkura dla standardowego wierzcho�ka
typedef struct VertexNormalTexCord1
{
	DirectX::XMFLOAT3	position;		///<Pozycja wierzcho�ka
	DirectX::XMFLOAT3	normal;			///<Wektor normalny wierzcho�ka
	DirectX::XMFLOAT2	tex_cords;		///<Wsp�rz�dne tekstury
} VertexNormalTexCord1;

// Opis s�u��cy do stworzenia layoutu zrozumia�ego dla shader�w w pliku vertex_layouts.cpp
extern D3D11_INPUT_ELEMENT_DESC VertexNormalTexCord1_desc[];
extern unsigned int VertexNormalTexCord1_desc_num_of_elements;

/// \brief Struktura wierzcho�ka stworzona z my�l� o GUI
typedef struct VertexTexCord1
{
	DirectX::XMFLOAT3	position;		///<Pozycja wierzcho�ka
	DirectX::XMFLOAT2	tex_cords;		///<Wsp�rz�dne tekstury
} VertexTexCord1;

// Opis s�u��cy do stworzenia layoutu zrozumia�ego dla shader�w w pliku vertex_layouts.cpp
extern D3D11_INPUT_ELEMENT_DESC VertexTexCord1_desc[];
extern unsigned int VertexTexCord1_desc_num_of_elements;


//-------------------------------------------------------------------------------//
//	Enumeracje dla klasy Model3DFromFile i wszystkich obiekt�w zasob�w


/** \brief Indeksy tekstur w tablicy ModelPart.

S� to warto�ci domy�lne u�ywane przez wbudowane shadery.
W przypadku w�asnor�cznie pisanych shader�w nie trzeba si� trzyma� tych sta�ych.*/
typedef enum TEXTURES_TYPES
{
#if ENGINE_MAX_TEXTURES > 0
	TEX_DIFFUSE			///<Tekstura dla kana�u diffuse
#endif
#if ENGINE_MAX_TEXTURES > 1
	, TEX_LIGHTMAP		///<Lightmapa
#endif
#if ENGINE_MAX_TEXTURES > 2
	, TEX_SPECULAR		///<Tekstura dla kana�u specular
#endif
#if ENGINE_MAX_TEXTURES > 3
	, TEX_BUMP_MAP		///<Bump mapa
#endif
#if ENGINE_MAX_TEXTURES > 4
	, TEX_AMBIENT		///<Tekstura dla kana�y ambient
#endif
#if ENGINE_MAX_TEXTURES > 5
	, TEX_DISPLACEMENT_MAP	///<Tekstura przemieszcze� wierzcho�k�w, w przypadku u�ywania teselacji wierzcho�k�w
#endif
#if ENGINE_MAX_TEXTURES > 6
	, TEX_OTHER1		///<Tekstura o dowolnym znaczeniu
#endif
#if ENGINE_MAX_TEXTURES > 7
	, TEX_OTHER2		///<Tekstura o dowolnym znaczeniu
#endif

} TEXTURES_TYPES;

/// \brief Definiuje offset bufora indeks�w wzgl�dem bufora wierzcho�k�w. (Dla funkcji Model3DFromFile::add_index_buffer)
typedef enum VERTEX_BUFFER_OFFSET
{
	LAST = -1,
	BEGIN = 0,
};


/** \brief Klasa u�atwiaj�ca zarz�dzanie odwo�aniami do asset�w.
*
*Obiekty asset�w (np. MaterialObject, TextureObject, VertexShaderObject, PixelShaderObject itp.) wymagaj� jakiego� systemu zapewniaj�cego wsp�dzielenie mi�dzy innymi obiektami.
*
*Do ka�dego pojedynczego obiektu mog� istnie� wilokrotne odwo�ania w klasie Model3DFromFile,
*a tak�e w obiektach dziedzicz�cych po Dynamic_mesh_object.
*Z tego wzgl�du istniej� zmienne file_references i object_references.
*Pierwsza okre�la, ile razy wyst�puj� odwo�ania do obiektu w klasie Model3DFromFile.
*(uwaga: nie zliczamy, ile klas si� odwo�uje. Je�eli w klasie pojawiaj� si� 3 odwo�ania, to licz� si� jako 3. Taka konwencja
*u�atwia zliczanie i zwalnianie pami�ci)
*Druga zmienna okre�la, ile wystepuje odwo�a� bezpo�rednich przez obiekty, kt�re b�d� nast�pnie wy�wietlane. Oznacza to, �e przypisuj�c
*jakiemus obiektowi plik z modelem, musimy zinkrementowa� other_references o tyle, ile by�o odwo�a� w tym pliku.
*�aden obiekt nie powinien by� kasowany, dop�ki istniej� do niego odwo�ania.

Zmienna unique_id jest na pocz�tku ustawiana na 0. Jej faktyczne ustawienie odbywa robi klasa ResourceContainer.
Jest to wymagane do u�atwienia obs�ugi wielow�tkowo�ci. Inaczej mog�yby si� pokrywa� identyfikatory.
**/

class referenced_object
{//definicja w pliku Model3DFormFile
private:
	unsigned int	file_references;	///< Liczba plik�w, kt�re sie odwo�uj�
	unsigned int	object_references;	///< Liczba modeli, kt�re si� odwo�uj�
	unsigned int	unique_id;			///< Unikalny identyfikator materia�u

protected:
	virtual ~referenced_object( );		//Nie ka�dy mo�e skasowa� obiekt

public:
	referenced_object( int id );


	inline void set_id( unsigned int id ) { unique_id = id; }	///<Ustawia identyfikator obiektu

	//sprawdza czy mo�na zwolni� zmienn�
	bool can_delete( unsigned int& file_ref, unsigned int& other_ref );

	/*Funkcje s�u��ce do zarz�dzania odwo�aniami.
	*Nale�y pilnowa�, aby wszystkie funkcje, kt�re modyfikuj� jakiekolwiek przypisania obiekt�w
	*do tekstur, materia��w i meshy, modyfikowa�y r�wnie� ilo�� odwo�a�.
	*U�ytkownik silnika powinien mie� udost�pnion� wartstw� po�redniczac�, �eby nie musia�
	*pami�ta� o odwo�aniach.*/
	inline void add_file_reference( ) { ++file_references; }		///< Dodaje odwo�anie plikowe do assetu
	inline void add_object_reference( ) { ++object_references; }	///< Dodaje odwo�anie bezpo�rednie obiektu do assetu
	inline void delete_file_reference( ) { --file_references; }		///< Kasuje odwo�anie plikowe do assetu
	inline void delete_object_reference( ) { --object_references; }	///< Kasuje odwo�anie bezpo�rednie obiektu do assetu

	inline unsigned int get_id() { return unique_id; }				///< Zwraca identyfikator nadany assetowi
};





/** @brief Struktura opisuje pojedyncz� cz�stk� mesha o jednym materiale, teksturze i shaderach.

W zale�no�ci od zawarto�ci pola index_buffer w strukturze ModelPart, mesh jest wy�wietlany w trybie
indeksowanym lub nie.
Je�eli warto�� tego pola wynosi nullptr, to wtedy u�ywane s� zmienne buffer offset i vertices count, kt�re jednoznacznie wskazuj�, kt�ra cz�� bufora wierzcho�k�w ma zosta� wy�wietlona.

Je�eli wska�nik index_buffer wskazuje na obiekt, to wtedy u�ywany jest tryb indeksowany
i zmienne buffer_offset, vertices_count i base_vertex.

Klasa jest alokowana w Model3DFromFile i to w�a�nie ta klasa odpowiada za zwolnienie pami�ci.

Pomimo dziedziczenia po klasie referenced_object, nie jest u�ywane pole unique_id. Dlatego
jest ono w kontruktorze ustawiane na WRONG_ID. MeshPartObject nie mog� by� wsp�dzielone
mi�dzy obiektami.*/
struct MeshPartObject : public referenced_object
{
	DirectX::XMFLOAT4X4		transform_matrix;	///<Macierz przekszta�cenia wzgl�dem �rodka modelu
	unsigned int			buffer_offset;		///<Offset wzgl�dem pocz�tku bufora indeks�w albo wierzcho�k�w (zobacz: opis klasy)
	unsigned int			vertices_count;		///<Liczba wierzcho�k�w do wy�wietlenia
	int						base_vertex;		///<Warto�� dodawana do ka�dego indeksu przed przeczytaniem wierzcho�ka z bufora. (Tylko wersja indeksowana)
	bool					use_index_buf;		///<Informacja czy jest u�ywany bufor indeks�w czy nie
	
	/** @brief inicjuje objekt neutralnymi warto�ciami tzn. zerami, ustawia use_index_buf na false i
	ustawia macierz przekszta�cenia na macierz identyczno�ciow�.*/
	MeshPartObject( )
		: referenced_object( 0 )		//W tym przypadku identyfikator nie ma znaczenia
	{
		buffer_offset = 0;
		vertices_count = 0;
		base_vertex = 0;
		use_index_buf = false;
		//domy�lnie nie wykonujemy �adnego przekszta�cenia
		DirectX::XMMATRIX identity = DirectX::XMMatrixIdentity( );
		XMStoreFloat4x4( &transform_matrix, identity );
	}
};

/** @brief Struktura opisuj�ca pojedyncz� cz�� mesha gotow� do wy�wietlenia.

Meshe s� przechowywane w cz�ciach, poniewa� do r�nych wierzcho�k�w mog� by� przypisane r�ne
materia�y, tekstury i inne elementy. Ta struktura zawiera wska�niki na te elementy.

Struktura nie zawiera bufora wierzcho�k�w ani bufora indeks�w. S� one przechowywane zewn�trznie
w klasie Model3DFromFile lub Dynamic_mesh_object i na jeden mesh przypada tylko jeden bufor wierzcho�k�w i maksymalnie
jeden bufor indeks�w (mo�e go nie by�). 

Obecno�c bufora indeks�w nie oznacza, �e ca�y mesh jest wy�wietlany w trybie indeksowanym. Mozliwe jest mieszanie tryb�w.
Tryb odnosi si� wi�c nie do ca�ego mesha, a pojednyczego obiektu ModelPart.

Tablica texture zawiera wska�niki na obiekty tekstur, kt�rych maksymalna liczba wynosi ENGINE_MAX_TEXTURES.
Aby obs�u�y� wszystkie tekstury jakie mog� byc przypisane do obiektu, nale�y poda� mu odpowiedni shader, kt�ry
umie to zrobi�. Znaczenie poszczeg�lnych p�l tablicy tekstur jest opisywane przez enumeracj� TEXTURES_TYPES
i w taki spos�b wykorzystuj� je domy�lne shadery.
*/
struct ModelPart
{
	VertexShaderObject*		vertex_shader;
	PixelShaderObject*		pixel_shader;
	MaterialObject*			material;
	TextureObject*			texture[ENGINE_MAX_TEXTURES];
	MeshPartObject*			mesh;

	ModelPart( )
	{
		vertex_shader = nullptr;
		pixel_shader = nullptr;
		material = nullptr;
		mesh = nullptr;

#if ENGINE_MAX_TEXTURES > 0
		texture[0] = nullptr;
#endif
#if ENGINE_MAX_TEXTURES > 1
		texture[1] = nullptr;
#endif
#if ENGINE_MAX_TEXTURES > 2
		texture[2] = nullptr;
#endif
#if ENGINE_MAX_TEXTURES > 3
		texture[3] = nullptr;
#endif
#if ENGINE_MAX_TEXTURES > 4
		texture[4] = nullptr;
#endif
#if ENGINE_MAX_TEXTURES > 5
		texture[5] = nullptr;
#endif
#if ENGINE_MAX_TEXTURES > 6
		texture[6] = nullptr;
#endif
#if ENGINE_MAX_TEXTURES > 7
		texture[7] = nullptr;
#endif
	}
};


/** @brief Klasa przechowuj�ca tekstury.

Istniej� obiekty nazywaj�ce si� ID3D11Texture2D i s� teksturami, ale stworzenie takiego
nie wystarczy, �eby shader m�g� go u�y�. Do tego trzeba stworzy� widok i w�a�nie jego
przechowuje ta funkcja.*/
class TextureObject : public referenced_object, public DX11_interfaces_container
{
	friend ObjectDeleter<TextureObject>;
private:
	std::wstring					file_path;			//�cie�ka do tekstury (nie do pliku z meshem)
	ID3D11ShaderResourceView*		texture;
protected:
	//�eby unikn�� pomy�ki, obiekt mo�e by� kasowany tylko przez ModelsManager.
	~TextureObject( ) override;
public:
	TextureObject( unsigned int id );
	TextureObject( unsigned int id, const std::wstring& path, ID3D11ShaderResourceView* tex );

	bool operator==(const TextureObject& object);
	bool operator==(const std::wstring& file_name);

	inline ID3D11ShaderResourceView* get( ) { return texture; }		///<Zwraca widok tekstury obs�ugiwany przez DirectX11
	static TextureObject* create_from_file( const std::wstring& file_name );
};

/** @brief Klasa przechowuj�ca vertex shader*/
class VertexShaderObject : public referenced_object, public DX11_interfaces_container
{
	friend ObjectDeleter<VertexShaderObject>;
private:
	ID3D11VertexShader*		vertex_shader;
protected:
	~VertexShaderObject( ) override;
public:
	VertexShaderObject();
	VertexShaderObject( ID3D11VertexShader* shader ) : referenced_object( WRONG_ID ), vertex_shader( shader ){}

	inline ID3D11VertexShader* get( ) { return vertex_shader; }		///<Zwraca obiekt vertex shadera, kt�y mo�na poda� do potoku przetwarzania
	static VertexShaderObject* create_from_file( const std::wstring& file_name, const std::string& shader_name, const char* shader_model = "vs_4_0" );
	static VertexShaderObject* create_from_file( const std::wstring& file_name, const std::string& shader_name, ID3D11InputLayout** layout,
												 D3D11_INPUT_ELEMENT_DESC* layout_desc, unsigned int array_size, const char* shader_model = "vs_4_0" );
};

/**@brief Klasa przechowuj�ca pixel shader*/
class PixelShaderObject : public referenced_object, public DX11_interfaces_container
{
	friend ObjectDeleter<PixelShaderObject>;
private:
	ID3D11PixelShader*		pixel_shader;
protected:
	~PixelShaderObject( ) override;
public:
	PixelShaderObject();
	PixelShaderObject( ID3D11PixelShader* shader ) : referenced_object( WRONG_ID ), pixel_shader( shader ){}

	inline ID3D11PixelShader* get( ) { return pixel_shader; }	///<Zwraca obiekt pixel shadera, kt�y mo�na poda� do potoku przetwarzania
	static PixelShaderObject* create_from_file( const std::wstring& file_name, const std::string& shader_name, const char* shader_model = "ps_4_0" );
};

/** \brief Obiekt opakowuj�cy bufor DirectXa.

Bufor wierzcho�k�w i bufor u�ywaj� tego samego interfejsu ID3D11Buffer,
dlatego nie ma potrzeby tworzenia oddzielnych klas.
Obiekty tego typu mog� tak�e s�u�y� do przekazywania parametr�w shaderom, jako bufory sta�ych.
*/
class BufferObject : public referenced_object, public DX11_interfaces_container
{
	friend ObjectDeleter<BufferObject>;
private:
	ID3D11Buffer*		buffer;
	unsigned int		stride;
protected:
	~BufferObject( ) override;
public:
	BufferObject( unsigned int element_size );
	BufferObject( unsigned int element_size, ID3D11Buffer* buff );

	inline ID3D11Buffer* get( ) { return buffer; }			///<Zwraca wska�nik na bufor
	inline unsigned int get_stride() { return stride; }		///<Zwraca rozmiar pojedynczego elementu w buforze

	static BufferObject* create_from_memory( const void* buffer,
											 unsigned int element_size,
											 unsigned int vert_count,
											 unsigned int bind_flag,
											 D3D11_USAGE usage = D3D11_USAGE_DEFAULT );
};


/**@brief Struktura przechowuj�ca materia�.

DirectX 11 nie ma w�asnych obiekt�w na materia�y, poniewa� nie ma ju� domy�lnego
potoku przetwarzania na karcie graficznej. Na wszystko trzeba napisa� shader i dostarcza
mu si� takie dane, jakie si� chce dostarczy�. Dlatego informacja o materia�ach b�dzie
przekazywana z buforze sta�ych.

Struktura zachowuje si� jak asset w zwi�zku z czym mo�e
by� wsp�dzielona przez wiele obiekt�w. Z tego wzgl�du nie mo�na jej u�y� bezpo�rednio w ConstantPerMesh,
poniewa� nie chcemy przekazywa� do bufora sta�ych zmiennych odziedziczonych po referenced_object.
Zamiast tego trzeba t� strukture przepisa�.

Zwracam uwag�, �e tylko kana� Diffuse jest wektorem 4 wymiarowym, w kt�rym sk�adowa w jest odpowiedzialna
za przezroczysto��. Pozosta�e s� takie tylko dlatego, �e jest to domy�lny format przechowywania danych 
w rejestrach karty graficznej i przypsiesza to operacj� kopiowania.
@see ConstantPerFrame
*/
typedef struct MaterialObject : public referenced_object
{
	friend ObjectDeleter<MaterialObject>;

	DirectX::XMFLOAT4		Diffuse;		//Sk�adowa przezroczysto�ci odnosi si� do ca�ego materia�u
	DirectX::XMFLOAT4		Ambient;
	DirectX::XMFLOAT4		Specular;
	DirectX::XMFLOAT4		Emissive;
	float					Power;

	MaterialObject( unsigned int id = WRONG_ID ) : referenced_object( id ){}
	MaterialObject( const MaterialObject* material );

	void set_null_material();
} MaterialObject;



//-------------------------------------------------------------------------------//
//	Model3DFronFile



/** @brief Struktura tymczasowe dla klasy Model3DFromFile, u�ywane podczas wype�niania
obiektu danymi. Struktura jest kasowana po zakonczeniu edycji.*/

struct TMP_data
{
	VertexNormalTexCord1*		vertices_tab	= nullptr;
	unsigned int				vertices_count	= 0;
	VERT_INDEX*					indicies_tab = nullptr;
	unsigned int				indicies_count	= 0;
	unsigned int				indicies_offset	= 0;	//offset w buforze wierzcho�k�w, wzgl�dem kt�rego lic�a si� indexy 
	ModelPart					new_part;
};

/** @brief Struktura tymczasowe dla klasy Model3DFromFile, u�ywane podczas wype�niania
obiektu danymi. Struktura jest kasowana po zakonczeniu edycji.*/
struct EditTMP
{
	int			current_pointer		= 0;
	TMP_data**	table			= nullptr;
	int			table_length	= 20;
};

/** @brief Klasa s�u�y do przechowywania danych modelu wczytanego z pliku.
Dzi�ki temu po ponownym u�yciu pliku dla innego obiektu, nie musimy wczytywa� modelu ponownie.
Plik(i model jednocze�nie) mo�e by� identyfikowany po �cie�ce do pliku lub unikalnym identyfikatorem.

Klasa zawiera tablic� element�w ModelPart, kt�re przechowuj� informacje potrzebne do wyrenderowania
poszczeg�lnych cz�ci mesha. Meshe musza by� trzymane w cz�ciach, �eby mo�na by�o ka�dej z tych cz�ci
przypisa� r�ne materia�y i parametry. Gdyby nie taka struktura, to wczytanie pliku nie by�oby mo�liwe.
Meshe rzadko sk�adaj� si� z jednego kawa�ka.

Wszystkie wierzcho�ki przechowywane s� tylko w jednym buforze vertex_buffer. Na podstawie zalece�
w dokumentacji DrectXa, lepiej jest mie� jeden du�y bufor ni� wiele ma�ych.
Poszczeg�lne cz�ci mesha zawieraj� albo bufor indeks�w, kt�ry decyduje, kt�re wierzcho�ki
s� wy�wietlane, albo odpowiedni offset od pocz�tku bufora i ilo�� tr�jk�t�w.
Mo�liwe jest r�wnie�, �e mesh b�dzie wy�wietlany w trybie mieszanym, u�ywaj�c
dla ka�dej cz�ci raz bufora indeks�w, a raz offset�w.

## Wype�nianie obiekt�w danymi
Obiekty s� wype�niane najcz�ciej przez loadery dla odpowiednich format�w. Wi�cej o pisaniu loader�w znajduje si�
tutaj: $ref MakingLoaders.

Aby wype�ni� obiekt danymi nale�y wywo�a� funkcj� BeginEdit, a po zako�czeniu EndEdit. BeginEdit tworzy struktur�
tymczasow�, do kt�rej b�d� zapisywane informacje. EndEdit przetwarza t� strutkur� do postaci docelowej. Struktura po�rednia
jest konieczna, poniewa� bufory wierzcho�k�w i indeks�w b�dziemy dostawali w cz�ciach, a musimy je scali� do pojedynczych
bufor�w i stworzy� obiekt DirectXa.

Nast�pnie mi�dzy wywo�aniami BeginEdit i EndEdit podaje si� wszystkie informacje, jakie maj� zosta� zawarte w poszczeg�lnych
obiektach ModelPart. Wype�nianie ka�dego parta musi by� otoczone wywo�aniem funkcji BeginPart i EndPart. BeginPart alokuje
pami�� dla kolejnego ModelParta, a EndEdit przesuwa aktualny indeks, pod kt�rego dodajemy cz�� mesha.

Niedodanie bufora indeks�w powoduje, �e aktualny Part jest wy�wietlany w trybie bezpo�rednim, w przeciwnym razie
jest u�ywany tryb indeksowany.
W strukturze musi istnie� przynajmniej jeden bufor wierzcho�k�w. Brak bufora wierzcho�k�w jest b��dem, ale zasadniczo
nie skutkuje niczym powa�nym, bo DisplayEngine pomija takie obiekty.

Nie ma konieczno�ci podawania bufora wierzcho�k�w dla ka�dego ModelParta. Bufory indeks�w mog� by� ustawiane wzgl�dem
poprzednio dodanych bufor�w wierzcho�k�w. B��dem jest za to niepodanie ani bufora wierzcho�k�w ani bufora indeks�w.
DisplayEngine b�dzie pr�bowa� wy�wietli� takiego mesha o liczbie wierzcho�k�w r�wnej 0, co niepotrzebnie spowalnia
program.

Je�eli dodane tekstury s� zgodne z domy�lnym przeznaczeniem (zdefiniowanym enumeracj� TEXTURES_TYPES), to nie ma potrzeby
dodawania w�asnych shader�w. Funkcja ModelsManager::find_best_vertex_shader i ModelsManager::find_best_pixel_shader znajd�
najlepiej pasuj�cy shader do podanej tablicy tekstur.

Je�eli nie zostanie podany �aden materia�, ModelPart dostanie domy�lny materia�, kt�ry jest ustawiany przez funkcj�
MaterialObject::set_null_material.

@see BufferObject, ModelPart, TextureObject, VertexShaderObject, PixelShaderObject, MaterialObject, MeshPartObject*/

class Model3DFromFile : public referenced_object
{
	friend ObjectDeleter<Model3DFromFile>;
	friend ModelsManager;
private:
	static ModelsManager* models_manager;

	//tekstura i materia� odpowiadaj� meshowi spod danego indeksu
	std::vector<ModelPart>			model_parts;		///<Vector zawieraj�cy info o poszczeg�lnych cz�ciach mesha
	BufferObject*					vertex_buffer;		///<Bufor wierzcho�k�w
	BufferObject*					index_buffer;		///<Bufor indeks�w
	std::wstring					file_path;			///<Plik, z kt�rego powsta� obiekt

	//wskaxnik na tymczasow� strukture, kt�ra b�dzie u�ywana podczas wype�niania obiektu danymi
	EditTMP*						tmp_data;			///<Dane tymczasowe u�ywane podczas wype�niania

protected:
	//�eby unikn�� pomy�ki, obiekt mo�e by� kasowany tylko przez ModelsManager.
	~Model3DFromFile() override;

	unsigned int get_buffer_offset_to_last();

	void EndEdit_vertex_buffer_processing();
	void EndEdit_index_buffer_processing();
	void EndEdit_prepare_ModelPart();
public:
	Model3DFromFile( const std::wstring& name );


	// Tworzenie obiektu
	void BeginEdit();
	void EndEdit();
	void BeginPart();
	void EndPart();

	// Wype�nianie danymi
	unsigned int add_texture( const std::wstring& file_name, TEXTURES_TYPES type = TEX_DIFFUSE );
	unsigned int add_material( const MaterialObject* material, const std::wstring& material_name );
	unsigned int add_vertex_shader( const std::wstring& file_name );
	unsigned int add_pixel_shader( const std::wstring& file_name );
	unsigned int add_vertex_buffer( const VertexNormalTexCord1* buffer, unsigned int vert_count );
	void add_transformation( const DirectX::XMFLOAT4X4& transform );
	unsigned int add_index_buffer( const VERT_INDEX* buffer, unsigned int ind_count, int vertex_buffer_offset );


	unsigned int get_parts_count( );
	const ModelPart* get_part( unsigned int index );

	inline BufferObject* get_vertex_buffer() { return vertex_buffer; }		///<Zwraca wska�nik na bufor wierzcho�k�w
	inline BufferObject* get_index_buffer() { return index_buffer; }		///<Zwraca wska�nik na bufor indeks�w
};

