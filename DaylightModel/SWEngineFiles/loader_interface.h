#pragma once

/**@file loader_interface.h
@brief Zawiera deklaracj� interfejsu dla loader�w plik�w z modelami.
*/

#include "stdafx.h"
#include "meshes_textures_materials.h"
#include "ModelsManager.h"

/**@brief Warto�ci, mo�liwe do zwr�cenia przez klas� Loader
*/
enum LOADER_RESULT
{
	MESH_LOADING_OK,				///<Poprawne wczytywanie mesha
	MESH_LOADING_WRONG,				///<Niepoprawne wczytywanie mesha
	MESH_LOADING_OUT_OF_MEMORY,		///<Brakuje pami��
	MESH_LOADING_WRONG_FILE_FORMAT	///<Niepoprawny format pliku
};

/**@brief Interfejs klasy do wczytywania plik�w z modelami.
Aby stworzy� klas� do wczytywania w�asnego formatu plik�w nale�y odziedziczy� po tym interfejsie
i zaimplementowa� metody wirtualne.

Dok�adniejsze informacje o tworzeniu w�asnych loader�w znajduj� si� w paragrafie @ref MakingLoaders.
@note Po wywo�aniu konstruktora klasa ma by� w pe�ni gotowa do dzia�ania. Podobnie po ka�dym wywo�aniu funkcji �aduj�cej
plik, klasa tak�e ma by� gotowa do wczytania kolejnego.
*/
class Loader
{
protected:
	ModelsManager*		models_manager;
public:
	Loader(ModelsManager* models_manager) : models_manager(models_manager){};
	virtual ~Loader(){};

	/**@brief Funkcja ma za zadanie poinformowa� czy jest w stanie wczyta� podany plik.

	Implementacja powinna opiera� si� co najwy�ej na sprawdzeniu rozszerzenia pliku, bez zagl�dania
	do �rodka. Dla ka�dego pliku zostanie wywo�ana ta funkcja, aby znale�� pasuj�cy Loader.
	Zagl�danie do pliku mo�e znacznie spowolni� ten proces.

	@param[in] name Nazwa pliku do przetestowania.
	@return True je�eli plik nadaje si� do otworzenia, false w przeciwnym razie.
	*/
	virtual bool can_load(const std::wstring& name) = 0;

	/**@brief Funkcja ma za zadanie wczyta� podany plik.

	Wczytany plik ma zosta� umieszczony w zmiennej new_file_mesh. Klasa pod podanym wska�nikiem
	istnieje w momencie wywo�ania funkcji i trzeba j� wype�nia� tylko za pomoc� funkcji udust�pnianych
	przez klas� Model3DFromFile.
	@param[inout] new_file_mesh Funkcja dostaje zaalokowany obiekt, kt�ry ma zosta� wype�niony danymi.
	@param[in] name Nazwa pliku, kt�ry ma zosta� wczytany
	@return Zwraca wynik wczytywania.
	*/
	virtual LOADER_RESULT load_mesh( Model3DFromFile* new_file_mesh, const std::wstring& name ) = 0;
	//virtual void load_animation(const std::string& name) = 0;
};