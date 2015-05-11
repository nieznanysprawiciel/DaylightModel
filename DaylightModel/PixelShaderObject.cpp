#include "stdafx.h"
#include "meshes_textures_materials.h"

//#include "..\..\memory_leaks.h"
//----------------------------------------------------------------------------------------------//
//								contructors, destructors										//
//----------------------------------------------------------------------------------------------//

PixelShaderObject::PixelShaderObject()
: referenced_object( WRONG_ID )
{
	pixel_shader = nullptr;
}


///@brief Zwalnia obiekt shadera z directXa
PixelShaderObject::~PixelShaderObject( )
{
	if ( pixel_shader )
		pixel_shader->Release( );
}

/**@bref Tworzy obiekt PixelShaderObject na podstawie pliku.

W przypadku b³êdów kompilacji w trybie debug s¹ one przekierowane do okna Output.

Na razie obs³uguje tylko nieskompilowane pliki.
@param[in] file_name Nazwa pliku, z którego zostanie wczytany shader.
@param[in] shader_name Nazwa funkcji, która jest punktem poczatkowym wykonania shadera.
@param[in] shader_model £añcuch znaków opisuj¹cy shader model.
*/
PixelShaderObject* PixelShaderObject::create_from_file( const std::wstring& file_name, const std::string& shader_name, const char* shader_model )
{
	HRESULT result;
	ID3DBlob* compiled_shader;
	ID3D11PixelShader* pixel_shader;
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
	result = device->CreatePixelShader( compiled_shader->GetBufferPointer( ),
										 compiled_shader->GetBufferSize( ),
										 nullptr, &pixel_shader );

	if ( FAILED( result ) )
	{
		compiled_shader->Release( );
		return nullptr;
	}

	// Tworzymy obiekt nadaj¹cy siê do u¿ycia w silniku i zwracamy wskaŸnik na niego
	PixelShaderObject* new_shader = new PixelShaderObject( pixel_shader );
	return new_shader;
}
