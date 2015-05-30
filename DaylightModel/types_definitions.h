#pragma once

/**@file TypesDefinitions.h
@brief Plik zawiera definicje podstawowych typów zmiennych.

Zmienne s¹ maj¹ sta³y rozmiar, powinny byæ mniej wiêcej niezale¿ne od platformy bo
opieraj¹ siê na bibliotece standardowej. Mimo wszystko moze siê zdarzyæ, ¿e na jakichœ
platformach typy nie istniej¹ i wtedy jest problem.

Definicje s¹ stworzone w celu ³atwiejszego u¿ytku. Biblioteka standardowa definiuje np.
typ int8_t, który jest zamieniany na int8.
*/

#include <cstdint>


typedef int8_t int8;
typedef uint8_t uint8;
typedef int16_t int16;
typedef uint16_t uint16;
typedef int32_t int32;
typedef uint32_t uint32;
typedef int64_t int64;
typedef uint64_t uint64;
