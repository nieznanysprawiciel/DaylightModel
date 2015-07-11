#include "stdafx.h"
#include "meshes_textures_materials.h"

//#include "..\..\memory_leaks.h"
//----------------------------------------------------------------------------------------------//
//								contructors, destructors										//
//----------------------------------------------------------------------------------------------//

VertexShaderObject::VertexShaderObject()
: referenced_object( WRONG_ID )
{
	vertex_shader = nullptr;
}


///@brief Zwalnia obiekt shadera z directXa
VertexShaderObject::~VertexShaderObject( )
{
	if ( vertex_shader )
		vertex_shader->Release( );
}


/**@brief Tworzy obiekt VertexShaderObject na podstawie pliku.

W przypadku b³êdów kompilacji w trybie debug s¹ one przekierowane do okna Output.

Na razie obs³uguje tylko nieskompilowane pliki.
@param[in] file_name Nazwa pliku, z którego zostanie wczytany shader
@param[in] shader_name Nazwa funkcji, która jest punktem poczatkowym wykonania shadera
@param[in] shader_model £añcuch znaków opisuj¹cy shader model.
@return Zwraca wskaŸnik na obiekt VertexShaderObject lub nullptr w przypadku niepowodzenia.*/
VertexShaderObject* VertexShaderObject::create_from_file( const std::wstring& file_name, const std::string& shader_name, const char* shader_model )
{
	HRESULT result;
	ID3DBlob* compiled_shader;
	ID3D11VertexShader* vertex_shader;
	// Troszkê zamieszania, ale w trybie debug warto wiedzieæ co jest nie tak przy kompilacji shadera
#ifdef _DEBUG
	ID3D10Blob* error_blob = nullptr;	// Tu znajdzie siê komunikat o b³êdzie
#endif

	// Kompilujemy shader znaleziony w pliku
	D3DX11CompileFromFile( file_name.c_str( ), 0, 0, shader_name.c_str( ), shader_model,
						   0, 0, 0, &compiled_shader,
#ifdef _DEBUG
						   &error_blob,	// Funkcja wsadzi informacje o b³êdzie w to miejsce
#else
						   0,	// W trybie Release nie chcemy dostawaæ b³êdów
#endif
						   &result );

	if ( FAILED( result ) )
	{
#ifdef _DEBUG
		if ( error_blob )
		{
			// B³¹d zostanie wypisany na standardowe wyjœcie
			OutputDebugStringA( (char*)error_blob->GetBufferPointer( ) );
			error_blob->Release( );
		}
#endif
		return nullptr;
	}

	// Tworzymy obiekt shadera na podstawie tego co sie skompilowa³o
	result = device->CreateVertexShader( compiled_shader->GetBufferPointer( ),
										 compiled_shader->GetBufferSize( ),
										 nullptr, &vertex_shader );

	if ( FAILED( result ) )
	{
		compiled_shader->Release( );
		return nullptr;
	}

	// Tworzymy obiekt nadaj¹cy siê do u¿ycia w silniku i zwracamy wskaŸnik na niego
	VertexShaderObject* new_shader = new VertexShaderObject( vertex_shader );
	return new_shader;
}

/**@brief Tworzy obiekt VertexShaderObject na podstawie pliku. Zwraca równie¿ layout dla podanej struktury wierzcho³ka.
Nie nale¿y u¿ywaæ tej funkcji, je¿eli layout nie jest rzeczywiœcie potrzebny. Trzeba pamietaæ o zwolnieniu
go, kiedy przestanie byæ potrzebny.

W przypadku b³êdów kompilacji w trybie debug s¹ one przekierowane do okna Output.

Na razie obs³uguje tylko nieskompilowane pliki.
@param[in] file_name Nazwa pliku, z którego zostanie wczytany shader
@param[in] shader_name Nazwa funkcji, która jest punktem poczatkowym wykonania shadera
@param[out] layout W zmiennej umieszczany jest wskaŸnik na layout wierzcho³ka. Nale¿y pamiêtaæ o zwolnieniu go kiedy bêdzie niepotrzebny.
@param[in] layout_desc Deskryptor opisujacy tworzony layout.
@param[in] array_size Liczba elementów tablicy layout_desc.
@param[in] shader_model £añcuch znaków opisuj¹cy shader model.
@return Zwraca wskaŸnik na obiekt VertexShaderObject lub nullptr w przypadku niepowodzenia.*/
VertexShaderObject* VertexShaderObject::create_from_file( const std::wstring& file_name,
														  const std::string& shader_name,
														  ID3D11InputLayout** layout,
														  D3D11_INPUT_ELEMENT_DESC* layout_desc,
														  unsigned int array_size,
														  const char* shader_model )
{
	HRESULT result;
	ID3DBlob* compiled_shader;
	ID3D11VertexShader* vertex_shader;
	// Troszkê zamieszania, ale w trybie debug warto wiedzieæ co jest nie tak przy kompilacji shadera
#ifdef _DEBUG
	ID3D10Blob* error_blob = nullptr;	// Tu znajdzie siê komunikat o b³êdzie
#endif

	// Kompilujemy shader znaleziony w pliku
	D3DX11CompileFromFile( file_name.c_str( ), 0, 0, shader_name.c_str( ), shader_model,
						   0, 0, 0, &compiled_shader,
#ifdef _DEBUG
						   &error_blob,	// Funkcja wsadzi informacje o b³êdzie w to miejsce
#else
						   0,	// W trybie Release nie chcemy dostawaæ b³êdów
#endif
						   &result );

	if ( FAILED( result ) )
	{
#ifdef _DEBUG
		if ( error_blob )
		{
			// B³¹d zostanie wypisany na standardowe wyjœcie
			OutputDebugStringA( (char*)error_blob->GetBufferPointer( ) );
			error_blob->Release( );
		}
#endif
		layout = nullptr;
		return nullptr;
	}

	// Tworzymy obiekt shadera na podstawie tego co sie skompilowa³o
	result = device->CreateVertexShader( compiled_shader->GetBufferPointer( ),
										 compiled_shader->GetBufferSize( ),
										 nullptr, &vertex_shader );

	if ( FAILED( result ) )
	{
		compiled_shader->Release();
		layout = nullptr;
		return nullptr;
	}

	// Tworzymy layout
	result = device->CreateInputLayout( layout_desc, array_size, compiled_shader->GetBufferPointer( ),
												compiled_shader->GetBufferSize( ), layout );
	compiled_shader->Release( );
	if ( FAILED( result ) )
	{
		layout = nullptr;
		return nullptr;
	}

	// Tworzymy obiekt nadaj¹cy siê do u¿ycia w silniku i zwracamy wskaŸnik na niego
	VertexShaderObject* new_shader = new VertexShaderObject( vertex_shader );
	return new_shader;
}
