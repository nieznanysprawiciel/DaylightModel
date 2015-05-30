#include "SunPosition.h"

const float earth_axis_to_ecliptik = DirectX::XMConvertToRadians( 23.4393f );
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

DirectX::XMVECTOR SunPosition::computeSunDirection()
{
	DirectX::XMVECTOR zenith = DirectX::XMVectorSet( 0.0, 0.0, -1.0, 0.0 );
	DirectX::XMVECTOR south = DirectX::XMVectorSet( -1.0, 0.0, 0.0, 0.0 );
	DirectX::XMVECTOR right = DirectX::XMVectorSet( 0.0, 1.0, 0.0, 0.0 );

	DirectX::XMVECTOR zenith_axis_rotation = DirectX::XMQuaternionRotationNormal( zenith, -earth_rotation_axis );
	DirectX::XMVECTOR earth_axis = DirectX::XMVectorSet( cos( earth_rotation_axis ), sin( earth_rotation_axis ), 0.0, 0.0 );
	DirectX::XMVECTOR latitude_rotation = DirectX::XMQuaternionRotationAxis( DirectX::XMVector3Cross( zenith, earth_axis ), latitude );

	DirectX::XMVECTOR earth_axis_rotation = DirectX::XMQuaternionRotationNormal( earth_axis, angle_around_earth );
	DirectX::XMVECTOR sun_axis_rotation = DirectX::XMQuaternionRotationNormal( DirectX::XMVectorSet( 0.0, 1.0, 0.0, 0.0 ), -angle_around_sun );

	DirectX::XMVECTOR all_rotations = DirectX::XMQuaternionMultiply( zenith_axis_rotation, earth_axis_rotation );
	all_rotations = DirectX::XMQuaternionMultiply( all_rotations, latitude_rotation );
	all_rotations = DirectX::XMQuaternionMultiply( all_rotations, sun_axis_rotation );

	DirectX::XMMATRIX apply_rotations = DirectX::XMMatrixRotationQuaternion( all_rotations );

	DirectX::XMMATRIX change_base_matrix;
	change_base_matrix.r[0] = right;
	change_base_matrix.r[1] = zenith;
	change_base_matrix.r[2] = south;
	change_base_matrix.r[3] = DirectX::XMVectorSet( 0.0, 0.0, 0.0, 0.0 );
	change_base_matrix = DirectX::XMMatrixMultiply( change_base_matrix, apply_rotations );

	change_base_matrix = DirectX::XMMatrixTranspose( change_base_matrix );

	return DirectX::XMVector3Transform( DirectX::XMVectorSet( 0.0, 0.0, -1.0, 0.0 ), change_base_matrix );
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

	angle_around_earth = static_cast<float>( DirectX::XMConvertToRadians( longitude ) + DirectX::XMConvertToRadians( float(360.0 * fmod( time, day_length ) / day_length ) ) );
	angle_around_sun = static_cast<float>( DirectX::XMConvertToRadians( float( 360.0 * fmod( time, year_length ) / year_length ) ) );
}

