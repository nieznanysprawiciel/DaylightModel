#pragma once

/**@file ResourceContainer.h
#brief Zawiera deklaracj� szblonu klasy kontenera dla asset�w.
*/

#include <map>


/**@brief Szablon klasy do przechowywania asset�w.

Wszystkie assety s� identyfikowane po nazwie, kt�ra najcz�ciej jest nazw� pliku, z kt�rego asset
pochodzi. Mapa zapewnia logarytmiczny czas dost�pu po nazwie. Istnieje te� mo�liwo�� odwo�ania si�
po identyfikatorze, wtedy czas dost�pu jest liniowy (chyba �e iterowanie po kolejnych elementacj mapy
nie odbywa si� liniowo.*/
template <class TYPE>
class ResourceContainer
{
	friend class ModelsManager;
private:
	unsigned int count;		///<Indentyfikator jaki zostanie przydzielony kolejnemy elementowi

protected:
	std::map<std::wstring, TYPE*> container;	///<Kontener zawieraj�cy assety powiazane z ich nazw�

	// Kasowanie obiekt�w
	int force_remove( const std::wstring& name );
	int force_remove( unsigned int id );
	void force_remove_all();
	void release_memory( TYPE* );
public:
	ResourceContainer();
	~ResourceContainer();

	// Kasowanie obiekt�w
	int remove( const std::wstring& name );
	int remove( unsigned int id );
	int remove_unused();

	// Dodawanie obiekt�w
	void unsafe_add( const std::wstring& name, TYPE* resource );

	// Dost�p do obiekt�w
	TYPE* get( unsigned int id );
	inline unsigned int get_next_id() { return count; }	///<Zwraca identyfikator, kt�ry zostanie przydzielony kolejnemu elementowi

	/**@brief Zwraca element na podstawie jego nazwy
	@param[in] name Nazwa elementu, kt�ry chcemy dosta�
	@return Wska�nik na obiekt assetu*/
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

/**@brief Destruktor zwalnia wszystkie elementy w mapie (tak�e pami�� po nich)*/
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
Jednak�e por�wnania string�w mog� si� okaza� bardziej kosztowne.
@param[in] id Identyfikator elementu.
@return Wska�nik na poszukiwany element.*/
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
//							dodawanie obiekt�w
//-------------------------------------------------------------------------------//

/*Dodaje element do kontanera + nadaje mu unikalny identyfikator.
Je�eli element ju� istnia�, to po prostu nie zostanie wstawiony, dlatego
przed uzyciem warto pobra� element o danej nazwie, �eby sprawdzi�
czy dodawanie jest konieczne.*/
// Mo�e kiedy� zrobie wstawianie ze sprawdzaniem, na razie nie wydaje si� potrzebne


/**@brief Dodaje element do kontanera + nadaje mu unikalny identyfikator.

Je�eli element ju� istnia�, to zostanie nadpisany nowym, dlatego nale�y
zawsze przed u�yciem sprawdzi� czy pod tak� nazw�, co� ju� si� nie 
znajduje.
@param[in] name Nazwa elementu, pod jak� zostanie dodany.
@param[in] resource Element dodania.*/
template <class TYPE>
void ResourceContainer<TYPE>::unsafe_add( const std::wstring& name, TYPE* resource )
{
	if ( !resource )
		return;	//Nie mo�emy potem ustawi� id

	container[name] = resource;

	resource->set_id( count );
	++count;			// Inkrementujemy licznik
}

//-------------------------------------------------------------------------------//
//							kasowanie obiekt�w
//-------------------------------------------------------------------------------//
/**@brief Zwalnia obiekt podany w parametrze.

Kasowanie pami�ci nie jest mo�liwe przy pomocy operatora delete,
poniewa� destruktory w tych klasach s� prywatne. Dlatego trzeba zrobi� to 
za po�rednictwem obiektu, kt�ry ma uprawnienia do tego.

@param[in] object Objekt do skasowania.
*/
template <class TYPE>
void ResourceContainer<TYPE>::release_memory( TYPE* object )
{
	// Destruktor jest prywatny, wi�c nie mo�emy kasowa� obiektu bezpo�rednio.
	ObjectDeleterKey<TYPE> key;							// Tworzymy klucz.
	ObjectDeleter<TYPE> model_deleter( key );			// Tworzymy obiekt kasuj�cy i podajemy mu nasz klucz.
	model_deleter.delete_object( object );				// Kasujemy obiekt za po�rednictwem klucza.
}

/**@brief Usuwa element o podanej nazwie, je�eli nie ma do niego odwo�a�.

@param[in] name nazwa elementu do usuni�cia.
@return Zwracana warto��:
- 0	-	w przypadku powodzenia,
- -1	-	nie znaleziono elementu,
- 1	-	nie da si� usun��, bo jest w u�yciu*/
template <class TYPE>
int ResourceContainer<TYPE>::remove( const std::wstring& name )
{
	auto iter = container.find( name );
	if ( iter != container.end() )
		return -1;		// Nie znale�li�my elementu

	if ( !iter->second->can_delete() )
		return 1;		// Nie mo�emy skasowa�, bo s� odwo�ania

	release_memory( iter->second );		// Zwalniamy pami�� spod wska�nika
	container.erase( iter );	// Kasujemy element z mapy

	return 0;			// Wychodzimy z powodzeniem
}


/**@brief Usuwa element o podanym indeksie, je�eli nie ma do niego odwo�a�.

@param[in] id Identyfikator elementu
@return Zwracana warto��:
- 0	-	w przypadku powodzenia,
- -1	-	nie znaleziono elementu,
- 1	-	nie da si� usun��, bo jest w u�yciu*/
template <class TYPE>
int ResourceContainer<TYPE>::remove( unsigned int id )
{
	for ( auto iter = container.begin( ); iter != container.end( ); iter++ )
	{
		if ( iter->second->get_id() == id )
		{
			// Sprawdzamy czy nie ma jakich� odwo�a� do obiektu
			if ( !iter->second->can_delete() )
				return 1;				// S� odwo�ania, wi�c nie kasujemy

			release_memory( iter->second );		// Zwalniamy pami�� spod wska�nika
			container.erase( iter );	// Kasujemy element z mapy

			return 0;					// Zwracamy 0 jako powodzenie operacji
		}
	}
	return -1;		// Nie znale�li�my elementu
}

/**@brief Kasuje wszystkie elementy w kontenerze, kt�re nie s� u�ywane przez
�aden obiekt. Kasowanie jest w pe�ni bezpieczne.

@return Zwraca liczb� usuni�tych element�w.*/
template <class TYPE>
int ResourceContainer<TYPE>::remove_unused()
{
	int count = 0;
	for ( auto iter = container.begin(); iter != container.end(); iter++ )
	{// Iterujemy po ca�ej mapie
		if ( iter->second->can_delete() )
		{
			// Mo�emy skasowa� obiekt, bo nikt go nie u�ywa
			release_memory( iter->second );		// Zwalniamy pami�� spod wska�nika
			container.erase( iter );	// Kasujemy element z mapy

			++count;
		}
	}

	return count;
}


/**@brief Wymusza skasowanie podanego elementu, nawet je�eli jest u�ywany

@param[in] name Nazwa elementu do usuni�cia.
@return Zwracana warto��:
- 0	-	w przypadku powodzenia,
- -1	-	nie znaleziono elementu*/
template <class TYPE>
int ResourceContainer<TYPE>::force_remove( const std::wstring& name )
{
	auto iter = container.find( name );
	if ( iter != container.end( ) )
		return -1;		// Nie znale�li�my elementu

	delete iter->second;		// Zwalniamy pami�� spod wska�nika
	return 0;
}

/**@brief Wymusza skasowanie podanego elementu

@param[in] id Identyfkator elementu do usuni�cia
@return Zwracana warto��:
- 0	-	w przypadku powodzenia,
- -1	-	nie znaleziono elementu*/
template <class TYPE>
int ResourceContainer<TYPE>::force_remove( unsigned int id )
{
	for ( auto iter = container.begin( ); iter != container.end( ); iter++ )
	{
		if ( iter->second->get_id( ) == id )
		{
			delete iter->second;		// Zwalniamy pami�� spod wska�nika
			return 0;
		}
	}
	return -1;		// Nie znaleziono elementu
}

/**@brief Kasuje wszystkie elementy niezale�nie od tego czy by�y u�ywane,
a nast�pnie czy�ci map�.*/
template <class TYPE>
void ResourceContainer<TYPE>::force_remove_all( )
{
	for ( auto iter = container.begin(); iter != container.end(); iter++ )
	{// Iterujemy po ca�ej mapie
		delete iter->second;		// Zwalniamy pami�� spod wska�nika
	}
	container.clear();
}


