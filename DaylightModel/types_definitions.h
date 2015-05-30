#pragma once

/**@file TypesDefinitions.h
@brief Plik zawiera definicje podstawowych typ�w zmiennych.

Zmienne s� maj� sta�y rozmiar, powinny by� mniej wi�cej niezale�ne od platformy bo
opieraj� si� na bibliotece standardowej. Mimo wszystko moze si� zdarzy�, �e na jakich�
platformach typy nie istniej� i wtedy jest problem.

Definicje s� stworzone w celu �atwiejszego u�ytku. Biblioteka standardowa definiuje np.
typ int8_t, kt�ry jest zamieniany na int8.
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
