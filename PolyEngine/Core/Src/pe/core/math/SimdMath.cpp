#include <pe/core/CorePCH.hpp>

#include <pe/core/math/SimdMath.hpp>
#include <pe/core/math/BasicMath.hpp>

using namespace pe::core::math;

#if !DISABLE_SIMD

//------------------------------------------------------------------------------
__m128 _mm_dot_ps(__m128 a, __m128 b) {
  __m128 mult = _mm_mul_ps(a, b); //multiply x,y,z
  __m128 tmp_y = _mm_shuffle_ps(mult, mult, _MM_SHUFFLE(0, 0, 0, 1)); // get y to first position
  __m128 tmp_x = _mm_shuffle_ps(mult, mult, _MM_SHUFFLE(0, 0, 0, 2)); // get z to first position
  __m128 sum = _mm_add_ps(mult, _mm_add_ps(tmp_x, tmp_y)); // sum x, y and z
  return _mm_shuffle_ps(sum, sum, _MM_SHUFFLE(0, 0, 0, 0));
}

//------------------------------------------------------------------------------
__m128 _mm_cmpf_ps(__m128 a, __m128 b) {
  static const __m128 simd_eps = {CMPF_EPS, CMPF_EPS, CMPF_EPS, CMPF_EPS};
  static const __m128 simd_eps_neg = {-CMPF_EPS, -CMPF_EPS, -CMPF_EPS, -CMPF_EPS};
  __m128 sub = _mm_sub_ps(a, b);
  __m128 lower = _mm_cmplt_ps(sub, simd_eps);
  __m128 greater = _mm_cmpgt_ps(sub, simd_eps_neg);
  return _mm_and_ps(lower, greater);
}

#endif
