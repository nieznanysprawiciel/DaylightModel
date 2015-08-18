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

	return DirectX::XMVector3Normalize( DirectX::XMVectorSet( cosSolarElevation * sin( solarAzimuth ), sin( solarElevation ), cosSolarElevation * cos( solarAzimuth ), 0.0f ) );
}

double SunPosition::computeStandardMeridian( double longitude )
{
	double i = -187.5;
	for( ; i < longitude; i += 15 );
	i -= 7.5;

	if( i < -170 )
		return abs( i );
	return (double)i;
}

double SunPosition::computeAzimuth()
{
	double translateFun = -3 * M_PI;
	return atan2( -cos( m_solarDeclination ) * sin( M_PI * m_daytime / 12.0 - translateFun ), ( cos( m_latitude ) * sin( m_solarDeclination ) - sin( m_latitude ) * cos( m_solarDeclination ) * cos( M_PI * m_daytime / 12.0 - translateFun ) ) );
}

double SunPosition::computeElevation()
{
	return /*M_PI / 2 - */asin( sin( m_latitude ) * sin( m_solarDeclination ) - cos( m_latitude ) * cos( m_solarDeclination ) * cos( M_PI * m_daytime / 12.0 ) );
}

double SunPosition::setSunConditions( float latit, float longit, tm* time )
{
	mktime( time );

	m_latitude = static_cast<float>( latit *  M_PI / 180 );
	m_longitude = static_cast<float>( longit *  M_PI / 180 );

	// Czas standardowy w godzinach
	double standardMeridian = computeStandardMeridian( m_longitude );
	double standard_time = time->tm_hour + time->tm_min / 60.0 + time->tm_sec / 3600.0;
	double julian_day = time->tm_yday + 1;
	double solar_time = standard_time + 0.17*sin( 4* M_PI * ( julian_day - 80 ) / 373.0 );
	solar_time -= 0.129 * ( 2 * M_PI * ( julian_day - 8 ) / 355 );
	//solar_time += 12 * ( standardMeridian - m_longitude ) / M_PI;

	m_solarDeclination = -0.4093 * sin( 2 * M_PI * ( julian_day - 81 ) / 365.0 );

 	setSunConditions( solar_time );
	return solar_time;
}

void SunPosition::setSunConditions( double time )
{
	m_daytime = time;
}

