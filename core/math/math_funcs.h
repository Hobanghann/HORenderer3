#pragma once

#include <cmath>
#if defined(_MSC_VER) || defined(__SSE__)
#include <xmmintrin.h>
#endif

#include "math_defs.h"

namespace ho {
namespace math {
template <class T>
_ALWAYS_INLINE_ constexpr T Max(T x, T y) {
  return x < y ? y : x;
}

template <class T>
_ALWAYS_INLINE_ constexpr T Max(T x, T y, T z) {
  return Max(Max(x, y), z);
}

template <class T>
_ALWAYS_INLINE_ constexpr T Min(T x, T y) {
  return x < y ? x : y;
}

template <class T>
_ALWAYS_INLINE_ constexpr T Min(T x, T y, T z) {
  return Min(Min(x, y), z);
}

template <class T>
_ALWAYS_INLINE_ constexpr T Clamp(T target, T min, T max) {
  assert(min <= max);
  return target < min ? min : (target > max ? max : target);
}

_ALWAYS_INLINE_ constexpr real Fmod(real x, real y) {
  if (y == 0.0_r) return static_cast<real>(0);
  const real quotient = static_cast<real>(static_cast<long long>(x / y));
  return x - quotient * y;
}

_ALWAYS_INLINE_ real Round(real x) { return std::round(x); }

_ALWAYS_INLINE_ constexpr real Floor(real x) {
  const long long i = static_cast<long long>(x);
  return (x < 0 && static_cast<real>(i) != x) ? static_cast<real>(i - 1)
                                              : static_cast<real>(i);
}
_ALWAYS_INLINE_ constexpr real Ceil(real x) {
  const long long i = static_cast<long long>(x);
  return (x > 0 && static_cast<real>(i) != x) ? static_cast<real>(i + 1)
                                              : static_cast<real>(i);
}

_ALWAYS_INLINE_ constexpr real Abs(real x) { return (x < 0) ? -x : x; }

_ALWAYS_INLINE_ constexpr bool IsEqualApprox(real lhs, real rhs,
                                             real tolerance) {
  if (lhs == rhs) {
    return true;
  }
  return math::Abs(lhs - rhs) < tolerance;
}
_ALWAYS_INLINE_ constexpr bool IsEqualApprox(real lhs, real rhs) {
  if (lhs == rhs) {
    return true;
  }
  real tolerance = EPSILON_CMP * math::Abs(lhs);
  if (tolerance < EPSILON_CMP) {
    tolerance = EPSILON_CMP;
  }
  return math::Abs(lhs - rhs) < tolerance;
}

_ALWAYS_INLINE_ constexpr bool IsNotEqualApprox(real lhs, real rhs,
                                                real tolerance) {
  return !IsEqualApprox(lhs, rhs, tolerance);
}
_ALWAYS_INLINE_ constexpr bool IsNotEqualApprox(real lhs, real rhs) {
  return !IsEqualApprox(lhs, rhs);
}

_ALWAYS_INLINE_ constexpr bool IsZeroApprox(real p_value) {
  return math::Abs(p_value) < EPSILON_CMP;
}

_ALWAYS_INLINE_ bool IsFinite(real n) { return std::isfinite(n); }

_ALWAYS_INLINE_ bool IsInf(real n) { return std::isinf(n); }

_ALWAYS_INLINE_ bool IsNaN(real n) { return std::isnan(n); }

_ALWAYS_INLINE_ real Pow(real base, real exp) { return std::pow(base, exp); }

_INLINE_ constexpr real PowFast(real base, int exp) {
  if (exp == 0) return 1.0_r;

  if (base == 0.0_r) {
    return (exp < 0) ? math::REAL_INF : 0.0_r;
  }

  bool neg_exp = (exp < 0);
  unsigned int e = static_cast<unsigned int>(neg_exp ? -(exp + 1) : exp);
  if (neg_exp) ++e;

  real b = base;
  real result = 1.0_r;

  if (neg_exp) b = 1.0_r / b;

  while (e) {
    if (e & 1u) result *= b;
    b *= b;
    e >>= 1u;
  }

  return result;
}

_ALWAYS_INLINE_ float InvSqrtFast(float f) noexcept {
  // Performs two passes of Newton-Raphson iteration on the hardware estimate
  //    v^-0.5 = x
  // => x^2 = v^-1
  // => 1/(x^2) = v
  // => F(x) = x^-2 - v
  //    F'(x) = -2x^-3

  //    x1 = x0 - F(x0)/F'(x0)
  // => x1 = x0 + 0.5 * (x0^-2 - Vec) * x0^3
  // => x1 = x0 + 0.5 * (x0 - Vec * x0^3)
  // => x1 = x0 + x0 * (0.5 - 0.5 * Vec * x0^2)
  //
  // This final form has one more operation than the legacy factorization (X1 =
  // 0.5*X0*(3-(Y*X0)*X0) but retains better accuracy (namely InvSqrt(1) = 1
  // exactly).
  if (f < 0.0f) return math::REAL_NaN;
  if (f == 0.0f) return math::REAL_INF;

#if defined(_MSC_VER) || defined(__SSE__)
  const __m128 fOneHalf = _mm_set_ss(0.5f);
  __m128 y0 = _mm_set_ss(f);
  __m128 x0 = _mm_rsqrt_ss(y0);
  __m128 fOver2 = _mm_mul_ss(y0, fOneHalf);

  // 1st Newton-Raphson iteration
  __m128 x1 = _mm_mul_ss(x0, x0);
  x1 = _mm_sub_ss(fOneHalf, _mm_mul_ss(fOver2, x1));
  x1 = _mm_add_ss(x0, _mm_mul_ss(x0, x1));

  // 2nd Newton-Raphson iteration
  __m128 x2 = _mm_mul_ss(x1, x1);
  x2 = _mm_sub_ss(fOneHalf, _mm_mul_ss(fOver2, x2));
  x2 = _mm_add_ss(x1, _mm_mul_ss(x1, x2));
  float out;
  _mm_store_ss(&out, x2);
  return out;
#else

  return 1.0f / std::sqrt(f);
#endif
}

_ALWAYS_INLINE_ double InvSqrtFast(double f) noexcept {
  return 1.0 / std::sqrt(f);
}

_ALWAYS_INLINE_ float SqrtFast(float f) noexcept { return f * InvSqrtFast(f); }
_ALWAYS_INLINE_ float Sqrt(float f) noexcept { return std::sqrt(f); }
_ALWAYS_INLINE_ double Sqrt(double f) noexcept { return std::sqrt(f); }

_ALWAYS_INLINE_ real Sin(real radian) { return std::sin(radian); }
_ALWAYS_INLINE_ real Cos(real radian) { return std::cos(radian); }
_ALWAYS_INLINE_ real Tan(real radian) { return std::tan(radian); }

_ALWAYS_INLINE_ real Asin(real radian) { return std::asin(radian); }
_ALWAYS_INLINE_ real Acos(real radian) { return std::acos(radian); }
_ALWAYS_INLINE_ real Atan(real radian) { return std::atan(radian); }
_ALWAYS_INLINE_ real Atan2(real y, real x) { return std::atan2(y, x); }

_ALWAYS_INLINE_ void SinCos(float& out_sin, float& out_cos, float radian) {
  // Copied from UE4 Source Code
  // Map Value to y in [-pi,pi], x = 2*pi*quotient + remainder.
  float quotient = (INV_PI * 0.5f) * radian;
  if (radian >= 0.0f) {
    quotient = (float)((int)(quotient + 0.5f));
  } else {
    quotient = (float)((int)(quotient - 0.5f));
  }
  float y = radian - (2.0f * PI) * quotient;

  // Map y to [-pi/2,pi/2] with sin(y) = sin(Value).
  float sign = 0.f;
  if (y > PI2) {
    y = PI - y;
    sign = -1.0f;
  } else if (y < -PI2) {
    y = -PI - y;
    sign = -1.0f;
  } else {
    sign = +1.0f;
  }

  float y2 = y * y;

  // 11-degree minimax approximation
  out_sin =
      (((((-2.3889859e-08f * y2 + 2.7525562e-06f) * y2 - 0.00019840874f) * y2 +
         0.0083333310f) *
            y2 -
        0.16666667f) *
           y2 +
       1.0f) *
      y;

  // 10-degree minimax approximation
  float p =
      ((((-2.6051615e-07f * y2 + 2.4760495e-05f) * y2 - 0.0013888378f) * y2 +
        0.041666638f) *
           y2 -
       0.5f) *
          y2 +
      1.0f;
  out_cos = sign * p;
}

_ALWAYS_INLINE_ constexpr real DegToRad(real deg) { return deg * DEG1_IN_RAD; }

_ALWAYS_INLINE_ constexpr real RadToDeg(real rad) { return rad * RAD1_IN_DEG; }

template <class T>
_INLINE_ real Distance(const T& vec1, const T& vec2) {
  const T d = (vec1 - vec2);
  return Sqrt(d.Dot(d));
}
template <class T>
_INLINE_ constexpr real SqrdDistance(const T& vec1, const T& vec2) {
  const T d = (vec1 - vec2);
  return d.Dot(d);
}

}  // namespace math
}  // namespace ho