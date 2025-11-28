#include <gtest/gtest.h>

#include "core/math/interp_funcs.h"
#include "core/math/quaternion.h"
#include "core/math/vector2.h"
#include "core/math/vector3.h"
#include "core/math/vector4.h"

using namespace ho;
using namespace ho::math;

TEST(InterpFuncsTest, Barycentric2DNormalCase) {
  Vector3 v1(0.0_r, 0.0_r, 0.0_r);
  Vector3 v2(10.0_r, 0.0_r, 0.0_r);
  Vector3 target(5.0_r, 0.0_r, 0.0_r);

  Vector2 bary = Barycentric(target, v1, v2);
  EXPECT_NEAR(bary.x, 0.5_r, math::EPSILON_CMP);
  EXPECT_NEAR(bary.y, 0.5_r, math::EPSILON_CMP);
  EXPECT_NEAR(bary.x + bary.y, 1.0_r, math::EPSILON_CMP);
}

TEST(InterpFuncsTest, Barycentric2DEndpointCase) {
  Vector3 v1(0.0_r, 0.0_r, 0.0_r);
  Vector3 v2(10.0_r, 0.0_r, 0.0_r);

  Vector3 target = v1;
  Vector2 bary = Barycentric(target, v1, v2);
  EXPECT_NEAR(bary.x, 1.0_r, math::EPSILON_CMP);
  EXPECT_NEAR(bary.y, 0.0_r, math::EPSILON_CMP);
  EXPECT_NEAR(bary.x + bary.y, 1.0_r, math::EPSILON_CMP);

  target = v2;
  bary = Barycentric(target, v1, v2);
  EXPECT_NEAR(bary.x, 0.0_r, math::EPSILON_CMP);
  EXPECT_NEAR(bary.y, 1.0_r, math::EPSILON_CMP);
  EXPECT_NEAR(bary.x + bary.y, 1.0_r, math::EPSILON_CMP);
}

TEST(InterpFuncsTest, Barycentric2DDegenerateCase) {
  Vector3 v1(0.0_r, 0.0_r, 0.0_r);
  Vector3 v2(0.0_r, 0.0_r, 0.0_r);
  Vector3 target(1.0_r, 0.0_r, 0.0_r);

  Vector2 bary = Barycentric(target, v1, v2);
  EXPECT_NEAR(bary.x, 1.0_r, math::EPSILON_CMP);
  EXPECT_NEAR(bary.y, 0.0_r, math::EPSILON_CMP);
}

TEST(InterpFuncsTest, Barycentric3DNormalCase) {
  Vector3 v1(0.0_r, 0.0_r, 0.0_r);
  Vector3 v2(1.0_r, 0.0_r, 0.0_r);
  Vector3 v3(0.0_r, 1.0_r, 0.0_r);
  Vector3 p(0.25_r, 0.25_r, 0.0_r);

  Vector3 bary = Barycentric(p, v1, v2, v3);

  EXPECT_NEAR(bary.x + bary.y + bary.z, 1.0_r, math::EPSILON_CMP);
  EXPECT_NEAR(bary.x, 0.5_r, math::EPSILON_CMP);
  EXPECT_NEAR(bary.y, 0.25_r, math::EPSILON_CMP);
  EXPECT_NEAR(bary.z, 0.25_r, math::EPSILON_CMP);
}

TEST(InterpFuncsTest, Barycentric3DEndPointCase) {
  Vector3 v1(0.0_r, 0.0_r, 0.0_r);
  Vector3 v2(1.0_r, 0.0_r, 0.0_r);
  Vector3 v3(0.0_r, 1.0_r, 0.0_r);

  Vector3 p = v1;
  Vector3 bary = Barycentric(p, v1, v2, v3);

  EXPECT_NEAR(bary.x, 1.0_r, math::EPSILON_CMP);
  EXPECT_NEAR(bary.y, 0.0_r, math::EPSILON_CMP);
  EXPECT_NEAR(bary.z, 0.0_r, math::EPSILON_CMP);
  EXPECT_NEAR(bary.x + bary.y + bary.z, 1.0_r, math::EPSILON_CMP);
}

TEST(InterpFuncsTest, Barycentric3DEdgeCase) {
  Vector3 v1(0.0_r, 0.0_r, 0.0_r);
  Vector3 v2(1.0_r, 0.0_r, 0.0_r);
  Vector3 v3(0.0_r, 1.0_r, 0.0_r);

  Vector3 p(0.5_r, 0.0_r, 0.0_r);
  Vector3 bary = Barycentric(p, v1, v2, v3);

  EXPECT_NEAR(bary.x, 0.5_r, math::EPSILON_CMP);
  EXPECT_NEAR(bary.y, 0.5_r, math::EPSILON_CMP);
  EXPECT_NEAR(bary.z, 0.0_r, math::EPSILON_CMP);
  EXPECT_NEAR(bary.x + bary.y + bary.z, 1.0_r, math::EPSILON_CMP);
}

TEST(InterpFuncsTest, Barycentric3DDegeneratePointCase) {
  Vector3 v1(0.0_r, 0.0_r, 0.0_r);
  Vector3 v2(0.0_r, 0.0_r, 0.0_r);
  Vector3 v3(0.0_r, 0.0_r, 0.0_r);
  Vector3 p(1.0_r, 0.0_r, 0.0_r);

  Vector3 bary = Barycentric(p, v1, v2, v3);
  EXPECT_EQ(bary, Vector3::ZERO);
}

TEST(InterpFuncsTest, Barycentric3DDegenerateCollinearCase) {
  Vector3 v1(0.0_r, 0.0_r, 0.0_r);
  Vector3 v2(1.0_r, 0.0_r, 0.0_r);
  Vector3 v3(2.0_r, 0.0_r, 0.0_r);
  Vector3 p(1.0_r, 0.0_r, 0.0_r);

  Vector3 bary = Barycentric(p, v1, v2, v3);
  EXPECT_EQ(bary, Vector3::ZERO);
}

TEST(InterpFuncsTest, Lerp2DNormalCase) {
  Vector3 v1(0.0_r, 0.0_r, 0.0_r);
  Vector3 v2(10.0_r, 0.0_r, 0.0_r);
  Vector2 bary(0.8_r, 0.2_r);
  Vector3 result = Lerp(v1, v2, bary);
  EXPECT_FLOAT_EQ(result.x, 2.0_r);
  EXPECT_FLOAT_EQ(result.y, 0.0_r);
}

TEST(InterpFuncsTest, Lerp2DEndpointCase) {
  Vector3 v1(0.0_r, 0.0_r, 0.0_r);
  Vector3 v2(10.0_r, 0.0_r, 0.0_r);

  Vector2 bary0(1.0_r, 0.0_r);
  Vector3 res0 = Lerp(v1, v2, bary0);
  EXPECT_FLOAT_EQ(res0.x, 0.0_r);
  EXPECT_FLOAT_EQ(res0.y, 0.0_r);

  Vector2 bary1(0.0_r, 1.0_r);
  Vector3 res1 = Lerp(v1, v2, bary1);
  EXPECT_FLOAT_EQ(res1.x, 10.0_r);
  EXPECT_FLOAT_EQ(res1.y, 0.0_r);
}

TEST(InterpFuncsTest, Lerp2DDegenerateCase) {
  Vector3 v1(5.0_r, 5.0_r, 0.0_r);
  Vector3 v2(5.0_r, 5.0_r, 0.0_r);
  Vector2 bary(0.3_r, 0.7_r);
  Vector3 res = Lerp(v1, v2, bary);
  EXPECT_FLOAT_EQ(res.x, 5.0_r);
  EXPECT_FLOAT_EQ(res.y, 5.0_r);
}

TEST(InterpFuncsTest, Lerp3DNormalCase) {
  Vector3 v1(0.0_r, 0.0_r, 0.0_r);
  Vector3 v2(10.0_r, 0.0_r, 0.0_r);
  Vector3 v3(0.0_r, 10.0_r, 0.0_r);

  Vector3 bary(0.2_r, 0.3_r, 0.5_r);
  Vector3 res = Lerp(v1, v2, v3, bary);

  EXPECT_NEAR(res.x, 3.0_r, math::EPSILON_CMP);
  EXPECT_NEAR(res.y, 5.0_r, math::EPSILON_CMP);
}

TEST(InterpFuncsTest, Lerp3DEndpointCase) {
  Vector3 v1(0.0_r, 0.0_r, 0.0_r);
  Vector3 v2(10.0_r, 0.0_r, 0.0_r);
  Vector3 v3(0.0_r, 10.0_r, 0.0_r);

  Vector3 bary_v1(1.0_r, 0.0_r, 0.0_r);
  Vector3 res1 = Lerp(v1, v2, v3, bary_v1);
  EXPECT_EQ(res1, v1);

  Vector3 bary_v2(0.0_r, 1.0_r, 0.0_r);
  Vector3 res2 = Lerp(v1, v2, v3, bary_v2);
  EXPECT_EQ(res2, v2);

  Vector3 bary_v3(0.0_r, 0.0_r, 1.0_r);
  Vector3 res3 = Lerp(v1, v2, v3, bary_v3);
  EXPECT_EQ(res3, v3);
}

TEST(InterpFuncsTest, Lerp3DDegenerateCase) {
  Vector3 v(5.0_r, 5.0_r, 5.0_r);
  Vector3 bary(0.2_r, 0.3_r, 0.5_r);
  Vector3 res = Lerp(v, v, v, bary);

  EXPECT_EQ(res, v);
}

TEST(InterpFuncsTest, PcerpW2DNormalCase) {
  Vector2 inv_w(1.0_r, 0.5_r);  // w1=1, w2=2
  Vector2 bary(0.25_r, 0.75_r);
  // denom = 0.25*1 + 0.75*0.5 = 0.625 → 1/0.625 = 1.6
  real w = Pcerp_W(inv_w, bary);
  EXPECT_NEAR(w, 1.6_r, math::EPSILON_CMP);
}

TEST(InterpFuncsTest, PcerpW2DEndpointCase) {
  Vector2 inv_w(1.0_r, 0.5_r);  // w1=1, w2=2

  Vector2 bary1(1.0_r, 0.0_r);
  EXPECT_NEAR(Pcerp_W(inv_w, bary1), 1.0_r, math::EPSILON_CMP);

  Vector2 bary2(0.0_r, 1.0_r);
  EXPECT_NEAR(Pcerp_W(inv_w, bary2), 2.0_r, math::EPSILON_CMP);
}

TEST(InterpFuncsTest, PcerpW2DDegenerateCase) {
  Vector2 inv_w(0.0_r, 0.0_r);
  Vector2 bary(0.5_r, 0.5_r);

  real w = Pcerp_W(inv_w, bary);
  EXPECT_TRUE(std::isinf(w) || std::isnan(w));
}

TEST(InterpFuncsTest, PcerpW3DNormalCase) {
  Vector3 inv_w(1.0_r, 0.5_r, 0.25_r);
  Vector3 bary(0.2_r, 0.3_r, 0.5_r);
  // denom = 0.2*1 + 0.3*0.5 + 0.5*0.25 = 0.475
  // result = 1/0.475 ≈ 2.10526
  EXPECT_NEAR(Pcerp_W(inv_w, bary), 2.10526_r, math::EPSILON_CMP);
}

TEST(InterpFuncsTest, PcerpW3DEndpointCase) {
  Vector3 inv_w(1.0_r, 0.5_r, 0.25_r);

  Vector3 bary1(1.0_r, 0.0_r, 0.0_r);
  EXPECT_NEAR(Pcerp_W(inv_w, bary1), 1.0_r, math::EPSILON_CMP);

  Vector3 bary2(0.0_r, 1.0_r, 0.0_r);
  EXPECT_NEAR(Pcerp_W(inv_w, bary2), 2.0_r, math::EPSILON_CMP);

  Vector3 bary3(0.0_r, 0.0_r, 1.0_r);
  EXPECT_NEAR(Pcerp_W(inv_w, bary3), 4.0_r, math::EPSILON_CMP);
}

TEST(InterpFuncsTest, PcerpW3DDegenerateCase) {
  Vector3 inv_w(0.0_r, 0.0_r, 0.0_r);
  Vector3 bary(0.3_r, 0.3_r, 0.4_r);
  real w = Pcerp_W(inv_w, bary);
  EXPECT_TRUE(std::isinf(w) || std::isnan(w));
}

TEST(InterpFuncsTest, PcerpWithInvW2DNormalCase) {
  real a1 = 10.0_r, a2 = 20.0_r;
  Vector2 inv_w(1.0_r, 0.5_r);  // w1=1, w2=2
  Vector2 bary(0.25_r, 0.75_r);
  real interpolated_w =
      Pcerp_W(inv_w, bary);  // = 1 / (0.25*1 + 0.75*0.5) = 1.6

  real result = Pcerp(a1, a2, bary, inv_w, interpolated_w);

  // expected: 1.6 * (0.25*1*10 + 0.75*0.5*20) = 1.6 * (2.5 + 7.5) = 16
  EXPECT_NEAR(result, 16.0_r, math::EPSILON_CMP);
}

TEST(InterpFuncsTest, PcerpWithInvW2DEndpointCase) {
  Vector2 inv_w(1.0_r, 0.5_r);
  real a1 = 10.0_r, a2 = 20.0_r;

  Vector2 bary(1.0_r, 0.0_r);
  real interpolated_w = Pcerp_W(inv_w, bary);  // = 1/1 = 1
  real result = Pcerp(a1, a2, bary, inv_w, interpolated_w);
  EXPECT_NEAR(result, a1, math::EPSILON_CMP);

  bary = Vector2(0.0_r, 1.0_r);
  interpolated_w = Pcerp_W(inv_w, bary);  // = 1/0.5 = 2
  result = Pcerp(a1, a2, bary, inv_w, interpolated_w);
  EXPECT_NEAR(result, a2, math::EPSILON_CMP);
}

TEST(InterpFuncsTest, PcerpWithInvW2DDegenerateCase) {
  Vector2 inv_w(0.0_r, 0.0_r);
  Vector2 bary(0.5_r, 0.5_r);
  real a1 = 10.0_r, a2 = 20.0_r;
  real interpolated_w = Pcerp_W(inv_w, bary);  // denom = inf

  real result = Pcerp(a1, a2, bary, inv_w, interpolated_w);
  EXPECT_TRUE(std::isnan(result) || std::isinf(result));
}

TEST(InterpFuncsTest, PcerpWithInvW3DNormalCase) {
  Vector3 inv_w(1.0_r, 0.5_r, 0.25_r);
  Vector3 bary(0.2_r, 0.3_r, 0.5_r);
  real a1 = 10.0_r, a2 = 20.0_r, a3 = 30.0_r;
  real interpolated_w = Pcerp_W(inv_w, bary);  // 1 / (0.475) ≈ 2.10526

  real result = Pcerp(a1, a2, a3, bary, inv_w, interpolated_w);
  // numerator = 0.2*1*10 + 0.3*0.5*20 + 0.5*0.25*30 = 2 + 3 + 3.75 = 8.75
  // result = 2.10526 * 8.75 = 18.42
  EXPECT_NEAR(result, 18.42_r, 0.01_r);
}

TEST(InterpFuncsTest, PcerpWithInvW3DEndpointCase) {
  Vector3 inv_w(1.0_r, 0.5_r, 0.25_r);
  real a1 = 10.0_r, a2 = 20.0_r, a3 = 30.0_r;

  Vector3 bary(1.0_r, 0.0_r, 0.0_r);
  real w = Pcerp_W(inv_w, bary);
  real result = Pcerp(a1, a2, a3, bary, inv_w, w);
  EXPECT_NEAR(result, 10.0_r, math::EPSILON_CMP);

  bary = Vector3(0.0_r, 1.0_r, 0.0_r);
  w = Pcerp_W(inv_w, bary);
  result = Pcerp(a1, a2, a3, bary, inv_w, w);
  EXPECT_NEAR(result, 20.0_r, math::EPSILON_CMP);

  bary = Vector3(0.0_r, 0.0_r, 1.0_r);
  w = Pcerp_W(inv_w, bary);
  result = Pcerp(a1, a2, a3, bary, inv_w, w);
  EXPECT_NEAR(result, 30.0_r, math::EPSILON_CMP);
}

TEST(InterpFuncsTest, PcerpWithInvW3DDegenerateCase) {
  Vector3 inv_w(0.0_r, 0.0_r, 0.0_r);
  Vector3 bary(0.3_r, 0.3_r, 0.4_r);
  real a1 = 10.0_r, a2 = 20.0_r, a3 = 30.0_r;
  real w = Pcerp_W(inv_w, bary);

  real result = Pcerp(a1, a2, a3, bary, inv_w, w);
  EXPECT_TRUE(std::isinf(result) || std::isnan(result));
}

TEST(InterpFuncsTest, Pcerp2DNormalCase) {
  // a1=10, a2=20 → w1=1, w2=2
  Vector4 v1(0.0_r, 0.0_r, 0.0_r, 1.0_r);
  Vector4 v2(1.0_r, 0.0_r, 0.0_r, 2.0_r);
  Vector2 bary(0.25_r, 0.75_r);

  real a1 = 10.0_r, a2 = 20.0_r;
  real res = Pcerp(a1, a2, bary, v1, v2);

  // Perspective-correct interpolation:
  // inv_w = (1, 0.5)
  // denom = 0.25*1 + 0.75*0.5 = 0.625
  // result = (1/0.625) * (0.25*1*10 + 0.75*0.5*20)
  //         = 1.6 * (2.5 + 7.5) = 16
  EXPECT_NEAR(res, 16.0_r, math::EPSILON_CMP);
}

TEST(InterpFuncsTest, Pcerp2DEndpointCase) {
  Vector4 v1(0.0_r, 0.0_r, 0.0_r, 1.0_r);
  Vector4 v2(1.0_r, 0.0_r, 0.0_r, 2.0_r);
  real a1 = 10.0_r, a2 = 20.0_r;

  Vector2 bary1(1.0_r, 0.0_r);
  Vector2 bary2(0.0_r, 1.0_r);

  EXPECT_NEAR(Pcerp(a1, a2, bary1, v1, v2), 10.0_r, math::EPSILON_CMP);
  EXPECT_NEAR(Pcerp(a1, a2, bary2, v1, v2), 20.0_r, math::EPSILON_CMP);
}

TEST(InterpFuncsTest, Pcerp2DDegenerateCase) {
  Vector4 v1(0.0_r, 0.0_r, 0.0_r, 0.0_r);
  Vector4 v2(1.0_r, 0.0_r, 0.0_r, 0.0_r);
  real a1 = 10.0_r, a2 = 20.0_r;
  Vector2 bary(0.5_r, 0.5_r);

  real res = Pcerp(a1, a2, bary, v1, v2);
  EXPECT_TRUE(std::isnan(res) || std::isinf(res));
}

TEST(InterpFuncsTest, Pcerp3DNormalCase) {
  // a1=10, a2=20, a3=30 → w1=1, w2=2, w3=4
  Vector4 v1(0.0_r, 0.0_r, 0.0_r, 1.0_r);
  Vector4 v2(1.0_r, 0.0_r, 0.0_r, 2.0_r);
  Vector4 v3(0.0_r, 1.0_r, 0.0_r, 4.0_r);
  Vector3 bary(0.2_r, 0.3_r, 0.5_r);

  real a1 = 10.0_r, a2 = 20.0_r, a3 = 30.0_r;
  real res = Pcerp(a1, a2, a3, bary, v1, v2, v3);

  // inv_w = (1, 0.5, 0.25)
  // denom = 0.2*1 + 0.3*0.5 + 0.5*0.25 = 0.475
  // numerator = 0.2*1*10 + 0.3*0.5*20 + 0.5*0.25*30
  //            = 2 + 3 + 3.75 = 8.75
  // result = (1/0.475)*8.75 ≈ 18.42
  EXPECT_NEAR(res, 18.42_r, 0.01_r);
}

TEST(InterpFuncsTest, Pcerp3DEndpointCase) {
  Vector4 v1(0.0_r, 0.0_r, 0.0_r, 1.0_r);
  Vector4 v2(1.0_r, 0.0_r, 0.0_r, 2.0_r);
  Vector4 v3(0.0_r, 1.0_r, 0.0_r, 4.0_r);
  real a1 = 10.0_r, a2 = 20.0_r, a3 = 30.0_r;

  Vector3 b1(1.0_r, 0.0_r, 0.0_r);
  Vector3 b2(0.0_r, 1.0_r, 0.0_r);
  Vector3 b3(0.0_r, 0.0_r, 1.0_r);

  EXPECT_NEAR(Pcerp(a1, a2, a3, b1, v1, v2, v3), 10.0_r, math::EPSILON_CMP);
  EXPECT_NEAR(Pcerp(a1, a2, a3, b2, v1, v2, v3), 20.0_r, math::EPSILON_CMP);
  EXPECT_NEAR(Pcerp(a1, a2, a3, b3, v1, v2, v3), 30.0_r, math::EPSILON_CMP);
}

TEST(InterpFuncsTest, Pcerp3DDegenerateCase) {
  Vector4 v1(0.0_r, 0.0_r, 0.0_r, 0.0_r);
  Vector4 v2(1.0_r, 0.0_r, 0.0_r, 0.0_r);
  Vector4 v3(0.0_r, 1.0_r, 0.0_r, 0.0_r);
  real a1 = 10.0_r, a2 = 20.0_r, a3 = 30.0_r;
  Vector3 bary(0.3_r, 0.3_r, 0.4_r);

  real res = Pcerp(a1, a2, a3, bary, v1, v2, v3);
  EXPECT_TRUE(std::isnan(res) || std::isinf(res));
}

TEST(InterpFuncsTest, QuaternionSlerpNormalCase) {
  Quaternion q1 = Quaternion::FromAxisAngle(Vector3::UNIT_Y, 0.0_r);
  Quaternion q2 = Quaternion::FromAxisAngle(Vector3::UNIT_Y, math::PI * 0.5_r);

  Vector2 bary(0.5_r, 0.5_r);
  Quaternion result = Slerp(q1, q2, bary);

  Quaternion expected =
      Quaternion::FromAxisAngle(Vector3::UNIT_Y, math::PI * 0.25_r);

  // quaternions may be negated and still represent the same rotation
  EXPECT_NEAR(std::fabs(result.Dot(expected)), 1.0_r, math::EPSILON_CMP);
}

TEST(InterpFuncsTest, QuaternionSlerpSameQuaternionCase) {
  Quaternion q1 = Quaternion::FromAxisAngle(Vector3::UNIT_X, math::PI * 0.25_r);
  Quaternion q2 = q1;  // identical

  Vector2 bary(0.5_r, 0.5_r);
  Quaternion result = Slerp(q1, q2, bary);

  EXPECT_TRUE(result.IsEqualApprox(q1));
}

TEST(InterpFuncsTest, QuaternionSlerpOppositeCase) {
  Quaternion q1 = Quaternion::FromAxisAngle(Vector3::UNIT_X, math::PI * 0.25_r);
  Quaternion q2 = -q1;

  Vector2 bary(0.5_r, 0.5_r);
  Quaternion result = Slerp(q1, q2, bary);

  EXPECT_TRUE(result.IsUnitApprox());
  EXPECT_NEAR(std::fabs(result.Dot(q1)), 1.0_r, math::EPSILON_CMP);
}

TEST(InterpFuncsTest, QuaternionSlerpEndpointCase) {
  Quaternion q1 = Quaternion::FromAxisAngle(Vector3::UNIT_Y, 0.0_r);
  Quaternion q2 = Quaternion::FromAxisAngle(Vector3::UNIT_Y, math::PI * 0.5_r);

  Vector2 bary1(1.0_r, 0.0_r);
  Quaternion result1 = Slerp(q1, q2, bary1);
  EXPECT_TRUE(result1.IsEqualApprox(q1));

  Vector2 bary2(0.0_r, 1.0_r);
  Quaternion result2 = Slerp(q1, q2, bary2);
  EXPECT_TRUE(result2.IsEqualApprox(q2));
}

TEST(InterpFuncsTest, QuaternionSlerpLongNormalCase) {
  Quaternion q1 = Quaternion::FromAxisAngle(Vector3::UNIT_Y, 0.0_r);
  Quaternion q2 = Quaternion::FromAxisAngle(Vector3::UNIT_Y, math::PI * 0.5_r);

  Vector2 bary(0.5_r, 0.5_r);
  Quaternion result = SlerpLong(q1, q2, bary);

  Quaternion expected =
      Quaternion::FromAxisAngle(Vector3::UNIT_Y, math::PI + math::PI * 0.25_r);

  EXPECT_NEAR(std::fabs(result.Dot(expected)), 1.0_r, math::EPSILON_CMP);
  EXPECT_TRUE(result.IsUnitApprox());
}

TEST(InterpFuncsTest, QuaternionSlerpLongSameQuaternionCase) {
  Quaternion q1 = Quaternion::FromAxisAngle(Vector3::UNIT_X, math::PI * 0.25_r);
  Quaternion q2 = q1;  // identical

  Vector2 bary(0.5_r, 0.5_r);
  Quaternion result = SlerpLong(q1, q2, bary);

  EXPECT_TRUE(result.IsEqualApprox(q1));
}

TEST(InterpFuncsTest, QuaternionSlerpLongOppositeCase) {
  Quaternion q1 = Quaternion::FromAxisAngle(Vector3::UNIT_X, 0.0_r);
  Quaternion q2 = -q1;

  Vector2 bary(0.5_r, 0.5_r);
  Quaternion result = SlerpLong(q1, q2, bary);

  EXPECT_TRUE(result.IsUnitApprox());
  EXPECT_NEAR(std::fabs(result.Dot(q1)), 1.0_r, math::EPSILON_CMP);
}

TEST(InterpFuncsTest, QuaternionSlerpLongEndpointCase) {
  Quaternion q1 = Quaternion::FromAxisAngle(Vector3::UNIT_Z, 0.0_r);
  Quaternion q2 = Quaternion::FromAxisAngle(Vector3::UNIT_Z, math::PI);

  Vector2 bary1(1.0_r, 0.0_r);
  Quaternion res1 = SlerpLong(q1, q2, bary1);
  EXPECT_TRUE(res1.IsEqualApprox(q1));

  Vector2 bary2(0.0_r, 1.0_r);
  Quaternion res2 = SlerpLong(q1, q2, bary2);
  EXPECT_TRUE(res2.IsEqualApprox(q2));
}

TEST(InterpFuncsTest, QuaternionSlerpLongDegenerateCase) {
  Quaternion q1 = Quaternion::FromAxisAngle(Vector3::UNIT_X, math::PI * 0.25_r);
  Quaternion q2 = q1;

  Vector2 bary(0.5_r, 0.5_r);
  Quaternion result = SlerpLong(q1, q2, bary);

  EXPECT_TRUE(result.IsEqualApprox(q1));
  EXPECT_TRUE(result.IsUnitApprox());
}
