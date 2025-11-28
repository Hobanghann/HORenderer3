#include <gtest/gtest.h>

#include "core/math/matrix2x2.h"
#include "core/math/vector2.h"

using namespace ho;
using namespace ho::math;

class Matrix2x2Test : public ::testing::Test {
 protected:
  void SetUp() override {
    m_default = Matrix2x2();
    m_eye = Matrix2x2(Vector2(1.0_r, 0.0_r), Vector2(0.0_r, 1.0_r));

    m_14 = Matrix2x2(Vector2(1.0_r, 2.0_r), Vector2(3.0_r, 4.0_r));
    m_58 = Matrix2x2(Vector2(5.0_r, 6.0_r), Vector2(7.0_r, 8.0_r));

    const real angle = PI / 4.0_r;
    const real s = std::sin(angle), c = std::cos(angle);
    m_orthonormal = Matrix2x2(Vector2(c, -s), Vector2(s, c));
    m_orthogonal = Matrix2x2(Vector2(2.0_r * c, 3.0_r * -s),
                             Vector2(2.0_r * s, 3.0_r * c));
  }

  Matrix2x2 m_default, m_eye;
  Matrix2x2 m_14, m_58;
  Matrix2x2 m_orthogonal, m_orthonormal;
};

TEST_F(Matrix2x2Test, Constructors) {
  EXPECT_EQ(m_default.row0, Vector2(0.0_r, 0.0_r));
  EXPECT_EQ(m_default.row1, Vector2(0.0_r, 0.0_r));

  EXPECT_EQ(m_14.row0, Vector2(1.0_r, 2.0_r));
  EXPECT_EQ(m_14.row1, Vector2(3.0_r, 4.0_r));
}

TEST_F(Matrix2x2Test, Assignment) {
  m_default = m_14;
  EXPECT_EQ(m_default, m_14);
}

TEST_F(Matrix2x2Test, StaticConstant) { EXPECT_EQ(Matrix2x2::IDENTITY, m_eye); }

TEST_F(Matrix2x2Test, Accessors) {
  EXPECT_EQ(m_14.col0(), Vector2(1.0_r, 3.0_r));
  EXPECT_EQ(m_14.col1(), Vector2(2.0_r, 4.0_r));

  m_14.set_col0(Vector2(10.0_r, 30.0_r));
  m_14.set_col1(Vector2(20.0_r, 40.0_r));
  EXPECT_EQ(m_14.col0(), Vector2(10.0_r, 30.0_r));
  EXPECT_EQ(m_14.col1(), Vector2(20.0_r, 40.0_r));
}

TEST_F(Matrix2x2Test, Addition) {
  Matrix2x2 sum = m_14 + m_58;
  EXPECT_EQ(sum.row0, Vector2(6.0_r, 8.0_r));
  EXPECT_EQ(sum.row1, Vector2(10.0_r, 12.0_r));

  m_14 += m_58;
  EXPECT_EQ(m_14, Matrix2x2(Vector2(6.0_r, 8.0_r), Vector2(10.0_r, 12.0_r)));
}

TEST_F(Matrix2x2Test, Subtraction) {
  Matrix2x2 sub = m_58 - m_14;
  EXPECT_EQ(sub.row0, Vector2(4.0_r, 4.0_r));
  EXPECT_EQ(sub.row1, Vector2(4.0_r, 4.0_r));

  m_58 -= m_14;
  EXPECT_EQ(m_58, Matrix2x2(Vector2(4.0_r, 4.0_r), Vector2(4.0_r, 4.0_r)));
}

TEST_F(Matrix2x2Test, MatrixMultiplication) {
  // m_14 * m_58 = [[1*5+3*6, 1*7+3*8],
  //          [2*5+4*6, 2*7+4*8]]
  Matrix2x2 expected(Vector2(19.0_r, 22.0_r), Vector2(43.0_r, 50.0_r));
  EXPECT_TRUE((m_14 * m_58).IsEqualApprox(expected));

  m_14 *= m_58;
  EXPECT_TRUE(m_14.IsEqualApprox(expected));
}

TEST_F(Matrix2x2Test, VectorMultiplication) {
  Vector2 v(5.0_r, 6.0_r);
  // row0·v = 1*5 + 2*6 = 17
  // row1·v = 3*5 + 4*6 = 39
  EXPECT_EQ(m_14 * v, Vector2(17.0_r, 39.0_r));
}

TEST_F(Matrix2x2Test, ScalarMultiplication) {
  Matrix2x2 mul = m_14 * 2.0_r;
  EXPECT_EQ(mul.row0, Vector2(2.0_r, 4.0_r));
  EXPECT_EQ(mul.row1, Vector2(6.0_r, 8.0_r));

  Matrix2x2 l_mul = 2.0_r * m_14;
  EXPECT_EQ(l_mul.row0, Vector2(2.0_r, 4.0_r));
  EXPECT_EQ(l_mul.row1, Vector2(6.0_r, 8.0_r));

  m_14 *= 2.0_r;
  EXPECT_EQ(m_14.row0, Vector2(2.0_r, 4.0_r));
  EXPECT_EQ(m_14.row1, Vector2(6.0_r, 8.0_r));
}

TEST_F(Matrix2x2Test, ScalarDivision) {
  Matrix2x2 div = m_14 / 2.0_r;
  EXPECT_EQ(div.row0, Vector2(1.0_r / 2.0_r, 2.0_r / 2.0_r));
  EXPECT_EQ(div.row1, Vector2(3.0_r / 2.0_r, 4.0_r / 2.0_r));

  m_14 /= 2.0_r;
  EXPECT_EQ(m_14.row0, Vector2(1.0_r / 2.0_r, 2.0_r / 2.0_r));
  EXPECT_EQ(m_14.row1, Vector2(3.0_r / 2.0_r, 4.0_r / 2.0_r));

  m_14 /= 0.0_r;
  EXPECT_TRUE(std::isinf(m_14.row0.x));
  EXPECT_TRUE(std::isinf(m_14.row0.y));
  EXPECT_TRUE(std::isinf(m_14.row1.x));
  EXPECT_TRUE(std::isinf(m_14.row1.y));
}

TEST_F(Matrix2x2Test, ComparisonOperators) {
  Matrix2x2 a(Vector2(1.0_r, 2.0_r), Vector2(3.0_r, 4.0_r));

  EXPECT_TRUE(m_14 == a);
  EXPECT_FALSE(m_14 != a);
  EXPECT_TRUE(m_14 != m_58);
}

TEST_F(Matrix2x2Test, ApproximateEqualityCheck) {
  Matrix2x2 a(Vector2(1.000001_r, 2.000001_r), Vector2(2.999999_r, 3.999999_r));
  EXPECT_TRUE(m_14.IsEqualApprox(a));
  EXPECT_FALSE(m_14.IsNotEqualApprox(a));
}

TEST_F(Matrix2x2Test, OrthogonalityCheck) {
  EXPECT_TRUE(m_orthogonal.IsOrthogonal());
  EXPECT_FALSE(m_14.IsOrthogonal());
}

TEST_F(Matrix2x2Test, Orthogonalization) {
  Matrix2x2 o = m_14.Orthogonalized();
  EXPECT_TRUE(o.IsOrthogonal()) << o.col0().Dot(o.col1());
  EXPECT_NEAR(o.col0().Magnitude(), m_14.col0().Magnitude(), EPSILON_CMP);
  EXPECT_NEAR(o.col1().Magnitude(), m_14.col1().Magnitude(), EPSILON_CMP);

  EXPECT_FALSE(m_14.IsOrthogonal());

   o = m_14.Orthogonalize();
  EXPECT_TRUE(o.IsOrthogonal());
  EXPECT_NEAR(o.col0().Magnitude(), m_14.col0().Magnitude(), EPSILON_CMP);
  EXPECT_NEAR(o.col1().Magnitude(), m_14.col1().Magnitude(), EPSILON_CMP);
}

TEST_F(Matrix2x2Test, OrthonormalityCheck) {
  EXPECT_TRUE(m_orthonormal.IsOrthonormal());
  EXPECT_FALSE(m_14.IsOrthonormal());
}

TEST_F(Matrix2x2Test, Orthonormalization) {

  Matrix2x2 n = m_14.Orthonormalized();
  EXPECT_TRUE(n.IsOrthonormal());
  EXPECT_FALSE(m_14.IsOrthonormal());

  m_14.Orthonormalize();
  EXPECT_TRUE(m_14.IsOrthonormal());
}

TEST_F(Matrix2x2Test, Transposition) {
  Matrix2x2 transposed(Vector2(1.0_r, 3.0_r), Vector2(2.0_r, 4.0_r));

  Matrix2x2 t = m_14.Transposed();
  EXPECT_TRUE(t.IsEqualApprox(transposed));
  EXPECT_TRUE(m_14.IsEqualApprox(
      Matrix2x2(Vector2(1.0_r, 2.0_r), Vector2(3.0_r, 4.0_r))));

  m_14.Transpose();
  EXPECT_TRUE(m_14.IsEqualApprox(transposed));
}

TEST_F(Matrix2x2Test, Inversion) {
  Matrix2x2 inversed(Vector2(-2.0_r, 1.0_r), Vector2(1.5_r, -0.5_r));

  Matrix2x2 inv = m_14.Inverse();
  EXPECT_TRUE(inv.IsEqualApprox(inversed));
  Matrix2x2 id = m_14 * inv;
  EXPECT_TRUE(id.IsEqualApprox(Matrix2x2::IDENTITY));
  EXPECT_TRUE(m_14.IsEqualApprox(
      Matrix2x2(Vector2(1.0_r, 2.0_r), Vector2(3.0_r, 4.0_r))));

  Matrix2x2 a = m_14;
  a.Invert();
  EXPECT_TRUE(a.IsEqualApprox(inversed));
  id = a * m_14;
  EXPECT_TRUE(id.IsEqualApprox(Matrix2x2::IDENTITY));

  // determinant = 0 case
  Matrix2x2 singular(Vector2(1.0_r, 2.0_r), Vector2(2.0_r, 4.0_r));
  Matrix2x2 inv_singular = singular.Inverse();
  EXPECT_TRUE(std::isinf(inv_singular.row0.x));
  EXPECT_TRUE(std::isinf(inv_singular.row0.y));
  EXPECT_TRUE(std::isinf(inv_singular.row1.x));
  EXPECT_TRUE(std::isinf(inv_singular.row1.y));
}

TEST_F(Matrix2x2Test, Trace) { EXPECT_FLOAT_EQ(m_14.Trace(), 1.0_r + 4.0_r); }

TEST_F(Matrix2x2Test, Determinant) {
  EXPECT_FLOAT_EQ(m_14.Determinant(), 1.0_r * 4.0_r - 2.0_r * 3.0_r);
}
