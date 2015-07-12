#ifndef __SOLARRADIANCERGB
#define __SOLARRADIANCERGB

#include "HosekSkyModel.h"

double solar_radiance_RGB(
		ArHosekSkyModelState  * state,
		int						channel,
		double                  elevation,
		double                  gamma
		);

double arhosekskymodel_solar_radiance_internal2(
		ArHosekSkyModelState  * state,
		double                  wavelength,
		double                  elevation,
		double                  gamma
		);

double arhosekskymodel_sr_internalRGB(
	ArHosekSkyModelState  * state,
	int                     turbidity,
	int                     channel,
	double                  elevation
	);


#endif
