#pragma once

/**
@file HosekSkyModel.h
@brief Plik zawiera deklaracjê klasy obudowuj¹cej model nieba stworzony przez
Lukasa Hoseka i Alexandra Wilkiego.


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

typedef double ArHosekSkyModelConfiguration[9];


/** @brief
This struct holds the pre-computation data for one particular albedo value.
Most fields are self-explanatory, but users should never directly
manipulate any of them anyway. The only consistent way to manipulate such
structs is via the functions 'arhosekskymodelstate_alloc_init' and
'arhosekskymodelstate_free'.

'emission_correction_factor_sky'
'emission_correction_factor_sun'

The original model coefficients were fitted against the emission of
our local sun. If a different solar emission is desired (i.e. if the
model is being used to predict skydome appearance for an earth-like
planet that orbits a different star), these correction factors, which
are determined during the alloc_init step, are applied to each waveband
separately (they default to 1.0 in normal usage). This is the simplest
way to retrofit this sort of capability to the existing model. The
different factors for sky and sun are needed since the solar disc may
be of a different size compared to the terrestrial sun.

Tê strukturê da siê sporo zoptymalizowaæ ze wzglêdu na to, ¿e u¿ywamy tylko trybu RGB.
To jest praca na kiedy indziej.

---------------------------------------------------------------------------- */
typedef struct ArHosekSkyModelState
{
	ArHosekSkyModelConfiguration  configs[11];
	double                        radiances[11];
	double                        turbidity;
	double                        solar_radius;
	double                        emission_correction_factor_sky[11];
	double                        emission_correction_factor_sun[11];
	double                        albedo;
	double                        elevation;
}
ArHosekSkyModelState;

typedef double *ArHosekSkyModel_Dataset;
typedef double *ArHosekSkyModel_Radiance_Dataset;

/**@brief Klasa zawieraj¹ca funkcje do obs³ugi modelu nieba Hoseka i Wilkiego.

Klasa jest przepisana na architekturê SSE, aby wykonywaæ instrukcje w jednostce wektorowej.
Wykorzystuje jedynie model w wersji RGB, nie ma mo¿liwoœci generowania nieba w wersji XYZ albo
spektralnej.

Z racji wykorzystania jednostki wektorowej, obliczenia obarczone s¹ wiêkszym b³êdem,
poniewa¿ s¹ robione na floatach 32 bitowych, a nie doublach, jak by³o w oryginalnym
kodzie modelu.*/
class HosekSkylightModel
{
private:
	ArHosekSkyModelState* skymodel_state[3];	///<Tablica parametrów dla aktualnego stanu nieba
	float sky_intensity;
	float sun_intensity;


	//Funkcje pomocnicze
	double ArHosekSkyModel_CookRadianceConfiguration( ArHosekSkyModel_Radiance_Dataset dataset,
													  double turbidity,
													  double albedo,
													  double solar_elevation );

	void ArHosekSkyModel_CookConfiguration( ArHosekSkyModel_Dataset dataset,
											ArHosekSkyModelConfiguration config,
											double turbidity,
											double albedo,
											double solar_elevation );
public:
	HosekSkylightModel();
	~HosekSkylightModel();

	void init( const double turbidity,
			   const double* albedo,
			   const double elevation,
			   float sky_intens,
			   float sun_intens);

	DirectX::XMVECTOR sky_radiance( float theta, float gamma );
	DirectX::XMFLOAT3 sky_solar_radiance( float theta, float gamma );
};

