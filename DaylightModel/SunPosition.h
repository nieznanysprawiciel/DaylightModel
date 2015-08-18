#pragma once

#include <DirectXMath.h>
#include <ctime>

/**@brief Klasa Wylicza wektor w kierunku s³oñca dla obserwatora stoj¹cego na ziemi.*/
class SunPosition
{
private:
	float		m_latitude;					///< Szerokoœæ geograficzna na jakiej znajduje siê obserwator.
	float		m_longitude;					///< D³ugoœæ geograficzna obserwatora.

	double		m_solarDeclination;
	double		m_daytime;
public:
	SunPosition();
	~SunPosition();

	DirectX::XMVECTOR computeSunDirection();
	double computeAzimuth();
	double computeElevation();
	double computeStandardMeridian( double longitude );

	double setSunConditions( float latit, float longit, tm* time );
	void setSunConditions( double time );
};

