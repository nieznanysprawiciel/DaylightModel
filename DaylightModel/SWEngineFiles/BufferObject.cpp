#include "stdafx.h"
#include "meshes_textures_materials.h"


//#include "..\..\memory_leaks.h"

//----------------------------------------------------------------------------------------------//
//								contructors, destructors										//
//----------------------------------------------------------------------------------------------//

BufferObject::BufferObject( unsigned int element_size )
: referenced_object( WRONG_ID )
{
	buffer = nullptr;
	stride = element_size;
}


BufferObject::BufferObject( unsigned int element_size, ID3D11Buffer* buff )
: referenced_object( WRONG_ID )
{
	buffer = buff;
	stride = element_size;
}


///@brief Zwalnia obiekt shadera z directXa
BufferObject::~BufferObject( )
{
	if ( buffer )
		buffer->Release( );
	//buffer = nullptr;
}

/**@brief Tworzy bufor wierzcho�k�w lub indeks�w o podanych parametrach.

@param[in] buffer Wska�nik na bufor z danym, kt�re maj� by� przeniesione do bufora DirectXowego.
@param[in] element_size Rozmiar pojedynczego elementu w buforze
@param[in] vert_count Liczba wierzcho�k�w/indeks�w w buforze
@param[in] bind_flag Okre�la z czym zwi�za� podany bufor w potoku graficznym.
Dla bufora wierzcho�k�w nale�y poda� D3D11_BIND_VERTEX_BUFFER, a dla bufora indeks�w
D3D11_BIND_INDEX_BUFFER.
@param[in] usage Jedna z warto�ci D3D11_USAGE, kt�r� nale�y poda� DirectXowi.
@return Wska�nik na BufferObject w przypadku powodzenia lub nullptr, je�eli co� p�jdzie nie tak.*/
BufferObject* BufferObject::create_from_memory( const void* buffer,
												unsigned int element_size,
												unsigned int vert_count,
												unsigned int bind_flag,
												D3D11_USAGE usage )
{
	// Wype�niamy deskryptor bufora
	D3D11_BUFFER_DESC buffer_desc;
	ZeroMemory( &buffer_desc, sizeof(buffer_desc) );
	buffer_desc.Usage = usage;
	buffer_desc.BindFlags = bind_flag;
	buffer_desc.ByteWidth = vert_count*element_size;

	D3D11_SUBRESOURCE_DATA InitData;
	ZeroMemory( &InitData, sizeof(InitData) );
	InitData.pSysMem = buffer;


	HRESULT result;
	ID3D11Buffer* new_buffer;
	result = device->CreateBuffer( &buffer_desc, &InitData, &new_buffer );
	if ( FAILED( result ) )
		return nullptr;

	BufferObject* new_buffer_object = new BufferObject(  element_size, new_buffer );
	return new_buffer_object;
}
