#pragma once

#include "math_defs.h"
#include "math_funcs.h"
#include "quaternion.h"
#include "vector2.h"
#include "vector3.h"
#include "vector4.h"

namespace ho {
namespace math {

template <class T>
_ALWAYS_INLINE_ Vector2 Barycentric(const T& target, const T& v1, const T& v2) {
  const T line_vec = v2 - v1;
  const T target_vec = target - v1;

  const real sq_mag = line_vec.SqrdMagnitude();
  if (math::IsZeroApprox(sq_mag)) {
    return Vector2(1.0_r, 0.0_r);
  }

  const real lambda = line_vec.Dot(target_vec) / sq_mag;

  return Vector2(1.0_r - lambda, lambda);
}

template <class T>
_ALWAYS_INLINE_ Vector3 Barycentric(const T& target, const T& v1, const T& v2,
                                    const T& v3) {
  // Standard triangle barycentric formula:
  // a = v1 - v3, b = v2 - v3, c = target - v3
  // d00=a·a, d01=a·b, d11=b·b, d20=c·a, d21=c·b
  // denom = d00*d11 - d01*d01
  // λ1 = (d11*d20 - d01*d21)/denom
  // λ2 = (d00*d21 - d01*d20)/denom
  // λ3 = 1 - λ1 - λ2

  const T a = v1 - v3;
  const T b = v2 - v3;
  const T c = target - v3;

  const real d00 = a.Dot(a);
  const real d11 = b.Dot(b);
  const real d01 = a.Dot(b);
  const real d20 = c.Dot(a);
  const real d21 = c.Dot(b);

  const real denom = d00 * d11 - d01 * d01;
  if (math::IsZeroApprox(denom)) {
    // degenerated case
    return Vector3::ZERO;
  }
  const real inv = 1.0_r / denom;
  const real lambda1 = (d11 * d20 - d01 * d21) * inv;
  const real lambda2 = (d00 * d21 - d01 * d20) * inv;
  return {lambda1, lambda2, 1.0_r - lambda1 - lambda2};
}

template <class T>
_ALWAYS_INLINE_ T Lerp(const T& v1, const T& v2, const Vector2& barycentric) {
  return barycentric.x * v1 + barycentric.y * v2;
}

template <class T>
_ALWAYS_INLINE_ T Lerp(const T& v1, const T& v2, const T& v3,
                       const Vector3& barycentric) {
  return barycentric.x * v1 + barycentric.y * v2 + barycentric.z * v3;
}

_ALWAYS_INLINE_ real Pcerp_W(const Vector2& inv_w, const Vector2& barycentric) {
  const real denom = (barycentric.x * inv_w.x + barycentric.y * inv_w.y);

  return 1.0_r / denom;
}
_ALWAYS_INLINE_ real Pcerp_W(const Vector3& inv_w, const Vector3& barycentric) {
  const real denom = (barycentric.x * inv_w.x + barycentric.y * inv_w.y +
                      barycentric.z * inv_w.z);
  return 1.0_r / denom;
}

template <class T>
_ALWAYS_INLINE_ T Pcerp(const T& a1, const T& a2, const Vector2& barycentric,
                        const Vector2& inv_w, real interpolated_w) {
  return interpolated_w *
         (inv_w.x * barycentric.x * a1 + inv_w.y * barycentric.y * a2);
}

template <class T>
_ALWAYS_INLINE_ T Pcerp(const T& a1, const T& a2, const T& a3,
                        const Vector3& barycentric, const Vector3& inv_w,
                        real interpolated_w) {
  return interpolated_w *
         (inv_w.x * barycentric.x * a1 + inv_w.y * barycentric.y * a2 +
          inv_w.z * barycentric.z * a3);
}

template <class T>
_ALWAYS_INLINE_ T Pcerp(const T& a1, const T& a2, const Vector2& barycentric,
                        const Vector4& v1, const Vector4& v2) {
  const Vector2 inv_w = Vector2(1.0_r / v1.w, 1.0_r / v2.w);
  const real interpolated_w = Pcerp_W(inv_w, barycentric);
  return interpolated_w *
         (inv_w.x * barycentric.x * a1 + inv_w.y * barycentric.y * a2);
}

template <class T>
_ALWAYS_INLINE_ T Pcerp(const T& a1, const T& a2, const T& a3,
                        const Vector3& barycentric, const Vector4& v1,
                        const Vector4& v2, const Vector4& v3) {
  const Vector3 inv_w = Vector3(1.0_r / v1.w, 1.0_r / v2.w, 1.0_r / v3.w);
  const real interpolated_w = Pcerp_W(inv_w, barycentric);
  return interpolated_w *
         (inv_w.x * barycentric.x * a1 + inv_w.y * barycentric.y * a2 +
          inv_w.z * barycentric.z * a3);
}

_ALWAYS_INLINE_ Quaternion Slerp(const Quaternion& q1, const Quaternion& q2,
                                 const Vector2& barycentric) {
  real cos = q1.Dot(q2);
  if (math::IsEqualApprox(cos, 1.0_r) || math::IsEqualApprox(cos, -1.0_r)) {
    return q1;
  }

  Quaternion adj_q2 = q2;
  real alpha = barycentric.x, beta = barycentric.y;

  if (cos < 0.f) {
    cos = -cos;
    adj_q2 = -q2;
  }
  const real angle = math::Acos(cos);
  const real inv_sin = 1.0_r / math::Sin(angle);
  alpha = math::Sin(barycentric.x * angle) * inv_sin;
  beta = math::Sin(barycentric.y * angle) * inv_sin;
  return alpha * q1 + beta * adj_q2;
}

_ALWAYS_INLINE_ Quaternion SlerpLong(const Quaternion& q1, const Quaternion& q2,
                                     const Vector2& barycentric) {
  real cos = q1.Dot(q2);
  if (math::IsEqualApprox(cos, 1.0_r) || math::IsEqualApprox(cos, -1.0_r)) {
    return q1;
  }

  Quaternion adj_q2 = q2;
  real alpha = barycentric.x, beta = barycentric.y;

  if (cos > 0.0_r) {
    cos = -cos;
    adj_q2 = -q2;
  }
  const real angle = math::Acos(cos);
  const real inv_sin = 1.f / math::Sin(angle);
  alpha = math::Sin(barycentric.x * angle) * inv_sin;
  beta = math::Sin(barycentric.y * angle) * inv_sin;
  return alpha * q1 + beta * adj_q2;
}

};  // namespace math
}  // namespace ho
