#include <gtest/gtest.h>

#include "core/math/vector2.h"
#include "core/math/vector3.h"
#include "core/math/vector4.h"

using namespace ho;

class Vector2Test : public ::testing::Test {
 protected:
  void SetUp() override {
    v_zero = Vector2();
    v_unitx = Vector2(1.0_r, 0.0_r);
    v_unitx_a = Vector2(0.99999f, 0.f);
    v_unity = Vector2(0.0_r, 1.0_r);
    v_12 = Vector2(1.0_r, 2.0_r);
    v_12_a = Vector2(1.0000001_r, 2.0000001_r);
    v_34 = Vector2(3.0_r, 4.0_r);
    v_0p1_0p2 = Vector2(0.1_r, 0.2_r);
  }

 protected:
  Vector2 v_zero, v_unitx, v_unitx_a, v_unity;
  Vector2 v_12, v_12_a, v_34, v_0p1_0p2;
};

TEST_F(Vector2Test, Constructors) {
  EXPECT_EQ(v_zero.x, 0.0_r);
  EXPECT_EQ(v_zero.y, 0.0_r);

  EXPECT_EQ(v_12.x, 1.0_r);
  EXPECT_EQ(v_12.y, 2.0_r);

  Vector3 v2_3(1.0_r, 2.0_r, 3.0_r);
  Vector2 v2_from_3(v2_3);
  EXPECT_EQ(v2_from_3.x, 1.0_r);
  EXPECT_EQ(v2_from_3.y, 2.0_r);

  Vector4 v2_4(1.0_r, 2.0_r, 3.0_r, 4.0_r);
  Vector2 v2_from_4(v2_4);
  EXPECT_EQ(v2_from_4.x, 1.0_r);
  EXPECT_EQ(v2_from_4.y, 2.0_r);
}

TEST_F(Vector2Test, Assignment) {
  v_zero = v_unitx;
  EXPECT_EQ(v_zero, v_unitx);
}

TEST_F(Vector2Test, StaticConstants) {
  EXPECT_EQ(Vector2::UNIT_X, Vector2(1.0_r, 0.0_r));
  EXPECT_EQ(Vector2::UNIT_Y, Vector2(0.0_r, 1.0_r));
  EXPECT_EQ(Vector2::ZERO, Vector2(0.0_r, 0.0_r));
}

TEST_F(Vector2Test, Addition) {
  EXPECT_EQ(v_12 + v_34, Vector2(4.0_r, 6.0_r));

  v_12 += v_34;
  EXPECT_EQ(v_12, Vector2(4.0_r, 6.0_r));
}

TEST_F(Vector2Test, Subtraction) {
  EXPECT_EQ(v_34 - v_12, Vector2(2.0_r, 2.0_r));

  v_34 -= v_12;
  EXPECT_EQ(v_34, Vector2(2.0_r, 2.0_r));
}

TEST_F(Vector2Test, Multiplication) {
  EXPECT_EQ(v_12 * v_34, Vector2(3.0_r, 8.0_r));

  v_12 *= v_34;
  EXPECT_EQ(v_12, Vector2(3.0_r, 8.0_r));
}

TEST_F(Vector2Test, Division) {
  EXPECT_TRUE((v_34 / v_12).IsEqualApprox(Vector2(3.0_r, 2.0_r)));

  v_34 /= v_12;
  EXPECT_TRUE(v_34.IsEqualApprox(Vector2(3.0_r, 2.0_r)));

  v_34 /= v_zero;
  EXPECT_TRUE(std::isinf(v_34.x));
  EXPECT_TRUE(std::isinf(v_34.y));
}

TEST_F(Vector2Test, ScalarMultiplication) {
  EXPECT_EQ(v_12 * 2.0_r, Vector2(2.0_r, 4.0_r));
  EXPECT_EQ(2.0_r * v_12, Vector2(2.0_r, 4.0_r));

  v_12 *= 2.0_r;
  EXPECT_EQ(v_12, Vector2(2.0_r, 4.0_r));
}

TEST_F(Vector2Test, ScalarDivision) {
  EXPECT_EQ(v_34 / 2.0_r, Vector2(1.5_r, 2.0_r));

  v_34 /= 2.0_r;
  EXPECT_EQ(v_34, Vector2(1.5_r, 2.0_r));

  v_34 /= 0.0_r;
  EXPECT_TRUE(std::isinf(v_34.x));
  EXPECT_TRUE(std::isinf(v_34.y));
}

TEST_F(Vector2Test, UnaryMinus) { EXPECT_EQ(-v_12, Vector2(-1.0_r, -2.0_r)); }

TEST_F(Vector2Test, ComparisonOperators) {
  Vector2 v2e(1.0_r, 2.0_r);

  EXPECT_TRUE(v_12 == v2e);
  EXPECT_FALSE(v_12 != v2e);
  EXPECT_TRUE(v_12 != v_34);

  EXPECT_TRUE(v_12 < v_34);
  EXPECT_TRUE(v_12 <= v_34);
  EXPECT_TRUE(v_12 > v_unitx);
  EXPECT_TRUE(v_12 >= v_unitx);
}

TEST_F(Vector2Test, ApproximateEqualityCheck) {
  EXPECT_TRUE(v_12.IsEqualApprox(v_12_a));
  EXPECT_FALSE(v_12.IsNotEqualApprox(v_12_a));
}

TEST_F(Vector2Test, DotProduct) {
  EXPECT_NEAR(v_zero.Dot(v_unitx), 0.0_r, math::EPSILON_CMP);
  EXPECT_NEAR(v_12.Dot(v_34), 11.0_r, math::EPSILON_CMP);
}

TEST_F(Vector2Test, CrossProduct) {
  EXPECT_NEAR(v_zero.Cross(v_unitx), 0.0_r, math::EPSILON_CMP);
  EXPECT_NEAR(v_unitx.Cross(v_unitx), 0.0_r, math::EPSILON_CMP);
  EXPECT_NEAR(-v_unitx.Cross(v_unitx), 0.0_r, math::EPSILON_CMP);
  EXPECT_NEAR(v_12.Cross(v_34), -2.0_r, math::EPSILON_CMP);
}

TEST_F(Vector2Test, Magnitude) {
  EXPECT_NEAR(v_34.Magnitude(), 5.0_r, math::EPSILON_CMP);
  EXPECT_NEAR(v_34.SqrdMagnitude(), 25.0_r, math::EPSILON_CMP);
}

TEST_F(Vector2Test, Normalization) {
  Vector2 n = v_34.Normalized();
  EXPECT_NEAR(n.Magnitude(), 1.0_r, math::EPSILON_CMP);
  EXPECT_NEAR(n.x, 0.6_r, math::EPSILON_CMP);
  EXPECT_NEAR(n.y, 0.8_r, math::EPSILON_CMP);

  v_34.Normalize();
  EXPECT_NEAR(v_34.Magnitude(), 1.0_r, math::EPSILON_CMP);
  EXPECT_NEAR(v_34.x, 0.6_r, math::EPSILON_CMP);
  EXPECT_NEAR(v_34.y, 0.8_r, math::EPSILON_CMP);

  v_0p1_0p2.Normalize();
  EXPECT_NEAR(v_0p1_0p2.Magnitude(), 1.0_r, math::EPSILON_CMP);
}

TEST_F(Vector2Test, NormalizationOfZeroVector) {
  Vector2 n = v_zero.Normalized();

  EXPECT_TRUE(std::isnan(n.x));
  EXPECT_TRUE(std::isnan(n.y));

  v_zero.Normalize();
  EXPECT_TRUE(std::isnan(v_zero.x));
  EXPECT_TRUE(std::isnan(v_zero.y));
}

TEST_F(Vector2Test, UnitCheck) {
  EXPECT_TRUE(v_unitx.IsUnit());
  EXPECT_TRUE(v_unitx.IsUnitApprox());

  EXPECT_FALSE(v_unitx_a.IsUnit());
  EXPECT_TRUE(v_unitx_a.IsUnitApprox());
}

TEST_F(Vector2Test, ProjectionGeneralCase) {
  Vector2 onto(2.0_r, 5.0_r);

  real dot = v_34.Dot(onto);
  real denom = onto.SqrdMagnitude();
  Vector2 expected = (dot / denom) * onto;

  Vector2 projected = v_34.Projected(onto);
  EXPECT_TRUE(projected.IsEqualApprox(expected));

  v_34.Project(onto);
  EXPECT_TRUE(v_34.IsEqualApprox(expected));
}

TEST_F(Vector2Test, ProjectionParallelAndPerpendicularCases) {
  Vector2 a = 2.0_r * v_12;
  Vector2 p = a.Projected(v_12);
  EXPECT_TRUE(p.IsEqualApprox(a));

  Vector2 proj = v_unitx.Projected(v_unity);
  EXPECT_TRUE(proj.IsEqualApprox(Vector2::ZERO));
}

TEST_F(Vector2Test, ProjectionHandlesNormalizationProperly) {
  Vector2 onto1(0.0_r, 1.0_r);
  Vector2 onto2(0.0_r, 10.0_r);

  Vector2 p1 = v_34.Projected(onto1);
  Vector2 p2 = v_34.Projected(onto2);

  EXPECT_TRUE(p1.IsEqualApprox(p2));
}

TEST_F(Vector2Test, FiniteCheck) {
  Vector2 v(1.0_r, std::numeric_limits<real>::infinity());
  EXPECT_FALSE(v.IsFinite());

  Vector2 u(1.0_r, 2.0_r);
  EXPECT_TRUE(u.IsFinite());
}

TEST_F(Vector2Test, HomogeneousChecks) {
  EXPECT_EQ(v_12.ToHomogeneous(), Vector3(1.0_r, 2.0_r, 1.0_r));
}
