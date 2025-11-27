#include "basis_2d.h"

#include <cstdio>

#include "math_funcs.h"

namespace ho {
Basis2D Basis2D::FromAngle(real angle) {
  angle = math::Fmod(angle, math::PI * 2.0_r);
  if (angle < 0.0_r) {
    angle += math::PI * 2.0_r;
  }
  real sin, cos;
  math::SinCos(sin, cos, angle);
  return Basis2D(Vector2(cos, sin), Vector2(-sin, cos));
}

Basis2D& Basis2D::Rotate(real angle) {
  angle = math::Fmod(angle, math::PI * 2.0_r);
  if (angle < 0.0_r) {
    angle += math::PI * 2.0_r;
  }
  real sin, cos;
  math::SinCos(sin, cos, angle);
  Matrix2x2 rm = Matrix2x2({cos, sin}, {-sin, cos});
  matrix = rm * matrix;
  return *this;
}

Basis2D Basis2D::Rotated(real angle) const {
  Basis2D b = *this;
  return b.Rotate(angle);
}

Basis2D& Basis2D::RotateLocal(real angle) {
  angle = math::Fmod(angle, math::PI * 2.0_r);
  if (angle < 0.0_r) {
    angle += math::PI * 2.0_r;
  }
  real sin, cos;
  math::SinCos(sin, cos, angle);
  Matrix2x2 rm = Matrix2x2({cos, sin}, {-sin, cos});
  matrix = matrix * rm;
  return *this;
}

Basis2D Basis2D::RotatedLocal(real angle) const {
  Basis2D b = *this;
  return b.RotateLocal(angle);
}

Basis2D& Basis2D::LookAt(const Vector2& at) {
  Vector2 y = at;
  if (y.IsEqualApprox(Vector2::ZERO)) {
    return *this;
  }
  const real sx = basis_x().Magnitude();
  const real sy = basis_y().Magnitude();
  y.Normalize();
  Vector2 x = Vector2(y.y, -y.x);
  set_basis_x(sx * x);
  set_basis_y(sy * y);
  return *this;
}

Basis2D Basis2D::LookedAt(const Vector2& at) const {
  Basis2D b = *this;
  return b.LookAt(at);
}

std::string Basis2D::ToString() const {
  char buf[160];
  std::snprintf(buf, sizeof(buf), "Basis2D X: %s, Y: %s",
                basis_x().ToString().c_str(), basis_y().ToString().c_str());
  return std::string(buf);
}

}  // namespace ho