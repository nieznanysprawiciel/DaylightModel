#pragma once

/**@file ModelsManager.h
Plik zawiera deklaracj� klasy ModelsManager.
*/

#include "stdafx.h"
#include "meshes_textures_materials.h"
#include "ResourceContainer.h"

//class Engine;
class Loader;



// Nazwy dla domy�lnych shader�w i materia�u
const wchar_t DEFAULT_MATERIAL_STRING[] = L"::default_material";			///<Neutralny materia�
const wchar_t DEFAULT_VERTEX_SHADER_STRING[] = L"default_vertex_shader";	///<Shader bez obs�ugi tekstur
const wchar_t DEFAULT_PIXEL_SHADER_STRING[] = L"default_pixel_shader";		///<Shader bez obs�ugi tekstur
//const wchar_t DEFAULT_TEX_DIFFUSE_VERTEX_SHADER_PATH[] = L"shaders\\tex_diffuse_shader.fx";	///<Shader z obs�ug� tesktury diffuse
const wchar_t DEFAULT_TEX_DIFFUSE_PIXEL_SHADER_PATH[] = L"shaders\\tex_diffuse_shader.fx";	///<Shader z obs�ug� tesktury diffuse


// Domy�lne nazwy funkcji w vertex i pixel shaderze
const char DEFAULT_VERTEX_SHADER_ENTRY[] = "vertex_shader";		///<Domy�lna nazwa funkcji, od kt�rej zaczyna si� wykonanie vertex shadera
const char DEFAULT_PIXEL_SHADER_ENTRY[] = "pixel_shader";		///<Domy�lna nazwa funkcji, od kt�rej zaczyna si� wykonanie pixel shadera

//-------------------------------------------------------------------------------//
//							wersja DirectX11
//-------------------------------------------------------------------------------//

/**@brief Zawiera wyniki mo�liwe do zwr�cenia przez ModelsManager.*/
typedef enum MODELS_MANAGER_RESULT
{
	MODELS_MANAGER_OK,					///<Poprawne wykonanie funkcji
	MODELS_MANAGER_LOADER_NOT_FOUND,	///<Nie znaleziono loadera do podanego typu pliku
	MODELS_MANAGER_CANNOT_LOAD			///<Nie da si� wczyta� pliku
};


/**@brief Klasa przechowuje wszystkie obiekty zasob�w w silniku (opr�cz d�wi�k�w, kt�re raczej
zostan� oddelegowane w inne miejsce).

Zasoby s� identyfikowane jednoznacznym identyfikatorem, kt�ry jest unikalny
jedynie w danej grupie zasob�w, lub te� nazw� pliku na podstawie, kt�rego dany zas�b powsta�.

Materia�y s� specyficzne, poniewa� jeden plik z programu do modelowania raczej zawiera
wiele r�nych materia��w. �eby je mo�na by�o w ten spos�b jednoznacznie identyfikowa�,
do nazwy pliku doklejane s� dwa dwukropki :: i nazwa materia�u, jaka zosta�a mu nadana
w pliku, z kt�rego pochodzi.
Oczywi�cie, je�eli materia� zosta� stworzony r�cznie w silniku, to nie ma potrzeby
nadawania mu takiej nazwy, wystarczy, �eby by�a ona unikatowa.*/
class ModelsManager
{
	friend Model3DFromFile;
private:
	//Engine* engine;			///<Wska�nik na obiekt g��wny silnika

	ResourceContainer<VertexShaderObject>		vertex_shader;		///<Vertex shadery
	ResourceContainer<PixelShaderObject>		pixel_shader;		///<Pixel shadery
	ResourceContainer<TextureObject>			texture;			///<Tekstury
	ResourceContainer<BufferObject>				vertex_buffer;		///<Bufory wierzcho�k�w
	ResourceContainer<BufferObject>				index_buffer;		///<Bufory indeks�w
	ResourceContainer<MaterialObject>			material;			///<Materia�y
	// UWAGA! file_model musi by� na ko�cu. Jego destruktor kasuje odwo�ania do obiekt�w powy�ej.
	// Musz� one w tym czasie istnie�, a destruktory s� wywo�ywane w kolejno�ci odwrotnej do zadeklarowanej w klasie.
	ResourceContainer<Model3DFromFile>			file_model;			///<Obiekty modeli 3D z plik�w

	/*loadery dla r�nych format�w plik�w z modelami*/
	std::vector<Loader*>			loader;			///<Loadery do plik�w z modelami 3D

public:
	ModelsManager(/* Engine* engine */);
	~ModelsManager( );

	// Funkcje pomocnicze
	VertexShaderObject* find_best_vertex_shader( TextureObject** textures );
	PixelShaderObject* find_best_pixel_shader( TextureObject** textures );
	void set_default_assets( ID3D11VertexShader* vert_shader, ID3D11PixelShader* pix_shader );


	// Funkcje do zarz�dzania assetami
	MODELS_MANAGER_RESULT load_model_from_file( const std::wstring& file );

	inline Model3DFromFile* get_model( const std::wstring& name ) { return file_model.get( name ); }	///<Zwraca model z pliku o podanej nazwie
	TextureObject* add_texture( const std::wstring& file_name );
	MaterialObject* add_material( const MaterialObject* material, const std::wstring& material_name );
	VertexShaderObject* add_vertex_shader( const std::wstring& file_name, const std::string& shader_entry );
	VertexShaderObject* add_vertex_shader( const std::wstring& file_name, const std::string& shader_entry, ID3D11InputLayout** layout, D3D11_INPUT_ELEMENT_DESC* layout_desc, unsigned int array_size );
	PixelShaderObject* add_pixel_shader( const std::wstring& file_name, const std::string& shader_entry );
	BufferObject* add_vertex_buffer( const std::wstring& name, const void* buffer, unsigned int element_size, unsigned int vert_count );
	BufferObject* add_index_buffer( const std::wstring& name, const void* buffer, unsigned int element_size, unsigned int vert_count );
private:
	Loader* find_loader( const std::wstring& path );
	

#ifdef __TEST
public:
	void test( );
#endif
};


