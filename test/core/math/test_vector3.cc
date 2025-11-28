#include <gtest/gtest.h>

#include <limits>

#include "core/math/vector2.h"
#include "core/math/vector3.h"
#include "core/math/vector4.h"

using namespace ho;

class Vector3Test : public ::testing::Test {
 protected:
  void SetUp() override {
    v_zero = Vector3();
    v_unitx = Vector3(1.0_r, 0.0_r, 0.0_r);
    v_unity = Vector3(0.0_r, 1.0_r, 0.0_r);
    v_unitz = Vector3(0.0_r, 0.0_r, 1.0_r);
    v_123 = Vector3(1.0_r, 2.0_r, 3.0_r);
    v_345 = Vector3(3.0_r, 4.0_r, 5.0_r);
    v_123_a = Vector3(1.000001_r, 2.000001_r, 3.000001_r);
    v_0p1_0p2_0p3 = Vector3(0.1_r, 0.2_r, 0.3_r);
  }

 protected:
  Vector3 v_zero, v_unitx, v_unity, v_unitz;
  Vector3 v_123, v_123_a, v_345, v_0p1_0p2_0p3;
};

TEST_F(Vector3Test, Constructors) {
  EXPECT_EQ(v_zero.x, 0.0_r);
  EXPECT_EQ(v_zero.y, 0.0_r);
  EXPECT_EQ(v_zero.z, 0.0_r);

  EXPECT_EQ(v_123.x, 1.0_r);
  EXPECT_EQ(v_123.y, 2.0_r);
  EXPECT_EQ(v_123.z, 3.0_r);

  Vector2 v2(1.0_r, 2.0_r);
  Vector3 from2(v2);
  EXPECT_EQ(from2.x, 1.0_r);
  EXPECT_EQ(from2.y, 2.0_r);
  EXPECT_EQ(from2.z, 0.0_r);

  Vector4 v4(1.0_r, 2.0_r, 3.0_r, 4.0_r);
  Vector3 from4(v4);
  EXPECT_EQ(from4.x, 1.0_r);
  EXPECT_EQ(from4.y, 2.0_r);
  EXPECT_EQ(from4.z, 3.0_r);
}

TEST_F(Vector3Test, Assignment) {
  v_zero = v_unitx;
  EXPECT_EQ(v_zero, v_unitx);
}

TEST_F(Vector3Test, StaticConstants) {
  EXPECT_EQ(Vector3::UNIT_X, Vector3(1.0_r, 0.0_r, 0.0_r));
  EXPECT_EQ(Vector3::UNIT_Y, Vector3(0.0_r, 1.0_r, 0.0_r));
  EXPECT_EQ(Vector3::UNIT_Z, Vector3(0.0_r, 0.0_r, 1.0_r));
  EXPECT_EQ(Vector3::ZERO, Vector3(0.0_r, 0.0_r, 0.0_r));
}

TEST_F(Vector3Test, Addition) {
  EXPECT_EQ(v_123 + v_345, Vector3(4.0_r, 6.0_r, 8.0_r));
  v_123 += v_345;
  EXPECT_EQ(v_123, Vector3(4.0_r, 6.0_r, 8.0_r));
}

TEST_F(Vector3Test, Subtraction) {
  EXPECT_EQ(v_345 - v_123, Vector3(2.0_r, 2.0_r, 2.0_r));
  v_345 -= v_123;
  EXPECT_EQ(v_345, Vector3(2.0_r, 2.0_r, 2.0_r));
}

TEST_F(Vector3Test, Multiplication) {
  EXPECT_EQ(v_123 * v_345, Vector3(3.0_r, 8.0_r, 15.0_r));
  v_123 *= v_345;
  EXPECT_EQ(v_123, Vector3(3.0_r, 8.0_r, 15.0_r));
}

TEST_F(Vector3Test, Division) {
  EXPECT_EQ(v_345 / v_123, Vector3(3.0_r, 2.0_r, 5.0_r / 3.0_r));
  v_345 /= v_123;
  EXPECT_EQ(v_345, Vector3(3.0_r, 2.0_r, 5.0_r / 3.0_r));

  Vector3 v(1.0_r, 2.0_r, 3.0_r);
  v /= v_zero;
  EXPECT_TRUE(std::isinf(v.x));
  EXPECT_TRUE(std::isinf(v.y));
  EXPECT_TRUE(std::isinf(v.z));
}

TEST_F(Vector3Test, ScalarMultiplication) {
  EXPECT_EQ(v_123 * 2.0_r, Vector3(2.0_r, 4.0_r, 6.0_r));
  EXPECT_EQ(2.0_r * v_123, Vector3(2.0_r, 4.0_r, 6.0_r));
  v_123 *= 2.0_r;
  EXPECT_EQ(v_123, Vector3(2.0_r, 4.0_r, 6.0_r));
}

TEST_F(Vector3Test, ScalarDivision) {
  EXPECT_EQ(v_345 / 2.0_r, Vector3(1.5_r, 2.0_r, 2.5_r));
  v_345 /= 2.0_r;
  EXPECT_EQ(v_345, Vector3(1.5_r, 2.0_r, 2.5_r));

  v_345 /= 0.0_r;
  EXPECT_TRUE(std::isinf(v_345.x));
  EXPECT_TRUE(std::isinf(v_345.y));
  EXPECT_TRUE(std::isinf(v_345.z));
}

TEST_F(Vector3Test, UnaryMinus) {
  EXPECT_EQ(-v_123, Vector3(-1.0_r, -2.0_r, -3.0_r));
}

TEST_F(Vector3Test, ComparisonOperators) {
  Vector3 v_eq(1.0_r, 2.0_r, 3.0_r);

  EXPECT_TRUE(v_123 == v_eq);
  EXPECT_FALSE(v_123 != v_eq);
  EXPECT_TRUE(v_123 != v_345);

  EXPECT_TRUE(v_123 < v_345);
  EXPECT_TRUE(v_123 <= v_345);
  EXPECT_TRUE(v_123 > v_unitx);
  EXPECT_TRUE(v_123 >= v_unitx);
}

TEST_F(Vector3Test, ApproximateEqualityCheck) {
  EXPECT_TRUE(v_123.IsEqualApprox(v_123_a));
  EXPECT_FALSE(v_123.IsNotEqualApprox(v_123_a));
}

TEST_F(Vector3Test, DotProduct) {
  EXPECT_NEAR(v_zero.Dot(v_unitx), 0.0_r, math::EPSILON_CMP);
  EXPECT_NEAR(v_123.Dot(v_345), 26.0_r, math::EPSILON_CMP);  // 1*3 + 2*4 + 3*5
}

TEST_F(Vector3Test, CrossProduct) {
  EXPECT_TRUE(v_zero.Cross(v_123).IsEqualApprox(Vector3(0.0_r, 0.0_r, 0.0_r)));
  EXPECT_TRUE(
      v_unitx.Cross(v_unitx).IsEqualApprox(Vector3(0.0_r, 0.0_r, 0.0_r)));
  EXPECT_TRUE(
      v_unitx.Cross(-v_unitx).IsEqualApprox(Vector3(0.0_r, 0.0_r, 0.0_r)));
  Vector3 c = v_123.Cross(v_345);
  EXPECT_TRUE(c.IsEqualApprox(Vector3(-2.0_r, 4.0_r, -2.0_r)));
  EXPECT_TRUE(
      v_unitx.Cross(v_unity).IsEqualApprox(Vector3(0.0_r, 0.0_r, 1.0_r)));
  EXPECT_TRUE(
      v_unity.Cross(v_unitx).IsEqualApprox(Vector3(0.0_r, 0.0_r, -1.0_r)));
}

TEST_F(Vector3Test, Magnitude) {
  EXPECT_NEAR(v_345.Magnitude(), std::sqrt(50.0_r), math::EPSILON_CMP);
  EXPECT_NEAR(v_345.SqrdMagnitude(), 50.0_r, math::EPSILON_CMP);
}

TEST_F(Vector3Test, Normalization) {
  Vector3 n = v_345.Normalized();
  EXPECT_NEAR(n.Magnitude(), 1.0_r, math::EPSILON_CMP);

  v_345.Normalize();
  EXPECT_NEAR(v_345.Magnitude(), 1.0_r, math::EPSILON_CMP);

  v_0p1_0p2_0p3.Normalize();
  EXPECT_NEAR(v_0p1_0p2_0p3.Magnitude(), 1.0_r, math::EPSILON_CMP);
}

TEST_F(Vector3Test, NormalizationOfZeroVector) {
  Vector3 n = v_zero.Normalized();
  EXPECT_TRUE(std::isnan(n.x));
  EXPECT_TRUE(std::isnan(n.y));
  EXPECT_TRUE(std::isnan(n.z));

  v_zero.Normalize();
  EXPECT_TRUE(std::isnan(v_zero.x));
  EXPECT_TRUE(std::isnan(v_zero.y));
  EXPECT_TRUE(std::isnan(v_zero.z));
}

TEST_F(Vector3Test, UnitChecks) {
  EXPECT_TRUE(v_unitx.IsUnit());
  EXPECT_TRUE(v_unitx.IsUnitApprox());

  Vector3 near_unit(0.9999_r, 0.0_r, 0.0_r);
  EXPECT_FALSE(near_unit.IsUnit());
  EXPECT_TRUE(near_unit.IsUnitApprox());
}

TEST_F(Vector3Test, ProjectionGeneralCase) {
  Vector3 onto(2.0_r, 5.0_r, 1.0_r);
  real dot = v_345.Dot(onto);
  real denom = onto.SqrdMagnitude();
  Vector3 expected = (dot / denom) * onto;

  Vector3 projected = v_345.Projected(onto);
  EXPECT_TRUE(projected.IsEqualApprox(expected));

  v_345.Project(onto);
  EXPECT_TRUE(v_345.IsEqualApprox(expected));
}

TEST_F(Vector3Test, ProjectionParallelAndPerpendicularCases) {
  Vector3 a = 2.0_r * v_123;
  Vector3 p = a.Projected(v_123);
  EXPECT_TRUE(p.IsEqualApprox(a));

  Vector3 proj = v_unitx.Projected(v_unity);
  EXPECT_TRUE(proj.IsEqualApprox(Vector3::ZERO));
}

TEST_F(Vector3Test, ProjectionHandlesNormalizationProperly) {
  Vector3 onto1(0.0_r, 1.0_r, 0.0_r);
  Vector3 onto2(0.0_r, 10.0_r, 0.0_r);

  Vector3 p1 = v_345.Projected(onto1);
  Vector3 p2 = v_345.Projected(onto2);

  EXPECT_TRUE(p1.IsEqualApprox(p2));
}

TEST_F(Vector3Test, FiniteCheck) {
  Vector3 v(1.0_r, std::numeric_limits<real>::infinity(), 3.0_r);
  EXPECT_FALSE(v.IsFinite());

  Vector3 u(1.0_r, 2.0_r, 3.0_r);
  EXPECT_TRUE(u.IsFinite());
}

TEST_F(Vector3Test, ToCartesian) {
  Vector2 c = v_123.ToCartesian();
  EXPECT_TRUE(c.IsEqualApprox(Vector2(1.0_r / 3.0_r, 2.0_r / 3.0_r)));

  Vector3 v = Vector3(3.0_r, 4.0_r, 0.0_r);
  c = v.ToCartesian();
  EXPECT_TRUE(std::isinf(c.x));
  EXPECT_TRUE(std::isinf(c.y));
}

TEST_F(Vector3Test, ToHomogeneous) {
  EXPECT_EQ(v_123.ToHomogeneous(), Vector4(1.0_r, 2.0_r, 3.0_r, 1.0_r));
}
