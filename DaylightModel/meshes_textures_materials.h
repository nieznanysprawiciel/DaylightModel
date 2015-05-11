#pragma once

/**@file meshes_textures_materials.h
@brief plik zawieta deklaracje foramtów wierzcho³ków, klas zawieraj¹cych assety oraz Model3DFromFile.*/


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


#define INDEX_BUFFER_UINT16		// Definiujemy czy chcemy mieæ bufor indeksów 16-bitowy czy 32-bitowy
//#define INDEX_BUFFER_UINT32			// Wybraæ tylko jedno, inaczej bêdzie b³¹d kompilacji

///@def VERTICIES_MAX_COUNT
///@brief VERTICIES_MAX_COUNT zawiera sta³¹, która jest maksymaln¹ liczb¹ wierzcho³ków w buforze
#if defined(INDEX_BUFFER_UINT16)
#define VERTICIES_MAX_COUNT 0xFFFF
#elif defined(INDEX_BUFFER_UINT32)
#define VERTICIES_MAX_COUNT 0xFFFFFFFF
#else
#define VERTICIES_MAX_COUNT	0xFFFFFFFF
#endif

///@typedef VERT_INDEX
///@brief Definiujemy typ w buforze indeksów.

///W zale¿noœci od tego czy wybierzemy bufor 16 bitowy czy 32 bitowy VERT_INDEX jest ustawiany jako UINT16 lub UINT32.
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


/// \def WRONG_ID B³êdny identyfikator assetu w klasie referenced_obbject

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
// W buforze wierzcho³ków znajduj¹ siê elementy typu VERT_INDEX.
// Definicja typu VERT_INDEX znajduje siê w pliku macros_switches.h i wygl¹da tak:

#if defined(INDEX_BUFFER_UINT16)
typedef UINT16 VERT_INDEX;
#elif defined(INDEX_BUFFER_UINT32)
typedef UINT32 VERT_INDEX;
#else
typedef UINT32 VERT_INDEX;
#endif

*/


//-------------------------------------------------------------------------------//
//	definicje wierzcho³ków



/// @brief Strutkura dla standardowego wierzcho³ka
typedef struct VertexNormalTexCord1
{
	DirectX::XMFLOAT3	position;		///<Pozycja wierzcho³ka
	DirectX::XMFLOAT3	normal;			///<Wektor normalny wierzcho³ka
	DirectX::XMFLOAT2	tex_cords;		///<Wspó³rzêdne tekstury
} VertexNormalTexCord1;

// Opis s³u¿¹cy do stworzenia layoutu zrozumia³ego dla shaderów w pliku vertex_layouts.cpp
extern D3D11_INPUT_ELEMENT_DESC VertexNormalTexCord1_desc[];
extern unsigned int VertexNormalTexCord1_desc_num_of_elements;

/// \brief Struktura wierzcho³ka stworzona z myœl¹ o GUI
typedef struct VertexTexCord1
{
	DirectX::XMFLOAT3	position;		///<Pozycja wierzcho³ka
	DirectX::XMFLOAT2	tex_cords;		///<Wspó³rzêdne tekstury
} VertexTexCord1;

// Opis s³u¿¹cy do stworzenia layoutu zrozumia³ego dla shaderów w pliku vertex_layouts.cpp
extern D3D11_INPUT_ELEMENT_DESC VertexTexCord1_desc[];
extern unsigned int VertexTexCord1_desc_num_of_elements;


//-------------------------------------------------------------------------------//
//	Enumeracje dla klasy Model3DFromFile i wszystkich obiektów zasobów


/** \brief Indeksy tekstur w tablicy ModelPart.

S¹ to wartoœci domyœlne u¿ywane przez wbudowane shadery.
W przypadku w³asnorêcznie pisanych shaderów nie trzeba siê trzymaæ tych sta³ych.*/
typedef enum TEXTURES_TYPES
{
#if ENGINE_MAX_TEXTURES > 0
	TEX_DIFFUSE			///<Tekstura dla kana³u diffuse
#endif
#if ENGINE_MAX_TEXTURES > 1
	, TEX_LIGHTMAP		///<Lightmapa
#endif
#if ENGINE_MAX_TEXTURES > 2
	, TEX_SPECULAR		///<Tekstura dla kana³u specular
#endif
#if ENGINE_MAX_TEXTURES > 3
	, TEX_BUMP_MAP		///<Bump mapa
#endif
#if ENGINE_MAX_TEXTURES > 4
	, TEX_AMBIENT		///<Tekstura dla kana³y ambient
#endif
#if ENGINE_MAX_TEXTURES > 5
	, TEX_DISPLACEMENT_MAP	///<Tekstura przemieszczeñ wierzcho³ków, w przypadku u¿ywania teselacji wierzcho³ków
#endif
#if ENGINE_MAX_TEXTURES > 6
	, TEX_OTHER1		///<Tekstura o dowolnym znaczeniu
#endif
#if ENGINE_MAX_TEXTURES > 7
	, TEX_OTHER2		///<Tekstura o dowolnym znaczeniu
#endif

} TEXTURES_TYPES;

/// \brief Definiuje offset bufora indeksów wzglêdem bufora wierzcho³ków. (Dla funkcji Model3DFromFile::add_index_buffer)
typedef enum VERTEX_BUFFER_OFFSET
{
	LAST = -1,
	BEGIN = 0,
};


/** \brief Klasa u³atwiaj¹ca zarz¹dzanie odwo³aniami do assetów.
*
*Obiekty assetów (np. MaterialObject, TextureObject, VertexShaderObject, PixelShaderObject itp.) wymagaj¹ jakiegoœ systemu zapewniaj¹cego wspó³dzielenie miêdzy innymi obiektami.
*
*Do ka¿dego pojedynczego obiektu mog¹ istnieæ wilokrotne odwo³ania w klasie Model3DFromFile,
*a tak¿e w obiektach dziedzicz¹cych po Dynamic_mesh_object.
*Z tego wzglêdu istniej¹ zmienne file_references i object_references.
*Pierwsza okreœla, ile razy wystêpuj¹ odwo³ania do obiektu w klasie Model3DFromFile.
*(uwaga: nie zliczamy, ile klas siê odwo³uje. Je¿eli w klasie pojawiaj¹ siê 3 odwo³ania, to licz¹ siê jako 3. Taka konwencja
*u³atwia zliczanie i zwalnianie pamiêci)
*Druga zmienna okreœla, ile wystepuje odwo³añ bezpoœrednich przez obiekty, które bêd¹ nastêpnie wyœwietlane. Oznacza to, ¿e przypisuj¹c
*jakiemus obiektowi plik z modelem, musimy zinkrementowaæ other_references o tyle, ile by³o odwo³añ w tym pliku.
*¯aden obiekt nie powinien byæ kasowany, dopóki istniej¹ do niego odwo³ania.

Zmienna unique_id jest na pocz¹tku ustawiana na 0. Jej faktyczne ustawienie odbywa robi klasa ResourceContainer.
Jest to wymagane do u³atwienia obs³ugi wielow¹tkowoœci. Inaczej mog³yby siê pokrywaæ identyfikatory.
**/

class referenced_object
{//definicja w pliku Model3DFormFile
private:
	unsigned int	file_references;	///< Liczba plików, które sie odwo³uj¹
	unsigned int	object_references;	///< Liczba modeli, które siê odwo³uj¹
	unsigned int	unique_id;			///< Unikalny identyfikator materia³u

protected:
	virtual ~referenced_object( );		//Nie ka¿dy mo¿e skasowaæ obiekt

public:
	referenced_object( int id );


	inline void set_id( unsigned int id ) { unique_id = id; }	///<Ustawia identyfikator obiektu

	//sprawdza czy mo¿na zwolniæ zmienn¹
	bool can_delete( unsigned int& file_ref, unsigned int& other_ref );

	/*Funkcje s³u¿¹ce do zarz¹dzania odwo³aniami.
	*Nale¿y pilnowaæ, aby wszystkie funkcje, które modyfikuj¹ jakiekolwiek przypisania obiektów
	*do tekstur, materia³ów i meshy, modyfikowa³y równie¿ iloœæ odwo³añ.
	*U¿ytkownik silnika powinien mieæ udostêpnion¹ wartstwê poœredniczac¹, ¿eby nie musia³
	*pamiêtaæ o odwo³aniach.*/
	inline void add_file_reference( ) { ++file_references; }		///< Dodaje odwo³anie plikowe do assetu
	inline void add_object_reference( ) { ++object_references; }	///< Dodaje odwo³anie bezpoœrednie obiektu do assetu
	inline void delete_file_reference( ) { --file_references; }		///< Kasuje odwo³anie plikowe do assetu
	inline void delete_object_reference( ) { --object_references; }	///< Kasuje odwo³anie bezpoœrednie obiektu do assetu

	inline unsigned int get_id() { return unique_id; }				///< Zwraca identyfikator nadany assetowi
};





/** @brief Struktura opisuje pojedyncz¹ cz¹stkê mesha o jednym materiale, teksturze i shaderach.

W zale¿noœci od zawartoœci pola index_buffer w strukturze ModelPart, mesh jest wyœwietlany w trybie
indeksowanym lub nie.
Je¿eli wartoœæ tego pola wynosi nullptr, to wtedy u¿ywane s¹ zmienne buffer offset i vertices count, które jednoznacznie wskazuj¹, która czêœæ bufora wierzcho³ków ma zostaæ wyœwietlona.

Je¿eli wskaŸnik index_buffer wskazuje na obiekt, to wtedy u¿ywany jest tryb indeksowany
i zmienne buffer_offset, vertices_count i base_vertex.

Klasa jest alokowana w Model3DFromFile i to w³aœnie ta klasa odpowiada za zwolnienie pamiêci.

Pomimo dziedziczenia po klasie referenced_object, nie jest u¿ywane pole unique_id. Dlatego
jest ono w kontruktorze ustawiane na WRONG_ID. MeshPartObject nie mog¹ byæ wspó³dzielone
miêdzy obiektami.*/
struct MeshPartObject : public referenced_object
{
	DirectX::XMFLOAT4X4		transform_matrix;	///<Macierz przekszta³cenia wzglêdem œrodka modelu
	unsigned int			buffer_offset;		///<Offset wzglêdem pocz¹tku bufora indeksów albo wierzcho³ków (zobacz: opis klasy)
	unsigned int			vertices_count;		///<Liczba wierzcho³ków do wyœwietlenia
	int						base_vertex;		///<Wartoœæ dodawana do ka¿dego indeksu przed przeczytaniem wierzcho³ka z bufora. (Tylko wersja indeksowana)
	bool					use_index_buf;		///<Informacja czy jest u¿ywany bufor indeksów czy nie
	
	/** @brief inicjuje objekt neutralnymi wartoœciami tzn. zerami, ustawia use_index_buf na false i
	ustawia macierz przekszta³cenia na macierz identycznoœciow¹.*/
	MeshPartObject( )
		: referenced_object( 0 )		//W tym przypadku identyfikator nie ma znaczenia
	{
		buffer_offset = 0;
		vertices_count = 0;
		base_vertex = 0;
		use_index_buf = false;
		//domyœlnie nie wykonujemy ¿adnego przekszta³cenia
		DirectX::XMMATRIX identity = DirectX::XMMatrixIdentity( );
		XMStoreFloat4x4( &transform_matrix, identity );
	}
};

/** @brief Struktura opisuj¹ca pojedyncz¹ czêœæ mesha gotow¹ do wyœwietlenia.

Meshe s¹ przechowywane w czêœciach, poniewa¿ do ró¿nych wierzcho³ków mog¹ byæ przypisane ró¿ne
materia³y, tekstury i inne elementy. Ta struktura zawiera wskaŸniki na te elementy.

Struktura nie zawiera bufora wierzcho³ków ani bufora indeksów. S¹ one przechowywane zewnêtrznie
w klasie Model3DFromFile lub Dynamic_mesh_object i na jeden mesh przypada tylko jeden bufor wierzcho³ków i maksymalnie
jeden bufor indeksów (mo¿e go nie byæ). 

Obecnoœc bufora indeksów nie oznacza, ¿e ca³y mesh jest wyœwietlany w trybie indeksowanym. Mozliwe jest mieszanie trybów.
Tryb odnosi siê wiêc nie do ca³ego mesha, a pojednyczego obiektu ModelPart.

Tablica texture zawiera wskaŸniki na obiekty tekstur, których maksymalna liczba wynosi ENGINE_MAX_TEXTURES.
Aby obs³u¿yæ wszystkie tekstury jakie mog¹ byc przypisane do obiektu, nale¿y podaæ mu odpowiedni shader, który
umie to zrobiæ. Znaczenie poszczególnych pól tablicy tekstur jest opisywane przez enumeracjê TEXTURES_TYPES
i w taki sposób wykorzystuj¹ je domyœlne shadery.
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


/** @brief Klasa przechowuj¹ca tekstury.

Istniej¹ obiekty nazywaj¹ce siê ID3D11Texture2D i s¹ teksturami, ale stworzenie takiego
nie wystarczy, ¿eby shader móg³ go u¿yæ. Do tego trzeba stworzyæ widok i w³aœnie jego
przechowuje ta funkcja.*/
class TextureObject : public referenced_object, public DX11_interfaces_container
{
	friend ObjectDeleter<TextureObject>;
private:
	std::wstring					file_path;			//œcie¿ka do tekstury (nie do pliku z meshem)
	ID3D11ShaderResourceView*		texture;
protected:
	//¯eby unikn¹æ pomy³ki, obiekt mo¿e byœ kasowany tylko przez ModelsManager.
	~TextureObject( ) override;
public:
	TextureObject( unsigned int id );
	TextureObject( unsigned int id, const std::wstring& path, ID3D11ShaderResourceView* tex );

	bool operator==(const TextureObject& object);
	bool operator==(const std::wstring& file_name);

	inline ID3D11ShaderResourceView* get( ) { return texture; }		///<Zwraca widok tekstury obs³ugiwany przez DirectX11
	static TextureObject* create_from_file( const std::wstring& file_name );
};

/** @brief Klasa przechowuj¹ca vertex shader*/
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

	inline ID3D11VertexShader* get( ) { return vertex_shader; }		///<Zwraca obiekt vertex shadera, któy mo¿na podaæ do potoku przetwarzania
	static VertexShaderObject* create_from_file( const std::wstring& file_name, const std::string& shader_name, const char* shader_model = "vs_4_0" );
	static VertexShaderObject* create_from_file( const std::wstring& file_name, const std::string& shader_name, ID3D11InputLayout** layout,
												 D3D11_INPUT_ELEMENT_DESC* layout_desc, unsigned int array_size, const char* shader_model = "vs_4_0" );
};

/**@brief Klasa przechowuj¹ca pixel shader*/
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

	inline ID3D11PixelShader* get( ) { return pixel_shader; }	///<Zwraca obiekt pixel shadera, któy mo¿na podaæ do potoku przetwarzania
	static PixelShaderObject* create_from_file( const std::wstring& file_name, const std::string& shader_name, const char* shader_model = "ps_4_0" );
};

/** \brief Obiekt opakowuj¹cy bufor DirectXa.

Bufor wierzcho³ków i bufor u¿ywaj¹ tego samego interfejsu ID3D11Buffer,
dlatego nie ma potrzeby tworzenia oddzielnych klas.
Obiekty tego typu mog¹ tak¿e s³u¿yæ do przekazywania parametrów shaderom, jako bufory sta³ych.
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

	inline ID3D11Buffer* get( ) { return buffer; }			///<Zwraca wskaŸnik na bufor
	inline unsigned int get_stride() { return stride; }		///<Zwraca rozmiar pojedynczego elementu w buforze

	static BufferObject* create_from_memory( const void* buffer,
											 unsigned int element_size,
											 unsigned int vert_count,
											 unsigned int bind_flag,
											 D3D11_USAGE usage = D3D11_USAGE_DEFAULT );
};


/**@brief Struktura przechowuj¹ca materia³.

DirectX 11 nie ma w³asnych obiektów na materia³y, poniewa¿ nie ma ju¿ domyœlnego
potoku przetwarzania na karcie graficznej. Na wszystko trzeba napisaæ shader i dostarcza
mu siê takie dane, jakie siê chce dostarczyæ. Dlatego informacja o materia³ach bêdzie
przekazywana z buforze sta³ych.

Struktura zachowuje siê jak asset w zwi¹zku z czym mo¿e
byæ wspó³dzielona przez wiele obiektów. Z tego wzglêdu nie mo¿na jej u¿yæ bezpoœrednio w ConstantPerMesh,
poniewa¿ nie chcemy przekazywaæ do bufora sta³ych zmiennych odziedziczonych po referenced_object.
Zamiast tego trzeba tê strukture przepisaæ.

Zwracam uwagê, ¿e tylko kana³ Diffuse jest wektorem 4 wymiarowym, w którym sk³adowa w jest odpowiedzialna
za przezroczystoœæ. Pozosta³e s¹ takie tylko dlatego, ¿e jest to domyœlny format przechowywania danych 
w rejestrach karty graficznej i przypsiesza to operacjê kopiowania.
@see ConstantPerFrame
*/
typedef struct MaterialObject : public referenced_object
{
	friend ObjectDeleter<MaterialObject>;

	DirectX::XMFLOAT4		Diffuse;		//Sk³adowa przezroczystoœci odnosi siê do ca³ego materia³u
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



/** @brief Struktura tymczasowe dla klasy Model3DFromFile, u¿ywane podczas wype³niania
obiektu danymi. Struktura jest kasowana po zakonczeniu edycji.*/

struct TMP_data
{
	VertexNormalTexCord1*		vertices_tab	= nullptr;
	unsigned int				vertices_count	= 0;
	VERT_INDEX*					indicies_tab = nullptr;
	unsigned int				indicies_count	= 0;
	unsigned int				indicies_offset	= 0;	//offset w buforze wierzcho³ków, wzglêdem którego lic¿a siê indexy 
	ModelPart					new_part;
};

/** @brief Struktura tymczasowe dla klasy Model3DFromFile, u¿ywane podczas wype³niania
obiektu danymi. Struktura jest kasowana po zakonczeniu edycji.*/
struct EditTMP
{
	int			current_pointer		= 0;
	TMP_data**	table			= nullptr;
	int			table_length	= 20;
};

/** @brief Klasa s³u¿y do przechowywania danych modelu wczytanego z pliku.
Dziêki temu po ponownym u¿yciu pliku dla innego obiektu, nie musimy wczytywaæ modelu ponownie.
Plik(i model jednoczeœnie) mo¿e byæ identyfikowany po œcie¿ce do pliku lub unikalnym identyfikatorem.

Klasa zawiera tablicê elementów ModelPart, które przechowuj¹ informacje potrzebne do wyrenderowania
poszczególnych czêœci mesha. Meshe musza byæ trzymane w czêœciach, ¿eby mo¿na by³o ka¿dej z tych czêœci
przypisaæ ró¿ne materia³y i parametry. Gdyby nie taka struktura, to wczytanie pliku nie by³oby mo¿liwe.
Meshe rzadko sk³adaj¹ siê z jednego kawa³ka.

Wszystkie wierzcho³ki przechowywane s¹ tylko w jednym buforze vertex_buffer. Na podstawie zaleceñ
w dokumentacji DrectXa, lepiej jest mieæ jeden du¿y bufor ni¿ wiele ma³ych.
Poszczególne czêœci mesha zawieraj¹ albo bufor indeksów, który decyduje, które wierzcho³ki
s¹ wyœwietlane, albo odpowiedni offset od pocz¹tku bufora i iloœæ trójk¹tów.
Mo¿liwe jest równie¿, ¿e mesh bêdzie wyœwietlany w trybie mieszanym, u¿ywaj¹c
dla ka¿dej czêœci raz bufora indeksów, a raz offsetów.

## Wype³nianie obiektów danymi
Obiekty s¹ wype³niane najczêœciej przez loadery dla odpowiednich formatów. Wiêcej o pisaniu loaderów znajduje siê
tutaj: $ref MakingLoaders.

Aby wype³niæ obiekt danymi nale¿y wywo³aæ funkcjê BeginEdit, a po zakoñczeniu EndEdit. BeginEdit tworzy strukturê
tymczasow¹, do której bêd¹ zapisywane informacje. EndEdit przetwarza tê strutkurê do postaci docelowej. Struktura poœrednia
jest konieczna, poniewa¿ bufory wierzcho³ków i indeksów bêdziemy dostawali w czêœciach, a musimy je scaliæ do pojedynczych
buforów i stworzyæ obiekt DirectXa.

Nastêpnie miêdzy wywo³aniami BeginEdit i EndEdit podaje siê wszystkie informacje, jakie maj¹ zostaæ zawarte w poszczególnych
obiektach ModelPart. Wype³nianie ka¿dego parta musi byæ otoczone wywo³aniem funkcji BeginPart i EndPart. BeginPart alokuje
pamiêæ dla kolejnego ModelParta, a EndEdit przesuwa aktualny indeks, pod którego dodajemy czêœæ mesha.

Niedodanie bufora indeksów powoduje, ¿e aktualny Part jest wyœwietlany w trybie bezpoœrednim, w przeciwnym razie
jest u¿ywany tryb indeksowany.
W strukturze musi istnieæ przynajmniej jeden bufor wierzcho³ków. Brak bufora wierzcho³ków jest b³êdem, ale zasadniczo
nie skutkuje niczym powa¿nym, bo DisplayEngine pomija takie obiekty.

Nie ma koniecznoœci podawania bufora wierzcho³ków dla ka¿dego ModelParta. Bufory indeksów mog¹ byæ ustawiane wzglêdem
poprzednio dodanych buforów wierzcho³ków. B³êdem jest za to niepodanie ani bufora wierzcho³ków ani bufora indeksów.
DisplayEngine bêdzie próbowa³ wyœwietliæ takiego mesha o liczbie wierzcho³ków równej 0, co niepotrzebnie spowalnia
program.

Je¿eli dodane tekstury s¹ zgodne z domyœlnym przeznaczeniem (zdefiniowanym enumeracj¹ TEXTURES_TYPES), to nie ma potrzeby
dodawania w³asnych shaderów. Funkcja ModelsManager::find_best_vertex_shader i ModelsManager::find_best_pixel_shader znajd¹
najlepiej pasuj¹cy shader do podanej tablicy tekstur.

Je¿eli nie zostanie podany ¿aden materia³, ModelPart dostanie domyœlny materia³, który jest ustawiany przez funkcjê
MaterialObject::set_null_material.

@see BufferObject, ModelPart, TextureObject, VertexShaderObject, PixelShaderObject, MaterialObject, MeshPartObject*/

class Model3DFromFile : public referenced_object
{
	friend ObjectDeleter<Model3DFromFile>;
	friend ModelsManager;
private:
	static ModelsManager* models_manager;

	//tekstura i materia³ odpowiadaj¹ meshowi spod danego indeksu
	std::vector<ModelPart>			model_parts;		///<Vector zawieraj¹cy info o poszczególnych czêœciach mesha
	BufferObject*					vertex_buffer;		///<Bufor wierzcho³ków
	BufferObject*					index_buffer;		///<Bufor indeksów
	std::wstring					file_path;			///<Plik, z którego powsta³ obiekt

	//wskaxnik na tymczasow¹ strukture, która bêdzie u¿ywana podczas wype³niania obiektu danymi
	EditTMP*						tmp_data;			///<Dane tymczasowe u¿ywane podczas wype³niania

protected:
	//¯eby unikn¹æ pomy³ki, obiekt mo¿e byœ kasowany tylko przez ModelsManager.
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

	// Wype³nianie danymi
	unsigned int add_texture( const std::wstring& file_name, TEXTURES_TYPES type = TEX_DIFFUSE );
	unsigned int add_material( const MaterialObject* material, const std::wstring& material_name );
	unsigned int add_vertex_shader( const std::wstring& file_name );
	unsigned int add_pixel_shader( const std::wstring& file_name );
	unsigned int add_vertex_buffer( const VertexNormalTexCord1* buffer, unsigned int vert_count );
	void add_transformation( const DirectX::XMFLOAT4X4& transform );
	unsigned int add_index_buffer( const VERT_INDEX* buffer, unsigned int ind_count, int vertex_buffer_offset );


	unsigned int get_parts_count( );
	const ModelPart* get_part( unsigned int index );

	inline BufferObject* get_vertex_buffer() { return vertex_buffer; }		///<Zwraca wskaŸnik na bufor wierzcho³ków
	inline BufferObject* get_index_buffer() { return index_buffer; }		///<Zwraca wskaŸnik na bufor indeksów
};

