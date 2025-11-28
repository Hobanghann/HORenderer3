#include <gtest/gtest.h>

#include "core/math/math_funcs.h"
#include "core/math/vector3.h"

using namespace ho;
using namespace ho::math;

TEST(MathFuncsTest, Max) {
  EXPECT_EQ(Max(3.f, 5.f), 5.f);
  EXPECT_EQ(Max(-1, -2), -1);
  EXPECT_EQ(Max(1.0, 2.0, 3.0), 3.0);
}

TEST(MathFuncsTest, Min) {
  EXPECT_EQ(Min(3.f, 5.f), 3.f);
  EXPECT_EQ(Min(-1, -2), -2);
  EXPECT_EQ(Min(3.0, 2.0, 1.0), 1.0);
}

TEST(MathFuncsTest, Clamp) {
  EXPECT_EQ(Clamp(5.f, 1.f, 10.f), 5.f);
  EXPECT_EQ(Clamp(-1, 0, 10), 0);
  EXPECT_EQ(Clamp(20.0, 0.0, 10.0), 10.0);
}

TEST(MathFuncsTest, Fmod) {
  EXPECT_FLOAT_EQ(Fmod(5.5_r, 2.0_r), 1.5_r);
  EXPECT_FLOAT_EQ(Fmod(-5.5_r, 2.0_r), -1.5_r);
  EXPECT_FLOAT_EQ(Fmod(5.5_r, -2.0_r), 1.5_r);  
  EXPECT_FLOAT_EQ(Fmod(-5.5_r, -2.0_r), -1.5_r);
  EXPECT_FLOAT_EQ(Fmod(5.5_r, 0._r), 0.0_r);
}

TEST(MathFuncsTest, Round) {
  EXPECT_FLOAT_EQ(Round(3.0_r), 3.0_r);
  EXPECT_FLOAT_EQ(Round(2.6_r), 3.0_r);
  EXPECT_FLOAT_EQ(Round(2.4_r), 2.0_r);
  EXPECT_FLOAT_EQ(Round(2.0_r), 2.0_r);
  EXPECT_FLOAT_EQ(Round(-3.0_r), -3.0_r);
  EXPECT_FLOAT_EQ(Round(-2.6_r), -3.0_r);
  EXPECT_FLOAT_EQ(Round(-2.4_r), -2.0_r);
  EXPECT_FLOAT_EQ(Round(-2.0_r), -2.0_r);
}

TEST(MathFuncsTest, Floor) {
  EXPECT_FLOAT_EQ(Floor(3.0_r), 3.0_r);
  EXPECT_FLOAT_EQ(Floor(2.9_r), 2.0_r);
  EXPECT_FLOAT_EQ(Floor(2.0_r), 2.0_r);
  EXPECT_FLOAT_EQ(Floor(-3.0_r), -3.0_r);
  EXPECT_FLOAT_EQ(Floor(-2.9_r), -3.0_r);
  EXPECT_FLOAT_EQ(Floor(-2.0_r), -2.0_r);
}

TEST(MathFuncsTest, Ceil) {
  EXPECT_FLOAT_EQ(Ceil(3.0_r), 3.0_r);
  EXPECT_FLOAT_EQ(Ceil(2.1_r), 3.0_r);
  EXPECT_FLOAT_EQ(Ceil(2.0_r), 2.0_r);
  EXPECT_FLOAT_EQ(Ceil(-3.0_r), -3.0_r);
  EXPECT_FLOAT_EQ(Ceil(-2.1_r), -2.0_r);
  EXPECT_FLOAT_EQ(Ceil(-2.0_r), -2.0_r);
}

TEST(MathFuncsTest, Abs) {
  EXPECT_FLOAT_EQ(Abs(0.0_r), 0.0_r);
  EXPECT_FLOAT_EQ(Abs(5.0_r), 5.0_r);
  EXPECT_FLOAT_EQ(Abs(-5.0_r), 5.0_r);
}

TEST(MathFuncsTest, IsEqualApprox) {
  EXPECT_TRUE(IsEqualApprox(1.0000001_r, 1.0000002_r, 0.000001_r));
  EXPECT_FALSE(IsEqualApprox(1.0_r, 1.1_r, 0.000001_r));

  EXPECT_TRUE(IsNotEqualApprox(1.0_r, 2.0_r));
  EXPECT_FALSE(IsNotEqualApprox(1.0_r, 1.0_r));
}

TEST(MathFuncsTest, IsZeroApprox) {
  EXPECT_TRUE(IsZeroApprox(0.0_r));
  EXPECT_TRUE(IsZeroApprox(1e-10_r));
  EXPECT_FALSE(IsZeroApprox(1e-2_r));
}

TEST(MathFuncsTest, IsFinite) {
  EXPECT_TRUE(IsFinite(0.0_r));
  EXPECT_TRUE(IsFinite(123.456_r));
  EXPECT_TRUE(IsFinite(-9999.99_r));
  EXPECT_TRUE(IsFinite(math::REAL_MAX));
  EXPECT_TRUE(IsFinite(math::REAL_MIN));

  EXPECT_FALSE(IsFinite(math::REAL_INF));
  EXPECT_FALSE(IsFinite(-math::REAL_INF));
  EXPECT_FALSE(IsFinite(math::REAL_NaN));
}

TEST(MathFuncsTest, IsInf) {
  EXPECT_TRUE(IsInf(math::REAL_INF));
  EXPECT_TRUE(IsInf(-math::REAL_INF));

  EXPECT_FALSE(IsInf(0.0_r));
  EXPECT_FALSE(IsInf(1.0_r));
  EXPECT_FALSE(IsInf(math::REAL_NaN));
  EXPECT_FALSE(IsInf(math::REAL_MAX));
  EXPECT_FALSE(IsInf(math::REAL_MIN));
}

TEST(MathFuncsTest, IsNaN) {
  EXPECT_TRUE(IsNaN(math::REAL_NaN));

  EXPECT_FALSE(IsNaN(0.0_r));
  EXPECT_FALSE(IsNaN(123.45_r));
  EXPECT_FALSE(IsNaN(math::REAL_INF));
  EXPECT_FALSE(IsNaN(-math::REAL_INF));
}

TEST(MathFuncsTest, Pow) {
  EXPECT_FLOAT_EQ(Pow(2.0_r, 3.0_r), 8.0_r);
  EXPECT_FLOAT_EQ(Pow(4.0_r, 0.5_r), 2.0_r);
  EXPECT_FLOAT_EQ(Pow(4.0_r, -1.0_r), 0.25_r);
  EXPECT_FLOAT_EQ(Pow(4.0_r, 0.0_r), 1.0_r);
  EXPECT_FLOAT_EQ(Pow(-2.0_r, 3.0_r), -8.0_r);
  EXPECT_FLOAT_EQ(Pow(-2.0_r, 4.0_r), 16.0_r);
  EXPECT_TRUE(std::isnan(Pow(-2.0_r, 0.5_r)));
  EXPECT_TRUE(std::isinf(Pow(0.0_r, -1.0_r)));
  EXPECT_FLOAT_EQ(Pow(-3.5_r, 0.0_r), 1.0_r);
}

TEST(MathFuncsTest, FastPow) {
  EXPECT_FLOAT_EQ(PowFast(2.0_r, 3), 8.0_r);
  EXPECT_FLOAT_EQ(PowFast(2.0_r, -3), 0.125_r);
  EXPECT_FLOAT_EQ(PowFast(5.0_r, 0), 1.0_r);
  EXPECT_FLOAT_EQ(PowFast(-2.0_r, 3), -8.0_r);
  EXPECT_FLOAT_EQ(PowFast(-2.0_r, 4), 16.0_r);
  EXPECT_TRUE(std::isinf(PowFast(0.0_r, -1)));
  EXPECT_FLOAT_EQ(PowFast(-3.5_r, 0), 1.0_r);
}

TEST(MathFuncsTest, FastInvSqrtAndFastSqrt) {
  float f = 4.0f;

  EXPECT_NEAR(InvSqrtFast(f), 1.0f / std::sqrt(f), math::EPSILON_CMP);
  EXPECT_TRUE(std::isinf(InvSqrtFast(0.f)));
  EXPECT_TRUE(std::isnan(InvSqrtFast(-1.f)));

  EXPECT_NEAR(SqrtFast(f), 2.0f, math::EPSILON_CMP);
  EXPECT_TRUE(std::isnan(SqrtFast(0.f)));
  EXPECT_TRUE(std::isnan(SqrtFast(-1.f)));

  EXPECT_FLOAT_EQ(Sqrt(9.0f), 3.0f);
}

TEST(MathFuncsTest, TrigonometricFunctions) {
  real rad = PI / 4.0f;
  EXPECT_NEAR(Sin(rad), std::sqrt(2.0f) / 2.0f, math::EPSILON_CMP);
  EXPECT_NEAR(Cos(rad), std::sqrt(2.0f) / 2.0f, math::EPSILON_CMP);
  EXPECT_NEAR(Tan(rad), 1.0f, math::EPSILON_CMP);

  float s, c;
  SinCos(s, c, static_cast<float>(PI / 4.0f));
  EXPECT_NEAR(s, std::sqrt(2.0f) / 2.0f, math::EPSILON_CMP);
  EXPECT_NEAR(c, std::sqrt(2.0f) / 2.0f, math::EPSILON_CMP);
}

TEST(MathFuncsTest, AngleConversions) {
  EXPECT_FLOAT_EQ(DegToRad(180.0f), PI);
  EXPECT_FLOAT_EQ(DegToRad(45.0f), PI / 4.f);
  EXPECT_FLOAT_EQ(RadToDeg(PI), 180.0f);
  EXPECT_FLOAT_EQ(RadToDeg(PI / 4.f), 45.0f);
}

TEST(MathFuncsTest, DistanceFunctions) {
  Vector3 a(0.f, 0.f, 0.f);
  Vector3 b(3.f, 4.f, 0.f);
  EXPECT_FLOAT_EQ(Distance(a, a), 0.f);
  EXPECT_FLOAT_EQ(Distance(a, b), 5.f);
  EXPECT_FLOAT_EQ(SqrdDistance(a, a), 0.f);
  EXPECT_FLOAT_EQ(SqrdDistance(a, b), 25.f);
}
