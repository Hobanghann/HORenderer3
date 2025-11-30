#pragma once

#include <string>

#include "matrix3x3.h"
#include "quaternion.h"

namespace ho {
struct Basis3D {
  static Basis3D FromAxisAngle(const Vector3& axis, real angle);
  static Basis3D FromEuler(real x, real y, real z,
                           math::EulerOrder order = math::YXZ);

  constexpr Basis3D();
  constexpr Basis3D(const Basis3D& transform) = default;
  constexpr Basis3D(const Vector3& basis_x, const Vector3& basis_y,
                    const Vector3& basis_z);
  constexpr Basis3D(real scale_x, real scale_y, real scale_z,
                    const Vector3& basis_x, const Vector3& basis_y,
                    const Vector3& basis_z);
  constexpr explicit Basis3D(const Matrix3x3& m);
  constexpr explicit Basis3D(const Quaternion& q);
  constexpr Basis3D& operator=(const Basis3D& rhs);
  ~Basis3D() = default;

  _ALWAYS_INLINE_ real scale_x() const;
  _ALWAYS_INLINE_ real scale_y() const;
  _ALWAYS_INLINE_ real scale_z() const;
  constexpr Vector3 basis_x() const;
  constexpr Vector3 basis_y() const;
  constexpr Vector3 basis_z() const;

  constexpr void set_basis_x(const Vector3& basis_x);
  constexpr void set_basis_y(const Vector3& basis_y);
  constexpr void set_basis_z(const Vector3& basis_z);

  constexpr bool operator==(const Basis3D& rhs) const;
  constexpr bool operator!=(const Basis3D& rhs) const;

  constexpr bool IsEqualApprox(const Basis3D& rhs) const;
  constexpr bool IsNotEqualApprox(const Basis3D& rhs) const;

  constexpr bool IsOrthogonal() const;
  _ALWAYS_INLINE_ Basis3D& Orthogonalize();
  _ALWAYS_INLINE_ Basis3D Orthogonalized() const;

  constexpr bool IsOrthonormal() const;
  _ALWAYS_INLINE_ Basis3D& Orthonormalize();
  _ALWAYS_INLINE_ Basis3D Orthonormalized() const;

  constexpr Basis3D& Scale(const Vector3& scale);
  constexpr Basis3D Scaled(const Vector3& scale) const;

  constexpr Basis3D& ScaleLocal(const Vector3& scale);
  constexpr Basis3D ScaledLocal(const Vector3& scale) const;

  constexpr Basis3D& ScaleUniform(real scale);
  constexpr Basis3D ScaledUniform(real scale) const;

  constexpr Basis3D& ScaleUniformLocal(real scale);
  constexpr Basis3D ScaledUniformLocal(real scale) const;

  Basis3D& RotateEuler(real x_angle, real y_angle, real z_angle,
                       math::EulerOrder order = math::YXZ);
  Basis3D& RotateAxisAngle(const Vector3& axis, real angle);
  constexpr Basis3D& RotateQuaternion(const Quaternion& q);
  Basis3D RotatedEuler(real x_angle, real y_angle, real z_angle,
                       math::EulerOrder order = math::YXZ) const;
  Basis3D RotatedAxisAngle(const Vector3& axis, real angle) const;
  constexpr Basis3D RotatedQuaternion(const Quaternion& q) const;

  Basis3D& RotateEulerLocal(real x_angle, real y_angle, real z_angle,
                            math::EulerOrder order = math::YXZ);
  Basis3D& RotateAxisAngleLocal(const Vector3& axis, real angle);
  constexpr Basis3D& RotateQuaternionLocal(const Quaternion& q);
  Basis3D RotatedEulerLocal(real x_angle, real y_angle, real z_angle,
                            math::EulerOrder order = math::YXZ) const;
  Basis3D RotatedAxisAngleLocal(const Vector3& axis, real angle) const;
  constexpr Basis3D RotatedQuaternionLocal(const Quaternion& q) const;

  constexpr Basis3D& Invert();
  constexpr Basis3D Inverse() const;

  constexpr Basis3D& InvertFast();
  constexpr Basis3D InverseFast() const;

  Basis3D& LookAt(const Vector3& at, const Vector3& up, bool is_facing = false);
  Basis3D LookedAt(const Vector3& at, const Vector3& up,
                   bool is_facing = false) const;

  constexpr Basis3D& operator*=(const Basis3D& rhs);
  constexpr Basis3D operator*(const Basis3D& rhs) const;

  _ALWAYS_INLINE_ constexpr Vector3 Transform(const Vector3& v) const;
  _ALWAYS_INLINE_ constexpr Vector3 InvTransform(const Vector3& v) const;
  _ALWAYS_INLINE_ constexpr Vector3 InvTransformFast(const Vector3& v) const;

  std::string ToString() const;

  union {
    struct {
      Vector3 row0;
      Vector3 row1;
      Vector3 row2;
    };
    Matrix3x3 matrix;
  };
};

constexpr Basis3D::Basis3D()
    : row0(Vector3::UNIT_X), row1(Vector3::UNIT_Y), row2(Vector3::UNIT_Z) {}
constexpr Basis3D::Basis3D(const Vector3& basis_x, const Vector3& basis_y,
                           const Vector3& basis_z)
    : row0(basis_x.x, basis_y.x, basis_z.x),
      row1(basis_x.y, basis_y.y, basis_z.y),
      row2(basis_x.z, basis_y.z, basis_z.z) {}
constexpr Basis3D::Basis3D(real scale_x, real scale_y, real scale_z,
                           const Vector3& basis_x, const Vector3& basis_y,
                           const Vector3& basis_z)
    : row0(scale_x * basis_x.x, scale_y * basis_y.x, scale_z * basis_z.x),
      row1(scale_x * basis_x.y, scale_y * basis_y.y, scale_z * basis_z.y),
      row2(scale_x * basis_x.z, scale_y * basis_y.z, scale_z * basis_z.z) {}
constexpr Basis3D::Basis3D(const Matrix3x3& m) : matrix(m) {}
constexpr Basis3D::Basis3D(const Quaternion& q)
    : matrix(Matrix3x3::FromQuaternion(q)) {}

constexpr Basis3D& Basis3D::operator=(const Basis3D& rhs) {
  if (this == &rhs) {
    return *this;
  }
  matrix = rhs.matrix;
  return *this;
}

real Basis3D::scale_x() const { return basis_x().Magnitude(); }
real Basis3D::scale_y() const { return basis_y().Magnitude(); }
real Basis3D::scale_z() const { return basis_z().Magnitude(); }
constexpr Vector3 Basis3D::basis_x() const {
  return Vector3(row0.x, row1.x, row2.x);
}
constexpr Vector3 Basis3D::basis_y() const {
  return Vector3(row0.y, row1.y, row2.y);
}
constexpr Vector3 Basis3D::basis_z() const {
  return Vector3(row0.z, row1.z, row2.z);
}

constexpr void Basis3D::set_basis_x(const Vector3& basis_x) {
  row0.x = basis_x.x;
  row1.x = basis_x.y;
  row2.x = basis_x.z;
}
constexpr void Basis3D::set_basis_y(const Vector3& basis_y) {
  row0.y = basis_y.x;
  row1.y = basis_y.y;
  row2.y = basis_y.z;
}
constexpr void Basis3D::set_basis_z(const Vector3& basis_z) {
  row0.z = basis_z.x;
  row1.z = basis_z.y;
  row2.z = basis_z.z;
}

constexpr bool Basis3D::operator==(const Basis3D& rhs) const {
  return matrix == rhs.matrix;
}
constexpr bool Basis3D::operator!=(const Basis3D& rhs) const {
  return !(*this == rhs);
}

constexpr bool Basis3D::IsEqualApprox(const Basis3D& rhs) const {
  return matrix.IsEqualApprox(rhs.matrix);
}
constexpr bool Basis3D::IsNotEqualApprox(const Basis3D& rhs) const {
  return !IsEqualApprox(rhs);
}

constexpr bool Basis3D::IsOrthogonal() const { return matrix.IsOrthogonal(); }
Basis3D& Basis3D::Orthogonalize() {
  matrix.Orthogonalize();
  return *this;
}
Basis3D Basis3D::Orthogonalized() const {
  Basis3D copy = *this;
  return copy.Orthogonalize();
}

constexpr bool Basis3D::IsOrthonormal() const { return matrix.IsOrthonormal(); }
Basis3D& Basis3D::Orthonormalize() {
  matrix.Orthonormalize();
  return *this;
}
Basis3D Basis3D::Orthonormalized() const {
  Basis3D copy = *this;
  return copy.Orthonormalize();
}

constexpr Basis3D& Basis3D::Scale(const Vector3& scale) {
  row0 *= scale.x;
  row1 *= scale.y;
  row2 *= scale.z;
  return *this;
}
constexpr Basis3D Basis3D::Scaled(const Vector3& scale) const {
  Basis3D copy = *this;
  return copy.Scale(scale);
}

constexpr Basis3D& Basis3D::ScaleLocal(const Vector3& scale) {
  row0 *= scale;
  row1 *= scale;
  row2 *= scale;
  return *this;
}
constexpr Basis3D Basis3D::ScaledLocal(const Vector3& scale) const {
  Basis3D copy = *this;
  return copy.ScaleLocal(scale);
}

constexpr Basis3D& Basis3D::ScaleUniform(real scale) {
  row0 *= scale;
  row1 *= scale;
  row2 *= scale;
  return *this;
}
constexpr Basis3D Basis3D::ScaledUniform(real scale) const {
  Basis3D copy = *this;
  return copy.ScaleUniform(scale);
}

constexpr Basis3D& Basis3D::ScaleUniformLocal(real scale) {
  Vector3 s(scale, scale, scale);
  row0 *= scale;
  row1 *= scale;
  row2 *= scale;
  return *this;
}
constexpr Basis3D Basis3D::ScaledUniformLocal(real scale) const {
  Basis3D copy = *this;
  return copy.ScaleUniformLocal(scale);
}

constexpr Basis3D& Basis3D::RotateQuaternion(const Quaternion& q) {
  assert(q.IsUnitApprox());

  const Vector3 bx = basis_x();
  const Vector3 by = basis_y();
  const Vector3 bz = basis_z();

  set_basis_x(q.Transform(bx));
  set_basis_y(q.Transform(by));
  set_basis_z(q.Transform(bz));
  return *this;
}
constexpr Basis3D Basis3D::RotatedQuaternion(const Quaternion& q) const {
  Basis3D b = *this;
  return b.RotateQuaternion(q);
}

constexpr Basis3D& Basis3D::RotateQuaternionLocal(const Quaternion& q) {
  assert(q.IsUnitApprox());

  Matrix3x3 rm = Matrix3x3::FromQuaternion(q);
  matrix = matrix * rm;
  return *this;
}
constexpr Basis3D Basis3D::RotatedQuaternionLocal(const Quaternion& q) const {
  Basis3D b = *this;
  return b.RotateQuaternionLocal(q);
}

constexpr Basis3D& Basis3D::Invert() {
  matrix.Invert();
  return *this;
}
constexpr Basis3D Basis3D::Inverse() const {
  Basis3D b = *this;
  return b.Invert();
}

constexpr Basis3D& Basis3D::InvertFast() {
  matrix.Transpose();
  return *this;
}
constexpr Basis3D Basis3D::InverseFast() const {
  Basis3D b = *this;
  return b.InvertFast();
}

constexpr Basis3D& Basis3D::operator*=(const Basis3D& rhs) {
  matrix *= rhs.matrix;
  return *this;
}

constexpr Basis3D Basis3D::operator*(const Basis3D& rhs) const {
  Basis3D b = *this;
  return b *= rhs;
}

constexpr Vector3 Basis3D::Transform(const Vector3& v) const {
  return matrix * v;
}
constexpr Vector3 Basis3D::InvTransform(const Vector3& v) const {
  return Inverse().Transform(v);
}
constexpr Vector3 Basis3D::InvTransformFast(const Vector3& v) const {
  return InverseFast().Transform(v);
}
}  // namespace ho
