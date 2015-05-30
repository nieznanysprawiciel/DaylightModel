#pragma once
/**@file TimeManager.h
@brief Plik zawiera deklaracj� klasy TimeManager do obs�ugi rzeczy zwi�zanych z czasem.*/

#include "types_definitions.h"


/**@brief Klasa obs�uguje wszystko co zwi�zane z czasem.

Do zada� klasy nale�y:
- pobieranie i przeliczanie czasu
- obs�uga event�w czasowych
- Robienie statystyk wykonania poszczeg�lnych fragment�w kodu

Funkcje zaczynaj�ce si� od query pobieraj� czas z systemu.
Funkcje get zwracaj� warto�ci obliczone wcze�niej i przechowywane w klasie.
Tak naprawd� powinno si� raczej korzysta� z funkcji get, poniewa� w silniku nie liczy si�
czas aktualny, ale czas jaki jest w danej klatce.*/
class TimeManager
{
private:
	//time control
	int64						begin_time;			///<Zawiera czas rozpocz�cia dzia�ania aplikacji. (Tak dok�adniej czas wej�cia w p�tl� renderingu. Wcze�nie odbywa si� wczytywanie Levelu, co mo�e trwa�.)

	int64						timer_frequency;	///<Cz�stotliwo�� timera
	int64						time_previous;		///<B�dziemy mierzy� r�nic� miedzy czasami w kolejnych klatkach.
	int64						elapsed_time;		///<Do licznika FPS�w, czas od ostatniej aktualizacji frames_per_sec
	float						lag;				///<Op�nienie wzgl�dem normalego tempa updatowania sceny
	unsigned int				frames;				///<Zlicza ramki od ostatniego wyzerowania
	float						frames_per_sec;		///<Najwa�niejszy licznik w grze
public:
	TimeManager();
	~TimeManager() = default;

	void initTimer();
	float onStartRenderFrame();
	int64 queryCurrentTime();
	double queryTimeFromBegin();
	inline int64 getBeginTime() { return begin_time; }				///<Zwraca czas jaki zosta� zarejestrowany przy inicjacji silnika.
	inline int64 getTimerFrequency() { return timer_frequency; }	///<Zwraca cz�stotliwo�� taktowania uzywanego timera.
	inline float getTimeLag() { return lag; }						///<Zwraca zmienn� lag.
	inline int64 getTime() { return time_previous; }				///<Zwraca czas, jaki zosta� ostatnio zarejestrowany przy wywo�aniu onStartRenderFrame.
	inline void updateTimeLag( float timeLag ) { lag = timeLag; }	///<Aktualizuje zmienn� lag.
};

