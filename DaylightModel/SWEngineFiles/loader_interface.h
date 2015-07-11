#pragma once

/**@file loader_interface.h
@brief Zawiera deklaracjê interfejsu dla loaderów plików z modelami.
*/

#include "stdafx.h"
#include "meshes_textures_materials.h"
#include "ModelsManager.h"

/**@brief Wartoœci, mo¿liwe do zwrócenia przez klasê Loader
*/
enum LOADER_RESULT
{
	MESH_LOADING_OK,				///<Poprawne wczytywanie mesha
	MESH_LOADING_WRONG,				///<Niepoprawne wczytywanie mesha
	MESH_LOADING_OUT_OF_MEMORY,		///<Brakuje pamiêæ
	MESH_LOADING_WRONG_FILE_FORMAT	///<Niepoprawny format pliku
};

/**@brief Interfejs klasy do wczytywania plików z modelami.
Aby stworzyæ klasê do wczytywania w³asnego formatu plików nale¿y odziedziczyæ po tym interfejsie
i zaimplementowaæ metody wirtualne.

Dok³adniejsze informacje o tworzeniu w³asnych loaderów znajduj¹ siê w paragrafie @ref MakingLoaders.
@note Po wywo³aniu konstruktora klasa ma byæ w pe³ni gotowa do dzia³ania. Podobnie po ka¿dym wywo³aniu funkcji ³aduj¹cej
plik, klasa tak¿e ma byæ gotowa do wczytania kolejnego.
*/
class Loader
{
protected:
	ModelsManager*		models_manager;
public:
	Loader(ModelsManager* models_manager) : models_manager(models_manager){};
	virtual ~Loader(){};

	/**@brief Funkcja ma za zadanie poinformowaæ czy jest w stanie wczytaæ podany plik.

	Implementacja powinna opieraæ siê co najwy¿ej na sprawdzeniu rozszerzenia pliku, bez zagl¹dania
	do œrodka. Dla ka¿dego pliku zostanie wywo³ana ta funkcja, aby znaleŸæ pasuj¹cy Loader.
	Zagl¹danie do pliku mo¿e znacznie spowolniæ ten proces.

	@param[in] name Nazwa pliku do przetestowania.
	@return True je¿eli plik nadaje siê do otworzenia, false w przeciwnym razie.
	*/
	virtual bool can_load(const std::wstring& name) = 0;

	/**@brief Funkcja ma za zadanie wczytaæ podany plik.

	Wczytany plik ma zostaæ umieszczony w zmiennej new_file_mesh. Klasa pod podanym wskaŸnikiem
	istnieje w momencie wywo³ania funkcji i trzeba j¹ wype³niaæ tylko za pomoc¹ funkcji udustêpnianych
	przez klasê Model3DFromFile.
	@param[inout] new_file_mesh Funkcja dostaje zaalokowany obiekt, który ma zostaæ wype³niony danymi.
	@param[in] name Nazwa pliku, który ma zostaæ wczytany
	@return Zwraca wynik wczytywania.
	*/
	virtual LOADER_RESULT load_mesh( Model3DFromFile* new_file_mesh, const std::wstring& name ) = 0;
	//virtual void load_animation(const std::string& name) = 0;
};