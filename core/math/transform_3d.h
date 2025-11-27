#pragma once

#include "basis_3d.h"
#include "matrix4x4.h"
#include "vector3.h"

namespace ho {
struct Transform3D {
  constexpr Transform3D();
  constexpr Transform3D(const Basis3D& p_basis, const Vector3& p_origin);
  constexpr Transform3D(const Matrix4x4& m);
  constexpr Transform3D(const Transform3D&) = default;
  constexpr Transform3D& operator=(const Transform3D& rhs);
  ~Transform3D() = default;

  _ALWAYS_INLINE_ real scale_x() const;
  _ALWAYS_INLINE_ real scale_y() const;
  _ALWAYS_INLINE_ real scale_z() const;
  constexpr Vector3 basis_x() const;
  constexpr Vector3 basis_y() const;
  constexpr Vector3 basis_z() const;
  constexpr void set_basis_x(const Vector3& basis_x);
  constexpr void set_basis_y(const Vector3& basis_y);
  constexpr void set_basis_z(const Vector3& basis_z);

  constexpr bool operator==(const Transform3D& rhs) const;
  constexpr bool operator!=(const Transform3D& rhs) const;

  constexpr bool IsEqualApprox(const Transform3D& rhs) const;
  constexpr bool IsNotEqualApprox(const Transform3D& rhs) const;

  constexpr bool IsOrthogonal() const;
  _ALWAYS_INLINE_ Transform3D& Orthogonalize();
  _ALWAYS_INLINE_ Transform3D Orthogonalized() const;

  constexpr bool IsOrthonormal() const;
  _ALWAYS_INLINE_ Transform3D& Orthonormalize();
  _ALWAYS_INLINE_ Transform3D Orthonormalized() const;

  constexpr Transform3D& Scale(const Vector3& scale);
  constexpr Transform3D& ScaleUniform(real scale);
  _ALWAYS_INLINE_ Transform3D& RotateAxisAngle(const Vector3& axis, real angle);
  _ALWAYS_INLINE_ Transform3D& RotateEuler(real x_angle, real y_angle,
                                           real z_angle,
                                           math::EulerOrder order = math::ZYX);
  constexpr Transform3D& RotateQuaternion(const Quaternion& q);
  constexpr Transform3D& Translate(const Vector3& translation_v);

  constexpr Transform3D Scaled(const Vector3& scale) const;
  constexpr Transform3D ScaledUniform(real scale) const;
  _ALWAYS_INLINE_ Transform3D RotatedAxisAngle(const Vector3& axis,
                                               real angle) const;
  _ALWAYS_INLINE_ Transform3D
  RotatedEuler(real x_angle, real y_angle, real z_angle,
               math::EulerOrder order = math::ZYX) const;
  constexpr Transform3D RotatedQuaternion(const Quaternion& q) const;
  constexpr Transform3D Translated(const Vector3& translation_v) const;

  constexpr Transform3D& ScaleLocal(const Vector3& scale);
  constexpr Transform3D& ScaleUniformLocal(real scale);
  _ALWAYS_INLINE_ Transform3D& RotateAxisAngleLocal(const Vector3& axis,
                                                    real angle);
  _ALWAYS_INLINE_ Transform3D& RotateEulerLocal(
      real x_angle, real y_angle, real z_angle,
      math::EulerOrder order = math::ZYX);
  constexpr Transform3D& RotateQuaternionLocal(const Quaternion& q);
  constexpr Transform3D& TranslateLocal(const Vector3& translation_v);

  constexpr Transform3D ScaledLocal(const Vector3& scale) const;
  constexpr Transform3D ScaledUniformLocal(real scale) const;
  _ALWAYS_INLINE_ Transform3D RotatedAxisAngleLocal(const Vector3& axis,
                                                    real angle) const;
  _ALWAYS_INLINE_ Transform3D
  RotatedEulerLocal(real x_angle, real y_angle, real z_angle,
                    math::EulerOrder order = math::ZYX) const;
  constexpr Transform3D RotatedQuaternionLocal(const Quaternion& q) const;
  constexpr Transform3D TranslatedLocal(const Vector3& translation_v) const;

  constexpr Transform3D& Invert();
  constexpr Transform3D Inverse() const;

  constexpr Transform3D& InvertFast();
  constexpr Transform3D InverseFast() const;

  _ALWAYS_INLINE_ Transform3D& LookAt(const Vector3& at, const Vector3& up,
                                      bool is_facing_target = false);
  _ALWAYS_INLINE_ Transform3D LookedAt(const Vector3& at, const Vector3& up,
                                       bool is_facing_target = false) const;

  constexpr Transform3D& operator*=(const Transform3D& rhs);
  constexpr Transform3D operator*(const Transform3D& rhs) const;

  constexpr Vector3 Transform(const Vector3& v) const;
  constexpr Vector3 InvTransform(const Vector3& v) const;
  constexpr Vector3 InvTransformFast(const Vector3& v) const;

  constexpr Matrix4x4 ToMatrix() const;

  std::string ToString() const;

  Basis3D basis;
  Vector3 origin;
};

constexpr Transform3D::Transform3D()
    : basis(Basis3D()), origin(Vector3::ZERO) {}

constexpr Transform3D::Transform3D(const Basis3D& p_basis,
                                   const Vector3& p_origin)
    : basis(p_basis), origin(p_origin) {}
constexpr Transform3D::Transform3D(const Matrix4x4& m)
    : basis(Basis3D(Vector3(m.data[0][0], m.data[1][0], m.data[2][0]),
                    Vector3(m.data[0][1], m.data[1][1], m.data[2][1]),
                    Vector3(m.data[0][2], m.data[1][2], m.data[2][2]))),
      origin(Vector3(m.data[0][3], m.data[1][3], m.data[2][3])) {}

constexpr Transform3D& Transform3D::operator=(const Transform3D& rhs) {
  if (this == &rhs) {
    return *this;
  }
  basis = rhs.basis;
  origin = rhs.origin;
  return *this;
}

real Transform3D::scale_x() const { return basis.scale_x(); }
real Transform3D::scale_y() const { return basis.scale_y(); }
real Transform3D::scale_z() const { return basis.scale_z(); }

constexpr Vector3 Transform3D::basis_x() const { return basis.basis_x(); }
constexpr Vector3 Transform3D::basis_y() const { return basis.basis_y(); }
constexpr Vector3 Transform3D::basis_z() const { return basis.basis_z(); }

constexpr void Transform3D::set_basis_x(const Vector3& basis_x) {
  basis.set_basis_x(basis_x);
}
constexpr void Transform3D::set_basis_y(const Vector3& basis_y) {
  basis.set_basis_y(basis_y);
}
constexpr void Transform3D::set_basis_z(const Vector3& basis_z) {
  basis.set_basis_z(basis_z);
}

constexpr bool Transform3D::operator==(const Transform3D& rhs) const {
  return basis == rhs.basis && origin == rhs.origin;
}
constexpr bool Transform3D::operator!=(const Transform3D& rhs) const {
  return !(*this == rhs);
}

constexpr bool Transform3D::IsEqualApprox(const Transform3D& rhs) const {
  return basis.IsEqualApprox(rhs.basis) && origin.IsEqualApprox(rhs.origin);
}
constexpr bool Transform3D::IsNotEqualApprox(const Transform3D& rhs) const {
  return !IsEqualApprox(rhs);
}

constexpr bool Transform3D::IsOrthogonal() const {
  return basis.IsOrthogonal();
}
Transform3D& Transform3D::Orthogonalize() {
  basis.Orthogonalize();
  return *this;
}
Transform3D Transform3D::Orthogonalized() const {
  Transform3D copy = *this;
  return copy.Orthogonalize();
}
constexpr bool Transform3D::IsOrthonormal() const {
  return basis.IsOrthonormal();
}
Transform3D& Transform3D::Orthonormalize() {
  basis.Orthonormalize();
  return *this;
}
Transform3D Transform3D::Orthonormalized() const {
  Transform3D copy = *this;
  return copy.Orthonormalize();
}

constexpr Transform3D& Transform3D::Scale(const Vector3& scale) {
  basis.Scale(scale);
  origin *= scale;
  return *this;
}
constexpr Transform3D& Transform3D::ScaleUniform(real scale) {
  basis.ScaleUniform(scale);
  origin *= scale;
  return *this;
}
_ALWAYS_INLINE_ Transform3D& Transform3D::RotateAxisAngle(const Vector3& axis,
                                                          real angle) {
  Quaternion q = Quaternion::FromAxisAngle(axis, angle);
  basis.RotateQuaternion(q);
  origin = q.Transform(origin);
  return *this;
}
_ALWAYS_INLINE_ Transform3D& Transform3D::RotateEuler(
    real x_angle, real y_angle, real z_angle,
    math::EulerOrder order) {
  Quaternion q = Quaternion::FromEuler(x_angle, y_angle, z_angle, order);
  basis.RotateQuaternion(q);
  origin = q.Transform(origin);
  return *this;
}
constexpr Transform3D& Transform3D::RotateQuaternion(const Quaternion& q) {
  basis.RotateQuaternion(q);
  origin = q.Transform(origin);
  return *this;
}
constexpr Transform3D& Transform3D::Translate(const Vector3& translation_v) {
  origin += translation_v;
  return *this;
}

constexpr Transform3D Transform3D::Scaled(const Vector3& scale) const {
  Transform3D copy = *this;
  return copy.Scale(scale);
}
constexpr Transform3D Transform3D::ScaledUniform(real scale) const {
  Transform3D copy = *this;
  return copy.ScaleUniform(scale);
}
_ALWAYS_INLINE_ Transform3D Transform3D::RotatedAxisAngle(const Vector3& axis,
                                                          real angle) const {
  Transform3D copy = *this;
  return copy.RotateAxisAngle(axis, angle);
}
_ALWAYS_INLINE_ Transform3D
Transform3D::RotatedEuler(real x_angle, real y_angle, real z_angle,
                          math::EulerOrder order) const {
  Transform3D copy = *this;
  return copy.RotateEuler(x_angle, y_angle, z_angle, order);
}
constexpr Transform3D Transform3D::RotatedQuaternion(
    const Quaternion& q) const {
  Transform3D copy = *this;
  return copy.RotateQuaternion(q);
}
constexpr Transform3D Transform3D::Translated(
    const Vector3& translation_v) const {
  Transform3D copy = *this;
  return copy.Translate(translation_v);
}

constexpr Transform3D& Transform3D::ScaleLocal(const Vector3& scale) {
  basis.ScaleLocal(scale);
  return *this;
}
constexpr Transform3D& Transform3D::ScaleUniformLocal(real scale) {
  basis.ScaleUniformLocal(scale);
  return *this;
}
_ALWAYS_INLINE_ Transform3D& Transform3D::RotateAxisAngleLocal(
    const Vector3& axis, real angle) {
  basis.RotateAxisAngleLocal(axis, angle);
  return *this;
}
_ALWAYS_INLINE_ Transform3D& Transform3D::RotateEulerLocal(
    real x_angle, real y_angle, real z_angle,
    math::EulerOrder order) {
  basis.RotateEulerLocal(x_angle, y_angle, z_angle, order);
  return *this;
}
constexpr Transform3D& Transform3D::RotateQuaternionLocal(const Quaternion& q) {
  basis.RotateQuaternionLocal(q);
  return *this;
}
constexpr Transform3D& Transform3D::TranslateLocal(
    const Vector3& translation_v) {
  origin += basis_x() * translation_v.x + basis_y() * translation_v.y +
            basis_z() * translation_v.z;
  return *this;
}

constexpr Transform3D Transform3D::ScaledLocal(const Vector3& scale) const {
  Transform3D copy = *this;
  return copy.ScaleLocal(scale);
}
constexpr Transform3D Transform3D::ScaledUniformLocal(real scale) const {
  Transform3D copy = *this;
  return copy.ScaleUniformLocal(scale);
}
_ALWAYS_INLINE_ Transform3D
Transform3D::RotatedAxisAngleLocal(const Vector3& axis, real angle) const {
  Transform3D copy = *this;
  return copy.RotateAxisAngleLocal(axis, angle);
}
_ALWAYS_INLINE_ Transform3D
Transform3D::RotatedEulerLocal(real x_angle, real y_angle, real z_angle,
                               math::EulerOrder order) const {
  Transform3D copy = *this;
  return copy.RotateEulerLocal(x_angle, y_angle, z_angle, order);
}
constexpr Transform3D Transform3D::RotatedQuaternionLocal(
    const Quaternion& q) const {
  Transform3D copy = *this;
  return copy.RotateQuaternionLocal(q);
}
constexpr Transform3D Transform3D::TranslatedLocal(
    const Vector3& translation_v) const {
  Transform3D copy = *this;
  return copy.TranslateLocal(translation_v);
}

constexpr Transform3D& Transform3D::Invert() {
  Matrix4x4 m = ToMatrix();
  Matrix4x4 inv_m = m.Inverse();
  *this = Transform3D(inv_m);
  return *this;
}
constexpr Transform3D Transform3D::Inverse() const {
  Transform3D copy = *this;
  return copy.Invert();
}

constexpr Transform3D& Transform3D::InvertFast() {
  basis.InvertFast();
  origin = basis.Transform(-origin);
  return *this;
}
constexpr Transform3D Transform3D::InverseFast() const {
  Transform3D copy = *this;
  return copy.InvertFast();
}

_ALWAYS_INLINE_ Transform3D& Transform3D::LookAt(
    const Vector3& at, const Vector3& up, bool is_facing_target) {
  {
    Vector3 adj_at = at - origin;
    if (adj_at.IsEqualApprox(Vector3::ZERO)) return *this;
    basis.LookAt(adj_at, up, is_facing_target);
    return *this;
  }
}
_ALWAYS_INLINE_ Transform3D Transform3D::LookedAt(
    const Vector3& at, const Vector3& up, bool is_facing_target) const {
  Transform3D copy = *this;
  return copy.LookAt(at, up, is_facing_target);
}

constexpr Transform3D& Transform3D::operator*=(const Transform3D& rhs) {
  origin += basis.Transform(rhs.origin);
  basis *= rhs.basis;

  return *this;
}
constexpr Transform3D Transform3D::operator*(const Transform3D& rhs) const {
  Transform3D copy = *this;
  return copy *= rhs;
}

constexpr Vector3 Transform3D::Transform(const Vector3& v) const {
  return basis.Transform(v) + origin;
}
constexpr Vector3 Transform3D::InvTransform(const Vector3& v) const {
  const Vector3 w = v - origin;
  return basis.InvTransform(w);
}
constexpr Vector3 Transform3D::InvTransformFast(const Vector3& v) const {
  const Vector3 w = v - origin;
  return basis.InvTransformFast(w);
}

constexpr Matrix4x4 Transform3D::ToMatrix() const {
  return Matrix4x4(Vector4(basis.row0.x, basis.row0.y, basis.row0.z, origin.x),
                   Vector4(basis.row1.x, basis.row1.y, basis.row1.z, origin.y),
                   Vector4(basis.row2.x, basis.row2.y, basis.row2.z, origin.z),
                   Vector4(0.0_r, 0.0_r, 0.0_r, 1.0_r));
}

}  // namespace ho