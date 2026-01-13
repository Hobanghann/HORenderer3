#include <gtest/gtest.h>

#include "core/math/color128.h"
#include "core/math/color32.h"
#include "core/math/vector3.h"

using namespace ho;
using namespace ho::math;

TEST(Color128Test, Constructors) {
    Color128 c;
    EXPECT_FLOAT_EQ(c.r, 0.0f);
    EXPECT_FLOAT_EQ(c.g, 0.0f);
    EXPECT_FLOAT_EQ(c.b, 0.0f);
    EXPECT_FLOAT_EQ(c.a, 1.0f);

    Color128 c1(0.5f, 0.25f, 0.75f);
    EXPECT_FLOAT_EQ(c1.r, 0.5f);
    EXPECT_FLOAT_EQ(c1.g, 0.25f);
    EXPECT_FLOAT_EQ(c1.b, 0.75f);
    EXPECT_FLOAT_EQ(c1.a, 1.0f);

    Color128 c2(1.2f, -0.5f, 2.0f, 0.0f);
    EXPECT_FLOAT_EQ(c2.r, 1.2f);
    EXPECT_FLOAT_EQ(c2.g, -0.5f);
    EXPECT_FLOAT_EQ(c2.b, 2.0f);
    EXPECT_FLOAT_EQ(c2.a, 0.0f);

    Color32 c32(128, 64, 32, 255);
    Color128 from32(c32);
    EXPECT_FLOAT_EQ(from32.r, 128.0f / 255.0f);
    EXPECT_FLOAT_EQ(from32.g, 64.0f / 255.0f);
    EXPECT_FLOAT_EQ(from32.b, 32.0f / 255.0f);
    EXPECT_FLOAT_EQ(from32.a, 255.0f / 255.0f);
}

TEST(Color128Test, Assignment) {
    Color128 c(0.1f, 0.2f, 0.3f, 0.4f);
    Color128 c1;
    c1 = c;
    EXPECT_FLOAT_EQ(c1.r, 0.1f);
    EXPECT_FLOAT_EQ(c1.g, 0.2f);
    EXPECT_FLOAT_EQ(c1.b, 0.3f);
    EXPECT_FLOAT_EQ(c1.a, 0.4f);
}

TEST(Color128Test, Addition) {
    Color128 a(0.2f, 0.2f, 0.4f, 0.5f);
    Color128 b(0.1f, 0.3f, 0.8f, 0.5f);

    Color128 sum = a + b;

    EXPECT_FLOAT_EQ(sum.r, 0.3f);
    EXPECT_FLOAT_EQ(sum.g, 0.5f);
    EXPECT_FLOAT_EQ(sum.b, 1.2f);
    EXPECT_FLOAT_EQ(sum.a, 1.0f);

    a += b;
    EXPECT_TRUE(a.IsEqualApprox(sum));
}

TEST(Color128Test, Subtraction) {
    Color128 a(0.2f, 0.2f, 0.4f, 0.5f);
    Color128 b(0.1f, 0.3f, 0.8f, 0.5f);

    Color128 diff = a - b;
    EXPECT_FLOAT_EQ(diff.r, 0.1f);
    EXPECT_FLOAT_EQ(diff.g, -0.1f);
    EXPECT_FLOAT_EQ(diff.b, -0.4f);
    EXPECT_FLOAT_EQ(diff.a, 0.0f);

    a -= b;
    EXPECT_TRUE(a.IsEqualApprox(Color128(0.1f, -0.1f, -0.4f, 0.0f)));
}

TEST(Color128Test, Multiplication) {
    Color128 a(0.2f, 0.4f, 0.6f, 0.8f);
    Color128 b(0.5f, 0.25f, 0.1f, 0.5f);

    Color128 mul = a * b;
    EXPECT_FLOAT_EQ(mul.r, 0.1f);
    EXPECT_FLOAT_EQ(mul.g, 0.1f);
    EXPECT_FLOAT_EQ(mul.b, 0.06f);
    EXPECT_FLOAT_EQ(mul.a, 0.4f);

    a *= b;
    EXPECT_TRUE(a.IsEqualApprox(mul));
}

TEST(Color128Test, Division) {
    Color128 div = Color128(0.5f, 0.6f, 0.8f, 1.0f) / Color128(0.5f, 0.2f, 0.4f, 0.0f);
    EXPECT_FLOAT_EQ(div.r, 1.0f);
    EXPECT_FLOAT_EQ(div.g, 3.0f);
    EXPECT_FLOAT_EQ(div.b, 2.0f);
    EXPECT_TRUE(std::isinf(div.a));
}

TEST(Color128Test, ScalarMultiplication) {
    Color128 a(0.1f, 0.2f, 0.3f, 0.4f);

    Color128 mul = a * 2.0f;
    EXPECT_FLOAT_EQ(mul.r, 0.2f);
    EXPECT_FLOAT_EQ(mul.g, 0.4f);
    EXPECT_FLOAT_EQ(mul.b, 0.6f);
    EXPECT_FLOAT_EQ(mul.a, 0.8f);

    a *= 10.0f;
    EXPECT_FLOAT_EQ(a.r, 1.0f);
    EXPECT_FLOAT_EQ(a.g, 2.0f);
    EXPECT_FLOAT_EQ(a.b, 3.0f);
    EXPECT_FLOAT_EQ(a.a, 4.0f);

    Color128 left = 2.0f * Color128(0.2f, 0.3f, 0.4f, 0.5f);
    EXPECT_FLOAT_EQ(left.r, 0.4f);
    EXPECT_FLOAT_EQ(left.g, 0.6f);
    EXPECT_FLOAT_EQ(left.b, 0.8f);
    EXPECT_FLOAT_EQ(left.a, 1.0f);
}

TEST(Color128Test, ScalarDivision) {
    Color128 a(0.1f, 0.2f, 0.3f, 0.4f);

    Color128 div = a / 0.5f;
    EXPECT_FLOAT_EQ(div.r, 0.2f);
    EXPECT_FLOAT_EQ(div.g, 0.4f);
    EXPECT_FLOAT_EQ(div.b, 0.6f);
    EXPECT_FLOAT_EQ(div.a, 0.8f);

    div = a / 0.0f;
    EXPECT_TRUE(std::isinf(div.r));
    EXPECT_TRUE(std::isinf(div.g));
    EXPECT_TRUE(std::isinf(div.b));
    EXPECT_TRUE(std::isinf(div.a));

    a /= 2.0f;
    EXPECT_FLOAT_EQ(a.r, 0.1f / 2.0f);
    EXPECT_FLOAT_EQ(a.g, 0.2f / 2.0f);
    EXPECT_FLOAT_EQ(a.b, 0.3f / 2.0f);
    EXPECT_FLOAT_EQ(a.a, 0.4f / 2.0f);
}

TEST(Color128Test, ComparisonOperators) {
    Color128 a(0.1f, 0.2f, 0.3f, 0.4f);
    Color128 b(0.1f, 0.2f, 0.3f, 0.4f);
    Color128 c(0.1f, 0.2f, 0.31f, 0.4f);

    EXPECT_TRUE(a == b);
    EXPECT_FALSE(a != b);
    EXPECT_TRUE(a != c);
    EXPECT_FALSE(a == c);
}

TEST(Color128Test, ApproximateEqualityCheck) {
    Color128 a(0.1f, 0.2f, 0.3f, 0.4f);
    Color128 b(0.1f, 0.2f, 0.3f, 0.4f);
    Color128 c(0.100001f, 0.199999f, 0.3f, 0.4f);

    EXPECT_TRUE(a.IsEqualApprox(b));
    EXPECT_FALSE(a.IsNotEqualApprox(b));
    EXPECT_TRUE(a.IsEqualApprox(c));
}

TEST(Color128Test, Luminance) {
    Color128 c(0.4f, 0.5f, 0.6f);
    real lum_default = c.Luminance();
    real lum_manual = 0.2126f * 0.4f + 0.7152f * 0.5f + 0.0722f * 0.6f;
    EXPECT_NEAR(lum_default, lum_manual, math::EPSILON_CMP);

    real lum_custom = c.Luminance(0.3f, 0.59f, 0.11f);
    EXPECT_NEAR(lum_custom, 0.3f * 0.4f + 0.59f * 0.5f + 0.11f * 0.6f, math::EPSILON_CMP);
}

TEST(Color128Test, OverOperation) {
    Color128 foreground(1.0f, 0.0f, 0.0f, 0.5f);
    Color128 background(0.0f, 0.0f, 1.0f, 1.0f);

    Color128 result = foreground.Over(background);
    EXPECT_NEAR(result.a, 1.0f, math::EPSILON_CMP);
    EXPECT_NEAR(result.r, 0.5f * 1.0f + 0.0f, math::EPSILON_CMP);
    EXPECT_NEAR(result.g, 0.0f, math::EPSILON_CMP);
}

TEST(Color128Test, UnderOperation) {
    Color128 background(0.0f, 1.0f, 0.0f, 1.0f);
    Color128 foreground(1.0f, 0.0f, 0.0f, 0.5f);

    Color128 result = background.Under(foreground);
    EXPECT_NEAR(result.a, 1.0f, math::EPSILON_CMP);
    EXPECT_NEAR(result.r, 0.5f, math::EPSILON_CMP);
    EXPECT_NEAR(result.g, 0.5f, math::EPSILON_CMP);
    EXPECT_NEAR(result.b, 0.0f, math::EPSILON_CMP);
}

TEST(Color128Test, sRGBToLinearAndLinearTosRGB) {
    Color128 srgb(0.5f, 0.5f, 0.5f, 1.0f);
    Color128 linear = srgb.sRGBToLinear();    // Expect 0.5 * 0.5 = 0.25
    Color128 reconv = linear.LinearTosRGB();  // Expect sqrt(0.25) = 0.5

    // Round-trip must approximately equal original
    EXPECT_NEAR(srgb.r, reconv.r, math::EPSILON_CMP);
    EXPECT_NEAR(srgb.g, reconv.g, math::EPSILON_CMP);
    EXPECT_NEAR(srgb.b, reconv.b, math::EPSILON_CMP);
    EXPECT_NEAR(srgb.a, reconv.a, math::EPSILON_CMP);

    Color128 small(0.2f, 0.4f, 0.8f, 1.0f);
    Color128 small_lin = small.sRGBToLinear();

    // 0.2 * 0.2 = 0.04
    EXPECT_NEAR(small_lin.r, 0.2f * 0.2f, math::EPSILON_CMP);
    // 0.4 * 0.4 = 0.16
    EXPECT_NEAR(small_lin.g, 0.4f * 0.4f, math::EPSILON_CMP);
    // 0.8 * 0.8 = 0.64
    EXPECT_NEAR(small_lin.b, 0.8f * 0.8f, math::EPSILON_CMP);

    Color128 input_lin(0.04f, 0.16f, 0.64f, 1.0f);
    Color128 output_srgb = input_lin.LinearTosRGB();

    // sqrt(0.04) = 0.2
    EXPECT_NEAR(output_srgb.r, 0.2f, math::EPSILON_CMP);
    // sqrt(0.16) = 0.4
    EXPECT_NEAR(output_srgb.g, 0.4f, math::EPSILON_CMP);
    // sqrt(0.64) = 0.8
    EXPECT_NEAR(output_srgb.b, 0.8f, math::EPSILON_CMP);
}

TEST(Color128Test, ToVector3Conversion) {
    Color128 c(0.0f, 0.5f, 1.0f, 1.0f);
    Vector3 v = c.ToVector3();

    EXPECT_NEAR(v.x, -1.0f, math::EPSILON_CMP);  // 0.0f * 2 - 1 = -1
    EXPECT_NEAR(v.y, 0.0f, math::EPSILON_CMP);   // 0.5f * 2 - 1 = 0
    EXPECT_NEAR(v.z, 1.0f, math::EPSILON_CMP);   // 1.0f * 2 - 1 = 1

    // Reverse check (approx)
    Color128 c2((v.x + 1.0f) / 2.0f, (v.y + 1.0f) / 2.0f, (v.z + 1.0f) / 2.0f);
    EXPECT_NEAR(c2.r, c.r, math::EPSILON_CMP);
    EXPECT_NEAR(c2.g, c.g, math::EPSILON_CMP);
    EXPECT_NEAR(c2.b, c.b, math::EPSILON_CMP);
}

TEST(Color128Test, FromVector3Conversion) {
    Vector3 v1(-1.0f, 0.0f, 1.0f);
    Color128 c1 = Color128::FromVector3(v1);

    EXPECT_NEAR(c1.r, 0.0f, math::EPSILON_CMP);
    EXPECT_NEAR(c1.g, 0.5f, math::EPSILON_CMP);
    EXPECT_NEAR(c1.b, 1.0f, math::EPSILON_CMP);
    EXPECT_NEAR(c1.a, 1.0f, math::EPSILON_CMP);

    // Round-trip test with ToVector3
    Vector3 v2(0.25f, -0.5f, 0.75f);
    Color128 c2 = Color128::FromVector3(v2);
    Vector3 back = c2.ToVector3();

    EXPECT_NEAR(v2.x, back.x, math::EPSILON_CMP);
    EXPECT_NEAR(v2.y, back.y, math::EPSILON_CMP);
    EXPECT_NEAR(v2.z, back.z, math::EPSILON_CMP);

    // Edge cases
    Vector3 v_min(-1.0f, -1.0f, -1.0f);
    Vector3 v_max(1.0f, 1.0f, 1.0f);
    Color128 c_min = Color128::FromVector3(v_min);
    Color128 c_max = Color128::FromVector3(v_max);

    EXPECT_NEAR(c_min.r, 0.0f, math::EPSILON_CMP);
    EXPECT_NEAR(c_min.g, 0.0f, math::EPSILON_CMP);
    EXPECT_NEAR(c_min.b, 0.0f, math::EPSILON_CMP);

    EXPECT_NEAR(c_max.r, 1.0f, math::EPSILON_CMP);
    EXPECT_NEAR(c_max.g, 1.0f, math::EPSILON_CMP);
    EXPECT_NEAR(c_max.b, 1.0f, math::EPSILON_CMP);
}
