#include "matrix4x4.h"

#include <string>
#include <vector>

#include "matrix3x3.h"
#include "vector3.h"

namespace ho {

Matrix4x4& Matrix4x4::Orthogonalize() {
  const real l0 = col0().Magnitude();
  const real l1 = col1().Magnitude();
  const real l2 = col2().Magnitude();
  const real l3 = col3().Magnitude();
  Orthonormalize();
  set_col0(col0() * l0);
  set_col1(col1() * l1);
  set_col2(col2() * l2);
  set_col3(col3() * l3);
  return *this;
}

Matrix4x4 Matrix4x4::Orthogonalized() const {
  Matrix4x4 copy = *this;
  return copy.Orthogonalize();
}

Matrix4x4& Matrix4x4::Orthonormalize() {
  Vector4 x = col0();
  Vector4 y = col1();
  Vector4 z = col2();
  Vector4 w = col3();

  x.Normalize();
  y -= x * (x.Dot(y));
  y.Normalize();
  z -= x * (x.Dot(z)) + y * (y.Dot(z));
  z.Normalize();
  w -= x * (x.Dot(w)) + y * (y.Dot(w)) + z * (z.Dot(w));
  w.Normalize();

  set_col0(x);
  set_col1(y);
  set_col2(z);
  set_col3(w);
  return *this;
}

Matrix4x4 Matrix4x4::Orthonormalized() const {
  Matrix4x4 copy = *this;
  return copy.Orthonormalize();
}

Matrix3x3 Matrix4x4::ToMatrix3x3() const {
  return Matrix3x3(Vector3(row0.x, row0.y, row0.z),
                   Vector3(row1.x, row1.y, row1.z),
                   Vector3(row2.x, row2.y, row2.z));
}

std::string Matrix4x4::ToString() const {
  char res[300];
  std::snprintf(res, sizeof(res),
                "| %.3f , %.3f , %.3f, %.3f |\n| %.3f , %.3f , %.3f, %.3f |\n| "
                "%.3f , %.3f , %.3f, %.3f |\n| %.3f , %.3f , %.3f, %.3f |",
                (float)row0.x, (float)row0.y, (float)row0.z, (float)row0.w,
                (float)row1.x, (float)row1.y, (float)row1.z, (float)row1.w,
                (float)row2.x, (float)row2.y, (float)row2.z, (float)row2.w,
                (float)row3.x, (float)row3.y, (float)row3.z, (float)row3.w);

  return res;
}

}  // namespace ho