#include "SunPosition.h"

#include <cmath>

const double M_PI = 3.14159265358979323846;




SunPosition::SunPosition()
{
	m_latitude = 0.0;
	m_longitude = 0.0;

	m_solarDeclination = 0.0;
	m_daytime = 0.0;
}


SunPosition::~SunPosition()
{
}

DirectX::XMVECTOR SunPosition::computeSunDirection()
{
	double solarAzimuth = computeAzimuth();
	double solarElevation = computeElevation();

	double cosSolarElevation = cos( solarElevation );

	return DirectX::XMVectorSet( cosSolarElevation * sin( solarAzimuth ), sin( solarElevation ), cosSolarElevation * cos( solarAzimuth ), 0.0f );
}

double SunPosition::computeAzimuth()
{
	return atan2( -cos( m_solarDeclination ) * sin( M_PI * m_daytime / 12.0 ), cos( m_latitude ) * sin( m_solarDeclination ) - sin( m_latitude ) * cos( m_solarDeclination ) * cos( M_PI * m_daytime / 12.0 ) );
}

double SunPosition::computeElevation()
{
	return asin( sin( m_latitude ) * sin( m_solarDeclination ) - cos( m_latitude ) * cos( m_solarDeclination ) * cos( M_PI * m_daytime / 12.0 ) );
}

void SunPosition::setSunConditions( float latit, float longit, tm* time )
{
	mktime( time );

	// Czas standardowy w godzinach
	double standard_time = time->tm_hour + time->tm_min / 60.0 + time->tm_sec / 3600.0;
	double julian_day = time->tm_yday + 1;
	double solar_time = standard_time + 0.17*sin( 4* M_PI * ( julian_day - 80 ) / 373.0 );
	solar_time -= 0.129 * ( 2 * M_PI * ( julian_day - 8 ) / 355 );
	//solar_time +=	time_zone

	m_solarDeclination = 0.4093 * sin( 2 * M_PI * ( julian_day - 81 ) / 368.0 );

	setSunConditions( latit, longit, solar_time );
}

void SunPosition::setSunConditions( float latit, float longit, double time )
{
	m_latitude = latit;
	m_longitude = longit;
	m_daytime = time;
}

