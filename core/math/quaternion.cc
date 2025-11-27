#include "quaternion.h"

#include <string>

#include "math_funcs.h"
#include "matrix3x3.h"

namespace ho {

Quaternion Quaternion::FromAxisAngle(const Vector3& axis, real angle) {
  const real d = axis.Magnitude();
  if (math::IsZeroApprox(d)) {
    return Quaternion();
  }

  real sin, cos;
  math::SinCos(sin, cos, angle * 0.5_r);
  sin /= d;

  return Quaternion(axis.x * sin, axis.y * sin, axis.z * sin, cos);
}
Quaternion Quaternion::FromEuler(real pitch, real yaw, real roll,
                                 math::EulerOrder order) {
  const Quaternion qx = FromAxisAngle(Vector3::UNIT_X, pitch);
  const Quaternion qy = FromAxisAngle(Vector3::UNIT_Y, yaw);
  const Quaternion qz = FromAxisAngle(Vector3::UNIT_Z, roll);

  Quaternion q;
  switch (order) {
    case math::XYZ:
      q = qx * qy * qz;
      break;
    case math::XZY:
      q = qx * qz * qy;
      break;
    case math::YXZ:
      q = qy * qx * qz;
      break;
    case math::YZX:
      q = qy * qz * qx;
      break;
    case math::ZXY:
      q = qz * qx * qy;
      break;
    case math::ZYX:
      q = qz * qy * qx;
      break;
    default:
      q = Quaternion();
      break;
  }
  return q;
}
// This function for converting a matrix3x3 to a quaternion is adapted from
// the get_quaternion() method in the Godot Engine's Basis class.
// Source: https://github.com/godotengine/godot/blob/master/core/math/basis.cpp
Quaternion Quaternion::FromMatrix(const Matrix3x3& m) {
  assert(!m.IsOrthonormal());
  /* Allow getting a quaternion from an unnormalized transform */
  real trace = m.Trace();
  real temp[4];

  if (trace > 0.0_r) {
    real s = math::Sqrt(trace + 1.0_r);
    temp[3] = (s * 0.5_r);
    s = 0.5_r / s;

    temp[0] = ((m.data[2][1] - m.data[1][2]) * s);
    temp[1] = ((m.data[0][2] - m.data[2][0]) * s);
    temp[2] = ((m.data[1][0] - m.data[0][1]) * s);
  } else {
    int i = m.data[0][0] < m.data[1][1] ? (m.data[1][1] < m.data[2][2] ? 2 : 1)
                                        : (m.data[0][0] < m.data[2][2] ? 2 : 0);
    int j = (i + 1) % 3;
    int k = (i + 2) % 3;

    real s = math::Sqrt(m.data[i][i] - m.data[j][j] - m.data[k][k] + 1.0_r);
    temp[i] = s * 0.5f;
    s = 0.5f / s;

    temp[3] = (m.data[k][j] - m.data[j][k]) * s;
    temp[j] = (m.data[j][i] + m.data[i][j]) * s;
    temp[k] = (m.data[k][i] + m.data[i][k]) * s;
  }

  return Quaternion(temp[0], temp[1], temp[2], temp[3]);
}

real Quaternion::Magnitude() const { return math::Sqrt(SqrdMagnitude()); }

Quaternion& Quaternion::Normalize() {
  const real inv_norm = 1.0_r / Magnitude();
  *this *= inv_norm;
  return *this;
}
Quaternion Quaternion::Normalized() const {
  Quaternion q = *this;
  return q.Normalize();
}

Vector3 Quaternion::axis() const { return Vector3(x, y, z).Normalized(); }
real Quaternion::angle() const { return math::Acos(w) * 2.0_r; }

std::string Quaternion::ToString() const {
  char buf[100];
  snprintf(buf, sizeof(buf), "%.3f + %.3f*i, + %.3f*j + %.3f*k", (float)x,
           (float)y, (float)z, (float)w);
  return buf;
}
}  // namespace ho