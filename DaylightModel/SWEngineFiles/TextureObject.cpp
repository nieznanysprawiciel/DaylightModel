#include "stdafx.h"
#include "meshes_textures_materials.h"

//#include "..\..\memory_leaks.h"

//----------------------------------------------------------------------------------------------//
//								contructors, destructors										//
//----------------------------------------------------------------------------------------------//

TextureObject::TextureObject(unsigned int id)
	: referenced_object(id)
{
	texture = nullptr;
}


/*
//do wywalenia
TextureObject::TextureObject( unsigned int id, const std::wstring& path, LPDIRECT3DTEXTURE9 tex )
: referenced_object( id )
{
	file_path = path;
	texture = tex;
}
*/

TextureObject::TextureObject( unsigned int id, const std::wstring& path, ID3D11ShaderResourceView* tex )
	: referenced_object(id)
{
	file_path = path;
	texture = tex;
}


///@brief Zwalnia obiekt shadera z directXa
TextureObject::~TextureObject()
{
	if (texture != nullptr)
		texture->Release();
}


/**@brief Porównuje ze soba dwa obiekty tekstur.

Obiekty s¹ takie same, kiedy odwo³uj¹ siê do tego samego pliku.
*/
bool TextureObject::operator==(const TextureObject& object)
{
	if (this->file_path == object.file_path)
		return TRUE;
	return FALSE;
}

/**@brief Porównuje ze soba dwa obiekty tekstur.

Obiekty s¹ takie same, kiedy odwo³uj¹ siê do tego samego pliku.
*/
bool TextureObject::operator==(const std::wstring& file_name)
{
	if (this->file_path == file_name)
		return TRUE;
	return FALSE;
}

/** @brief Tworzy z podanego pliku obiekt tekstury o identyfikatorze z pola id.

Generalnie ta funkcja nadaje siê do u¿ywania wielow¹tkowego. Obiekt directXa
ID3D11Device jest synchronizowany wewnêtrznie i nie trzeba siê o to martwiæ.
Z tego wzglêdu mo¿na tworzyæ wiele ró¿nych zasobów jednoczeœnie.

W przeciwieñstwie do ID3D11Device, ID3D11DeviceContext nie jest synchronizowany,
wiêc nie mo¿na renderowaæ z wielu w¹tków jednoczeœnie przy u¿yciu jednego obiektu.

@param[in] file_name Nazwa pliku zawieraj¹cego teksturê
@return Zawraca stworzony wewn¹trz obiekt TextureObject z wczytan¹ tekstur¹ lub nullptr w przypadku niepowodzenia.
*/
TextureObject* TextureObject::create_from_file( const std::wstring& file_name )
{
	ID3D11ShaderResourceView* texture_view;
	HRESULT result = D3DX11CreateShaderResourceViewFromFile( device, file_name.c_str(),
															 NULL, NULL,
															 &texture_view, NULL );
	if ( FAILED( result ) )
		return nullptr;

	TextureObject* texture = new TextureObject( WRONG_ID, file_name, texture_view );

	return texture;
}


