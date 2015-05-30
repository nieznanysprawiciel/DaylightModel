#pragma once
/**@file TimeManager.h
@brief Plik zawiera deklaracjê klasy TimeManager do obs³ugi rzeczy zwi¹zanych z czasem.*/

#include "types_definitions.h"


/**@brief Klasa obs³uguje wszystko co zwi¹zane z czasem.

Do zadañ klasy nale¿y:
- pobieranie i przeliczanie czasu
- obs³uga eventów czasowych
- Robienie statystyk wykonania poszczególnych fragmentów kodu

Funkcje zaczynaj¹ce siê od query pobieraj¹ czas z systemu.
Funkcje get zwracaj¹ wartoœci obliczone wczeœniej i przechowywane w klasie.
Tak naprawdê powinno siê raczej korzystaæ z funkcji get, poniewa¿ w silniku nie liczy siê
czas aktualny, ale czas jaki jest w danej klatce.*/
class TimeManager
{
private:
	//time control
	int64						begin_time;			///<Zawiera czas rozpoczêcia dzia³ania aplikacji. (Tak dok³adniej czas wejœcia w pêtlê renderingu. Wczeœnie odbywa siê wczytywanie Levelu, co mo¿e trwaæ.)

	int64						timer_frequency;	///<Czêstotliwoœæ timera
	int64						time_previous;		///<Bêdziemy mierzyæ ró¿nicê miedzy czasami w kolejnych klatkach.
	int64						elapsed_time;		///<Do licznika FPSów, czas od ostatniej aktualizacji frames_per_sec
	float						lag;				///<OpóŸnienie wzglêdem normalego tempa updatowania sceny
	unsigned int				frames;				///<Zlicza ramki od ostatniego wyzerowania
	float						frames_per_sec;		///<Najwa¿niejszy licznik w grze
public:
	TimeManager();
	~TimeManager() = default;

	void initTimer();
	float onStartRenderFrame();
	int64 queryCurrentTime();
	double queryTimeFromBegin();
	inline int64 getBeginTime() { return begin_time; }				///<Zwraca czas jaki zosta³ zarejestrowany przy inicjacji silnika.
	inline int64 getTimerFrequency() { return timer_frequency; }	///<Zwraca czêstotliwoœæ taktowania uzywanego timera.
	inline float getTimeLag() { return lag; }						///<Zwraca zmienn¹ lag.
	inline int64 getTime() { return time_previous; }				///<Zwraca czas, jaki zosta³ ostatnio zarejestrowany przy wywo³aniu onStartRenderFrame.
	inline void updateTimeLag( float timeLag ) { lag = timeLag; }	///<Aktualizuje zmienn¹ lag.
};

