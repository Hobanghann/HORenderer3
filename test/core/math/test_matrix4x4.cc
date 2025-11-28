#include <gtest/gtest.h>

#include <limits>

#include "core/math/matrix3x3.h"
#include "core/math/matrix4x4.h"
#include "core/math/quaternion.h"
#include "core/math/vector4.h"

using namespace ho;
using namespace ho::math;

class Matrix4x4Test : public ::testing::Test {
 protected:
  void SetUp() override {
    m_default = Matrix4x4();
    m_eye = Matrix4x4(Vector4(1.0_r, 0.0_r, 0.0_r, 0.0_r),
                      Vector4(0.0_r, 1.0_r, 0.0_r, 0.0_r),
                      Vector4(0.0_r, 0.0_r, 1.0_r, 0.0_r),
                      Vector4(0.0_r, 0.0_r, 0.0_r, 1.0_r));

    m_116 = Matrix4x4(Vector4(1.0_r, 2.0_r, 3.0_r, 4.0_r),
                      Vector4(5.0_r, 6.0_r, 7.0_r, 8.0_r),
                      Vector4(9.0_r, 10.0_r, 11.0_r, 12.0_r),
                      Vector4(13.0_r, 14.0_r, 15.0_r, 16.0_r));

    m_161 = Matrix4x4(Vector4(16.0_r, 15.0_r, 14.0_r, 13.0_r),
                      Vector4(12.0_r, 11.0_r, 10.0_r, 9.0_r),
                      Vector4(8.0_r, 7.0_r, 6.0_r, 5.0_r),
                      Vector4(4.0_r, 3.0_r, 2.0_r, 1.0_r));

    const real angle = PI / 4.0_r;
    const real s = std::sin(angle);
    const real c = std::cos(angle);
    m_orthonormal =
        Matrix4x4(Vector4(c, -s, 0.0_r, 0.0_r), Vector4(s, c, 0.0_r, 0.0_r),
                  Vector4(0.0_r, 0.0_r, 1.0_r, 0.0_r),
                  Vector4(0.0_r, 0.0_r, 0.0_r, 1.0_r));
    m_orthogonal = Matrix4x4(Vector4(2.0_r * c, -3.0_r * s, 0.0_r, 0.0_r),
                             Vector4(2.0_r * s, 3.0_r * c, 0.0_r, 0.0_r),
                             Vector4(0.0_r, 0.0_r, 1.0_r, 0.0_r),
                             Vector4(0.0_r, 0.0_r, 0.0_r, 1.0_r));
    m_non_singular = Matrix4x4(Vector4(1.0_r, 2.0_r, 3.0_r, 4.0_r),
                               Vector4(5.0_r, 6.0_r, 7.0_r, 8.0_r),
                               Vector4(2.0_r, 6.0_r, 4.0_r, 8.0_r),
                               Vector4(3.0_r, 1.0_r, 1.0_r, 2.0_r));
  }

  Matrix4x4 m_default, m_eye, m_116, m_161, m_orthogonal, m_orthonormal,
      m_non_singular;
};

TEST_F(Matrix4x4Test, Constructors) {
  EXPECT_EQ(m_default.row0, Vector4(0.0_r, 0.0_r, 0.0_r, 0.0_r));
  EXPECT_EQ(m_default.row1, Vector4(0.0_r, 0.0_r, 0.0_r, 0.0_r));
  EXPECT_EQ(m_default.row2, Vector4(0.0_r, 0.0_r, 0.0_r, 0.0_r));
  EXPECT_EQ(m_default.row3, Vector4(0.0_r, 0.0_r, 0.0_r, 0.0_r));

  EXPECT_EQ(m_116.row0, Vector4(1.0_r, 2.0_r, 3.0_r, 4.0_r));
  EXPECT_EQ(m_116.row1, Vector4(5.0_r, 6.0_r, 7.0_r, 8.0_r));
  EXPECT_EQ(m_116.row2, Vector4(9.0_r, 10.0_r, 11.0_r, 12.0_r));
  EXPECT_EQ(m_116.row3, Vector4(13.0_r, 14.0_r, 15.0_r, 16.0_r));
}

TEST_F(Matrix4x4Test, Assignment) {
  m_default = m_116;
  EXPECT_EQ(m_default, m_116);
}

TEST_F(Matrix4x4Test, StaticConstant) { EXPECT_EQ(Matrix4x4::IDENTITY, m_eye); }

TEST_F(Matrix4x4Test, Accessors) {
  EXPECT_EQ(m_116.col0(), Vector4(1.0_r, 5.0_r, 9.0_r, 13.0_r));
  EXPECT_EQ(m_116.col1(), Vector4(2.0_r, 6.0_r, 10.0_r, 14.0_r));
  EXPECT_EQ(m_116.col2(), Vector4(3.0_r, 7.0_r, 11.0_r, 15.0_r));
  EXPECT_EQ(m_116.col3(), Vector4(4.0_r, 8.0_r, 12.0_r, 16.0_r));

  m_116.set_col0(Vector4(10.0_r, 50.0_r, 90.0_r, 130.0_r));
  m_116.set_col1(Vector4(20.0_r, 60.0_r, 100.0_r, 140.0_r));
  m_116.set_col2(Vector4(30.0_r, 70.0_r, 110.0_r, 150.0_r));
  m_116.set_col3(Vector4(40.0_r, 80.0_r, 120.0_r, 160.0_r));
  EXPECT_EQ(m_116.col0(), Vector4(10.0_r, 50.0_r, 90.0_r, 130.0_r));
  EXPECT_EQ(m_116.col1(), Vector4(20.0_r, 60.0_r, 100.0_r, 140.0_r));
  EXPECT_EQ(m_116.col2(), Vector4(30.0_r, 70.0_r, 110.0_r, 150.0_r));
  EXPECT_EQ(m_116.col3(), Vector4(40.0_r, 80.0_r, 120.0_r, 160.0_r));
}

TEST_F(Matrix4x4Test, Addition) {
  Matrix4x4 sum = m_116 + m_161;
  EXPECT_EQ(sum.row0, Vector4(17.0_r, 17.0_r, 17.0_r, 17.0_r));
  EXPECT_EQ(sum.row1, Vector4(17.0_r, 17.0_r, 17.0_r, 17.0_r));
  EXPECT_EQ(sum.row2, Vector4(17.0_r, 17.0_r, 17.0_r, 17.0_r));
  EXPECT_EQ(sum.row3, Vector4(17.0_r, 17.0_r, 17.0_r, 17.0_r));

  m_116 += m_161;
  EXPECT_EQ(m_116.row0, Vector4(17.0_r, 17.0_r, 17.0_r, 17.0_r));
  EXPECT_EQ(m_116.row1, Vector4(17.0_r, 17.0_r, 17.0_r, 17.0_r));
  EXPECT_EQ(m_116.row2, Vector4(17.0_r, 17.0_r, 17.0_r, 17.0_r));
  EXPECT_EQ(m_116.row3, Vector4(17.0_r, 17.0_r, 17.0_r, 17.0_r));
}

TEST_F(Matrix4x4Test, Subtraction) {
  Matrix4x4 sub = m_161 - m_116;
  EXPECT_EQ(sub.row0, Vector4(15.0_r, 13.0_r, 11.0_r, 9.0_r));
  EXPECT_EQ(sub.row1, Vector4(7.0_r, 5.0_r, 3.0_r, 1.0_r));
  EXPECT_EQ(sub.row2, Vector4(-1.0_r, -3.0_r, -5.0_r, -7.0_r));
  EXPECT_EQ(sub.row3, Vector4(-9.0_r, -11.0_r, -13.0_r, -15.0_r));

  m_161 -= m_116;
  EXPECT_EQ(m_161.row0, Vector4(15.0_r, 13.0_r, 11.0_r, 9.0_r));
  EXPECT_EQ(m_161.row1, Vector4(7.0_r, 5.0_r, 3.0_r, 1.0_r));
  EXPECT_EQ(m_161.row2, Vector4(-1.0_r, -3.0_r, -5.0_r, -7.0_r));
  EXPECT_EQ(m_161.row3, Vector4(-9.0_r, -11.0_r, -13.0_r, -15.0_r));
}

TEST_F(Matrix4x4Test, MatrixMultiplication) {
  Matrix4x4 expected(Vector4(80.0_r, 70.0_r, 60.0_r, 50.0_r),
                     Vector4(240.0_r, 214.0_r, 188.0_r, 162.0_r),
                     Vector4(400.0_r, 358.0_r, 316.0_r, 274.0_r),
                     Vector4(560.0_r, 502.0_r, 444.0_r, 386.0_r));

  EXPECT_EQ(m_116 * m_161, expected);

  m_116 *= m_161;
  EXPECT_EQ(m_116, expected);
}

TEST_F(Matrix4x4Test, ScalarMultiplication) {
  Matrix4x4 m = m_116 * 2.0_r;
  EXPECT_EQ(m.row0, Vector4(2.0_r, 4.0_r, 6.0_r, 8.0_r));
  EXPECT_EQ(m.row1, Vector4(10.0_r, 12.0_r, 14.0_r, 16.0_r));
  EXPECT_EQ(m.row2, Vector4(18.0_r, 20.0_r, 22.0_r, 24.0_r));
  EXPECT_EQ(m.row3, Vector4(26.0_r, 28.0_r, 30.0_r, 32.0_r));

  m_116 *= 2.0_r;
  EXPECT_EQ(m_116.row0, Vector4(2.0_r, 4.0_r, 6.0_r, 8.0_r));
  EXPECT_EQ(m_116.row1, Vector4(10.0_r, 12.0_r, 14.0_r, 16.0_r));
  EXPECT_EQ(m_116.row2, Vector4(18.0_r, 20.0_r, 22.0_r, 24.0_r));
  EXPECT_EQ(m_116.row3, Vector4(26.0_r, 28.0_r, 30.0_r, 32.0_r));
}

TEST_F(Matrix4x4Test, ScalarDivision) {
  Matrix4x4 div = m_116 / 2.0_r;
  EXPECT_EQ(div.row0, Vector4(1.0_r / 2.0_r, 2.0_r / 2.0_r, 3.0_r / 2.0_r,
                              4.0_r / 2.0_r));
  EXPECT_EQ(div.row1, Vector4(5.0_r / 2.0_r, 6.0_r / 2.0_r, 7.0_r / 2.0_r,
                              8.0_r / 2.0_r));
  EXPECT_EQ(div.row2, Vector4(9.0_r / 2.0_r, 10.0_r / 2.0_r, 11.0_r / 2.0_r,
                              12.0_r / 2.0_r));
  EXPECT_EQ(div.row3, Vector4(13.0_r / 2.0_r, 14.0_r / 2.0_r, 15.0_r / 2.0_r,
                              16.0_r / 2.0_r));

  m_116 /= 2.0_r;
  EXPECT_EQ(m_116.row0, Vector4(1.0_r / 2.0_r, 2.0_r / 2.0_r, 3.0_r / 2.0_r,
                                4.0_r / 2.0_r));
  EXPECT_EQ(m_116.row1, Vector4(5.0_r / 2.0_r, 6.0_r / 2.0_r, 7.0_r / 2.0_r,
                                8.0_r / 2.0_r));
  EXPECT_EQ(m_116.row2, Vector4(9.0_r / 2.0_r, 10.0_r / 2.0_r, 11.0_r / 2.0_r,
                                12.0_r / 2.0_r));
  EXPECT_EQ(m_116.row3, Vector4(13.0_r / 2.0_r, 14.0_r / 2.0_r, 15.0_r / 2.0_r,
                                16.0_r / 2.0_r));

  m_116 /= 0.0_r;
  EXPECT_TRUE(std::isinf(m_116.row0.x));
  EXPECT_TRUE(std::isinf(m_116.row0.y));
  EXPECT_TRUE(std::isinf(m_116.row0.z));
  EXPECT_TRUE(std::isinf(m_116.row0.w));
  EXPECT_TRUE(std::isinf(m_116.row1.x));
  EXPECT_TRUE(std::isinf(m_116.row1.y));
  EXPECT_TRUE(std::isinf(m_116.row1.z));
  EXPECT_TRUE(std::isinf(m_116.row1.w));
  EXPECT_TRUE(std::isinf(m_116.row2.x));
  EXPECT_TRUE(std::isinf(m_116.row2.y));
  EXPECT_TRUE(std::isinf(m_116.row2.z));
  EXPECT_TRUE(std::isinf(m_116.row2.w));
  EXPECT_TRUE(std::isinf(m_116.row3.x));
  EXPECT_TRUE(std::isinf(m_116.row3.y));
  EXPECT_TRUE(std::isinf(m_116.row3.z));
  EXPECT_TRUE(std::isinf(m_116.row3.w));
}

TEST_F(Matrix4x4Test, ComparisonOperators) {
  Matrix4x4 a(Vector4(1.0_r, 2.0_r, 3.0_r, 4.0_r),
              Vector4(5.0_r, 6.0_r, 7.0_r, 8.0_r),
              Vector4(9.0_r, 10.0_r, 11.0_r, 12.0_r),
              Vector4(13.0_r, 14.0_r, 15.0_r, 16.0_r));
  EXPECT_TRUE(m_116 == a);
  EXPECT_FALSE(m_116 != a);
  EXPECT_TRUE(m_116 != m_161);
}

TEST_F(Matrix4x4Test, ApproximateEqualityCheck) {
  Matrix4x4 near_116(
      Vector4(1.000001_r, 2.000001_r, 3.000001_r, 4.000001_r),
      Vector4(5.000001_r, 6.000001_r, 7.000001_r, 8.000001_r),
      Vector4(9.000001_r, 10.000001_r, 11.000001_r, 12.000001_r),
      Vector4(13.000001_r, 14.000001_r, 15.000001_r, 16.000001_r));

  EXPECT_TRUE(m_116.IsEqualApprox(near_116));
  EXPECT_FALSE(m_116.IsNotEqualApprox(near_116));
}

TEST_F(Matrix4x4Test, OrthogonalityCheck) {
  EXPECT_TRUE(m_orthogonal.IsOrthogonal());
  EXPECT_FALSE(m_116.IsOrthogonal());
}

TEST_F(Matrix4x4Test, Orthogonalization) {
  Matrix4x4 o = m_non_singular.Orthogonalized();
  EXPECT_TRUE(o.IsOrthogonal());
  EXPECT_FALSE(m_non_singular.IsOrthogonal());
  EXPECT_NEAR(o.col0().Magnitude(), m_non_singular.col0().Magnitude(),
              EPSILON_CMP);
  EXPECT_NEAR(o.col1().Magnitude(), m_non_singular.col1().Magnitude(),
              EPSILON_CMP);
  EXPECT_NEAR(o.col2().Magnitude(), m_non_singular.col2().Magnitude(),
              EPSILON_CMP);
  EXPECT_NEAR(o.col3().Magnitude(), m_non_singular.col3().Magnitude(),
              EPSILON_CMP);

  o = m_non_singular.Orthogonalize();
  EXPECT_TRUE(o.IsOrthogonal());
  EXPECT_NEAR(o.col0().Magnitude(), m_non_singular.col0().Magnitude(),
              EPSILON_CMP);
  EXPECT_NEAR(o.col1().Magnitude(), m_non_singular.col1().Magnitude(),
              EPSILON_CMP);
  EXPECT_NEAR(o.col2().Magnitude(), m_non_singular.col2().Magnitude(),
              EPSILON_CMP);
  EXPECT_NEAR(o.col3().Magnitude(), m_non_singular.col3().Magnitude(),
              EPSILON_CMP);
}

TEST_F(Matrix4x4Test, OrthonormalityCheck) {
  EXPECT_TRUE(m_orthonormal.IsOrthonormal());
  EXPECT_FALSE(m_116.IsOrthonormal());
}

TEST_F(Matrix4x4Test, Orthonormalization) {
  Matrix4x4 n = m_non_singular.Orthonormalized();
  EXPECT_TRUE(n.IsOrthonormal());
  EXPECT_FALSE(m_non_singular.IsOrthonormal());

  m_non_singular.Orthonormalize();
  EXPECT_TRUE(m_non_singular.IsOrthonormal());
}

TEST_F(Matrix4x4Test, Transposition) {
  Matrix4x4 t = m_116.Transposed();
  EXPECT_EQ(t.row0, Vector4(1.0_r, 5.0_r, 9.0_r, 13.0_r));
  EXPECT_EQ(t.row1, Vector4(2.0_r, 6.0_r, 10.0_r, 14.0_r));
  EXPECT_EQ(t.row2, Vector4(3.0_r, 7.0_r, 11.0_r, 15.0_r));
  EXPECT_EQ(t.row3, Vector4(4.0_r, 8.0_r, 12.0_r, 16.0_r));

  m_116.Transpose();
  EXPECT_EQ(m_116, t);
}

TEST_F(Matrix4x4Test, Inversion) {
  Matrix4x4 m(
      Vector4(1.0_r, 0.0_r, 0.0_r, 0.0_r), Vector4(0.0_r, 2.0_r, 0.0_r, 0.0_r),
      Vector4(0.0_r, 0.0_r, 3.0_r, 0.0_r), Vector4(0.0_r, 0.0_r, 0.0_r, 1.0_r));
  Matrix4x4 expected(Vector4(1.0_r, 0.0_r, 0.0_r, 0.0_r),
                     Vector4(0.0_r, 0.5_r, 0.0_r, 0.0_r),
                     Vector4(0.0_r, 0.0_r, 1.0_r / 3.0_r, 0.0_r),
                     Vector4(0.0_r, 0.0_r, 0.0_r, 1.0_r));

  Matrix4x4 inv = m.Inverse();
  EXPECT_TRUE(inv.IsEqualApprox(expected));
  Matrix4x4 id = m * inv;
  EXPECT_TRUE(id.IsEqualApprox(Matrix4x4::IDENTITY));

  inv = m;
  inv.Invert();
  id = m * inv;
  EXPECT_TRUE(inv.IsEqualApprox(expected));
  EXPECT_TRUE(id.IsEqualApprox(Matrix4x4::IDENTITY));

  Matrix4x4 singular(Vector4(1.0_r, 2.0_r, 3.0_r, 4.0_r),
                     Vector4(2.0_r, 4.0_r, 6.0_r, 8.0_r),
                     Vector4(3.0_r, 6.0_r, 9.0_r, 12.0_r),
                     Vector4(4.0_r, 8.0_r, 12.0_r, 16.0_r));
  Matrix4x4 inv_singular = singular.Inverse();
  EXPECT_TRUE(std::isnan(inv_singular.row0.x));
  EXPECT_TRUE(std::isnan(inv_singular.row3.w));
}

TEST_F(Matrix4x4Test, Trace) {
  EXPECT_FLOAT_EQ(m_116.Trace(), 1.0_r + 6.0_r + 11.0_r + 16.0_r);
}

TEST_F(Matrix4x4Test, Determinant) {
  Matrix4x4 m(
      Vector4(1.0_r, 0.0_r, 0.0_r, 0.0_r), Vector4(0.0_r, 2.0_r, 0.0_r, 0.0_r),
      Vector4(0.0_r, 0.0_r, 3.0_r, 0.0_r), Vector4(0.0_r, 0.0_r, 0.0_r, 4.0_r));
  EXPECT_NEAR(m.Determinant(), 24.0_r, EPSILON_CMP);
}

TEST_F(Matrix4x4Test, ToMatrix3x3Conversion) {
  Matrix3x3 sub = m_116.ToMatrix3x3();
  EXPECT_EQ(sub.row0, Vector3(1.0_r, 2.0_r, 3.0_r));
  EXPECT_EQ(sub.row1, Vector3(5.0_r, 6.0_r, 7.0_r));
  EXPECT_EQ(sub.row2, Vector3(9.0_r, 10.0_r, 11.0_r));
}
