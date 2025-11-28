#include <gtest/gtest.h>

#include <limits>

#include "core/math/quaternion.h"
#include "core/math/vector2.h"
#include "core/math/vector3.h"
#include "core/math/vector4.h"

using namespace ho;

class Vector4Test : public ::testing::Test {
 protected:
  void SetUp() override {
    v_zero = Vector4();
    v_unitx = Vector4(1.0_r, 0.0_r, 0.0_r, 0.0_r);
    v_unity = Vector4(0.0_r, 1.0_r, 0.0_r, 0.0_r);
    v_unitz = Vector4(0.0_r, 0.0_r, 1.0_r, 0.0_r);
    v_unitw = Vector4(0.0_r, 0.0_r, 0.0_r, 1.0_r);
    v_1234 = Vector4(1.0_r, 2.0_r, 3.0_r, 4.0_r);
    v_3456 = Vector4(3.0_r, 4.0_r, 5.0_r, 6.0_r);
    v_1234_a = Vector4(1.0000001_r, 2.0000001_r, 3.0000001_r, 4.0000001_r);
    v_0p1_0p2_0p3_0p4 = Vector4(0.1_r, 0.2_r, 0.3_r, 0.4_r);
  }

 protected:
  Vector4 v_zero, v_unitx, v_unity, v_unitz, v_unitw;
  Vector4 v_1234, v_3456, v_1234_a, v_0p1_0p2_0p3_0p4;
};

TEST_F(Vector4Test, Constructors) {
  EXPECT_EQ(v_zero.x, 0.0_r);
  EXPECT_EQ(v_zero.y, 0.0_r);
  EXPECT_EQ(v_zero.z, 0.0_r);
  EXPECT_EQ(v_zero.w, 0.0_r);

  EXPECT_EQ(v_1234.x, 1.0_r);
  EXPECT_EQ(v_1234.y, 2.0_r);
  EXPECT_EQ(v_1234.z, 3.0_r);
  EXPECT_EQ(v_1234.w, 4.0_r);

  Vector2 v2(1.0_r, 2.0_r);
  Vector4 from2(v2);
  EXPECT_EQ(from2.x, 1.0_r);
  EXPECT_EQ(from2.y, 2.0_r);
  EXPECT_EQ(from2.z, 0.0_r);
  EXPECT_EQ(from2.w, 0.0_r);

  Vector3 v3(1.0_r, 2.0_r, 3.0_r);
  Vector4 from3(v3);
  EXPECT_EQ(from3.x, 1.0_r);
  EXPECT_EQ(from3.y, 2.0_r);
  EXPECT_EQ(from3.z, 3.0_r);
  EXPECT_EQ(from3.w, 0.0_r);

  Quaternion q(1.0_r, 2.0_r, 3.0_r, 4.0_r);
  Vector4 from_q(q);
  EXPECT_EQ(from_q.x, 1.0_r);
  EXPECT_EQ(from_q.y, 2.0_r);
  EXPECT_EQ(from_q.z, 3.0_r);
  EXPECT_EQ(from_q.w, 4.0_r);
}

TEST_F(Vector4Test, Assignment) {
  v_zero = v_unitx;
  EXPECT_EQ(v_zero, v_unitx);
}

TEST_F(Vector4Test, StaticConstants) {
  EXPECT_EQ(Vector4::UNIT_X, Vector4(1.0_r, 0.0_r, 0.0_r, 0.0_r));
  EXPECT_EQ(Vector4::UNIT_Y, Vector4(0.0_r, 1.0_r, 0.0_r, 0.0_r));
  EXPECT_EQ(Vector4::UNIT_Z, Vector4(0.0_r, 0.0_r, 1.0_r, 0.0_r));
  EXPECT_EQ(Vector4::UNIT_W, Vector4(0.0_r, 0.0_r, 0.0_r, 1.0_r));
  EXPECT_EQ(Vector4::ZERO, Vector4(0.0_r, 0.0_r, 0.0_r, 0.0_r));
}

TEST_F(Vector4Test, Addition) {
  EXPECT_EQ(v_1234 + v_3456, Vector4(4.0_r, 6.0_r, 8.0_r, 10.0_r));
  v_1234 += v_3456;
  EXPECT_EQ(v_1234, Vector4(4.0_r, 6.0_r, 8.0_r, 10.0_r));
}

TEST_F(Vector4Test, Subtraction) {
  EXPECT_EQ(v_3456 - v_1234, Vector4(2.0_r, 2.0_r, 2.0_r, 2.0_r));
  v_3456 -= v_1234;
  EXPECT_EQ(v_3456, Vector4(2.0_r, 2.0_r, 2.0_r, 2.0_r));
}

TEST_F(Vector4Test, Multiplication) {
  EXPECT_EQ(v_1234 * v_3456, Vector4(3.0_r, 8.0_r, 15.0_r, 24.0_r));
  v_1234 *= v_3456;
  EXPECT_EQ(v_1234, Vector4(3.0_r, 8.0_r, 15.0_r, 24.0_r));
}

TEST_F(Vector4Test, Division) {
  EXPECT_EQ(v_3456 / v_1234, Vector4(3.0_r, 2.0_r, 5.0_r / 3.0_r, 1.5_r));
  v_3456 /= v_1234;
  EXPECT_EQ(v_3456, Vector4(3.0_r, 2.0_r, 5.0_r / 3.0_r, 1.5_r));

  Vector4 v(1.0_r, 2.0_r, 3.0_r, 4.0_r);
  v /= v_zero;
  EXPECT_TRUE(std::isinf(v.x));
  EXPECT_TRUE(std::isinf(v.y));
  EXPECT_TRUE(std::isinf(v.z));
  EXPECT_TRUE(std::isinf(v.w));
}

TEST_F(Vector4Test, ScalarMultiplication) {
  EXPECT_EQ(v_1234 * 2.0_r, Vector4(2.0_r, 4.0_r, 6.0_r, 8.0_r));
  EXPECT_EQ(2.0_r * v_1234, Vector4(2.0_r, 4.0_r, 6.0_r, 8.0_r));
  v_1234 *= 2.0_r;
  EXPECT_EQ(v_1234, Vector4(2.0_r, 4.0_r, 6.0_r, 8.0_r));
}

TEST_F(Vector4Test, ScalarDivision) {
  EXPECT_EQ(v_3456 / 2.0_r, Vector4(1.5_r, 2.0_r, 2.5_r, 3.0_r));
  v_3456 /= 2.0_r;
  EXPECT_EQ(v_3456, Vector4(1.5_r, 2.0_r, 2.5_r, 3.0_r));

  v_3456 /= 0.0_r;
  EXPECT_TRUE(std::isinf(v_3456.x));
  EXPECT_TRUE(std::isinf(v_3456.y));
  EXPECT_TRUE(std::isinf(v_3456.z));
  EXPECT_TRUE(std::isinf(v_3456.w));
}

TEST_F(Vector4Test, UnaryMinus) {
  EXPECT_EQ(-v_1234, Vector4(-1.0_r, -2.0_r, -3.0_r, -4.0_r));
}

TEST_F(Vector4Test, ComparisonOperators) {
  Vector4 v_eq(1.0_r, 2.0_r, 3.0_r, 4.0_r);

  EXPECT_TRUE(v_1234 == v_eq);
  EXPECT_FALSE(v_1234 != v_eq);
  EXPECT_TRUE(v_1234 != v_3456);

  EXPECT_TRUE(v_1234 < v_3456);
  EXPECT_TRUE(v_1234 <= v_3456);
  EXPECT_TRUE(v_1234 > v_unitx);
  EXPECT_TRUE(v_1234 >= v_unitx);
}

TEST_F(Vector4Test, ApproximateEqualityCheck) {
  EXPECT_TRUE(v_1234.IsEqualApprox(v_1234_a));
  EXPECT_FALSE(v_1234.IsNotEqualApprox(v_1234_a));
}

TEST_F(Vector4Test, DotProduct) {
  EXPECT_NEAR(v_zero.Dot(v_unitx), 0.0_r, math::EPSILON_CMP);
  EXPECT_NEAR(v_1234.Dot(v_3456), 50.0_r,
              math::EPSILON_CMP);  // 1*3+2*4+3*5+4*6
}

TEST_F(Vector4Test, Magnitude) {
  EXPECT_NEAR(v_3456.Magnitude(), std::sqrt(86.0_r), math::EPSILON_CMP);
  EXPECT_NEAR(v_3456.SqrdMagnitude(), 86.0_r, math::EPSILON_CMP);
}

TEST_F(Vector4Test, Normalization) {
  Vector4 n = v_3456.Normalized();
  EXPECT_NEAR(n.Magnitude(), 1.0_r, math::EPSILON_CMP);

  v_3456.Normalize();
  EXPECT_NEAR(v_3456.Magnitude(), 1.0_r, math::EPSILON_CMP);

  v_0p1_0p2_0p3_0p4.Normalize();
  EXPECT_NEAR(v_0p1_0p2_0p3_0p4.Magnitude(), 1.0_r, math::EPSILON_CMP);
}

TEST_F(Vector4Test, NormalizationOfZeroVector) {
  Vector4 n = v_zero.Normalized();
  EXPECT_TRUE(std::isnan(n.x));
  EXPECT_TRUE(std::isnan(n.y));
  EXPECT_TRUE(std::isnan(n.z));
  EXPECT_TRUE(std::isnan(n.w));

  v_zero.Normalize();
  EXPECT_TRUE(std::isnan(v_zero.x));
  EXPECT_TRUE(std::isnan(v_zero.y));
  EXPECT_TRUE(std::isnan(v_zero.z));
  EXPECT_TRUE(std::isnan(v_zero.w));
}

TEST_F(Vector4Test, UnitChecks) {
  EXPECT_TRUE(v_unitx.IsUnit());
  EXPECT_TRUE(v_unitx.IsUnitApprox());

  Vector4 near_unit(0.9999_r, 0.0_r, 0.0_r, 0.0_r);
  EXPECT_FALSE(near_unit.IsUnit());
  EXPECT_TRUE(near_unit.IsUnitApprox());
}

TEST_F(Vector4Test, ProjectionGeneralCase) {
  Vector4 onto(2.0_r, 5.0_r, 1.0_r, 3.0_r);
  real dot = v_3456.Dot(onto);
  real denom = onto.SqrdMagnitude();
  Vector4 expected = (dot / denom) * onto;

  Vector4 projected = v_3456.Projected(onto);
  EXPECT_TRUE(projected.IsEqualApprox(expected));

  v_3456.Project(onto);
  EXPECT_TRUE(v_3456.IsEqualApprox(expected));
}

TEST_F(Vector4Test, ProjectionParallelAndPerpendicularCases) {
  Vector4 a = 2.0_r * v_1234;
  Vector4 p = a.Projected(v_1234);
  EXPECT_TRUE(p.IsEqualApprox(a));

  Vector4 proj = v_unitx.Projected(v_unity);
  EXPECT_TRUE(proj.IsEqualApprox(Vector4::ZERO));
}

TEST_F(Vector4Test, ProjectionHandlesNormalizationProperly) {
  Vector4 onto1(0.0_r, 1.0_r, 0.0_r, 0.0_r);
  Vector4 onto2(0.0_r, 10.0_r, 0.0_r, 0.0_r);

  Vector4 p1 = v_3456.Projected(onto1);
  Vector4 p2 = v_3456.Projected(onto2);

  EXPECT_TRUE(p1.IsEqualApprox(p2));
}

TEST_F(Vector4Test, FiniteCheck) {
  Vector4 v(1.0_r, std::numeric_limits<real>::infinity(), 3.0_r, 4.0_r);
  EXPECT_FALSE(v.IsFinite());

  Vector4 u(1.0_r, 2.0_r, 3.0_r, 4.0_r);
  EXPECT_TRUE(u.IsFinite());
}

TEST_F(Vector4Test, ToCartesian) {
  Vector3 c = v_1234.ToCartesian();

  EXPECT_TRUE(
      c.IsEqualApprox(Vector3(1.0_r / 4.0_r, 2.0_r / 4.0_r, 3.0_r / 4.0_r)));

  Vector4 v(3.0_r, 4.0_r, 5.0_r, 0.0_r);
  c = v.ToCartesian();
  EXPECT_TRUE(std::isinf(c.x));
  EXPECT_TRUE(std::isinf(c.y));
  EXPECT_TRUE(std::isinf(c.z));
}
