#pragma once

/**@file SkyDome.h
@brief Plik zawiera deklracjê interfejsu SkyDome, po którym dziedzicz¹ klasy obs³uguj¹ce 
obiekty typu skyboxy, skydome'y i wszystkie inne twory, wyœwietlaj¹ce t³o.*/


#include "meshes_textures_materials.h"
#include "ModelsManager.h"

#include <mutex>
#include <condition_variable> 


/**@brief Klasa jest interfejsem dla wszystkich klas, które maj¹ wyœwietlaæ jakieœ t³o.

Interfejs zawiera jedynie metody do komunikacji z klasa DisplayEngine. Poza tym nie ma koniecznoœci
tworzenia jednolitego interfejsu. Nawet nie jest to do koñca mo¿liwe, bo u¿ytkownik musi podaæ do inicjacji
dane specyficzne dla konkretnego modelu nieba.

Implementuj¹c w³asn¹ klasê, trzeba pamiêtaæ, ¿e po dodaniu do DisplayEngine, obiekt musi byæ w ka¿dej chwili gotowy
do wyœwietlania. Trzeba tak¿e uwa¿aæ na wielowatkowoœæ, aby nie wystêpowa³y niespójnoœci w danych. Klasa DisplayEngine
nie bêdzie u¿ywa³a ¿adnych narzêdzi do synchronizacji, wiêc jedyne istniej¹ce, to te wbudowane w bufory.

W strukturze ModelPart pole mesh jest zainicjowane zaalokowan¹ struktur¹ MeshPartObject i jest równie¿ zwalaniane
w destruktorze klasy SkyDome.

Polecam zapoznaæ siê z opisem funkcji DisplayEngine::display_sky_box w celu lepszego zorientowania siê
co siê dzieje podczas wyœwietlania kopu³y.

@note U¿ywaj¹c funkcji przeznaczonych do wype³niania poszczególnych pól klasy, zwalniamy z siebie obowi¹zek
dodawania odwo³añ. Funkcje pilnuj¹ tak¿e, ¿eby kasowaæ odwo³ania w przypadku, gdy jakieœ pole zosta³o nadpisane.
Wszystkie odwo³ania do obiektów s¹ kasowane w destruktorze, wiêc nie trzeba siê wogóle przejmowaæ ¿adnymi odwo³aniami.
*/
class SkyDome : protected DX11_interfaces_container
{
protected:
	bool				update_vertex_buffer;					///<Ustawiane w momencie zakoñczenia generowania nieba, w celu zaktualizowania bufora
private:
	ModelsManager*		models_manager;			///<WskaŸnik na ModelsManager. Bufory s¹ zarz¹dzane przez niego.

	std::mutex					mutex;
	std::condition_variable		condition;
protected:
	BufferObject*		vertex_buffer;			///<Bufor wierzcho³ków
	BufferObject*		index_buffer;			///<Bufor indeksów
	ID3D11Buffer*		constant_buffer;		///<Bufor sta³ych dla shaderów
	ID3D11InputLayout*	layout;					///<Layout wierzcho³ków u¿ywanych przez klasê
	ModelPart			display_data;			///<Shadery, tekstury itp.

public:
	SkyDome( ModelsManager* man )
		: models_manager( man ),
		vertex_buffer( nullptr ),
		index_buffer( nullptr ),
		constant_buffer( nullptr ),
		layout( nullptr )
	{
		display_data.mesh = new MeshPartObject;
		update_vertex_buffer = false;
	}

	virtual ~SkyDome()
	{
		if ( vertex_buffer )
			vertex_buffer->delete_object_reference(), vertex_buffer = nullptr;
		if ( index_buffer )
			index_buffer->delete_object_reference(), index_buffer = nullptr;
		if ( layout )
			layout->Release(), layout = nullptr;
		if ( constant_buffer )
			constant_buffer->Release(), constant_buffer = nullptr;

		if ( display_data.mesh )
			delete display_data.mesh, display_data.mesh = nullptr;
		if ( display_data.pixel_shader )
			display_data.pixel_shader->delete_object_reference(), display_data.pixel_shader = nullptr;
		if ( display_data.vertex_shader )
			display_data.vertex_shader->delete_object_reference(), display_data.vertex_shader = nullptr;
		if ( display_data.material )
			display_data.material->delete_object_reference(), display_data.material = nullptr;
		for ( int i = 0; i < ENGINE_MAX_TEXTURES; ++i )
			if ( display_data.texture[i] )
				display_data.texture[i]->delete_object_reference(), display_data.texture[i] = nullptr;
	}

	inline BufferObject*			get_vertex_buffer()				{ return vertex_buffer; }			///<Zwraca bufor wierzcho³ków
	inline BufferObject*			get_index_buffer()				{ return index_buffer; }			///<Zwraca bufor indeksów
	inline ID3D11Buffer*			get_constant_buffer()			{ return constant_buffer; }			///<Zwraca bufor sta³ych dla shaderów
	inline ID3D11InputLayout*		get_vertex_layout()				{ return layout; }					///<Zwraca layout wierzcho³ka
	inline ModelPart*				get_model_part()				{ return &display_data; }			///<Zwraca dane potrzebne do wyœwietlania

	inline bool						update_ready()					{ return update_vertex_buffer; }	///<Pozwlaa sprawdziæ czy mo¿na ju¿ updatowaæ bufory.
	inline void						wait_for_update()													///<Zawiesza w¹tek w oczekiwaniu a¿ bufory zostan¹ zapdatowane.
	{
		std::unique_lock<std::mutex> uniqueLock( mutex );
		while( update_vertex_buffer )
			condition.wait( uniqueLock );
	}
	/**Ma zostaæ wywo³ane na koñcu funkcji update_buffers. Informuje w¹tek generuj¹cy dane, ¿e bufory zosta³y ju¿ przepisane
	i mo¿na generowaæ kolejn¹ klatkê.*/
	inline void						signal()
	{
		std::unique_lock<std::mutex> uniqueLock( mutex );
		update_vertex_buffer = false;
		condition.notify_all();
	}

	/**@brief Funkcja jest wywo³ywana w momencie kiedy zmienna update_vertex_buffer zawiera wartoœæ true.
	
	Zadaniem funkcji jest wype³nienie bufora indeksów lub wierzcho³ków nowymi danymi, które zosta³y przygotowane
	gdzieœ w buforach tylnich. Dane powinny byæ generowane inn¹ funkcj¹, której nie ma w tym interfejsie, przez
	jak¹œ funkcjê spoza klasy DisplayEngine i najlepiej w innym w¹tku. Zadaniem tej funkcji jest jedynie
	wywo³anie funkcji UpdateSubresource na tych buforach, które wymagaj¹ aktualizacji.
	
	@attention Nie nale¿y generowaæ danych w tej funkcji chyba, ¿e jest ich naprawdê niewiele. Ta funkcja
	bêdzie wywo³ywana w pêtli g³ównej i mo¿e zaburzyæ renderowanie.
	
	Pole update_vertex_buffer powinno zostaæ ustawione na false po wykonaniu tej funkcji.*/
	virtual void update_buffers() = 0;
protected:
	// Funkcje do ustawiania

	/**@brief Funkcja ustawia w zmiennej display_data teksturê.
	Tekstura jest pobierana z ModelsManagera, je¿eli nie istnieje jest dodawana.
	
	@param[in] name Nazwa pliku z tekstur¹.
	@param[in] type Pozycja w tablicy, do której zostaniej przypisana tekstura.*/
	inline void set_texture( const std::wstring& name, TEXTURES_TYPES type )
	{
		if ( type > ENGINE_MAX_TEXTURES )
			return;

		auto tex = models_manager->add_texture( name );
		if ( !tex )
			return;	// Zabezpieczenie przed nullptrem

		if ( display_data.texture[type] )	// Je¿eli coœ wczeœniej by³o to kasujemy odwo³anie
			display_data.texture[type]->delete_object_reference();

		tex->add_object_reference();
		display_data.texture[type] = tex;
	}
	/**@brief Funkcja ustawia w zmiennej display_data vertex shader.
	Shader jest pobierany z ModelsManagera, je¿eli nie istnieje jest dodawany.

	@param[in] file_name Nazwa pliku z shaderem.
	@param[in] shader_entry Nazwa funkcji shadera.
	@param[in] layout_desc Deskryptor formatu wierzcho³ka.
	@param[in] array_size Liczba elementów tablicy layout_desc.*/
	inline void set_vertex_shader( const std::wstring& file_name,
								   const std::string& shader_entry,
								   D3D11_INPUT_ELEMENT_DESC* layout_desc,
								   unsigned int array_size )
	{
		if ( layout )	// Zwalniamy layout, je¿eli jakiœ by³
			layout->Release();

		auto shader = models_manager->add_vertex_shader( file_name, shader_entry, &layout, layout_desc, array_size );
		if ( !shader )
			return;	// Zabezpieczenie przed nullptrem
		if ( !layout )
			return;	// Je¿eli layout siê nie chcia³ stworzyæ to lepiej nie dodawaæ te¿ vertex shadera

		if ( display_data.vertex_shader )	// Je¿eli coœ wczeœniej by³o to kasujemy odwo³anie
			display_data.vertex_shader->delete_object_reference( );

		shader->add_object_reference( );
		display_data.vertex_shader = shader;
	}
	/**@brief Funkcja ustawia w zmiennej display_data pixel shader.
	Shader jest pobierany z ModelsManagera, je¿eli nie istnieje jest dodawany.

	@param[in] file_name Nazwa pliku z shaderem.
	@param[in] shader_entry Nazwa funkcji shadera.*/
	inline void set_pixel_shader( const std::wstring& file_name, const std::string& shader_entry )
	{
		auto shader = models_manager->add_pixel_shader( file_name, shader_entry );
		if ( !shader )
			return;	// Zabezpieczenie przed nullptrem

		if ( display_data.pixel_shader )	// Je¿eli coœ wczeœniej by³o to kasujemy odwo³anie
			display_data.pixel_shader->delete_object_reference( );

		shader->add_object_reference( );
		display_data.pixel_shader = shader;
	}
	/**@brief Funkcja ustawia w zmiennej display_data materia³.
	Materia³ jest pobierany z ModelsManagera, je¿eli nie istnieje jest dodawany.

	@param[in] material Materia³, który ma zostaæ dodany
	@param[in] material_name Nazwa materia³u. Do materia³u bêdzie mo¿na siê odwo³aæ podaj¹c ci¹g znaków*/
	inline void set_material( MaterialObject* mat, const std::wstring& name)
	{
		auto material = models_manager->add_material( mat, name );
		if ( !material )
			return;	// Zabezpieczenie przed nullptrem

		if ( display_data.material )	// Je¿eli coœ wczeœniej by³o to kasujemy odwo³anie
			display_data.material->delete_object_reference( );

		material->add_object_reference( );
		display_data.material = material;
	}
	/**@brief Funkcja tworzy, a potem ustawia w zmiennej vertex_buffer bufor wierzcho³ków.
	Bufor jest pobierany z ModelsManagera, je¿eli nie istnieje jest dodawany.

	@param[in] name Nazwa bufora, po której mo¿na siê bêdzie odwo³aæ.
	@param[in] buffer WskaŸnik na bufor z danym, które maj¹ byæ przeniesione do bufora DirectXowego.
	@param[in] element_size Rozmiar pojedynczego elementu w buforze.
	@param[in] vert_count Liczba wierzcho³ków/indeksów w buforze.*/
	inline void set_vertex_buffer( const std::wstring& name, const void* buffer, unsigned int element_size, unsigned int vert_count )
	{
		auto buff = models_manager->add_vertex_buffer( name, buffer, element_size, vert_count );
		if ( !buff )
			return;	// Zabezpieczenie przed nullptrem

		if ( vertex_buffer )	// Je¿eli coœ wczeœniej by³o to kasujemy odwo³anie
			vertex_buffer->delete_object_reference( );

		buff->add_object_reference( );
		vertex_buffer = buff;
	}
	/**@brief Funkcja tworzy, a potem ustawia w zmiennej index_buffer bufor indeksów.
	Bufor jest pobierany z ModelsManagera, je¿eli nie istnieje jest dodawany.

	@param[in] name Nazwa bufora, po której mo¿na siê bêdzie odwo³aæ.
	@param[in] buffer WskaŸnik na bufor z danym, które maj¹ byæ przeniesione do bufora DirectXowego.
	@param[in] element_size Rozmiar pojedynczego elementu w buforze.
	@param[in] vert_count Liczba wierzcho³ków/indeksów w buforze.*/
	inline void set_index_buffer( const std::wstring& name, const void* buffer, unsigned int element_size, unsigned int vert_count )
	{
		auto buff = models_manager->add_index_buffer( name, buffer, element_size, vert_count );
		if ( !buff )
			return;	// Zabezpieczenie przed nullptrem

		if ( index_buffer )	// Je¿eli coœ wczeœniej by³o to kasujemy odwo³anie
			index_buffer->delete_object_reference( );

		buff->add_object_reference( );
		index_buffer = buff;
	}
};
