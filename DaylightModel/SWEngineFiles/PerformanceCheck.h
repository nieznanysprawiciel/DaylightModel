#pragma once
/**@file PerformanceCheck.h
@author nieznanysprawiciel
@copyright Plik jest cz�ci� silnika graficznego SWEngine.

Zawiera deklaracj� klasy PerformanceCheck, kt�ra s�u�y do pomiaru 
czasu zu�ywanego na wykonanie wybranych zada�.*/

#include <string>
#include "types_definitions.h"



/**@brief Struktura przechowuje dane na temat wydajno�ci jednej badanej rzeczy.*/
struct _performance_data
{
	std::string			task_name;						///<Nazwa zadania jaka zostanie wy�wietlona w statystykach.
	int64				last_start_performance_value;	///<Ostatni zarejestrowany czas dla badanej zadania.
	int64				hole_time_spent;				///<Zlicza ca�y czas jaki zosta� sp�dzony na wykonywaniu zadania.
	int64				min_time_spent;					///<Minimalny czas wyknywania zadania.
	int64				max_time_spent;					///<Maksymalny czas wykonywania zadania.
	int32				task_executions;				///<Ile razy zosta�o wykonane zadanie - ilo�� parzystych wywo�a� startPerformanceCheck i EndPerformanceCheck.
};

/**@brief Klasa s�u�y do pomiaru wydajno�ci poszczeg�lnych fragment�w kodu.

Spos�b u�ycia klasy:
S� dwie metody. Mo�na normalnie korzysta� z klasy jak ze zwyk�ego obiektu i wywo�ywa� jej funkcje
lub mo�na u�y� zamiast tego specjalnie zdefiniowanych makr. W drugim przypadku kod s�u��cy do pomiaru b�dzie
za��czany jedynie w przypadku zdefiniowania makra PERFORMANCE_CHECK. W innym razie wszystkie u�yte makra b�d�
rozwija�y si� do pustych instrukcji.

1. Normalne u�ywanie klasy.
Aby u�y� klasy nale�y zarejestrowa� nazwy wybranych zada�. Robi si� to funkcj� registerTaskName.
Funkcja zwraca identyfikator, kt�ry potem pos�u�y do wywo�ywania pomiar�w dla tego zadania.

Ka�de zadanie musi by� otoczone wywo�aniami funkcji startPerformanceCheck i endPerformanceCheck z podaniem indeksu zadania.

Aby wypisa� wyniki, nale�y wywo�a� funkcj� printPerformanceStatisticsAsync podaj�c nazw� pliku, do kt�rego statystyki zostan� wys�ane.
Funkcja dzia�a asynchronicznie, wi�c nie czeka si� a� plik zostanie zapisany, tylko wykonuje kod silnika dalej.

\code
#include PerformanceCheck.h

unsigned int taskID;

myClass::myClass()
{
	PerformanceCheck stats;
	taskID = stats.registerTaskName("task_name");
}

void timer()
{
	stats.startPerformanceCheck( taskID );

	// Tutaj kod, kt�rego wydajno�� mierzymy

	stats.endPerformanceCheck( taskID );
}

\endcode

2. U�ytkowanie klasy z u�yciem makr.
Aby u�y� klasy nale�y zdefiniowa� makro PERFORMANCE_CHECK przed zaincludowaniem nag��wka.
Makro to robi dwie rzeczy:
- w��cza definicje pozosta�ych potrzebnych makr
- za��cza kod tworz�cy obiekt tej klasy (obiekt jest zmienn� globaln� w pliku PerformanceCheck.cpp)

Nast�pnie trzeba zadeklarowa� ch�� u�ycia pomiaru dla jakiego� zadania wywo�uj�� makro USE_PERFORMANCE_CHECK i podaj�c nazw�
zadania. Makro powinno zosta� wywo�ane w takim miejscu, jakby�my deklarowali zmienn�, kt�ra ma by� dost�pna w miejscach,
gdzie b�dziemy robili pomiary czasu. (Poniewa� to robi niejawnie to makro - definiuje tak� zmienn�).

Nazwa powinna by� pojedynczym ci�giem znak�w. NIE mo�e by� w cudzys�owie.

Zadanie musi zosta� zarejestrowane przed u�yciem makr do pomiaru poprzez wywo�anie REGISTER_PERFORMANCE_TASK
z podaniem nazwy zadania, kt�ra zostanie potem zapisana w pliku.

W miejscu pomiaru nale�y wywo�a� parami makra START_PERFORMANCE_CHECK i END_PERFORMANCE_CHECK podaj�c w parametrze
taki sam ci�g znak�w, jakim zosta�o zarejestrowane zadanie.

Wypisanie statystyk robi si� za pomoc� makra PRINT_STATISTICS i podaje nazw� pliku.
@attention Nazwa pliku jest w cudzys�owie w przeciwie�stwie do innych miejsc, w kt�rych wywo�uje si� makra.

\code
#include PerformanceCheck.h

USE_PERFORMANCE_CHECK( TASK_NAME )

myClass::myClass()
{
	REGISTER_PERFORMANCE_TASK( TASK_NAME )
}

void timer()
{
	START_PERFORMANCE_CHECK( TASK_NAME )

// Tutaj kod, kt�rego wydajno�� mierzymy

	END_PERFORMANCE_CHECK( TASK_NAME )
}

\endcode

*/
class PerformanceCheck
{
private:
	std::vector<_performance_data> performance_datas;		///<Zapisane dane na temat poszczeg�lnych zada�, dla kt�rych mierzymy wydajno��.

	static void print( const std::string& outputFile, std::vector<_performance_data> data_copy );
public:
	PerformanceCheck() = default;
	~PerformanceCheck() = default;

	void startPerformanceCheck( unsigned int index );
	void endPerformanceCheck( unsigned int index );
	void clear();

	unsigned int registerTaskName( const char* taskName );

	void printPerformanceStatisticsAsync( const std::string& outputFile );
};



#ifdef PERFORMANCE_CHECK

extern PerformanceCheck performanceCheck;

#define START_PERFORMANCE_CHECK( x ) performanceCheck.startPerformanceCheck( __performance_task_id ## x );
#define END_PERFORMANCE_CHECK( x ) performanceCheck.endPerformanceCheck( __performance_task_id ## x );
#define REGISTER_PERFORMANCE_TASK( x ) __performance_task_id ## x = performanceCheck.registerTaskName( # x );
#define USE_PERFORMANCE_CHECK( x ) unsigned int __performance_task_id ## x ;
#define PRINT_STATISTICS( s ) performanceCheck.printPerformanceStatisticsAsync( s );
#define PERFORMACE_CHECK_CLEAR performanceCheck.clear();

#else

#define START_PERFORMANCE_CHECK( x )
#define END_PERFORMANCE_CHECK( x )
#define REGISTER_PERFORMANCE_TASK( x )
#define USE_PERFORMANCE_CHECK( x )
#define PRINT_STATISTICS( s )
#define PERFORMACE_CHECK_CLEAR 

#endif
