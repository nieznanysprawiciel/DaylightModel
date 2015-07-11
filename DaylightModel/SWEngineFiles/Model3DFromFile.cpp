#include "stdafx.h"

#include "meshes_textures_materials.h"
#include "ModelsManager.h"

ModelsManager* Model3DFromFile::models_manager = nullptr;


//#include "..\..\memory_leaks.h"

//-------------------------------------------------------------------------------//
//							wersja DirectX11
//-------------------------------------------------------------------------------//




//----------------------------------------------------------------------------------------------//
//									referenced_object											//
//----------------------------------------------------------------------------------------------//

//==============================================================================================//

/**
Ustawia zerow� liczb� odwo�a�.*/
referenced_object::referenced_object(int id)
{
	file_references = 0;
	object_references = 0;
	unique_id = id;
}


referenced_object::~referenced_object()
{}

/** \brief Funkcja informuje czy obiekt s� obiektu, kt�re odwo�uj� si� do assetu.

@param[out] file_ref W zmiennej zostanie umieszczona liczba referencji plikowych.
@param[out] other_ref W zmiennej zostanie umieszczona liczba referencji bezpo�rednich od obiekt�w.
@return Zwraca warto�� logiczn� m�wi�c� czy asset nadaje si� do usuni�cia.
*/
bool referenced_object::can_delete(unsigned int& file_ref, unsigned int& other_ref)
{
	file_ref = file_references;
	other_ref = object_references;

	if (file_references == 0 && object_references == 0)
		return TRUE;
	return FALSE;
}

//----------------------------------------------------------------------------------------------//
//									Model3DFromFile												//
//----------------------------------------------------------------------------------------------//

//==============================================================================================//


//----------------------------------------------------------------------------------------------//
//								contructor, destructor											//
//----------------------------------------------------------------------------------------------//

/**@brief Inicjuje obiekt �cie�k� do pliku, kt�ry zostanie do niego wczytany ( nie wczytuje pliku).*/
Model3DFromFile::Model3DFromFile( const std::wstring& file_name )
: referenced_object( WRONG_ID )
{
	file_path = file_name;
	vertex_buffer = nullptr;
	index_buffer = nullptr;
	tmp_data = nullptr;
}


Model3DFromFile::~Model3DFromFile( )
{
	if ( tmp_data )
	{
		for ( int i = 0; i < tmp_data->current_pointer; ++i )
		{
			delete[] tmp_data->table[i]->indicies_tab;
			delete[] tmp_data->table[i]->vertices_tab;
			delete tmp_data->table[i]->new_part.mesh;
		}
		
		for ( int i = 0; i < tmp_data->current_pointer; ++i )
			delete tmp_data->table[i];
		delete[] tmp_data->table;
		delete tmp_data;
		tmp_data = nullptr;
	}

	// Kasujemy odwo�ania plikowe do bufor�w
	if ( vertex_buffer )
		vertex_buffer->delete_file_reference();
	if ( index_buffer )
		index_buffer->delete_file_reference();

	// Kasujemy odwo�ania plikowe do obiekt�w w talicach
	for ( unsigned int k = 0; k < model_parts.size( ); ++k )
	{//ka�dy element mo�e by� nullptrem
		register ModelPart* part = &model_parts[k];

		//ka�dy element mo�e by� nullptrem
		if ( part->material )
			part->material->delete_file_reference( );

		if ( part->mesh )
		{
			//kasujemy obiekty MeshParts (to s� jedyne, kt�rymi zarz�dza ta klasa)
			part->mesh->delete_file_reference();
			delete part->mesh;
		}
		if ( part->pixel_shader )
			part->pixel_shader->delete_file_reference( );
		for ( int i = 0; i < ENGINE_MAX_TEXTURES; ++i )
			if ( part->texture[i] )
				part->texture[i]->delete_file_reference( );
		if ( part->vertex_shader )
			part->vertex_shader->delete_file_reference( );
	}


	model_parts.clear( );
}




//----------------------------------------------------------------------------------------------//
//									Funkcje do tworzenia obiektu								//
//----------------------------------------------------------------------------------------------//


/**@brief BeginEdit przygotowuje struktur� pod wpisywanie danych.

Funkcje BeginEdit i EndEdit maj� by� wywo�ane przez klas� ModelsManager przed podaniem do loadera
i po zako�czeniu wpisywania danych.
*/
void Model3DFromFile::BeginEdit()
{
	tmp_data = new EditTMP;
	//Inicjujemy tablic� o pocz�tkowej d�ugo�ci domy�lnej
	tmp_data->table = new TMP_data*[tmp_data->table_length];
}


/**@brief EndEdit zatwierdza wpisane dane, tworzy w�a�ciwa struktur� i wpisuje nie istniej�ce jeszcze obiekty
do ModelsManagera.

Funkcje BeginEdit i EndEdit maj� by� wywo�ane przez klas� ModelsManager przed podaniem do loadera
i po zako�czeniu wpisywania danych.
*/
void Model3DFromFile::EndEdit()
{
	// Najpierw scalamy bufory wierzcho�k�w.
	EndEdit_vertex_buffer_processing();
	// Scalamy bufory indeks�w.
	EndEdit_index_buffer_processing();

	// Dodajemy do strutkury ModelPart dane o buforach wierzcho�k�w oraz przypisujemy wbudowane
	// shadery, je�eli nie podano �adnych
	EndEdit_prepare_ModelPart();

	// Zapisujemy gotowe struktury
	for ( int i = 0; i < tmp_data->current_pointer; ++i )
		model_parts.push_back( tmp_data->table[i]->new_part );

	//zwalniamy pami��
	for ( int i = 0; i < tmp_data->current_pointer; ++i )
		delete tmp_data->table[i];
	delete[] tmp_data->table;
	delete tmp_data;
	tmp_data = nullptr;
}


/**@brief Funkcje BeginPart i EndPart s� wywo�ywane przez loader i maj� otacza� wszystkie instrukcje dodaj�ce
jedn� cz�� mesha do struktury.*/
void Model3DFromFile::BeginPart()
{
	//sprawdzamy czy tablica nam si� nie sko�czy�a
	if ( tmp_data->current_pointer >= tmp_data->table_length )
	{//alokujemy troszke wi�cej pami�ci i przepisujemy
		unsigned int new_length = tmp_data->table_length << 1;

		TMP_data** new_table = new TMP_data*[new_length];
		for ( int i = 0; i < tmp_data->table_length; ++i )
			new_table[i] = tmp_data->table[i];

		tmp_data->table_length = new_length;
		delete[] tmp_data->table;
		tmp_data->table = new_table;
	}

	//dodajemy do tabeli nowy element
	tmp_data->table[tmp_data->current_pointer] = new TMP_data;
	tmp_data->table[tmp_data->current_pointer]->new_part.mesh = new MeshPartObject;

	//nie jest to konieczne, ale dla konwencji dodajemy
	tmp_data->table[tmp_data->current_pointer]->new_part.mesh->add_file_reference();
}

/**@brief Funkcje BeginPart i EndPart s� wywo�ywane przez loader i maj� otacza� wszystkie instrukcje dodaj�ce
jedn� cz�� mesha do struktury.*/
void Model3DFromFile::EndPart()
{
	//inkrementujemy aktualny index wewn�trz tablicy
	tmp_data->current_pointer++;
}



/**@brief Dodaje tekstur� na aktualnej pozycji w tablicy.

Tekstura jest umieszczana w tablicy ModelPart.texture zgodnie z indeksem reprezentowanym przez type.
Obiekt jest dodawany do ModelsManagera je�eli jeszcze nie istnia�. Je�eli istnia�
nie jest duplikowany, ale zamiast niego wstawia si� wska�nik na istniej�cy obiekt.
@param[in] file_name �cie�ka do tekstury
@param[in] type Type tekstury. Zobacz definicj� TEXTURES_TYPES.
@return Identyfiaktor tekstury lub WRONG_ID, je�eli co� posz�o nie tak.*/
unsigned int Model3DFromFile::add_texture( const std::wstring& file_name, TEXTURES_TYPES type )
{
	// Kto� m�g� poda� indeks w tablicy zamiast jednego z enum�w, wi�c musimy pilnowa� czy nie przekroczy� zakresu
	if ( type > ENGINE_MAX_TEXTURES )
		return WRONG_ID;

	TextureObject* texture = models_manager->texture.get( file_name );
	if ( !texture )
	{
		// Nie by�o tekstury, trzeba j� stworzy� i doda�
		texture = TextureObject::create_from_file( file_name );
		if ( !texture )		// Tekstura mog�a mie� z�y format, a nie chcemy dodawa� nullptra do ModelsManagera
			return WRONG_ID;

		models_manager->texture.unsafe_add( file_name, texture );	// Dodali�my tekstur�
	}

	// Teraz musimy doda� tekstur� na odpowiednie miejsce w tablicy
	tmp_data->table[tmp_data->current_pointer]->new_part.texture[type] = texture;
	// Dodajemy odwo�anie plikowe
	texture->add_file_reference();

	// Zwracamy id tekstury, ale w zasadzie tylko po to, �eby kto� m�g� sprawdzi� czy wszystko posz�o dobrze
	return texture->get_id();
}

/**@brief Dodaje materia� na aktualnej pozycji w tablicy.

Obiekt jest dodawany do ModelsManagera je�eli jeszcze nie istnia�. Je�eli istnia�
nie jest duplikowany, ale zamiast niego wstawia si� wska�nik na istniej�cy obiekt.

@param[in] material Materia�, kt�ry ma zosta� dodany
@param[in] material_name Nazwa materia�u. Do materia�u b�dzie mo�na si� odwo�a� podaj�c ci�g znak�w
[nazwa_pliku]::[nazwa_materia�u]. Oznacza to, �e mog� istnie� dwa takie same materia�y, poniewa� nie jest sprawdzana
zawarto��, a jedynie nazwy.
@return Indentyfikator obiektu.*/
unsigned int Model3DFromFile::add_material( const MaterialObject* material, const std::wstring& material_name )
{
	std::wstring name = file_path;
	name += L"::";
	name += material_name;

	MaterialObject* new_material = models_manager->material.get( name );
	if ( !new_material )
	{
		// Nie by�o materia�u, trzeba j� stworzy� i doda�
		new_material = new MaterialObject( material );

		models_manager->material.unsafe_add( name, new_material );	// Dodali�my tekstur�
	}

	// Teraz musimy doda� materia� na odpowiednie miejsce w tablicy
	tmp_data->table[tmp_data->current_pointer]->new_part.material = new_material;
	// Dodajemy odwo�anie plikowe
	new_material->add_file_reference( );

	// Zwracamy id materia�y, ale w zasadzie tylko po to, �eby kto� m�g� sprawdzi� czy wszystko posz�o dobrze
	// Akurat w przypadku materia��w nic nie mo�e p�j�� �le
	return new_material->get_id( );
}

/**@brief Dodaje vertex shader na aktualnej pozycji ModelPart.

Obiekt jest dodawany do ModelsManagera je�eli jeszcze nie istnia�. Je�eli istnia�
nie jest duplikowany, ale zamiast niego wstawia si� wska�nik na istniej�cy obiekt.

@param[in] file_name Nazwa pliku, w kt�rym znajduje si� vertex shader. Nazwa funkcji na razie jest niezmienna
i okre�la j� makro DEFAULT_VERTEX_SHADER_ENTRY.
@return Indentyfikator obiektu.*/
unsigned int Model3DFromFile::add_vertex_shader( const std::wstring& file_name )
{
	VertexShaderObject* vertex_shader = models_manager->vertex_shader.get( file_name );
	if ( !vertex_shader )
	{
		// Nie by�o shadera, trzeba go stworzy� i doda�
		vertex_shader = VertexShaderObject::create_from_file( file_name, DEFAULT_VERTEX_SHADER_ENTRY );
		if ( !vertex_shader )		// shader m�g� mie� z�y format, a nie chcemy dodawa� nullptra do ModelsManagera
			return WRONG_ID;

		models_manager->vertex_shader.unsafe_add( file_name, vertex_shader );	// Dodali�my tekstur�
	}

	// Teraz musimy doda� shader na odpowiednie miejsce w tablicy
	tmp_data->table[tmp_data->current_pointer]->new_part.vertex_shader = vertex_shader;
	// Dodajemy odwo�anie plikowe
	vertex_shader->add_file_reference( );

	// Zwracamy id shadera, ale w zasadzie tylko po to, �eby kto� m�g� sprawdzi� czy wszystko posz�o dobrze
	return vertex_shader->get_id( );
}

/**@brief Dodaje pixel shader na aktualnej pozycji ModelPart.

Obiekt jest dodawany do ModelsManagera je�eli jeszcze nie istnia�. Je�eli istnia�
nie jest duplikowany, ale zamiast niego wstawia si� wska�nik na istniej�cy obiekt.

@param[in] file_name Nazwa pliku, w kt�rym znajduje si� pixel shader. Nazwa funkcji na razie jest niezmienna
i okre�la j� makro DEFAULT_PIXEL_SHADER_ENTRY.
@return Indentyfikator obiektu.*/
unsigned int Model3DFromFile::add_pixel_shader( const std::wstring& file_name )
{
	PixelShaderObject* pixel_shader = models_manager->pixel_shader.get( file_name );
	if ( !pixel_shader )
	{
		// Nie by�o shadera, trzeba go stworzy� i doda�
		pixel_shader = PixelShaderObject::create_from_file( file_name, DEFAULT_PIXEL_SHADER_ENTRY );
		if ( !pixel_shader )		// shader m�g� mie� z�y format, a nie chcemy dodawa� nullptra do ModelsManagera
			return WRONG_ID;

		models_manager->pixel_shader.unsafe_add( file_name, pixel_shader );	// Dodali�my tekstur�
	}

	// Teraz musimy doda� shader na odpowiednie miejsce w tablicy
	tmp_data->table[tmp_data->current_pointer]->new_part.pixel_shader = pixel_shader;
	// Dodajemy odwo�anie plikowe
	pixel_shader->add_file_reference( );

	// Zwracamy id shadera, ale w zasadzie tylko po to, �eby kto� m�g� sprawdzi� czy wszystko posz�o dobrze
	return pixel_shader->get_id( );
}


/**@brief Funkcja dodaje do tymczasowej tablicy bufor wierzcho�k�w.

Funkcja dodaje do tymczasowej tablicy bufor wierzcho�k�w.
Podane dane s� przepisywane, a za zwolnienie pami�ci odpowiada ten kto wywo�a� funkcj�.
Nie wolno doda� kolejnego bufora, je�eli w danym ModelPart jaki� juz istania�.
W takiej sytuacji funkcja natychmiast powraca.

Bufory wierzcho�k�w s� ��czone w jedna ca�o�� w funkcji EndEdit.

@param[in] buffer Wska�nik na tablic� wierzcho�k�w. Zwalnia wywo�uj�cy.
@param[in] vert_count Liczba wierzcho�k�w w buforze.
@return Funkcja zawsze zwraca WRONG_ID.*/
unsigned int Model3DFromFile::add_vertex_buffer( const VertexNormalTexCord1* buffer, unsigned int vert_count )
{
	if ( vert_count == 0 )
		return WRONG_ID;		// Nie damy si� zrobi� w balona

	auto data = tmp_data->table[tmp_data->current_pointer];

	if ( data->vertices_tab )
		return WRONG_ID;		// Je�eli jaka� tablica ju� istania�a, to kto� pope�ni� b��d

	data->vertices_count = vert_count;							// Zapisujemy liczb� wierzcho�k�w
	data->vertices_tab = new VertexNormalTexCord1[vert_count];	// Tworzymy now� tablice wierzcho�k�w
	// Tablica zwalniana w funkcji EndEdit_vertex_buffer_processing()
	memcpy( data->vertices_tab, buffer, sizeof(VertexNormalTexCord1)*vert_count );	// Przepisujemy tablic�

	return 1;
}


/**@brief Nadaje aktualnej cz�ci mesha macierz transformacji.

Ka�da cz�� mesha ma przypisan� do siebie transformacj� wzgl�dem �rodka uk�adu wsp�rzednych modelu.
Je�eli dla jakiego� bufora wierzcho�k�w nie zostanie przydzielona transformacja, to 
domy�lnie znajduje si� tu macierz identyczno�ciowa.

@param[in] Macierz transformacji*/
void Model3DFromFile::add_transformation( const DirectX::XMFLOAT4X4& transform )
{
	tmp_data->table[tmp_data->current_pointer]->new_part.mesh->transform_matrix = transform;
}


/**@brief Funkcja dodaje do tymczasowej tablicy dane potrzebne do stworzenia bufora wierzcho�k�w.
Indeksy s� przepisywane z podanej tablicy i trzeba j� zwolni� samemu.
*/
unsigned int Model3DFromFile::add_index_buffer( const VERT_INDEX* buffer, unsigned int ind_count, int vertex_buffer_offset )
{
	if ( ind_count == 0 )
		return WRONG_ID;		// Nie damy si� zrobi� w balona

	// Bufor indeks�w przepisujemy do tablicy tymczasowej. Zostanie on potem scalony z reszt� bufor�w
	// indeks�w w funkcji EndEdit.
	unsigned int cur_ptr = tmp_data->current_pointer;

	if ( tmp_data->table[cur_ptr]->indicies_tab )
		return WRONG_ID;		// Je�eli jaka� tablica ju� istania�a, to kto� pope�ni� b��d

	tmp_data->table[cur_ptr]->indicies_count = ind_count;			// Przepisujemy liczb� indeks�w
	tmp_data->table[cur_ptr]->indicies_tab = new VERT_INDEX[ind_count];	// Alokujemy now� tablic� indeks�w
	// Tablica zwalniana w funkcji EndEdit_index_buffer_processing()
	memcpy( tmp_data->table[cur_ptr]->indicies_tab, buffer, sizeof(VERT_INDEX)*ind_count );	// Przepisujemy tablic�

	switch ( vertex_buffer_offset )
	{
		case VERTEX_BUFFER_OFFSET::BEGIN:
			tmp_data->table[cur_ptr]->indicies_offset = 0;
			break;
		case VERTEX_BUFFER_OFFSET::LAST:
			tmp_data->table[cur_ptr]->indicies_offset = get_buffer_offset_to_last();
			break;
	}

	return WRONG_ID;
}

//----------------------------------------------------------------------------------------------//
//								funkcje pomocnicze												//
//----------------------------------------------------------------------------------------------//

/**@brief Zwraca offset o jaki trzeba przesun�� bufor indeks�w wzgl�dem pocz�tka
bufora wierzcho�k�w, je�eli podano opcj� VERTEX_BUFFER_OFFSET::LAST.
Oznacza to zliczenie wszystkich wierzcho�k�w we wszystkich poprzedzaj�cych
buforach wierzcho�k�w.

@note Funkcja do wywo�ywania wew�trznego przez Model3DFromFile.
*/
unsigned int Model3DFromFile::get_buffer_offset_to_last( )
{
	unsigned int offset = 0;
	unsigned int max_ptr = 0;
	// Znajdujemy ostatni bufor wierzcho�k�w istniej�cy w tablicy
	for ( unsigned int i = tmp_data->current_pointer; i >= 0; --i )
		if ( tmp_data->table[i]->vertices_tab != nullptr )
			max_ptr = i;

	// Sumujemy liczb� wierzcho�k�w we wszystkich wcze�niejszych buforach (ostatniego nie wliczaj�c)
	for ( unsigned int i = 0; i < max_ptr; ++i )
		offset += tmp_data->table[tmp_data->current_pointer]->vertices_count;

	return offset;
}


/**@brief Tworzymy z rozproszonych bufor�w wierzcho�k�w jeden wi�kszy.
Nast�pnie tworzymy obiekt bufora i dodajemy go do ModelsManagera
oraz przypisujemy zmiennej vertex_buffer.

@note Funkcja do wywo�ywania wew�trznego przez Model3DFromFile.*/
void Model3DFromFile::EndEdit_vertex_buffer_processing( )
{
	// Podliczamy ile miejsca potrzeba dla bufora wierzcho�k�w
	unsigned int vertex_buffer_length = 0;

	for ( int i = 0; i < tmp_data->current_pointer; ++i )	// Bufor wierzcho�k�w
		vertex_buffer_length += tmp_data->table[i]->vertices_count;

	if ( vertex_buffer_length == 0 )
		return;			// Nie ma bufora wierzcho�k�w. Sytuacja jest chora, ale jak kto� poda� taki model, to jego sprawa.

	// Przepisujemy bufor wierzcho�k�w
	VertexNormalTexCord1* verticies = new VertexNormalTexCord1[vertex_buffer_length];
	unsigned int offset = 0;
	for ( int i = 0; i < tmp_data->current_pointer; ++i )
	{
		// verticies + offset - tu jest stosowana arytmetyka dla wska�nik�w, a nie zwyk�e dodawanie
		VertexNormalTexCord1* dest_ptr = verticies + offset;
		memcpy( dest_ptr, tmp_data->table[i]->vertices_tab, sizeof(VertexNormalTexCord1)*tmp_data->table[i]->vertices_count );
		
		// Je�eli nie ma bufora indeks�w to zapisujemy offset wzgl�dem pocz�tku bufora wierzcho�k�w
		if ( !tmp_data->table[i]->indicies_tab )
			tmp_data->table[i]->new_part.mesh->buffer_offset = offset;

		offset += tmp_data->table[i]->vertices_count;

		delete[] tmp_data->table[i]->vertices_tab;		// Tablica alokowana w add_vertex_buffer()
	}
	// Tworzymy obiekt bufora wierzcho�k�w i go zapisujemy
	vertex_buffer = BufferObject::create_from_memory( verticies,
													  sizeof(VertexNormalTexCord1),
													  vertex_buffer_length,
													  D3D11_BIND_VERTEX_BUFFER );
	delete[] verticies;							// Bufor by� tylko tymczasowy

	vertex_buffer->add_file_reference( );		// Zaznaczamy, �e si� do niego odwo�ujemy
	models_manager->vertex_buffer.unsafe_add( file_path, vertex_buffer );		// Dodajemy bufor do ModelsManagera
	// Nie sprawdzi�em czy pod tak� �cie�k� ju� czego� nie by�o. Nie mia�o prawa by�, bo 
	// bufory wierzcho�k�w nie s� wsp�dzielone miedzy plikami. Jednak je�eli pojawi�by si�
	// jaki� bli�ej nieokre�lony b��d (kt�rego troch� nie umiem sobie wyobrazi�), to takie co� mo�e
	// doprowadzi� do wyciek�w pami�ci. Z tego wzgl�du je�eli kiedykolwiek bufory b�d� wsp�dzielone
	// z innymi plikami, to trzeba to zmodyfikowa�.
}

/**@brief Scalamy tymczasowe bufory indeks�w w jeden du�y

@note Funkcja do wywo�ywania wew�trznego przez Model3DFromFile.*/
void Model3DFromFile::EndEdit_index_buffer_processing( )
{
	// Podliczamy ile miejsca potrzeba dla bufora indeks�w, a ile dla bufora wierzcho�k�w
	unsigned int index_buffer_length = 0;

	for ( int i = 0; i < tmp_data->current_pointer; ++i )	// Bufor indeks�w
		index_buffer_length += tmp_data->table[i]->indicies_count;

	if ( index_buffer_length == 0 )
		return;		// Najwyra�niej nie ma bufora indeks�w

	// Przepisujemy bufor indeks�w
	VERT_INDEX* indicies = new VERT_INDEX[index_buffer_length];
	unsigned int offset = 0;
	for ( int i = 0; i < tmp_data->current_pointer; ++i )
	{
		// verticies + offset - tu jest stosowana arytmetyka dla wska�nik�w, a nie zwyk�e dodawanie
		memcpy( indicies + offset, tmp_data->table[i]->indicies_tab, sizeof(VERT_INDEX)*tmp_data->table[i]->indicies_count );

		// Je�eli jest bufor indeks�w, to zapisujemy offset wzgl�dem pocz�tku tego bufora
		if ( tmp_data->table[i]->indicies_tab )
			tmp_data->table[i]->new_part.mesh->buffer_offset = offset;

		offset += tmp_data->table[i]->indicies_count;

		delete[] tmp_data->table[i]->indicies_tab;		// Tablica alokowana w add_index_buffer()
		// UWAGA!! Nie przypisujemy nullptra, bo chcemy wiedzie�, �e jest bufor indeks�w !!!!
	}
	// Tworzymy obiekt bufora indeks�w i go zapisujemy
	index_buffer = BufferObject::create_from_memory( indicies,
													 sizeof(VERT_INDEX),
													 index_buffer_length,
													 D3D11_BIND_INDEX_BUFFER );
	delete[] indicies;							// Bufor by� tylko tymczasowy

	index_buffer->add_file_reference( );		// Zaznaczamy, �e si� do niego odwo�ujemy
	models_manager->vertex_buffer.unsafe_add( file_path, index_buffer );		// Dodajemy bufor do ModelsManagera
	// Nie sprawdzi�em czy pod tak� �cie�k� ju� czego� nie by�o. Komentarz ten sam co w funkcji EndEdit_vertex_buffer_processing
}


/**@brief Dodajemy do strutkury ModelPart dane o buforach wierzcho�k�w oraz przypisujemy wbudowane
shadery, je�eli nie podano �adnych.
Dodajemy te� domy�lny materia�, je�eli nie ma.

@note Funkcja do wywo�ywania wew�trznego przez Model3DFromFile.*/
void Model3DFromFile::EndEdit_prepare_ModelPart( )
{
	for ( int i = 0; i < tmp_data->current_pointer; ++i )
	{
		ModelPart& part = tmp_data->table[i]->new_part;

		// Je�eli istnieje bufor ideks�w, to dzia�amy w trybie indeksowanym
		if ( tmp_data->table[i]->indicies_tab )
		{
			part.mesh->use_index_buf = true;		// U�ywamy bufora wierzcho�k�w
			part.mesh->vertices_count = tmp_data->table[i]->indicies_count;	// Liczba wierzcho�k�w z bufora do wy�wietlenia
			part.mesh->base_vertex = tmp_data->table[i]->indicies_offset;	// Offset wzgl�dem pocz�tku bufora wierzcho�k�w
			// Do ka�dego indeksu z tablicy zostanie dodana ta warto�� i dopiero potem odczytany wierzcho�ek

			// buffer_offset jest ju� za�atwiony w funkcjach EndEdit_index_buffer_processing i EndEdit_vertex_buffer_processing
		}
		else // W przeciwnym razie jest tryb bezpo�redni
		{
			part.mesh->use_index_buf = false;		// Co prawda tak jest domy�lnie, ale nie zaszkodzi jawnie to napisa�
			part.mesh->vertices_count = tmp_data->table[i]->vertices_count;

			// buffer_offset jest ju� za�atwiony w funkcjach EndEdit_index_buffer_processing i EndEdit_vertex_buffer_processing
		}

		// Teraz trzeba dopisa� shadery
		if ( !part.pixel_shader )
			part.pixel_shader = models_manager->find_best_pixel_shader( part.texture );
		if ( !part.vertex_shader )
			part.vertex_shader = models_manager->find_best_vertex_shader( part.texture );
		// I materia�
		if ( part.material == nullptr )
			part.material = models_manager->material.get( DEFAULT_MATERIAL_STRING );
	}

}


//----------------------------------------------------------------------------------------------//
//									Funkcje do korzystania z obiekt�w							//
//----------------------------------------------------------------------------------------------//

/**@brief zwraca wska�nik na ModelPart spod podanego indeksu.
@param[in] Indeks.
@return Wska�nik na ModelPart lub nullptr, je�eli indeks by� nieprawid�owy.*/
const ModelPart* Model3DFromFile::get_part( unsigned int index )
{
	if ( index < model_parts.size( ) )
		return &model_parts[index];
	return nullptr;
}

/**@brief Zwraca liczb� obiekt�w ModelPart w tablicy.*/
unsigned int Model3DFromFile::get_parts_count()
{
	return model_parts.size();
}



#ifndef __UNUSED

//-------------------------------------------------------------------------------//
//							wersja DirectX 9
//-------------------------------------------------------------------------------//


//----------------------------------------------------------------------------------------------//
//								contructor, destructor											//
//----------------------------------------------------------------------------------------------//

Model3DFromFile::Model3DFromFile( int id )
: referenced_object( id )
{
}


Model3DFromFile::~Model3DFromFile( )
{
	//najpierw kasujemy odwo�ania do obiekt�w
	for ( unsigned int i = 0; i < mesh_parts.size( ); ++i )
		mesh_parts[i]->mesh_object->delete_file_reference( );
	for ( unsigned int i = 0; i < textures.size( ); ++i )
		textures[i]->delete_file_reference( );;
	for ( unsigned int i = 0; i < materials.size( ); ++i )
		materials[i]->delete_file_reference( );

	//kasujemy obiekty MeshParts (to s� jedyne, kt�rymi zarz�dza ta klasa)
	for ( unsigned int i = 0; i < mesh_parts.size( ); ++i )
		delete mesh_parts[i];
}



//----------------------------------------------------------------------------------------------//
//									building models												//
//----------------------------------------------------------------------------------------------//


unsigned int Model3DFromFile::add_material( const D3DMATERIAL9 &material )
{
	MaterialObject* result;
	unsigned int id;
	result = models_manager->add_material( material, id );

	if ( result == nullptr )
		return WRONG_MATERIAL_ID;

	materials.push_back( result );
	result->add_file_reference( );		//pamietajmy o dodawaniu odwo�a�

	return id;
}

/*Je�eli jakis materia� zosta� dodany bezpo�rednio do ModelsManagera, to mo�na si� do niego odwo�ywa� po id.
Zwraca nullptr je�eli nie istnieje taki materia� lub wska�nik na ten materia�.

Materia� nie zostaje dodany do listy, je�eli nie istnia�!!!!!!!!!!*/
MaterialObject* Model3DFromFile::add_material( unsigned int id )
{
	MaterialObject* result = models_manager->get_material( id );
	if ( result != nullptr )
	{
		materials.push_back( result );
		result->add_file_reference( );		//pamietajmy o dodawaniu odwo�a�
	}

	return result;
}

/*Niekt�re kawa�ki mesha mog� nie mie� materia�u.*/
void Model3DFromFile::add_null_material( )
{
	materials.push_back( nullptr );
}


unsigned int Model3DFromFile::add_texture( const std::wstring& path )
{
	TextureObject* result;
	unsigned int id;
	result = models_manager->add_texture( path, id );

	if ( result == nullptr )
		return WRONG_TEXTURE_ID;

	textures.push_back( result );
	result->add_file_reference( );		//pamietajmy o dodawaniu odwo�a�

	return id;
}


/*Je�eli jaka� tekstura zosta�a dodana bezpo�rednio do ModelsManagera, to mo�na si� do niej odwo�ywa� po id.
Zwraca nullptr je�eli nie istnieje taka tekstura lub wska�nik na teksture, je�eli istnieje.

Tekstura nie zostaje dodana do listy je�eli nie istnia�a!!!!!!*/
TextureObject* Model3DFromFile::add_texture( unsigned int id )
{
	TextureObject* result = models_manager->get_texture( id );
	if ( result != nullptr )
	{
		textures.push_back( result );
		result->add_file_reference( );		//pamietajmy o dodawaniu odwo�a�
	}

	return result;
}

/*Niekt�re kawa�ki mesha mog� nie mie� tekstury.*/
void Model3DFromFile::add_null_texture( )
{
	textures.push_back( nullptr );
}


MeshPart* Model3DFromFile::add_mesh( unsigned int id, DirectX::XMFLOAT4X4& matrix )
{
	MeshObject* result = models_manager->get_mesh( id );
	if ( result == nullptr )
		return nullptr;

	MeshPart* new_mesh_part = new MeshPart;
	new_mesh_part->transform_matrix = matrix;
	new_mesh_part->mesh_object = result;

	mesh_parts.push_back( new_mesh_part );
	result->add_file_reference( );		//pamietajmy o dodawaniu odwo�a�

	return new_mesh_part;
}


/*Dodajemy wierzcho�ki do listy modelu z danego pliku, przypisuj�c mu podan� w ostatnim parametrze macierz przekszta�cenia.
Mesh jest na pocz�tku dodawany do ModelsManagera, je�eli ju� istnia�, nie zostaje zduplikowany.*/
unsigned int Model3DFromFile::add_mesh( Vertex_Normal_TexCords1* vertices, unsigned int vert_num, const DirectX::XMFLOAT4X4& matrix )
{
	MeshObject* result;
	unsigned int id;
	result = models_manager->add_mesh( vertices, vert_num, id );
	if ( result == nullptr )	return WRONG_MESH_ID;		//co� posz�o nie tak

	MeshPart* mesh_part = new MeshPart( );
	mesh_part->mesh_object = result;
	mesh_part->transform_matrix = matrix;

	mesh_parts.push_back( mesh_part );

	result->add_file_reference( );		//pami�tajmy o dodawaniu odwo�a�

	return id;
}

void Model3DFromFile::add_reference( )
{
	++references;
}

void Model3DFromFile::delete_reference( )
{
	--references;
}

/*Sprawdza czy s� jakie� odwo�ania do podanego pliku. Je�eli nie ma to znaczy, �e obiekt mo�e
zostac bezpiecznie usuni�ty.*/
bool Model3DFromFile::can_delete( unsigned int& ref )
{
	if ( references > 0 )
		return FALSE;
	return TRUE;
}

unsigned int Model3DFromFile::get_parts_count( )
{
	unsigned int size1 = materials.size( );
	unsigned int size2 = textures.size( );
	unsigned int size3 = mesh_parts.size( );

	unsigned int min = size1 < size2 ? size1 : size2;
	return min < size3 ? min : size3;
}

MaterialObject* Model3DFromFile::get_material( unsigned int index )
{
	if ( index < materials.size( ) )
		return materials[index];
	return nullptr;
}

TextureObject* Model3DFromFile::get_texture( unsigned int index )
{
	if ( index < textures.size( ) )
		return textures[index];
	return nullptr;
}

MeshPart* Model3DFromFile::get_mesh_part( unsigned int index )
{
	if ( index < mesh_parts.size( ) )
		return mesh_parts[index];
	return nullptr;
}

#endif