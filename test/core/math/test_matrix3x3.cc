#include <gtest/gtest.h>

#include <limits>

#include "core/math/matrix3x3.h"
#include "core/math/quaternion.h"
#include "core/math/vector3.h"

using namespace ho;
using namespace ho::math;

class Matrix3x3Test : public ::testing::Test {
 protected:
  void SetUp() override {
    m_default = Matrix3x3();
    m_eye =
        Matrix3x3(Vector3(1.0_r, 0.0_r, 0.0_r), Vector3(0.0_r, 1.0_r, 0.0_r),
                  Vector3(0.0_r, 0.0_r, 1.0_r));

    m_19 = Matrix3x3(Vector3(1.0_r, 2.0_r, 3.0_r), Vector3(4.0_r, 5.0_r, 6.0_r),
                     Vector3(7.0_r, 8.0_r, 9.0_r));

    m_91 = Matrix3x3(Vector3(9.0_r, 8.0_r, 7.0_r), Vector3(6.0_r, 5.0_r, 4.0_r),
                     Vector3(3.0_r, 2.0_r, 1.0_r));

    const real angle = PI / 4.0_r;
    const real s = std::sin(angle);
    const real c = std::cos(angle);

    m_pitch = Matrix3x3(Vector3(1.0_r, 0.0_r, 0.0_r), Vector3(0.0_r, c, -s),
                        Vector3(0.0_r, s, c));
    m_yaw = Matrix3x3(Vector3(c, 0.0_r, s), Vector3(0.0_r, 1.0_r, 0.0_r),
                      Vector3(-s, 0.0_r, c));
    m_roll = Matrix3x3(Vector3(c, -s, 0.0_r), Vector3(s, c, 0.0_r),
                       Vector3(0.0_r, 0.0_r, 1.0_r));

    m_orthogonal =
        Matrix3x3(Vector3(2 * c, -3 * s, 0.0_r), Vector3(2 * s, 3 * c, 0.0_r),
                  Vector3(0.0_r, 0.0_r, 1.0_r));
    m_non_singular =
        Matrix3x3(Vector3(1.0_r, 2.0_r, 3.0_r), Vector3(0.0_r, 2.0_r, 4.0_r),
                  Vector3(5.0_r, 6.0_r, 0.0_r));
  }

  Matrix3x3 m_default, m_eye, m_19, m_91, m_pitch, m_yaw, m_roll, m_orthogonal,
      m_non_singular;
};

TEST_F(Matrix3x3Test, Constructors) {
  EXPECT_EQ(m_default.row0, Vector3(0.0_r, 0.0_r, 0.0_r));
  EXPECT_EQ(m_default.row1, Vector3(0.0_r, 0.0_r, 0.0_r));
  EXPECT_EQ(m_default.row2, Vector3(0.0_r, 0.0_r, 0.0_r));

  EXPECT_EQ(m_19.row0, Vector3(1.0_r, 2.0_r, 3.0_r));
  EXPECT_EQ(m_19.row1, Vector3(4.0_r, 5.0_r, 6.0_r));
  EXPECT_EQ(m_19.row2, Vector3(7.0_r, 8.0_r, 9.0_r));
}

TEST_F(Matrix3x3Test, Assignment) {
  m_default = m_19;
  EXPECT_EQ(m_default, m_19);
}

TEST_F(Matrix3x3Test, StaticConstant) { EXPECT_EQ(Matrix3x3::IDENTITY, m_eye); }

TEST_F(Matrix3x3Test, FromAxisAngle) {
  Vector3 axis(0.0_r, 0.0_r, 1.0_r);
  real angle = PI / 4.0_r;
  Matrix3x3 rot = Matrix3x3::FromAxisAngle(axis, angle);
  EXPECT_TRUE(rot.IsEqualApprox(m_roll));
}

TEST_F(Matrix3x3Test, FromEuler) {
  const real angle = PI / 4.0_r;
  EXPECT_TRUE(Matrix3x3::FromEuler(angle, angle, angle, math::XYZ)
                  .IsEqualApprox(m_pitch * m_yaw * m_roll));
  EXPECT_TRUE(Matrix3x3::FromEuler(angle, angle, angle, math::XZY)
                  .IsEqualApprox(m_pitch * m_roll * m_yaw));
  EXPECT_TRUE(Matrix3x3::FromEuler(angle, angle, angle, math::YZX)
                  .IsEqualApprox(m_yaw * m_roll * m_pitch));
  EXPECT_TRUE(Matrix3x3::FromEuler(angle, angle, angle, math::YXZ)
                  .IsEqualApprox(m_yaw * m_pitch * m_roll));
  EXPECT_TRUE(Matrix3x3::FromEuler(angle, angle, angle, math::ZXY)
                  .IsEqualApprox(m_roll * m_pitch * m_yaw));
  EXPECT_TRUE(Matrix3x3::FromEuler(angle, angle, angle, math::ZYX)
                  .IsEqualApprox(m_roll * m_yaw * m_pitch));
}

TEST_F(Matrix3x3Test, FromQuaternion) {
  Quaternion q =
      Quaternion::FromAxisAngle(Vector3(0.0_r, 0.0_r, 1.0_r), PI / 4.0_r);
  Matrix3x3 rot = Matrix3x3::FromQuaternion(q);
  EXPECT_TRUE(rot.IsEqualApprox(m_roll));
}

TEST_F(Matrix3x3Test, Accessors) {
  EXPECT_EQ(m_19.col0(), Vector3(1.0_r, 4.0_r, 7.0_r));
  EXPECT_EQ(m_19.col1(), Vector3(2.0_r, 5.0_r, 8.0_r));
  EXPECT_EQ(m_19.col2(), Vector3(3.0_r, 6.0_r, 9.0_r));

  m_19.set_col0(Vector3(10.0_r, 40.0_r, 70.0_r));
  m_19.set_col1(Vector3(20.0_r, 50.0_r, 80.0_r));
  m_19.set_col2(Vector3(30.0_r, 60.0_r, 90.0_r));

  EXPECT_EQ(m_19.col0(), Vector3(10.0_r, 40.0_r, 70.0_r));
  EXPECT_EQ(m_19.col1(), Vector3(20.0_r, 50.0_r, 80.0_r));
  EXPECT_EQ(m_19.col2(), Vector3(30.0_r, 60.0_r, 90.0_r));
}

TEST_F(Matrix3x3Test, Addition) {
  Matrix3x3 sum = m_19 + m_91;
  EXPECT_EQ(sum.row0, Vector3(10.0_r, 10.0_r, 10.0_r));
  EXPECT_EQ(sum.row1, Vector3(10.0_r, 10.0_r, 10.0_r));
  EXPECT_EQ(sum.row2, Vector3(10.0_r, 10.0_r, 10.0_r));

  m_19 += m_91;
  EXPECT_EQ(m_19, sum);
}

TEST_F(Matrix3x3Test, Subtraction) {
  Matrix3x3 sub = m_91 - m_19;
  EXPECT_EQ(sub.row0, Vector3(8.0_r, 6.0_r, 4.0_r));
  EXPECT_EQ(sub.row1, Vector3(2.0_r, 0.0_r, -2.0_r));
  EXPECT_EQ(sub.row2, Vector3(-4.0_r, -6.0_r, -8.0_r));

  m_91 -= m_19;
  EXPECT_EQ(m_91, sub);
}

TEST_F(Matrix3x3Test, MatrixMultiplication) {
  Matrix3x3 expected(Vector3(30.0_r, 24.0_r, 18.0_r),
                     Vector3(84.0_r, 69.0_r, 54.0_r),
                     Vector3(138.0_r, 114.0_r, 90.0_r));
  EXPECT_EQ(m_19 * m_91, expected);

  m_19 *= m_91;
  EXPECT_EQ(m_19, expected);
}

TEST_F(Matrix3x3Test, ScalarMultiplication) {
  Matrix3x3 mul = m_19 * 2.0_r;
  EXPECT_EQ(mul.row0, Vector3(2.0_r, 4.0_r, 6.0_r));
  EXPECT_EQ(mul.row1, Vector3(8.0_r, 10.0_r, 12.0_r));
  EXPECT_EQ(mul.row2, Vector3(14.0_r, 16.0_r, 18.0_r));

  Matrix3x3 l_mul = 2.0_r * m_19;
  EXPECT_EQ(l_mul, mul);

  m_19 *= 2.0_r;
  EXPECT_EQ(m_19, mul);
}

TEST_F(Matrix3x3Test, ScalarDivision) {
  Matrix3x3 div = m_19 / 2.0_r;
  EXPECT_EQ(div.row0, Vector3(1.0_r / 2.0_r, 2.0_r / 2.0_r, 3.0_r / 2.0_r));
  EXPECT_EQ(div.row1, Vector3(4.0_r / 2.0_r, 5.0_r / 2.0_r, 6.0_r / 2.0_r));
  EXPECT_EQ(div.row2, Vector3(7.0_r / 2.0_r, 8.0_r / 2.0_r, 9.0_r / 2.0_r));

  m_19 /= 2.0_r;
  EXPECT_EQ(m_19, div);

  m_19 /= 0.0_r;
  EXPECT_TRUE(std::isinf(m_19.row0.x));
  EXPECT_TRUE(std::isinf(m_19.row0.y));
  EXPECT_TRUE(std::isinf(m_19.row0.z));
  EXPECT_TRUE(std::isinf(m_19.row1.x));
  EXPECT_TRUE(std::isinf(m_19.row1.y));
  EXPECT_TRUE(std::isinf(m_19.row1.z));
  EXPECT_TRUE(std::isinf(m_19.row2.x));
  EXPECT_TRUE(std::isinf(m_19.row2.y));
  EXPECT_TRUE(std::isinf(m_19.row2.z));
}

TEST_F(Matrix3x3Test, ComparisonOperators) {
  Matrix3x3 a(Vector3(1.0_r, 2.0_r, 3.0_r), Vector3(4.0_r, 5.0_r, 6.0_r),
              Vector3(7.0_r, 8.0_r, 9.0_r));

  EXPECT_TRUE(m_19 == a);
  EXPECT_FALSE(m_19 != a);
  EXPECT_TRUE(m_19 != m_91);
}

TEST_F(Matrix3x3Test, ApproximateEqualityCheck) {
  Matrix3x3 near_19(Vector3(1.000001_r, 2.000001_r, 3.000001_r),
                    Vector3(4.000001_r, 5.000001_r, 6.000001_r),
                    Vector3(7.000001_r, 8.000001_r, 9.000001_r));

  EXPECT_TRUE(m_19.IsEqualApprox(near_19));
  EXPECT_FALSE(m_19.IsNotEqualApprox(near_19));
}

TEST_F(Matrix3x3Test, OrthogonalityCheck) {
  EXPECT_TRUE(m_orthogonal.IsOrthogonal());
  EXPECT_FALSE(m_19.IsOrthogonal());
}

TEST_F(Matrix3x3Test, Orthogonalization) {
  Matrix3x3 o = m_non_singular.Orthogonalized();
  EXPECT_TRUE(o.IsOrthogonal());
  EXPECT_FALSE(m_non_singular.IsOrthogonal());
  EXPECT_NEAR(o.col0().Magnitude(), m_non_singular.col0().Magnitude(),
              EPSILON_CMP);
  EXPECT_NEAR(o.col1().Magnitude(), m_non_singular.col1().Magnitude(),
              EPSILON_CMP);
  EXPECT_NEAR(o.col2().Magnitude(), m_non_singular.col2().Magnitude(),
              EPSILON_CMP);

  o = m_non_singular.Orthogonalize();
  EXPECT_TRUE(o.IsOrthogonal());
  EXPECT_NEAR(o.col0().Magnitude(), m_non_singular.col0().Magnitude(),
              EPSILON_CMP);
  EXPECT_NEAR(o.col1().Magnitude(), m_non_singular.col1().Magnitude(),
              EPSILON_CMP);
  EXPECT_NEAR(o.col2().Magnitude(), m_non_singular.col2().Magnitude(),
              EPSILON_CMP);
}

TEST_F(Matrix3x3Test, OrthonormalityCheck) {
  EXPECT_TRUE(m_yaw.IsOrthonormal());
  EXPECT_FALSE(m_19.IsOrthonormal());
}

TEST_F(Matrix3x3Test, Orthonormalization) {
  Matrix3x3 n = m_non_singular.Orthonormalized();
  EXPECT_TRUE(n.IsOrthonormal());
  EXPECT_FALSE(m_19.IsOrthonormal());

  m_non_singular.Orthonormalize();
  EXPECT_TRUE(m_non_singular.IsOrthonormal());
}

TEST_F(Matrix3x3Test, Transposition) {
  Matrix3x3 t = m_19.Transposed();
  EXPECT_EQ(t.row0, Vector3(1.0_r, 4.0_r, 7.0_r));
  EXPECT_EQ(t.row1, Vector3(2.0_r, 5.0_r, 8.0_r));
  EXPECT_EQ(t.row2, Vector3(3.0_r, 6.0_r, 9.0_r));

  m_19.Transpose();
  EXPECT_EQ(m_19, t);
}

TEST_F(Matrix3x3Test, Inversion) {
  Matrix3x3 m(Vector3(1.0_r, 2.0_r, 3.0_r), Vector3(0.0_r, 1.0_r, 4.0_r),
              Vector3(5.0_r, 6.0_r, 0.0_r));

  Matrix3x3 inv = m.Inverse();
  Matrix3x3 id = m * inv;
  EXPECT_TRUE(id.IsEqualApprox(Matrix3x3::IDENTITY));

  inv = m;
  inv.Invert();
  id = m * inv;
  EXPECT_TRUE(id.IsEqualApprox(Matrix3x3::IDENTITY));

  Matrix3x3 singular(Vector3(1.0_r, 2.0_r, 3.0_r), Vector3(2.0_r, 4.0_r, 6.0_r),
                     Vector3(3.0_r, 6.0_r, 9.0_r));
  Matrix3x3 inv_singular = singular.Inverse();
  EXPECT_TRUE(std::isnan(inv_singular.row0.x));
  EXPECT_TRUE(std::isnan(inv_singular.row1.y));
  EXPECT_TRUE(std::isnan(inv_singular.row2.z));
}

TEST_F(Matrix3x3Test, Trace) {
  EXPECT_FLOAT_EQ(m_19.Trace(), 1.0_r + 5.0_r + 9.0_r);
}

TEST_F(Matrix3x3Test, Determinant) {
  EXPECT_FLOAT_EQ(m_19.Determinant(), 0.0_r);
  Matrix3x3 m(Vector3(1.0_r, 2.0_r, 3.0_r), Vector3(0.0_r, 1.0_r, 4.0_r),
              Vector3(5.0_r, 6.0_r, 0.0_r));
  EXPECT_NEAR(m.Determinant(), 1.0_r, EPSILON_CMP);
}
