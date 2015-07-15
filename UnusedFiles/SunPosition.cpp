#include "SunPosition.h"
#include "glm/gtx/quaternion.hpp"

const float earth_axis_to_ecliptik = glm::radians( 23.4393f );
const double day_length = 86164.098903691;				///< D³ugoœæ jednego dnia w sekundach (wzglêdem gwiazd).
const double year_length = 365.256 * 24 * 60 * 60;		///< D³ugoœc roku w sekundach.


SunPosition::SunPosition()
{
	earth_rotation_axis = earth_axis_to_ecliptik;
	latitude = 0.0;
	angle_around_sun = 0.0;
	angle_around_earth = 0.0;
}


SunPosition::~SunPosition()
{
}

glm::vec3 SunPosition::computeSunDirection()
{
	glm::vec3 zenith( 0.0, 0.0, -1.0 );
	glm::vec3 south( -1.0, 0.0, 0.0 );
	glm::vec3 right( 0.0, 1.0, 0.0 );

	glm::quat zenith_axis_rotation = glm::angleAxis( -earth_rotation_axis, zenith );
	glm::vec3 earth_axis = glm::vec3( cos( earth_rotation_axis ), sin( earth_rotation_axis ), 0.0 );
	glm::quat latitude_rotation = glm::angleAxis( latitude, glm::normalize( glm::cross( zenith, earth_axis ) ) );

	glm::quat earth_axis_rotation = glm::angleAxis( angle_around_earth, earth_axis );
	glm::quat sun_axis_rotation = glm::angleAxis( -angle_around_sun, glm::vec3( 0.0, 1.0, 0.0 ) );

	glm::quat all_rotations = earth_axis_rotation * zenith_axis_rotation;
	all_rotations = latitude_rotation * all_rotations;
	all_rotations = sun_axis_rotation * all_rotations;

	glm::mat4 apply_rotations = glm::toMat4( all_rotations );

	glm::mat4 change_base_matrix;
	change_base_matrix[0] = glm::vec4( right, 0.0 );
	change_base_matrix[1] = glm::vec4( zenith, 0.0 );
	change_base_matrix[2] = glm::vec4( south, 0.0 );
	change_base_matrix[3] = glm::vec4( 0.0, 0.0, 0.0, 0.0 );
	change_base_matrix = glm::transpose( change_base_matrix );

	change_base_matrix = apply_rotations * change_base_matrix;


	return glm::vec3( change_base_matrix * glm::vec4( 0.0, 0.0, -1.0, 0.0 ) );
}

void SunPosition::setSunConditions( float latit, float longit, tm* time )
{
	time_t seconds = mktime( time );
	double seconds_since_reference_point = (double)seconds;

	setSunConditions( latit, longit, seconds_since_reference_point );
}

void SunPosition::setSunConditions( float latit, float longit, double time )
{
	latitude = latit;
	longitude = longit;

	angle_around_earth = static_cast<float>( glm::radians( longitude ) + glm::radians( float(360.0 * fmod( time, day_length ) / day_length ) ) );
	angle_around_sun = static_cast<float>( glm::radians( float( 360.0 * fmod( time, year_length ) / year_length ) ) );
}

