#pragma once

/**@file ResourceContainer.h
#brief Zawiera deklaracjê szblonu klasy kontenera dla assetów.
*/

#include <map>


/**@brief Szablon klasy do przechowywania assetów.

Wszystkie assety s¹ identyfikowane po nazwie, która najczêœciej jest nazw¹ pliku, z którego asset
pochodzi. Mapa zapewnia logarytmiczny czas dostêpu po nazwie. Istnieje te¿ mo¿liwoœæ odwo³ania siê
po identyfikatorze, wtedy czas dostêpu jest liniowy (chyba ¿e iterowanie po kolejnych elementacj mapy
nie odbywa siê liniowo.*/
template <class TYPE>
class ResourceContainer
{
	friend class ModelsManager;
private:
	unsigned int count;		///<Indentyfikator jaki zostanie przydzielony kolejnemy elementowi

protected:
	std::map<std::wstring, TYPE*> container;	///<Kontener zawieraj¹cy assety powiazane z ich nazw¹

	// Kasowanie obiektów
	int force_remove( const std::wstring& name );
	int force_remove( unsigned int id );
	void force_remove_all();
	void release_memory( TYPE* );
public:
	ResourceContainer();
	~ResourceContainer();

	// Kasowanie obiektów
	int remove( const std::wstring& name );
	int remove( unsigned int id );
	int remove_unused();

	// Dodawanie obiektów
	void unsafe_add( const std::wstring& name, TYPE* resource );

	// Dostêp do obiektów
	TYPE* get( unsigned int id );
	inline unsigned int get_next_id() { return count; }	///<Zwraca identyfikator, który zostanie przydzielony kolejnemu elementowi

	/**@brief Zwraca element na podstawie jego nazwy
	@param[in] name Nazwa elementu, który chcemy dostaæ
	@return WskaŸnik na obiekt assetu*/
	inline TYPE* get( const std::wstring& name )
	{
		auto iter = container.find( name );
		if ( iter != container.end() )
			return iter->second;
		return nullptr;
	}
};

template <class TYPE>
ResourceContainer<TYPE>::ResourceContainer()
{
	count = 1;
}

/**@brief Destruktor zwalnia wszystkie elementy w mapie (tak¿e pamiêæ po nich)*/
template <class TYPE>
ResourceContainer<TYPE>::~ResourceContainer( )
{
	for ( auto iter = container.begin( ); iter != container.end( ); iter++ )
	{
		release_memory( iter->second );
	}
	container.clear();
}

/**@brief Zwraca element na podstawie identyfikatora.

Wyszukiwanie po identyfikatorze jest liniowe, a po nazwie logarytmiczne.
Jednak¿e porównania stringów mog¹ siê okazaæ bardziej kosztowne.
@param[in] id Identyfikator elementu.
@return WskaŸnik na poszukiwany element.*/
template <class TYPE>
TYPE* ResourceContainer<TYPE>::get( unsigned int id )
{
	for ( auto iter = container.begin(); iter != container.end(); iter++ )
	{
		if ( iter->second->get_id() == id )
			return iter->second;
	}
	return nullptr;
}

//-------------------------------------------------------------------------------//
//							dodawanie obiektów
//-------------------------------------------------------------------------------//

/*Dodaje element do kontanera + nadaje mu unikalny identyfikator.
Je¿eli element ju¿ istnia³, to po prostu nie zostanie wstawiony, dlatego
przed uzyciem warto pobraæ element o danej nazwie, ¿eby sprawdziæ
czy dodawanie jest konieczne.*/
// Mo¿e kiedyœ zrobie wstawianie ze sprawdzaniem, na razie nie wydaje siê potrzebne


/**@brief Dodaje element do kontanera + nadaje mu unikalny identyfikator.

Je¿eli element ju¿ istnia³, to zostanie nadpisany nowym, dlatego nale¿y
zawsze przed u¿yciem sprawdziæ czy pod tak¹ nazw¹, coœ ju¿ siê nie 
znajduje.
@param[in] name Nazwa elementu, pod jak¹ zostanie dodany.
@param[in] resource Element dodania.*/
template <class TYPE>
void ResourceContainer<TYPE>::unsafe_add( const std::wstring& name, TYPE* resource )
{
	if ( !resource )
		return;	//Nie mo¿emy potem ustawiæ id

	container[name] = resource;

	resource->set_id( count );
	++count;			// Inkrementujemy licznik
}

//-------------------------------------------------------------------------------//
//							kasowanie obiektów
//-------------------------------------------------------------------------------//
/**@brief Zwalnia obiekt podany w parametrze.

Kasowanie pamiêci nie jest mo¿liwe przy pomocy operatora delete,
poniewa¿ destruktory w tych klasach s¹ prywatne. Dlatego trzeba zrobiæ to 
za poœrednictwem obiektu, który ma uprawnienia do tego.

@param[in] object Objekt do skasowania.
*/
template <class TYPE>
void ResourceContainer<TYPE>::release_memory( TYPE* object )
{
	// Destruktor jest prywatny, wiêc nie mo¿emy kasowaæ obiektu bezpoœrednio.
	ObjectDeleterKey<TYPE> key;							// Tworzymy klucz.
	ObjectDeleter<TYPE> model_deleter( key );			// Tworzymy obiekt kasuj¹cy i podajemy mu nasz klucz.
	model_deleter.delete_object( object );				// Kasujemy obiekt za poœrednictwem klucza.
}

/**@brief Usuwa element o podanej nazwie, je¿eli nie ma do niego odwo³añ.

@param[in] name nazwa elementu do usuniêcia.
@return Zwracana wartoœæ:
- 0	-	w przypadku powodzenia,
- -1	-	nie znaleziono elementu,
- 1	-	nie da siê usun¹æ, bo jest w u¿yciu*/
template <class TYPE>
int ResourceContainer<TYPE>::remove( const std::wstring& name )
{
	auto iter = container.find( name );
	if ( iter != container.end() )
		return -1;		// Nie znaleŸliœmy elementu

	if ( !iter->second->can_delete() )
		return 1;		// Nie mo¿emy skasowaæ, bo s¹ odwo³ania

	release_memory( iter->second );		// Zwalniamy pamiêæ spod wskaŸnika
	container.erase( iter );	// Kasujemy element z mapy

	return 0;			// Wychodzimy z powodzeniem
}


/**@brief Usuwa element o podanym indeksie, je¿eli nie ma do niego odwo³añ.

@param[in] id Identyfikator elementu
@return Zwracana wartoœæ:
- 0	-	w przypadku powodzenia,
- -1	-	nie znaleziono elementu,
- 1	-	nie da siê usun¹æ, bo jest w u¿yciu*/
template <class TYPE>
int ResourceContainer<TYPE>::remove( unsigned int id )
{
	for ( auto iter = container.begin( ); iter != container.end( ); iter++ )
	{
		if ( iter->second->get_id() == id )
		{
			// Sprawdzamy czy nie ma jakichœ odwo³añ do obiektu
			if ( !iter->second->can_delete() )
				return 1;				// S¹ odwo³ania, wiêc nie kasujemy

			release_memory( iter->second );		// Zwalniamy pamiêæ spod wskaŸnika
			container.erase( iter );	// Kasujemy element z mapy

			return 0;					// Zwracamy 0 jako powodzenie operacji
		}
	}
	return -1;		// Nie znaleŸliœmy elementu
}

/**@brief Kasuje wszystkie elementy w kontenerze, które nie s¹ u¿ywane przez
¿aden obiekt. Kasowanie jest w pe³ni bezpieczne.

@return Zwraca liczbê usuniêtych elementów.*/
template <class TYPE>
int ResourceContainer<TYPE>::remove_unused()
{
	int count = 0;
	for ( auto iter = container.begin(); iter != container.end(); iter++ )
	{// Iterujemy po ca³ej mapie
		if ( iter->second->can_delete() )
		{
			// Mo¿emy skasowaæ obiekt, bo nikt go nie u¿ywa
			release_memory( iter->second );		// Zwalniamy pamiêæ spod wskaŸnika
			container.erase( iter );	// Kasujemy element z mapy

			++count;
		}
	}

	return count;
}


/**@brief Wymusza skasowanie podanego elementu, nawet je¿eli jest u¿ywany

@param[in] name Nazwa elementu do usuniêcia.
@return Zwracana wartoœæ:
- 0	-	w przypadku powodzenia,
- -1	-	nie znaleziono elementu*/
template <class TYPE>
int ResourceContainer<TYPE>::force_remove( const std::wstring& name )
{
	auto iter = container.find( name );
	if ( iter != container.end( ) )
		return -1;		// Nie znaleŸliœmy elementu

	delete iter->second;		// Zwalniamy pamiêæ spod wskaŸnika
	return 0;
}

/**@brief Wymusza skasowanie podanego elementu

@param[in] id Identyfkator elementu do usuniêcia
@return Zwracana wartoœæ:
- 0	-	w przypadku powodzenia,
- -1	-	nie znaleziono elementu*/
template <class TYPE>
int ResourceContainer<TYPE>::force_remove( unsigned int id )
{
	for ( auto iter = container.begin( ); iter != container.end( ); iter++ )
	{
		if ( iter->second->get_id( ) == id )
		{
			delete iter->second;		// Zwalniamy pamiêæ spod wskaŸnika
			return 0;
		}
	}
	return -1;		// Nie znaleziono elementu
}

/**@brief Kasuje wszystkie elementy niezale¿nie od tego czy by³y u¿ywane,
a nastêpnie czyœci mapê.*/
template <class TYPE>
void ResourceContainer<TYPE>::force_remove_all( )
{
	for ( auto iter = container.begin(); iter != container.end(); iter++ )
	{// Iterujemy po ca³ej mapie
		delete iter->second;		// Zwalniamy pamiêæ spod wskaŸnika
	}
	container.clear();
}


