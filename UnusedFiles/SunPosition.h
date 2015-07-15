#pragma once

#include <glm/glm.hpp>
#include <ctime>

/**@brief Klasa Wylicza wektor w kierunku s³oñca dla obserwatora stoj¹cego na ziemi.*/
class SunPosition
{
private:
	float		earth_rotation_axis;		///< K¹t nachylenia osi obrotu do p³aszczyzny obiegu wzglêdem S³oñca.
	float		latitude;					///< Szerokoœæ geograficzna na jakiej znajduje siê obserwator.
	float		longitude;					///< D³ugoœæ geograficzna obserwatora.
	float		angle_around_sun;			///< K¹t o jaki obróci³a siê ziemia wokó³ S³oñca.
	float		angle_around_earth;			///< K¹t o jaki obróci³a siê ziemia wzglêdem swojej osi.
public:
	SunPosition();
	~SunPosition();

	glm::vec3 computeSunDirection();
	void setSunConditions( float latit, float longit, tm* time );
	void setSunConditions( float latit, float longit, double time );
};

