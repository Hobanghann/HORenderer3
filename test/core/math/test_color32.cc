#include <gtest/gtest.h>

#include "core/math/color128.h"
#include "core/math/color32.h"

using namespace ho;

TEST(Color32Test, Constructors) {
  Color32 c;
  EXPECT_EQ(c.r, 0);
  EXPECT_EQ(c.g, 0);
  EXPECT_EQ(c.b, 0);
  EXPECT_EQ(c.a, 255);

  Color32 c1(255, 255, 255);
  EXPECT_EQ(c1.r, 255);
  EXPECT_EQ(c1.g, 255);
  EXPECT_EQ(c1.b, 255);
  EXPECT_EQ(c1.a, 255);

  Color32 c2(300, 300, 300, 300);
  EXPECT_EQ(c2.r, 44);
  EXPECT_EQ(c2.g, 44);
  EXPECT_EQ(c2.b, 44);
  EXPECT_EQ(c2.a, 44);

  Color32 c3(-10, -10, -10, -10);
  EXPECT_EQ(c3.r, 246);
  EXPECT_EQ(c3.g, 246);
  EXPECT_EQ(c3.b, 246);
  EXPECT_EQ(c3.a, 246);

  Color128 c128(0.5f, 0.5f, 0.5f, 1.f);
  c = Color32(c128);
  EXPECT_EQ(c.r, 127);
  EXPECT_EQ(c.g, 127);
  EXPECT_EQ(c.b, 127);
  EXPECT_EQ(c.a, 255);

  c128 = Color128(2.f, 2.f, 2.f, 1.f);
  c = Color32(c128);
  EXPECT_EQ(c.r, 255);
  EXPECT_EQ(c.g, 255);
  EXPECT_EQ(c.b, 255);
  EXPECT_EQ(c.a, 255);
}

TEST(Color32Test, Assignment) {
  Color32 c(10, 20, 30, 40);
  Color32 c1;
  c1 = c;
  EXPECT_EQ(c1.r, 10);
  EXPECT_EQ(c1.g, 20);
  EXPECT_EQ(c1.b, 30);
  EXPECT_EQ(c1.a, 40);
}

TEST(Color32Test, Addition) {
  Color32 a(100, 150, 200, 250);
  Color32 b(110, 100, 100, 10);
  Color32 sum = a + b;

  EXPECT_EQ(sum, Color32(210, 250, 255, 255));  // clamped to 255

  a += b;
  EXPECT_EQ(a, sum);
}

TEST(Color32Test, Subtraction) {
  Color32 a(100, 150, 200, 250);
  Color32 b(110, 100, 220, 10);
  EXPECT_EQ(a-b, Color32(0, 50, 0, 240));  // floor to 0

  a -= b;
  EXPECT_EQ(a, Color32(0, 50, 0, 240));  // not same!
}

TEST(Color32Test, Multiplication) {
  Color32 a(10, 20, 30, 40);
  Color32 b(2, 4, 8, 10);
  Color32 mul = a * b;
  EXPECT_EQ(mul, Color32(20, 80, 240, 255));  // 40*10=400 -> clamped

  a *= b;
  EXPECT_EQ(a, mul);
}

TEST(Color32Test, Division) {
  Color32 div = Color32(100, 100, 100, 100) / Color32(10, 5, 2, 0);
  // Division by zero -> 255
  EXPECT_EQ(div, Color32(10, 20, 50, 255));
}

TEST(Color32Test, ScalarMultiplication) {
  Color32 a(10, 20, 30, 40);
  Color32 mul = a * 5;
  EXPECT_EQ(mul, Color32(50, 100, 150, 200));

  a *= 10;
  EXPECT_EQ(a, Color32(100, 200, 255, 255));  // clamped

  Color32 left = 2 * Color32(5, 6, 7, 8);
  EXPECT_EQ(left, Color32(10, 12, 14, 16));
}

TEST(Color32Test, ScalarDivision) {
  Color32 a(10, 20, 30, 40);
  Color32 div = a / 2;
  EXPECT_EQ(div, Color32(5, 10, 15, 20));

  Color32 div_zero = a / 0;
  EXPECT_EQ(div_zero, Color32(255, 255, 255, 255));

  a /= 10;
  EXPECT_EQ(a, Color32(1, 2, 3, 4));  // integer division
}

TEST(Color32Test, ComparisonOperators) {
  Color32 a(10, 20, 30, 40);
  Color32 b(10, 20, 30, 40);
  Color32 c(10, 20, 31, 40);

  EXPECT_TRUE(a == b);
  EXPECT_FALSE(a != b);
  EXPECT_TRUE(a != c);
  EXPECT_FALSE(a == c);
}

TEST(Color32Test, Luminance) {
  Color32 c(100, 150, 200, 255);
  real lum_default = c.Luminance();
  real lum_manual = 0.2126_r * 100 + 0.7152_r * 150 + 0.0722_r * 200;
  EXPECT_NEAR(lum_default, lum_manual, math::EPSILON_CMP);

  real lum_custom = c.Luminance(0.3_r, 0.59_r, 0.11_r);
  EXPECT_NEAR(lum_custom, 0.3_r * 100 + 0.59_r * 150 + 0.11_r * 200,
              math::EPSILON_CMP);
}

TEST(Color32Test, ToR8) {
  Color32 c(200, 150, 100, 50);
  EXPECT_EQ(c.ToR8(), 200);
}

TEST(Color32Test, ToRG8) {
  Color32 c(1, 2, 3, 4);
  uint16_t packed = c.ToRG8();
  uint8_t bytes[2];
  std::memcpy(bytes, &packed, 2);
  EXPECT_EQ(bytes[0], 1);
  EXPECT_EQ(bytes[1], 2);
}

TEST(Color32Test, ToRGB8) {
  Color32 c(10, 20, 30, 40);
  uint32_t packed = c.ToRGB8();
  uint8_t bytes[4];
  std::memcpy(bytes, &packed, 4);

  EXPECT_EQ(bytes[0], 10);
  EXPECT_EQ(bytes[1], 20);
  EXPECT_EQ(bytes[2], 30);
  EXPECT_EQ(bytes[3], 0);  // alpha dummy 0
}

TEST(Color32Test, ToRGBA8) {
  Color32 c(10, 20, 30, 40);
  uint32_t packed = c.ToRGBA8();
  uint8_t bytes[4];
  std::memcpy(bytes, &packed, 4);

  EXPECT_EQ(bytes[0], 10);
  EXPECT_EQ(bytes[1], 20);
  EXPECT_EQ(bytes[2], 30);
  EXPECT_EQ(bytes[3], 40);
}

TEST(Color32Test, ToBGRA8) {
  Color32 c(10, 20, 30, 40);
  uint32_t packed = c.ToBGRA8();
  uint8_t bytes[4];
  std::memcpy(bytes, &packed, 4);

  EXPECT_EQ(bytes[0], 30);
  EXPECT_EQ(bytes[1], 20);
  EXPECT_EQ(bytes[2], 10);
  EXPECT_EQ(bytes[3], 40);
}

TEST(Color32Test, ToARGB8) {
  Color32 c(10, 20, 30, 40);
  uint32_t packed = c.ToARGB8();
  uint8_t bytes[4];
  std::memcpy(bytes, &packed, 4);

  EXPECT_EQ(bytes[0], 40);
  EXPECT_EQ(bytes[1], 10);
  EXPECT_EQ(bytes[2], 20);
  EXPECT_EQ(bytes[3], 30);
}

TEST(Color32Test, ToABGR8) {
  Color32 c(10, 20, 30, 40);
  uint32_t packed = c.ToABGR8();
  uint8_t bytes[4];
  std::memcpy(bytes, &packed, 4);

  EXPECT_EQ(bytes[0], 40);
  EXPECT_EQ(bytes[1], 30);
  EXPECT_EQ(bytes[2], 20);
  EXPECT_EQ(bytes[3], 10);
}

TEST(Color32Test, ToL8) {
  Color32 c(128, 64, 32, 255);
  EXPECT_EQ(c.ToL8(), 128);
}

TEST(Color32Test, ToLA8) {
  Color32 c(128, 64, 32, 255);
  uint16_t packed = c.ToLA8();
  uint8_t bytes[2];
  std::memcpy(bytes, &packed, 2);
  EXPECT_EQ(bytes[0], 128);
  EXPECT_EQ(bytes[1], 255);
}

TEST(Color32Test, RoundTripToRGBA8) {
  Color32 c(1, 2, 3, 4);
  uint32_t packed = c.ToRGBA8();

  uint8_t bytes[4];
  std::memcpy(bytes, &packed, 4);

  Color32 recovered(bytes[0], bytes[1], bytes[2], bytes[3]);
  EXPECT_TRUE(c == recovered);
}
