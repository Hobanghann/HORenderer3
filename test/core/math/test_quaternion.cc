#include <gtest/gtest.h>

#include "core/math/math_funcs.h"
#include "core/math/quaternion.h"
#include "core/math/vector3.h"

using namespace ho;
TEST(QuaternionTest, Constructors) {
    Quaternion q;
    EXPECT_FLOAT_EQ(q.x, 0.0_r);
    EXPECT_FLOAT_EQ(q.y, 0.0_r);
    EXPECT_FLOAT_EQ(q.z, 0.0_r);
    EXPECT_FLOAT_EQ(q.w, 1.0_r);

    q = Quaternion(1.0_r, 2.0_r, 3.0_r, 4.0_r);
    EXPECT_FLOAT_EQ(q.x, 1.0_r);
    EXPECT_FLOAT_EQ(q.y, 2.0_r);
    EXPECT_FLOAT_EQ(q.z, 3.0_r);
    EXPECT_FLOAT_EQ(q.w, 4.0_r);

    Vector3 v(1.0_r, 2.0_r, 3.0_r);
    Quaternion q2(v, 9.0_r);
    EXPECT_EQ(q2.vector, v);
    EXPECT_FLOAT_EQ(q2.scalar, 9.0_r);
}

TEST(QuaternionTest, Assignment) {
    Quaternion a(1, 2, 3, 4);
    Quaternion b;
    b = a;
    EXPECT_EQ(a, b);
}

TEST(QuaternionTest, Addition) {
    Quaternion a(1.0_r, 2.0_r, 3.0_r, 4.0_r);
    Quaternion b(4.0_r, 5.0_r, 6.0_r, 7.0_r);

    EXPECT_EQ(a + b, Quaternion(5.0_r, 7.0_r, 9.0_r, 11.0_r));

    a += b;
    EXPECT_EQ(a, Quaternion(5.0_r, 7.0_r, 9.0_r, 11.0_r));
}

TEST(QuaternionTest, Subtraction) {
    Quaternion a(5.0_r, 7.0_r, 9.0_r, 11.0_r);
    Quaternion b(4.0_r, 5.0_r, 6.0_r, 7.0_r);
    EXPECT_EQ(a - b, Quaternion(1.0_r, 2.0_r, 3.0_r, 4.0_r));

    a -= b;
    EXPECT_EQ(a, Quaternion(1.0_r, 2.0_r, 3.0_r, 4.0_r));
}

TEST(QuaternionTest, Multiplication) {
    Quaternion id;
    Quaternion q(1.0_r, 2.0_r, 3.0_r, 4.0_r);
    EXPECT_TRUE((id * q).IsEqualApprox(q));
    EXPECT_TRUE((q * id).IsEqualApprox(q));

    id *= q;
    EXPECT_TRUE((id).IsEqualApprox(q));

    // Pure imaginary: (0, 1, 0, 0)
    Quaternion i(1.0_r, 0.0_r, 0.0_r, 0.0_r);
    Quaternion j(0.0_r, 1.0_r, 0.0_r, 0.0_r);
    Quaternion k = i * j;
    EXPECT_TRUE(k == Quaternion(0.0_r, 0.0_r, 1.0_r, 0.0_r));
}

TEST(QuaternionTest, ScalarMultiplication) {
    Quaternion q(1.0_r, 2.0_r, 3.0_r, 4.0_r);

    EXPECT_EQ(q * 2.0_r, Quaternion(2.0_r, 4.0_r, 6.0_r, 8.0_r));
    EXPECT_EQ(2.0_r * q, Quaternion(2.0_r, 4.0_r, 6.0_r, 8.0_r));

    q *= 2.0_r;
    EXPECT_EQ(q, Quaternion(2.0_r, 4.0_r, 6.0_r, 8.0_r));
}

TEST(QuaternionTest, ScalarDivision) {
    Quaternion q(2.0_r, 4.0_r, 6.0_r, 8.0_r);

    EXPECT_TRUE((q / 2.0_r).IsEqualApprox(Quaternion(2.0_r / 2.0_r, 4.0_r / 2.0_r, 6.0_r / 2.0_r, 8.0_r / 2.0_r)));

    q /= 2.0_r;
    EXPECT_TRUE(q.IsEqualApprox(Quaternion(2.0_r / 2.0_r, 4.0_r / 2.0_r, 6.0_r / 2.0_r, 8.0_r / 2.0_r)));

    q /= 0.0_r;
    EXPECT_TRUE(std::isinf(q.x));
    EXPECT_TRUE(std::isinf(q.y));
    EXPECT_TRUE(std::isinf(q.z));
    EXPECT_TRUE(std::isinf(q.w));
}

TEST(QuaternionTest, UnaryMinus) {
    Quaternion q(1.0_r, -2.0_r, 3.0_r, -4.0_r);
    EXPECT_EQ(-q, Quaternion(-1.0_r, 2.0_r, -3.0_r, 4.0_r));
}

TEST(QuaternionTest, ComparisonOperators) {
    Quaternion a(1.0_r, 2.0_r, 3.0_r, 4.0_r);
    Quaternion b(1.0_r, 2.0_r, 3.0_r, 4.0_r);

    EXPECT_TRUE(a == b);
    EXPECT_FALSE(a != b);
}

TEST(QuaternionTest, ApproximateEqualityCheck) {
    Quaternion q1(1.0_r, 2.0_r, 3.0_r, 4.0_r);
    Quaternion q2(1.000001_r, 2.000001_r, 3.000001_r, 4.000001_r);
    EXPECT_TRUE(q1.IsEqualApprox(q2));
    EXPECT_FALSE(q1.IsNotEqualApprox(q2));
}

TEST(QuaternionTest, DotProduct) {
    Quaternion q1(1.0_r, 2.0_r, 3.0_r, 4.0_r);
    Quaternion q2(5.0_r, 6.0_r, 7.0_r, 8.0_r);
    real dot = q1.Dot(q2);
    EXPECT_NEAR(dot, 1.0_r * 5.0_r + 2.0_r * 6.0_r + 3.0_r * 7.0_r + 4.0_r * 8.0_r, math::EPSILON_CMP);
}

TEST(QuaternionTest, Magnitude) {
    Quaternion q(2.0_r, 2.0_r, 2.0_r, 2.0_r);

    EXPECT_NEAR(q.SqrdMagnitude(), 16.0_r, math::EPSILON_CMP);
    EXPECT_NEAR(q.Magnitude(), 4.0_r, math::EPSILON_CMP);
}

TEST(QuaternionTest, Normalization) {
    Quaternion q(2.0_r, 2.0_r, 2.0_r, 2.0_r);
    const float inv_len = 1.0_r / 4.0_r;
    const float e = 2.0_r * inv_len;

    Quaternion n = q.Normalized();
    EXPECT_NEAR(n.Magnitude(), 1.0_r, math::EPSILON_CMP);
    EXPECT_NEAR(n.x, e, math::EPSILON_CMP);
    EXPECT_NEAR(n.y, e, math::EPSILON_CMP);
    EXPECT_NEAR(n.z, e, math::EPSILON_CMP);
    EXPECT_NEAR(n.w, e, math::EPSILON_CMP);

    q.Normalize();
    EXPECT_NEAR(q.Magnitude(), 1.0_r, math::EPSILON_CMP);
    EXPECT_NEAR(q.x, e, math::EPSILON_CMP);
    EXPECT_NEAR(q.y, e, math::EPSILON_CMP);
    EXPECT_NEAR(q.z, e, math::EPSILON_CMP);
    EXPECT_NEAR(q.w, e, math::EPSILON_CMP);
}

TEST(QuaternionTest, NormalizationOfZeroVector) {
    Quaternion v(0.0_r, 0.0_r, 0.0_r, 0.0_r);
    Quaternion n = v.Normalized();

    EXPECT_TRUE(std::isnan(n.x));
    EXPECT_TRUE(std::isnan(n.y));
    EXPECT_TRUE(std::isnan(n.z));
    EXPECT_TRUE(std::isnan(n.w));

    v.Normalize();
    EXPECT_TRUE(std::isnan(v.x));
    EXPECT_TRUE(std::isnan(v.y));
    EXPECT_TRUE(std::isnan(v.z));
    EXPECT_TRUE(std::isnan(v.w));
}

TEST(QuaternionTest, UnitCheck) {
    Quaternion q1(1.0_r, 0.0_r, 0.0_r, 0.0_r);
    EXPECT_TRUE(q1.IsUnit());
    EXPECT_TRUE(q1.IsUnitApprox());

    Quaternion q2(1.0_r, 1.0_r, 1.0_r, 1.0_r);
    EXPECT_FALSE(q2.IsUnit());
    EXPECT_FALSE(q2.IsUnitApprox());
}

TEST(QuaternionTest, Conjugation) {
    Quaternion q = Quaternion::FromAxisAngle(Vector3::UNIT_Y, math::PI / 4.0_r);
    Quaternion conj = q.Conjugated();

    EXPECT_NEAR(conj.x, -q.x, math::EPSILON_CMP);
    EXPECT_NEAR(conj.y, -q.y, math::EPSILON_CMP);
    EXPECT_NEAR(conj.z, -q.z, math::EPSILON_CMP);
    EXPECT_NEAR(conj.w, q.w, math::EPSILON_CMP);

    conj = q;
    conj.Conjugate();
    EXPECT_NEAR(conj.x, -q.x, math::EPSILON_CMP);
    EXPECT_NEAR(conj.y, -q.y, math::EPSILON_CMP);
    EXPECT_NEAR(conj.z, -q.z, math::EPSILON_CMP);
    EXPECT_NEAR(conj.w, q.w, math::EPSILON_CMP);
}

TEST(QuaternionTest, Inversion) {
    Quaternion q = Quaternion::FromAxisAngle(Vector3::UNIT_Y, math::PI / 4.0_r);
    q.Normalize();

    Quaternion conj = q.Conjugated();
    Quaternion inv = q.Inverse();

    // only true if |q| == 1
    EXPECT_TRUE(inv.IsEqualApprox(conj));

    Quaternion id = q * inv;
    EXPECT_TRUE(id.IsUnitApprox());
    EXPECT_NEAR(id.w, 1.0_r, math::EPSILON_CMP);

    inv = q;
    inv.Invert();
    EXPECT_TRUE(inv.IsEqualApprox(conj));
    id = q * inv;
    EXPECT_TRUE(id.IsUnitApprox());
    EXPECT_NEAR(id.w, 1.0_r, math::EPSILON_CMP);
}

TEST(QuaternionTest, PureImaginaryCheck) {
    Quaternion q1(1.0_r, 2.0_r, 3.0_r, 0.0_r);
    Quaternion q2(1.0_r, 2.0_r, 3.0_r, 4.0_r);
    Quaternion q3(0.0_r, 0.0_r, 0.0_r, 5.0_r);

    EXPECT_TRUE(q1.IsPurelyImaginary());
    EXPECT_FALSE(q2.IsPurelyImaginary());
    EXPECT_FALSE(q3.IsPurelyImaginary());
}

TEST(QuaternionTest, AxisAndAngle) {
    Vector3 axis = Vector3::UNIT_Y;
    real angle = math::PI / 2.0_r;
    Quaternion q = Quaternion::FromAxisAngle(axis, angle);

    EXPECT_TRUE(q.axis().IsEqualApprox(axis));
    EXPECT_NEAR(q.angle(), angle, math::EPSILON_CMP);
}

TEST(QuaternionTest, TransformAndInverseTransform) {
    Quaternion q = Quaternion::FromAxisAngle(Vector3::UNIT_Z, math::PI / 2.0_r);
    Vector3 v(1.0_r, 0.0_r, 0.0_r);

    Vector3 rotated = q.Transform(v);
    EXPECT_NEAR(rotated.x, 0.0_r, math::EPSILON_CMP);
    EXPECT_NEAR(rotated.y, 1.0_r, math::EPSILON_CMP);
    EXPECT_NEAR(rotated.z, 0.0_r, math::EPSILON_CMP);

    Vector3 inv = q.InvTransform(rotated);
    EXPECT_TRUE(inv.IsEqualApprox(v));
}

TEST(QuaternionTest, FromAxisAngle) {
    Vector3 axis = Vector3::UNIT_Y;
    Quaternion q = Quaternion::FromAxisAngle(axis, math::PI / 2.0_r);
    EXPECT_TRUE(q.IsUnitApprox());

    Vector3 v(1.0_r, 0.0_r, 0.0_r);
    Vector3 rotated = q.Transform(v);

    EXPECT_NEAR(rotated.x, 0.0_r, math::EPSILON_CMP);
    EXPECT_NEAR(rotated.y, 0.0_r, math::EPSILON_CMP);
    EXPECT_NEAR(rotated.z, -1.0_r, math::EPSILON_CMP);
}

TEST(QuaternionTest, FromEuler) {
    real pitch = math::DegToRad(10.0_r);
    real yaw = math::DegToRad(20.0_r);
    real roll = math::DegToRad(30.0_r);

    Quaternion qx = Quaternion::FromAxisAngle(Vector3::UNIT_X, pitch);
    Quaternion qy = Quaternion::FromAxisAngle(Vector3::UNIT_Y, yaw);
    Quaternion qz = Quaternion::FromAxisAngle(Vector3::UNIT_Z, roll);

    Quaternion expected_xyz = qx * qy * qz;
    Quaternion expected_yxz = qy * qx * qz;
    Quaternion expected_zyx = qz * qy * qx;
    Quaternion expected_xzy = qx * qz * qy;
    Quaternion expected_yzx = qy * qz * qx;
    Quaternion expected_zxy = qz * qx * qy;

    Quaternion test_yxz = Quaternion::FromEuler(pitch, yaw, roll, math::YXZ);
    Quaternion test_xyz = Quaternion::FromEuler(pitch, yaw, roll, math::XYZ);
    Quaternion test_zyx = Quaternion::FromEuler(pitch, yaw, roll, math::ZYX);
    Quaternion test_xzy = Quaternion::FromEuler(pitch, yaw, roll, math::XZY);
    Quaternion test_yzx = Quaternion::FromEuler(pitch, yaw, roll, math::YZX);
    Quaternion test_zxy = Quaternion::FromEuler(pitch, yaw, roll, math::ZXY);

    EXPECT_TRUE(test_yxz.IsEqualApprox(expected_yxz));
    EXPECT_TRUE(test_xyz.IsEqualApprox(expected_xyz));
    EXPECT_TRUE(test_zyx.IsEqualApprox(expected_zyx));
    EXPECT_TRUE(test_xzy.IsEqualApprox(expected_xzy));
    EXPECT_TRUE(test_yzx.IsEqualApprox(expected_yzx));
    EXPECT_TRUE(test_zxy.IsEqualApprox(expected_zxy));
}
