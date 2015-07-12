#include "HosekSkyModel.h"
#include "HosekSkyModelSolarRadiance_RGB.h"
#include "solarRadianceRGB.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

//   Some macro definitions that occur elsewhere in ART, and that have to be
//   replicated to make this a stand-alone module.

#ifndef NIL
#define NIL                         0
#endif

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

#ifndef ALLOC
#define ALLOC(_struct)              ((_struct *)malloc(sizeof(_struct)))
#endif



const int pieces = 45;
const int order = 4;

double arhosekskymodel_sr_internalRGB(
		ArHosekSkyModelState  * state,
		int                     turbidity,
		int                     channel,
		double                  elevation
		)
{
	int pos =
		(int) (pow(2.0*elevation / MATH_PI, 1.0/3.0) * pieces); // floor

	if ( pos > 44 ) pos = 44;

	const double break_x =
		pow(((double) pos / (double) pieces), 3.0) * (MATH_PI * 0.5);

	const double  * coefs =
		solarDatasetsRGB[channel] + (order * pieces * turbidity + order * (pos+1) - 1);

	double res = 0.0;
	const double x = elevation - break_x;
	double x_exp = 1.0;

	int i = 0;
	for (i = 0; i < order; ++i)
	{
		res += x_exp * *coefs--;
		x_exp *= x;
	}

	return res/* * state->emission_correction_factor_sun[channel]*/;		//potrzebny przy alien world
}

double solar_radiance_RGB(
		ArHosekSkyModelState  * state,
		int						channel,
		double                  elevation,
		double                  gamma
		)
{
	assert(
		   channel >= 0
		&& channel <= 2
		&& state->turbidity >= 1.0
		&& state->turbidity <= 10.0
		);

    //Jeżeli jesteśmy poza tarcza słoneczną, to zwracamy czarny kolor
    if( gamma > state->solar_radius )
        return 0.0;


	int     turb_low  = (int) state->turbidity - 1;
	double  turb_frac = state->turbidity - (double) (turb_low + 1);

	if ( turb_low == 9 )
	{
		turb_low  = 8;
		turb_frac = 1.0;
	}

	double direct_radiance =
		( 1.0 - turb_frac ) *
		arhosekskymodel_sr_internalRGB(
			state,
			turb_low,
			channel,
			elevation
			)
	  +   turb_frac *
		arhosekskymodel_sr_internalRGB(
			state,
			turb_low+1,
			channel,
			elevation
			);

	double ldCoefficient[6];

	int i = 0;
	for ( i = 0; i < 6; i++ )
		ldCoefficient[i] = limbDarkeningDatasetsRGB[channel][i];

	// sun distance to diameter ratio, squared

	const double sol_rad_sin = sin(state->solar_radius);
	const double ar2 = 1 / ( sol_rad_sin * sol_rad_sin );
	const double singamma = sin(gamma);
	double sc2 = 1.0 - ar2 * singamma * singamma;
	if (sc2 < 0.0 ) sc2 = 0.0;
	double sampleCosine = sqrt (sc2);

	//   The following will be improved in future versions of the model:
	//   here, we directly use fitted 5th order polynomials provided by the
	//   astronomical community for the limb darkening effect. Astronomers need
	//   such accurate fittings for their predictions. However, this sort of
	//   accuracy is not really needed for CG purposes, so an approximated
	//   dataset based on quadratic polynomials will be provided in a future
	//   release.

	/*double  darkeningFactor =
		  ldCoefficient[0]
		+ ldCoefficient[1] * sampleCosine
		+ ldCoefficient[2] * pow( sampleCosine, 2.0 )
		+ ldCoefficient[3] * pow( sampleCosine, 3.0 )
		+ ldCoefficient[4] * pow( sampleCosine, 4.0 )
		+ ldCoefficient[5] * pow( sampleCosine, 5.0 );
	*/

	//to samo co wyżej, tylko zapisane wydajniej
	double darkeningFactor = ldCoefficient[0];
	double polynomial_var = 1;

	for( int c = 1; c < 6; ++c )
	{
		polynomial_var *= sampleCosine;
		darkeningFactor += ldCoefficient[c]*polynomial_var;
	}

	direct_radiance *= darkeningFactor;

    return direct_radiance;
}


