#pragma once
/**@file PerformanceCheck.h
@author nieznanysprawiciel
@copyright Plik jest czêœci¹ silnika graficznego SWEngine.

Zawiera deklaracjê klasy PerformanceCheck, która s³u¿y do pomiaru 
czasu zu¿ywanego na wykonanie wybranych zadañ.*/

#include <string>
#include "types_definitions.h"



/**@brief Struktura przechowuje dane na temat wydajnoœci jednej badanej rzeczy.*/
struct _performance_data
{
	std::string			task_name;						///<Nazwa zadania jaka zostanie wyœwietlona w statystykach.
	int64				last_start_performance_value;	///<Ostatni zarejestrowany czas dla badanej zadania.
	int64				hole_time_spent;				///<Zlicza ca³y czas jaki zosta³ spêdzony na wykonywaniu zadania.
	int64				min_time_spent;					///<Minimalny czas wyknywania zadania.
	int64				max_time_spent;					///<Maksymalny czas wykonywania zadania.
	int32				task_executions;				///<Ile razy zosta³o wykonane zadanie - iloœæ parzystych wywo³añ startPerformanceCheck i EndPerformanceCheck.
};

/**@brief Klasa s³u¿y do pomiaru wydajnoœci poszczególnych fragmentów kodu.

Sposób u¿ycia klasy:
S¹ dwie metody. Mo¿na normalnie korzystaæ z klasy jak ze zwyk³ego obiektu i wywo³ywaæ jej funkcje
lub mo¿na u¿yæ zamiast tego specjalnie zdefiniowanych makr. W drugim przypadku kod s³u¿¹cy do pomiaru bêdzie
za³¹czany jedynie w przypadku zdefiniowania makra PERFORMANCE_CHECK. W innym razie wszystkie u¿yte makra bêd¹
rozwija³y siê do pustych instrukcji.

1. Normalne u¿ywanie klasy.
Aby u¿yæ klasy nale¿y zarejestrowaæ nazwy wybranych zadañ. Robi siê to funkcj¹ registerTaskName.
Funkcja zwraca identyfikator, który potem pos³u¿y do wywo³ywania pomiarów dla tego zadania.

Ka¿de zadanie musi byæ otoczone wywo³aniami funkcji startPerformanceCheck i endPerformanceCheck z podaniem indeksu zadania.

Aby wypisaæ wyniki, nale¿y wywo³aæ funkcjê printPerformanceStatisticsAsync podaj¹c nazwê pliku, do którego statystyki zostan¹ wys³ane.
Funkcja dzia³a asynchronicznie, wiêc nie czeka siê a¿ plik zostanie zapisany, tylko wykonuje kod silnika dalej.

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

	// Tutaj kod, którego wydajnoœæ mierzymy

	stats.endPerformanceCheck( taskID );
}

\endcode

2. U¿ytkowanie klasy z u¿yciem makr.
Aby u¿yæ klasy nale¿y zdefiniowaæ makro PERFORMANCE_CHECK przed zaincludowaniem nag³ówka.
Makro to robi dwie rzeczy:
- w³¹cza definicje pozosta³ych potrzebnych makr
- za³¹cza kod tworz¹cy obiekt tej klasy (obiekt jest zmienn¹ globaln¹ w pliku PerformanceCheck.cpp)

Nastêpnie trzeba zadeklarowaæ chêæ u¿ycia pomiaru dla jakiegoœ zadania wywo³uj¹æ makro USE_PERFORMANCE_CHECK i podaj¹c nazwê
zadania. Makro powinno zostaæ wywo³ane w takim miejscu, jakbyœmy deklarowali zmienn¹, która ma byæ dostêpna w miejscach,
gdzie bêdziemy robili pomiary czasu. (Poniewa¿ to robi niejawnie to makro - definiuje tak¹ zmienn¹).

Nazwa powinna byæ pojedynczym ci¹giem znaków. NIE mo¿e byæ w cudzys³owie.

Zadanie musi zostaæ zarejestrowane przed u¿yciem makr do pomiaru poprzez wywo³anie REGISTER_PERFORMANCE_TASK
z podaniem nazwy zadania, która zostanie potem zapisana w pliku.

W miejscu pomiaru nale¿y wywo³aæ parami makra START_PERFORMANCE_CHECK i END_PERFORMANCE_CHECK podaj¹c w parametrze
taki sam ci¹g znaków, jakim zosta³o zarejestrowane zadanie.

Wypisanie statystyk robi siê za pomoc¹ makra PRINT_STATISTICS i podaje nazwê pliku.
@attention Nazwa pliku jest w cudzys³owie w przeciwieñstwie do innych miejsc, w których wywo³uje siê makra.

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

// Tutaj kod, którego wydajnoœæ mierzymy

	END_PERFORMANCE_CHECK( TASK_NAME )
}

\endcode

*/
class PerformanceCheck
{
private:
	std::vector<_performance_data> performance_datas;		///<Zapisane dane na temat poszczególnych zadañ, dla których mierzymy wydajnoœæ.

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
