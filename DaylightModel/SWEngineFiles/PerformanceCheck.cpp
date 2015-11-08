#include "stdafx.h"
#include "PerformanceCheck.h"
#include <future>
#include <fstream>
#include <iomanip>



#ifdef PERFORMANCE_CHECK

PerformanceCheck performanceCheck;

#endif

const int floatPrintPrecision = 8;		///< Liczba miejsc po przecinku jakie b�d� wy�wietlane dla float�w.

/**@brief Funkcja wywo�ywana na poczatku wykonywania zadania. Zapisuje
aktualny czas.

@param[in] index Indeks zadania w tablicy performance_datas.*/
void PerformanceCheck::startPerformanceCheck( unsigned int index )
{
	if ( index < performance_datas.size() )
	{
		_LARGE_INTEGER currentTime;
		QueryPerformanceCounter( &currentTime );
		performance_datas[ index ].last_start_performance_value = currentTime.QuadPart;
	}
}

/**@brief Funkcja wywo�ywana pod koniec wykonywania zadania.
Aktualizuje wszystkie statystyki.

@param[in] index Indeks zadania w tablicy performance_datas.*/
void PerformanceCheck::endPerformanceCheck( unsigned int index )
{
	if ( index < performance_datas.size() )
	{
		_LARGE_INTEGER currentTime;
		QueryPerformanceCounter( &currentTime );

		int64 timeSpent = currentTime.QuadPart - performance_datas[ index ].last_start_performance_value;
		performance_datas[ index ].hole_time_spent += timeSpent;

		if ( timeSpent > performance_datas[index].max_time_spent )
			performance_datas[index].max_time_spent = timeSpent;
		else if ( timeSpent < performance_datas[index].min_time_spent )
			performance_datas[index].min_time_spent = timeSpent;

		performance_datas[index].task_executions++;
	}
}

/**@brief Rejestruje nazw� zadania do pomiaru wydajno�ci.

@param[in] taskName Nazwa zadania
@return Zwraca identyfikator zadania, kt�rym mo�na si� odwo�ywa� potem do funkcji mierz�cych czas.*/
unsigned int PerformanceCheck::registerTaskName( const char* taskName )
{
	_performance_data newTask;
	newTask.task_name = taskName;
	newTask.max_time_spent = 0;
	newTask.min_time_spent = UINT32_MAX;
	newTask.task_executions = 0;
	newTask.hole_time_spent = 0;

	performance_datas.push_back( newTask );
	return performance_datas.size() - 1;
}

/**@brief Asynchronicznie drukuje statystyki do podanego pliku.

Poniewa� statystyki s� zapisywane w oddzielnym w�tku, to zbyt szybkie wywo�ania tej funkcji po sobie
mog� spowodowa�, �e na�o�� si� one na siebie.

@param[in] outputFile Plik, do kt�rego maj� trafi� statystyki.*/
void PerformanceCheck::printPerformanceStatisticsAsync( const std::string& outputFile )
{
	std::future<void> printAsync = std::async( print, outputFile, performance_datas );
}

/**@brief Faktyczna funkcja drukuj�ca.*/
void PerformanceCheck::print( const std::string& outputFile, std::vector<_performance_data> data_copy )
{
	LARGE_INTEGER counterFreq;
	QueryPerformanceFrequency( &counterFreq );

	std::fstream output;
	output.open( outputFile.c_str(), std::fstream::out | std::fstream::app );
	if ( !output.good() )
		return;

	output << "Performance test:				Average			Minimum			Maximum" << std::endl;

	for ( auto taskData : data_copy )
	{
		output.width( 32 );
		output << std::left;
		output << taskData.task_name;
		output.width( 0 );
		output << std::fixed;
		output << std::setprecision( floatPrintPrecision ) << taskData.hole_time_spent / double( taskData.task_executions * counterFreq.QuadPart ) << " s	";
		output << std::setprecision( floatPrintPrecision ) << taskData.min_time_spent / double( counterFreq.QuadPart ) << " s	";
		output << std::setprecision( floatPrintPrecision ) << taskData.max_time_spent / double( counterFreq.QuadPart ) << " s" << std::endl;
	}
}


void PerformanceCheck::clear()
{
	performance_datas.clear();
}
