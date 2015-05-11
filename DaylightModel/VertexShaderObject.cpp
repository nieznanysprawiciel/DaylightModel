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

W przypadku b��d�w kompilacji w trybie debug s� one przekierowane do okna Output.

Na razie obs�uguje tylko nieskompilowane pliki.
@param[in] file_name Nazwa pliku, z kt�rego zostanie wczytany shader
@param[in] shader_name Nazwa funkcji, kt�ra jest punktem poczatkowym wykonania shadera
@param[in] shader_model �a�cuch znak�w opisuj�cy shader model.
@return Zwraca wska�nik na obiekt VertexShaderObject lub nullptr w przypadku niepowodzenia.*/
VertexShaderObject* VertexShaderObject::create_from_file( const std::wstring& file_name, const std::string& shader_name, const char* shader_model )
{
	HRESULT result;
	ID3DBlob* compiled_shader;
	ID3D11VertexShader* vertex_shader;
	// Troszk� zamieszania, ale w trybie debug warto wiedzie� co jest nie tak przy kompilacji shadera
#ifdef _DEBUG
	ID3D10Blob* error_blob = nullptr;	// Tu znajdzie si� komunikat o b��dzie
#endif

	// Kompilujemy shader znaleziony w pliku
	D3DX11CompileFromFile( file_name.c_str( ), 0, 0, shader_name.c_str( ), shader_model,
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
			OutputDebugStringA( (char*)error_blob->GetBufferPointer( ) );
			error_blob->Release( );
		}
#endif
		return nullptr;
	}

	// Tworzymy obiekt shadera na podstawie tego co sie skompilowa�o
	result = device->CreateVertexShader( compiled_shader->GetBufferPointer( ),
										 compiled_shader->GetBufferSize( ),
										 nullptr, &vertex_shader );

	if ( FAILED( result ) )
	{
		compiled_shader->Release( );
		return nullptr;
	}

	// Tworzymy obiekt nadaj�cy si� do u�ycia w silniku i zwracamy wska�nik na niego
	VertexShaderObject* new_shader = new VertexShaderObject( vertex_shader );
	return new_shader;
}

/**@brief Tworzy obiekt VertexShaderObject na podstawie pliku. Zwraca r�wnie� layout dla podanej struktury wierzcho�ka.
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
@return Zwraca wska�nik na obiekt VertexShaderObject lub nullptr w przypadku niepowodzenia.*/
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
	// Troszk� zamieszania, ale w trybie debug warto wiedzie� co jest nie tak przy kompilacji shadera
#ifdef _DEBUG
	ID3D10Blob* error_blob = nullptr;	// Tu znajdzie si� komunikat o b��dzie
#endif

	// Kompilujemy shader znaleziony w pliku
	D3DX11CompileFromFile( file_name.c_str( ), 0, 0, shader_name.c_str( ), shader_model,
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
			OutputDebugStringA( (char*)error_blob->GetBufferPointer( ) );
			error_blob->Release( );
		}
#endif
		layout = nullptr;
		return nullptr;
	}

	// Tworzymy obiekt shadera na podstawie tego co sie skompilowa�o
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

	// Tworzymy obiekt nadaj�cy si� do u�ycia w silniku i zwracamy wska�nik na niego
	VertexShaderObject* new_shader = new VertexShaderObject( vertex_shader );
	return new_shader;
}
