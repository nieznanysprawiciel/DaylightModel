#pragma once

/**@file ModelsManager.h
Plik zawiera deklaracjê klasy ModelsManager.
*/

#include "stdafx.h"
#include "meshes_textures_materials.h"
#include "ResourceContainer.h"

//class Engine;
class Loader;



// Nazwy dla domyœlnych shaderów i materia³u
const wchar_t DEFAULT_MATERIAL_STRING[] = L"::default_material";			///<Neutralny materia³
const wchar_t DEFAULT_VERTEX_SHADER_STRING[] = L"default_vertex_shader";	///<Shader bez obs³ugi tekstur
const wchar_t DEFAULT_PIXEL_SHADER_STRING[] = L"default_pixel_shader";		///<Shader bez obs³ugi tekstur
//const wchar_t DEFAULT_TEX_DIFFUSE_VERTEX_SHADER_PATH[] = L"shaders\\tex_diffuse_shader.fx";	///<Shader z obs³ug¹ tesktury diffuse
const wchar_t DEFAULT_TEX_DIFFUSE_PIXEL_SHADER_PATH[] = L"shaders\\tex_diffuse_shader.fx";	///<Shader z obs³ug¹ tesktury diffuse


// Domyœlne nazwy funkcji w vertex i pixel shaderze
const char DEFAULT_VERTEX_SHADER_ENTRY[] = "vertex_shader";		///<Domyœlna nazwa funkcji, od której zaczyna siê wykonanie vertex shadera
const char DEFAULT_PIXEL_SHADER_ENTRY[] = "pixel_shader";		///<Domyœlna nazwa funkcji, od której zaczyna siê wykonanie pixel shadera

//-------------------------------------------------------------------------------//
//							wersja DirectX11
//-------------------------------------------------------------------------------//

/**@brief Zawiera wyniki mo¿liwe do zwrócenia przez ModelsManager.*/
typedef enum MODELS_MANAGER_RESULT
{
	MODELS_MANAGER_OK,					///<Poprawne wykonanie funkcji
	MODELS_MANAGER_LOADER_NOT_FOUND,	///<Nie znaleziono loadera do podanego typu pliku
	MODELS_MANAGER_CANNOT_LOAD			///<Nie da siê wczytaæ pliku
};


/**@brief Klasa przechowuje wszystkie obiekty zasobów w silniku (oprócz dŸwiêków, które raczej
zostan¹ oddelegowane w inne miejsce).

Zasoby s¹ identyfikowane jednoznacznym identyfikatorem, który jest unikalny
jedynie w danej grupie zasobów, lub te¿ nazw¹ pliku na podstawie, którego dany zasób powsta³.

Materia³y s¹ specyficzne, poniewa¿ jeden plik z programu do modelowania raczej zawiera
wiele ró¿nych materia³ów. ¯eby je mo¿na by³o w ten sposób jednoznacznie identyfikowaæ,
do nazwy pliku doklejane s¹ dwa dwukropki :: i nazwa materia³u, jaka zosta³a mu nadana
w pliku, z którego pochodzi.
Oczywiœcie, je¿eli materia³ zosta³ stworzony rêcznie w silniku, to nie ma potrzeby
nadawania mu takiej nazwy, wystarczy, ¿eby by³a ona unikatowa.*/
class ModelsManager
{
	friend Model3DFromFile;
private:
	//Engine* engine;			///<WskaŸnik na obiekt g³ówny silnika

	ResourceContainer<VertexShaderObject>		vertex_shader;		///<Vertex shadery
	ResourceContainer<PixelShaderObject>		pixel_shader;		///<Pixel shadery
	ResourceContainer<TextureObject>			texture;			///<Tekstury
	ResourceContainer<BufferObject>				vertex_buffer;		///<Bufory wierzcho³ków
	ResourceContainer<BufferObject>				index_buffer;		///<Bufory indeksów
	ResourceContainer<MaterialObject>			material;			///<Materia³y
	// UWAGA! file_model musi byæ na koñcu. Jego destruktor kasuje odwo³ania do obiektów powy¿ej.
	// Musz¹ one w tym czasie istnieæ, a destruktory s¹ wywo³ywane w kolejnoœci odwrotnej do zadeklarowanej w klasie.
	ResourceContainer<Model3DFromFile>			file_model;			///<Obiekty modeli 3D z plików

	/*loadery dla ró¿nych formatów plików z modelami*/
	std::vector<Loader*>			loader;			///<Loadery do plików z modelami 3D

public:
	ModelsManager(/* Engine* engine */);
	~ModelsManager( );

	// Funkcje pomocnicze
	VertexShaderObject* find_best_vertex_shader( TextureObject** textures );
	PixelShaderObject* find_best_pixel_shader( TextureObject** textures );
	void set_default_assets( ID3D11VertexShader* vert_shader, ID3D11PixelShader* pix_shader );


	// Funkcje do zarz¹dzania assetami
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


