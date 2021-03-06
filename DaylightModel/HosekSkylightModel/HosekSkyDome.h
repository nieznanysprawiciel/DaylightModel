#pragma once

/**@file HosekSkyDome.h
@brief Plik zawiera deklaracj� klasy HosekSkyDome, do wy�wietlania nieba
u�ywaj�c modelu Hodeka i Wilkiego.*/

#include "..\SWEngineFiles\SkyDome.h"
#include "HosekSkyModel.h"
#include <DirectXMath.h>


/**@brief Struktura wierzcho�ka u�ywana przez klas� HosekSkyDome.

Kolor wierzcho�ka jest ustalany jest przypisaywany na sta�e do wierzcho�k�w
i mo�e by� zmieniony po wywo�aniu funkcji update_sky_dome. Kopu�a nieba
nie jest w �aden spos�b o�wietlona wi�c nie przechowujemy normalnych.
Nie u�ywamy te� tekstur, wi�c nie potrzeba �adnych UVs�w.*/
struct SkyDomeVertex
{
	DirectX::XMFLOAT3 position;		///<Pozycja wierzcho�ka
	DirectX::XMFLOAT3 color;		///<Kolor wierzcho�ka
};

struct SkyDomeConstants
{
	DirectX::XMFLOAT4X4		dither_mask;
	//DirectX::XMFLOAT4		limb_darkening[7];
	DirectX::XMFLOAT4		sun_base_color;
	DirectX::XMFLOAT3		sun_direction;
	float					solar_radius;
	
};


/**@brief Klasa s�u�y do wy�wietlania kopu�y nieba.

Wierzcho�ki i indeksy s� zapisywane do bufor�w tylnych. Nast�pnie po zako�czeniu generowania
s� przepisywane w jednym momencie do bufor�w DirectXowych. Dzieki temu mo�emy generowa� nowy uk�ad nieba
w innym w�tku, a potem w jednym momencie go przepisa�.
Generowanie nieba jest zasadniczo procesem d�ugim i nie mo�na go wykonywac w czasie rzeczywistym.*/
class HosekSkyDome : public SkyDome
{
private:
	VERT_INDEX*				back_ind_buffer;		///<Tylny bufor indeks�w. 
	SkyDomeVertex*			back_vert_buffer;		///<Tylny bufor wierzcho�k�w.
	unsigned int			vert_count;				///<Liczba wierzcho�k�w w buforze wierzcho�k�w

	HosekSkylightModel		skylight_model;			///<Obiekt, kt�ry b�dzie podawa� informacje o kolorach

	BufferObject*			constantBuffer;			///<Bufor sta�ych u�ywanych do ditheringu.
	SkyDomeConstants		constantBufferData;

	void generate_sphere(int vertical, int horizontal, float radius );
public:
	HosekSkyDome( ModelsManager* man);
	~HosekSkyDome();

	void init_sky_dome( DirectX::XMVECTOR sun_direction,
						const double turbidity,
						const double* albedo,
						int vertical,
						int horizontal,
						float radius,
						float sky_intensity = 1.0f,
						float sun_intensity = 1.0f );
	void update_sky_dome( DirectX::XMVECTOR sun_direction,
						  const double turbidity,
						  const double* albedo,
						  float sky_intensity = 1.0f,
						  float sun_intensity = 1.0f );
	void update_buffers( ) override;
};

