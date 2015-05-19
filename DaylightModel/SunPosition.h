#pragma once

#include <DirectXMath.h>
#include <ctime>

/**@brief Klasa Wylicza wektor w kierunku s�o�ca dla obserwatora stoj�cego na ziemi.*/
class SunPosition
{
private:
	float		earth_rotation_axis;		///< K�t nachylenia osi obrotu do p�aszczyzny obiegu wzgl�dem S�o�ca.
	float		latitude;					///< Szeroko�� geograficzna na jakiej znajduje si� obserwator.
	float		longitude;					///< D�ugo�� geograficzna obserwatora.
	float		angle_around_sun;			///< K�t o jaki obr�ci�a si� ziemia wok� S�o�ca.
	float		angle_around_earth;			///< K�t o jaki obr�ci�a si� ziemia wzgl�dem swojej osi.
public:
	SunPosition();
	~SunPosition();

	DirectX::XMVECTOR computeSunDirection();
	void setSunConditions( float latit, float longit, tm* time );
	void setSunConditions( float latit, float longit, double time );
};

