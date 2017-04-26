#pragma once

#include "Defines.hpp"


#if !DISABLE_SIMD

#include <pmmintrin.h>

/// SIMD intristic dot product of 3D vector
__m128 _mm_dot_ps(__m128 a, __m128 b);

// Compare two floats with precission
__m128 _mm_cmpf_ps(__m128 a, __m128 b);

#endif
