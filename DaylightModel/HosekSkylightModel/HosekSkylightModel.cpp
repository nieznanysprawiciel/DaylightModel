/*
This source is published under the following 3-clause BSD license.

Copyright (c) 2012 - 2013, Lukas Hosek and Alexander Wilkie
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

* Redistributions of source code must retain the above copyright
notice, this list of conditions and the following disclaimer.
* Redistributions in binary form must reproduce the above copyright
notice, this list of conditions and the following disclaimer in the
documentation and/or other materials provided with the distribution.
* None of the names of the contributors may be used to endorse or promote
products derived from this software without specific prior written
permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDERS BE LIABLE FOR ANY
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

/* ============================================================================

This file is part of a sample implementation of the analytical skylight and
solar radiance models presented in the SIGGRAPH 2012 paper


"An Analytic Model for Full Spectral Sky-Dome Radiance"

and the 2013 IEEE CG&A paper

"Adding a Solar Radiance Function to the Hosek Skylight Model"

both by

Lukas Hosek and Alexander Wilkie
Charles University in Prague, Czech Republic


Version: 1.4a, February 22nd, 2013

Version history:

1.4a  February 22nd, 2013
Removed unnecessary and counter-intuitive solar radius parameters
from the interface of the colourspace sky dome initialisation functions.

1.4   February 11th, 2013
Fixed a bug which caused the relative brightness of the solar disc
and the sky dome to be off by a factor of about 6. The sun was too
bright: this affected both normal and alien sun scenarios. The
coefficients of the solar radiance function were changed to fix this.

1.3   January 21st, 2013 (not released to the public)
Added support for solar discs that are not exactly the same size as
the terrestrial sun. Also added support for suns with a different
emission spectrum ("Alien World" functionality).

1.2a  December 18th, 2012
Fixed a mistake and some inaccuracies in the solar radiance function
explanations found in ArHosekSkyModel.h. The actual source code is
unchanged compared to version 1.2.

1.2   December 17th, 2012
Native RGB data and a solar radiance function that matches the turbidity
conditions were added.

1.1   September 2012
The coefficients of the spectral model are now scaled so that the output
is given in physical units: W / (m^-2 * sr * nm). Also, the output of the
XYZ model is now no longer scaled to the range [0...1]. Instead, it is
the result of a simple conversion from spectral data via the CIE 2 degree
standard observer matching functions. Therefore, after multiplication
with 683 lm / W, the Y channel now corresponds to luminance in lm.

1.0   May 11th, 2012
Initial release.


Please visit http://cgg.mff.cuni.cz/projects/SkylightModelling/ to check if
an updated version of this code has been published!

============================================================================ */

#include "..\stdafx.h"
#include "HosekSkyModel.h"
#include "ArHosekSkyModelData_RGB.h"
#include "HosekSkyModelSolarRadiance_RGB.h"

#ifndef MATH_PI 
#define MATH_PI                     3.141592653589793
#endif

#ifndef MATH_DEG_TO_RAD
#define MATH_DEG_TO_RAD             ( MATH_PI / 180.0 )
#endif

#ifndef MATH_RAD_TO_DEG
#define MATH_RAD_TO_DEG             ( 180.0 / MATH_PI )
#endif

#ifndef DEGREES
#define DEGREES                     * MATH_DEG_TO_RAD
#endif

#ifndef TERRESTRIAL_SOLAR_RADIUS
	#define TERRESTRIAL_SOLAR_RADIUS    ( ( 0.51 DEGREES ) / 2.0 )
#endif


//#include "..\..\..\memory_leaks.h"

using namespace DirectX;


HosekSkylightModel::HosekSkylightModel()
{
	sky_intensity = 1.0f;
	sun_intensity = 1.0f;

	for ( int i = 0; i < 3; ++i )
	{
		skymodel_state[ i ] = new ArHosekSkyModelState;
		skymodel_state[ i ]->solar_radius = TERRESTRIAL_SOLAR_RADIUS;
	}
}


HosekSkylightModel::~HosekSkylightModel()
{
	for ( int i = 0; i < 3; ++i )
		if ( skymodel_state[i] )
			delete skymodel_state[i];
}


/**@brief Funkcja s³u¿y do zainicjowania modelu.

Inicjacja modelu nie mo¿e siê odbywaæ w konstruktorze z dwóch powodów.
Po pierwsze chcemy mieæ mo¿liwoœæ modyfikowania modelu w trakcie. Zmiany wymagaj¹
wywo³ania funkcji init dla nowych parametrów.
Po drugie inicjacja nie jest procesem trywialnym i mo¿e zajmowac trochê czasu.
Z tego wzglêdu chcemy mieæ mo¿liwoœæ wys³ania inicjalizacji do innego w¹tku.

@param[in] turbidity Mêtnoœæ atmosfery.
@param[in] albedo Tablica albedo dla ka¿dego z kana³ów RGB.
@param[in] elevation Wysokoœæ s³oñca.*/
void HosekSkylightModel::init( const double turbidity,
							   const double* albedo,
							   const double elevation,
							   float sky_intens,
							   float sun_intens )
{
	sky_intensity = sky_intens / (float)255;
	sun_intensity = sun_intens / (float)255;

	for ( int j = 0; j < 3; ++j )
	{
		skymodel_state[j]->solar_radius = TERRESTRIAL_SOLAR_RADIUS;
		skymodel_state[j]->turbidity = turbidity;
		skymodel_state[j]->albedo = albedo[j];
		skymodel_state[j]->elevation = elevation;

		unsigned int channel = 0;
		for ( ; channel < 3; ++channel )
		{
			ArHosekSkyModel_CookConfiguration(
				datasetsRGB[channel],
				skymodel_state[j]->configs[channel],
				turbidity,
				albedo[j],
				elevation
				);

			skymodel_state[j]->radiances[channel] =
				ArHosekSkyModel_CookRadianceConfiguration(
				datasetsRGBRad[channel],
				turbidity,
				albedo[j],
				elevation
				);
		}
	}
}

void HosekSkylightModel::ArHosekSkyModel_CookConfiguration(
	ArHosekSkyModel_Dataset       dataset,
	ArHosekSkyModelConfiguration  config,
	double                        turbidity,
	double                        albedo,
	double                        solar_elevation
	)
{
	double  * elev_matrix;

	int     int_turbidity = (int)turbidity;
	double  turbidity_rem = turbidity - (double)int_turbidity;

	solar_elevation = pow( solar_elevation / (MATH_PI / 2.0), (1.0 / 3.0) );

	// alb 0 low turb

	elev_matrix = dataset + (9 * 6 * (int_turbidity - 1));

	unsigned int i = 0;
	for ( ; i < 9; ++i )
	{
		//(1-t).^3* A1 + 3*(1-t).^2.*t * A2 + 3*(1-t) .* t .^ 2 * A3 + t.^3 * A4;
		config[i] =
			(1.0 - albedo) * (1.0 - turbidity_rem)
			* (pow( 1.0 - solar_elevation, 5.0 ) * elev_matrix[i] +
			5.0  * pow( 1.0 - solar_elevation, 4.0 ) * solar_elevation * elev_matrix[i + 9] +
			10.0*pow( 1.0 - solar_elevation, 3.0 )*pow( solar_elevation, 2.0 ) * elev_matrix[i + 18] +
			10.0*pow( 1.0 - solar_elevation, 2.0 )*pow( solar_elevation, 3.0 ) * elev_matrix[i + 27] +
			5.0*(1.0 - solar_elevation)*pow( solar_elevation, 4.0 ) * elev_matrix[i + 36] +
			pow( solar_elevation, 5.0 )  * elev_matrix[i + 45]);
	}

	// alb 1 low turb
	elev_matrix = dataset + (9 * 6 * 10 + 9 * 6 * (int_turbidity - 1));
	for ( i = 0; i < 9; ++i )
	{
		//(1-t).^3* A1 + 3*(1-t).^2.*t * A2 + 3*(1-t) .* t .^ 2 * A3 + t.^3 * A4;
		config[i] +=
			(albedo)* (1.0 - turbidity_rem)
			* (pow( 1.0 - solar_elevation, 5.0 ) * elev_matrix[i] +
			5.0  * pow( 1.0 - solar_elevation, 4.0 ) * solar_elevation * elev_matrix[i + 9] +
			10.0*pow( 1.0 - solar_elevation, 3.0 )*pow( solar_elevation, 2.0 ) * elev_matrix[i + 18] +
			10.0*pow( 1.0 - solar_elevation, 2.0 )*pow( solar_elevation, 3.0 ) * elev_matrix[i + 27] +
			5.0*(1.0 - solar_elevation)*pow( solar_elevation, 4.0 ) * elev_matrix[i + 36] +
			pow( solar_elevation, 5.0 )  * elev_matrix[i + 45]);
	}

	if ( int_turbidity == 10 )
		return;

	// alb 0 high turb
	elev_matrix = dataset + (9 * 6 * (int_turbidity));
	for ( i = 0; i < 9; ++i )
	{
		//(1-t).^3* A1 + 3*(1-t).^2.*t * A2 + 3*(1-t) .* t .^ 2 * A3 + t.^3 * A4;
		config[i] +=
			(1.0 - albedo) * (turbidity_rem)
			* (pow( 1.0 - solar_elevation, 5.0 ) * elev_matrix[i] +
			5.0  * pow( 1.0 - solar_elevation, 4.0 ) * solar_elevation * elev_matrix[i + 9] +
			10.0*pow( 1.0 - solar_elevation, 3.0 )*pow( solar_elevation, 2.0 ) * elev_matrix[i + 18] +
			10.0*pow( 1.0 - solar_elevation, 2.0 )*pow( solar_elevation, 3.0 ) * elev_matrix[i + 27] +
			5.0*(1.0 - solar_elevation)*pow( solar_elevation, 4.0 ) * elev_matrix[i + 36] +
			pow( solar_elevation, 5.0 )  * elev_matrix[i + 45]);
	}

	// alb 1 high turb
	elev_matrix = dataset + (9 * 6 * 10 + 9 * 6 * (int_turbidity));
	for ( i = 0; i < 9; ++i )
	{
		//(1-t).^3* A1 + 3*(1-t).^2.*t * A2 + 3*(1-t) .* t .^ 2 * A3 + t.^3 * A4;
		config[i] +=
			(albedo)* (turbidity_rem)
			* (pow( 1.0 - solar_elevation, 5.0 ) * elev_matrix[i] +
			5.0  * pow( 1.0 - solar_elevation, 4.0 ) * solar_elevation * elev_matrix[i + 9] +
			10.0*pow( 1.0 - solar_elevation, 3.0 )*pow( solar_elevation, 2.0 ) * elev_matrix[i + 18] +
			10.0*pow( 1.0 - solar_elevation, 2.0 )*pow( solar_elevation, 3.0 ) * elev_matrix[i + 27] +
			5.0*(1.0 - solar_elevation)*pow( solar_elevation, 4.0 ) * elev_matrix[i + 36] +
			pow( solar_elevation, 5.0 )  * elev_matrix[i + 45]);
	}
}

double HosekSkylightModel::ArHosekSkyModel_CookRadianceConfiguration(
	ArHosekSkyModel_Radiance_Dataset  dataset,
	double                            turbidity,
	double                            albedo,
	double                            solar_elevation
	)
{
	double* elev_matrix;

	int int_turbidity = (int)turbidity;
	double turbidity_rem = turbidity - (double)int_turbidity;
	double res;
	solar_elevation = pow( solar_elevation / (MATH_PI / 2.0), (1.0 / 3.0) );

	// alb 0 low turb
	elev_matrix = dataset + (6 * (int_turbidity - 1));
	//(1-t).^3* A1 + 3*(1-t).^2.*t * A2 + 3*(1-t) .* t .^ 2 * A3 + t.^3 * A4;
	res = (1.0 - albedo) * (1.0 - turbidity_rem) *
		(pow( 1.0 - solar_elevation, 5.0 ) * elev_matrix[0] +
		5.0*pow( 1.0 - solar_elevation, 4.0 )*solar_elevation * elev_matrix[1] +
		10.0*pow( 1.0 - solar_elevation, 3.0 )*pow( solar_elevation, 2.0 ) * elev_matrix[2] +
		10.0*pow( 1.0 - solar_elevation, 2.0 )*pow( solar_elevation, 3.0 ) * elev_matrix[3] +
		5.0*(1.0 - solar_elevation)*pow( solar_elevation, 4.0 ) * elev_matrix[4] +
		pow( solar_elevation, 5.0 ) * elev_matrix[5]);

	// alb 1 low turb
	elev_matrix = dataset + (6 * 10 + 6 * (int_turbidity - 1));
	//(1-t).^3* A1 + 3*(1-t).^2.*t * A2 + 3*(1-t) .* t .^ 2 * A3 + t.^3 * A4;
	res += (albedo)* (1.0 - turbidity_rem) *
		(pow( 1.0 - solar_elevation, 5.0 ) * elev_matrix[0] +
		5.0*pow( 1.0 - solar_elevation, 4.0 )*solar_elevation * elev_matrix[1] +
		10.0*pow( 1.0 - solar_elevation, 3.0 )*pow( solar_elevation, 2.0 ) * elev_matrix[2] +
		10.0*pow( 1.0 - solar_elevation, 2.0 )*pow( solar_elevation, 3.0 ) * elev_matrix[3] +
		5.0*(1.0 - solar_elevation)*pow( solar_elevation, 4.0 ) * elev_matrix[4] +
		pow( solar_elevation, 5.0 ) * elev_matrix[5]);
	if ( int_turbidity == 10 )
		return res;

	// alb 0 high turb
	elev_matrix = dataset + (6 * (int_turbidity));
	//(1-t).^3* A1 + 3*(1-t).^2.*t * A2 + 3*(1-t) .* t .^ 2 * A3 + t.^3 * A4;
	res += (1.0 - albedo) * (turbidity_rem)*
		(pow( 1.0 - solar_elevation, 5.0 ) * elev_matrix[0] +
		5.0*pow( 1.0 - solar_elevation, 4.0 )*solar_elevation * elev_matrix[1] +
		10.0*pow( 1.0 - solar_elevation, 3.0 )*pow( solar_elevation, 2.0 ) * elev_matrix[2] +
		10.0*pow( 1.0 - solar_elevation, 2.0 )*pow( solar_elevation, 3.0 ) * elev_matrix[3] +
		5.0*(1.0 - solar_elevation)*pow( solar_elevation, 4.0 ) * elev_matrix[4] +
		pow( solar_elevation, 5.0 ) * elev_matrix[5]);

	// alb 1 high turb
	elev_matrix = dataset + (6 * 10 + 6 * (int_turbidity));
	//(1-t).^3* A1 + 3*(1-t).^2.*t * A2 + 3*(1-t) .* t .^ 2 * A3 + t.^3 * A4;
	res += (albedo)* (turbidity_rem)*
		(pow( 1.0 - solar_elevation, 5.0 ) * elev_matrix[0] +
		5.0*pow( 1.0 - solar_elevation, 4.0 )*solar_elevation * elev_matrix[1] +
		10.0*pow( 1.0 - solar_elevation, 3.0 )*pow( solar_elevation, 2.0 ) * elev_matrix[2] +
		10.0*pow( 1.0 - solar_elevation, 2.0 )*pow( solar_elevation, 3.0 ) * elev_matrix[3] +
		5.0*(1.0 - solar_elevation)*pow( solar_elevation, 4.0 ) * elev_matrix[4] +
		pow( solar_elevation, 5.0 ) * elev_matrix[5]);
	return res;
}



/**@brief Zwraca kolor nieba w podanym punkcie, nie uwzglêdniaj¹c promieni,
które dochodz¹ bezpoœrednio od s³oñca.

Funkcja zawiera du¿o obliczeñ, które s¹ tak napisane, ¿eby szybciej siê wykonywa³y, wiêc s¹ ma³o przejrzyste.
Je¿eli ktoœ pragnie zrozumieæ co siê dzieje, mo¿e obejrzeæ implementacjê stworzon¹ przez twórców
lub porównaæ kod z funkcj¹, która jest zawarta w ich pracy na temat modelowania nieba.

@param[in] theta K¹t miêdzy zenithem a kierunkiem patrzenia.
@param[in] gamma K¹t miedzy s³oñcem a kierunkiem patrzenia.
@return Wektor kolorów RGB.*/
DirectX::XMVECTOR HosekSkylightModel::sky_radiance( float theta, float gamma )
{
	// Dla ³atwiejszego dostêpu
	double* configR = skymodel_state[0]->configs[0];	// Tablica wspó³czynników dla kana³y R
	double* configG = skymodel_state[1]->configs[1];	// Tablica wspó³czynników dla kana³y G
	double* configB = skymodel_state[2]->configs[2];	// Tablica wspó³czynników dla kana³y B

	// Obliczamy wyniki posrednie w celu optymalizacji dalszych obliczeñ
	float cos_theta = cos( theta );
	float cos_gamma = cos( gamma );
	XMVECTOR rayM = XMVectorReplicate( cos_gamma*cos_gamma );		// Wynik poœredni do kolejnych obliczeñ
	XMVECTOR zenith = XMVectorReplicate( sqrt( cos_theta ) );		// Wynik poœredni do kolejnych obliczeñ

	XMVECTOR expM = XMVectorSet( static_cast<float>(configR[4]),
								 static_cast<float>(configG[4]),
								 static_cast<float>(configB[4]),
								 0.0f );
	XMVECTOR vec_gamma = XMVectorReplicate( gamma );
	expM = expM * vec_gamma;
	expM = XMVectorExpE( expM );									// Wynik poœredni do kolejnych obliczeñ

	XMVECTOR mieM = rayM + XMVectorReplicate( 1.0f );
	XMVECTOR config8 = XMVectorSet( static_cast<float>(configR[8]), static_cast<float>(configG[8]), static_cast<float>(configB[8]), 1.0f );
	XMVECTOR base = XMVectorReplicate( 1.0f ) + config8 * config8;
	base -= config8 * XMVectorReplicate( 2.0f * cos_gamma );
	mieM = mieM / XMVectorPow( base, XMVectorReplicate( 1.5f ) );	// Wynik poœredni do kolejnych obliczeñ

	// Treaz wykonujemy faktyczne obliczenia, czyli podstawiamy wszystkie wspó³czynniki i parametry do funkcji
	XMVECTOR result = XMVectorSet( static_cast<float>(configR[2]), static_cast<float>(configG[2]), static_cast<float>(configB[2]), 1.0f );
	result += expM * XMVectorSet( static_cast<float>(configR[3]), static_cast<float>(configG[3]), static_cast<float>(configB[3]), 1.0f );
	result += rayM * XMVectorSet( static_cast<float>(configR[5]), static_cast<float>(configG[5]), static_cast<float>(configB[5]), 1.0f );
	result += mieM * XMVectorSet( static_cast<float>(configR[6]), static_cast<float>(configG[6]), static_cast<float>(configB[6]), 1.0f );
	result += zenith * XMVectorSet( static_cast<float>(configR[7]), static_cast<float>(configG[7]), static_cast<float>(configB[7]), 1.0f );

	XMVECTOR middle_result = XMVectorReplicate( cos_theta );
	middle_result += XMVectorReplicate( 0.01f );
	middle_result = XMVectorSet( static_cast<float>(configR[1]), static_cast<float>(configG[1]), static_cast<float>(configB[1]), 1.0f ) / middle_result;
	middle_result = XMVectorExpE( middle_result );
	middle_result *= XMVectorSet( static_cast<float>(configR[0]), static_cast<float>(configG[0]), static_cast<float>(configB[0]), 1.0f );
	middle_result += XMVectorReplicate( 1.0f );

	result = result * middle_result;

	//Teraz trzeba zrobiæ coœ co normalnie by³o robione zewnêtrznie
	XMVECTOR radiances = XMVectorSet( static_cast<float>(skymodel_state[0]->radiances[0]),
									  static_cast<float>(skymodel_state[1]->radiances[1]),
									  static_cast<float>(skymodel_state[2]->radiances[2]), 1.0f );
	radiances *= XMVectorReplicate( sky_intensity );
	result *= radiances;

	return result;
}

/**@brief Zwraca kolor s³oñca z promieniowania docieraj¹cego bezpoœrednio.
Nie uwzglêdnia efektu limb darkening.

Wersja jest nieoptymalna i da siê osi¹gn¹æ o wiele wiêcej, ale to ju¿ zadanie na póŸniej.

@return Wektor kolorów RGB.*/
DirectX::XMFLOAT4 HosekSkylightModel::sun_base_color()
{
	int     turb_low = (int)skymodel_state[ 0 ]->turbidity - 1;
	double  turb_frac = skymodel_state[ 0 ]->turbidity - (double)(turb_low + 1);

	if( turb_low == 9 )
	{
		turb_low = 8;
		turb_frac = 1.0;
	}

	float direct_radiance[3];

	for( unsigned int channel = 0; channel < 3; ++channel )
	{
		direct_radiance[channel] = static_cast<float>(
			(1.0 - turb_frac) *
			arhosekskymodel_sr_internalRGB(
			skymodel_state[ channel ],
			turb_low,
			channel,
			skymodel_state[ channel ]->elevation
			)
			+ turb_frac *
			arhosekskymodel_sr_internalRGB(
			skymodel_state[ channel ],
			turb_low + 1,
			channel,
			skymodel_state[ channel ]->elevation
			) );
	}

	return DirectX::XMFLOAT4( direct_radiance[ 0 ], direct_radiance[ 1 ], direct_radiance[ 2 ], 1.0f );
}
/*
double arhosek_tristim_skymodel_radiance(
	ArHosekSkyModelState  * state,
	double                  theta,
	double                  gamma,
	int                     channel
	)
{
	return
		ArHosekSkyModel_GetRadianceInternal(
		state->configs[channel],
		theta,
		gamma
		)
		* state->radiances[channel];
}

double ArHosekSkyModel_GetRadianceInternal(
	ArHosekSkyModelConfiguration  configuration,
	double                        theta,
	double                        gamma
	)
{
	const double expM = exp( configuration[4] * gamma );
	const double rayM = cos( gamma )*cos( gamma );
	const double mieM = (1.0 + cos( gamma )*cos( gamma )) / pow( (1.0 + configuration[8] * configuration[8] - 2.0*configuration[8] * cos( gamma )), 1.5 );
	const double zenith = sqrt( cos( theta ) );

	return (1.0 + configuration[0] * exp( configuration[1] / (cos( theta ) + 0.01) )) *
		(configuration[2] + configuration[3] * expM + configuration[5] * rayM + configuration[6] * mieM + configuration[7] * zenith);
}
*/
