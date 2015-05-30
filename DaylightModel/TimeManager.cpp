#include "stdafx.h"
#include "TimeManager.h"

//#include "memory_leaks.h"

/**@brief Kontruktor inicjuje wartoœci frames i frames_per_sec na 0.*/
TimeManager::TimeManager()
{
	frames = 0;
	frames_per_sec = 0;
	lag = 0.0;
	timer_frequency = 1;	// ¯eby unikn¹æ dzielenia przez zero, je¿eli ktoœ nie zainicjuje timera.
}

/**@brief Funkcja wywo³ywana po zainicjowaniu silnika i przed wejœciem
w pêtlê renderingu. Ustawia odpowiednie dane dla zegara, w tym czêstotliwoœæ oraz czas pocz¹tkowy.*/
void TimeManager::initTimer()
{
	LARGE_INTEGER timeTemp;

	//Sprawdzamy czêstotliwoœæ timera systemowego, a potem inicjujemy licznik czasu
	QueryPerformanceFrequency( &timeTemp );
	timer_frequency = timeTemp.QuadPart;
	QueryPerformanceCounter( &timeTemp );		//inicjujemy licznik czasu
	time_previous = timeTemp.QuadPart;
	elapsed_time = time_previous;

	begin_time = time_previous;			// Poniewa¿ w³aœnie zainicjowaliœmy timer, to zapisujemy czas inicjacji.
}

/**@brief Funkcja oblicza interwa³ czasowy jaki up³yn¹³ od ostatniej ramki.

Poza tym s¹ tu generowane eventy dotycz¹ce czasu, opóŸnieñ itp.
@return Zwraca interwa³ jaki up³yn¹³ od ostatniego wywo³ania funkcji.*/
float TimeManager::onStartRenderFrame()
{
	int64 timeCurrent = queryCurrentTime();

	int64 timeDiff;
	timeDiff = timeCurrent - time_previous;
	float timeInterval = (float)timeDiff / timer_frequency;

	lag += timeInterval;


	//todo:	generujemy eventy czasowe

	//zapisujemy obecny czas i wychodzimy z funkcji
	time_previous = timeCurrent;
	++frames;		//inkrementujemy licznik klatek
	return timeInterval;
}

/**@brief Wywo³uje QueryPerformanceCounter, ¿eby pobraæ aktualny czas.

Nale¿y pamiêtaæ, ¿e najczêœciej interesuje nas czas aktualnej klatki albo lag.
Mo¿na je pobraæ funkcjami TimeManager::getTime i TimeManager::getTimeLag.
*/
int64 TimeManager::queryCurrentTime()
{
	LARGE_INTEGER time_temp;
	QueryPerformanceCounter( &time_temp );
	return time_temp.QuadPart;
}

/**@brief Zwraca czas w sekundach od w³¹czenia silnika.
*/
double TimeManager::queryTimeFromBegin()
{
	int64 currentTime = queryCurrentTime();
	int64 timeDiff = currentTime - begin_time;
	return (double)timeDiff / timer_frequency;
}
