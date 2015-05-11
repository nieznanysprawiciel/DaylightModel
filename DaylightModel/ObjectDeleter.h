#pragma once
/**@file ObjectDeleter.h
@brief Klasy s�u��ce do zapewnienia ograniczonej mo�liwo�ci kasowania niekt�rych obiekt�w silnika,
kt�re maj� prywatne destruktory.
*/

template <class TYPE> class ResourceContainer;


/*

@page DeletingObjects Usuwanie obiekt�w wewn�trz silnika

@section Contents Spis Tre�ci
- @ref DeletingObjects_selfDrawingObjects_briefDescription

@subsection DeletingObjects_selfDrawingObjects_briefDescription Og�lny opis
*/


/**@brief Szablon klasy klucza dla obiekt�w ObjectDeleter.

Niekt�re obiekty s� zadeklarowane z prywatnym (chronionym) destruktorem, w celu uniemo�liwienia
skasowania ich przez niepowo�ane klasy. Jednak�e musi istnie� jaki� dost�p do destruktor�w,
aby nie powodowa� wyciek�w pami�ci. Do kasowania obiekt�w s�u�y klasa ObjectDeleter. Aby stworzy�
obiekt tej klasy potrzebny jest "klucz", kt�rym jest w�asnie ta klasa.

Klucz ma prywatne konstruktory, w zwi�zku z czym mo�e zosta� stworzony jedynie przez klasy,
kt�re s� z nim zaprzyja�nione. Domy�lnie klasami zaprzyja�nionymi s� ModelsManager i ResourceContainer,
aby stworzy� klucz dla innych typ�w klas, nale�y napisa� specjalizacj� tego szablonu.

Odddelegowanie innej klasy do kasowania obiekt�w ma za zadanie zlikwidowanie niewygodnych zale�no�ci.
Dzi�ki temu obiekty, kt�re maj� by� kasowane, nie musz� si� przyja�ni� bezpo�rednio z klasami, kt�re mog� je usuwa�.
Wystarczy, �e zadeklaruj� przyja�� z obiektem ObjectDeleter. Taka przyja�� jest niegro�na, poniewa�
klasa ta nie s�u�y do niczego innego ni� kasowanie obiekt�w, wi�c jeste�my pewni, �e zmiany wprowadzone
w klasie deklaruj�cej przyja�� nie b�d� psu�y ca�ego kodu aplikacji.
*/
template<typename class_type> class ObjectDeleterKey
{
	friend class ModelsManager;
	friend class ResourceContainer<class_type>;
private:
	ObjectDeleterKey() = default;						///<Tylko klasa zaprzyja�niona mo�e stworzy� obiekt.
	ObjectDeleterKey( const ObjectDeleterKey& ) {};		///<Tylko klasa zaprzyja�niona mo�e stworzy� obiekt.
	ObjectDeleterKey( const ObjectDeleterKey&& ) {};	///<Tylko klasa zaprzyja�niona mo�e stworzy� obiekt.
public:
};

/**@brief Klasa s�u�y do kasowania obiekt�w, kt�rych destruktor jest prywatny.

Taka sytuacja zachodzi w momencie, gdy nie chcemy, aby ka�da klasa mog�a skasowa�
jaki� obiekt, ale chcemy da� takie uprawnienia jedynie wybra�com. Ta klasa implementuje
funkcjonalno�� kasowania obiek�w. Obiekt tej klasy mo�e zosta� stworzony jedynie
przez funkcj�, kt�ra dostarczy odpowiedni obiekt klucz @ref ObjectDeleterKey.
Dopiero wtedy mo�na wywo�a� funkcj� delete_object, kt�ra usunie ��dany obiekt z pami�ci.

@see @ref ObjectDeleterKey
*/
template<typename class_type> class ObjectDeleter
{
private:
	ObjectDeleter() = default;							///<Nie mo�na stworzy� obiektu nie maj�c klucza.
	ObjectDeleter( const ObjectDeleter& ) = default;	///<Nie mo�na stworzy� obiektu nie maj�c klucza.
public:
	ObjectDeleter( const ObjectDeleterKey<class_type>& deleter_key ) {};	///<Jedyny dost�pny do wywo�ania konstruktor, tylko dla wybra�c�w.

	/**@brief Kasuje podany w parametrze obiekt.
	
	@param[in] object Obiekt do skasowania.*/
	inline void delete_object( class_type* object )
	{
		delete object;
	}
};
