#include <gtest/gtest.h>

#include "core/math/basis_3d.h"
#include "core/math/math_funcs.h"
#include "core/math/matrix4x4.h"
#include "core/math/quaternion.h"
#include "core/math/transform_3d.h"
#include "core/math/vector3.h"

using namespace ho;
using namespace ho::math;

class Transform3DTest : public ::testing::Test {
   protected:
    void SetUp() override {
        b_default = Basis3D();
        b_scale = Basis3D(Vector3(2.0_r, 0.0_r, 0.0_r), Vector3(0.0_r, 3.0_r, 0.0_r), Vector3(0.0_r, 0.0_r, 4.0_r));

        real angle = PI / 4.0_r;
        real s = std::sin(angle), c = std::cos(angle);

        b_pitch = Basis3D(Matrix3x3(Vector3(1.0_r, 0.0_r, 0.0_r), Vector3(0.0_r, c, -s), Vector3(0.0_r, s, c)));
        b_yaw = Basis3D(Matrix3x3(Vector3(c, 0.0_r, s), Vector3(0.0_r, 1.0_r, 0.0_r), Vector3(-s, 0.0_r, c)));
        b_roll = Basis3D(Matrix3x3(Vector3(c, -s, 0.0_r), Vector3(s, c, 0.0_r), Vector3(0.0_r, 0.0_r, 1.0_r)));

        b_non_ortho = Basis3D(Vector3(1.0_r, 0.2_r, 0.3_r), Vector3(0.5_r, 1.0_r, 0.0_r), Vector3(0.0_r, 0.5_r, 1.0_r));

        t_default = Transform3D();
        t_scale = Transform3D(b_scale, Vector3(1.0_r, 2.0_r, 3.0_r));
        t_rot = Transform3D(b_yaw, Vector3(4.0_r, 5.0_r, 6.0_r));
        t_non_ortho = Transform3D(b_non_ortho, Vector3(2.0_r, 3.0_r, 4.0_r));
    }

    Basis3D b_default, b_scale, b_pitch, b_yaw, b_roll, b_non_ortho;
    Transform3D t_default, t_scale, t_rot, t_non_ortho;
};

TEST_F(Transform3DTest, Constructors) {
    EXPECT_EQ(t_default.basis, b_default);
    EXPECT_EQ(t_default.origin, Vector3(0.0_r, 0.0_r, 0.0_r));

    EXPECT_EQ(t_scale.basis, b_scale);
    EXPECT_EQ(t_scale.origin, Vector3(1.0_r, 2.0_r, 3.0_r));

    Matrix4x4 m = t_scale.ToMatrix();
    Transform3D t_from_m(m);
    EXPECT_TRUE(t_from_m.ToMatrix().IsEqualApprox(m));
}

TEST_F(Transform3DTest, Accessors) {
    Transform3D t = t_scale;
    EXPECT_EQ(t.basis_x(), Vector3(2.0_r, 0.0_r, 0.0_r));
    EXPECT_EQ(t.basis_y(), Vector3(0.0_r, 3.0_r, 0.0_r));
    EXPECT_EQ(t.basis_z(), Vector3(0.0_r, 0.0_r, 4.0_r));

    t.set_basis_x(Vector3(5.0_r, 1.0_r, 0.0_r));
    t.set_basis_y(Vector3(3.0_r, 4.0_r, 0.0_r));
    t.set_basis_z(Vector3(2.0_r, 4.0_r, 0.0_r));
    EXPECT_EQ(t.basis_x(), Vector3(5.0_r, 1.0_r, 0.0_r));
    EXPECT_EQ(t.basis_y(), Vector3(3.0_r, 4.0_r, 0.0_r));
    EXPECT_EQ(t.basis_z(), Vector3(2.0_r, 4.0_r, 0.0_r));

    EXPECT_NEAR(t.scale_x(), t.basis_x().Magnitude(), math::EPSILON_CMP);
    EXPECT_NEAR(t.scale_y(), t.basis_y().Magnitude(), math::EPSILON_CMP);
    EXPECT_NEAR(t.scale_z(), t.basis_z().Magnitude(), math::EPSILON_CMP);
}

TEST_F(Transform3DTest, ComparisonOperators) {
    Transform3D t1(b_scale, Vector3(1.0_r, 2.0_r, 3.0_r));
    Transform3D t2(b_scale, Vector3(1.0_r, 2.0_r, 3.0_r));
    Transform3D t3(b_yaw, Vector3(1.0_r, 2.0_r, 3.0_r));

    EXPECT_TRUE(t1 == t2);
    EXPECT_FALSE(t1 != t2);
    EXPECT_TRUE(t1 != t3);
}

TEST_F(Transform3DTest, ApproximateEqualityCheck) {
    Transform3D t1(b_scale, Vector3(1.0_r, 2.0_r, 3.0_r));
    Transform3D t2(Basis3D(Vector3(2.000001_r, 0.0_r, 0.0_r), Vector3(0.0_r, 3.000001_r, 0.0_r),
                           Vector3(0.0_r, 0.0_r, 4.000001_r)),
                   Vector3(1.000001_r, 2.000001_r, 3.000001_r));
    EXPECT_TRUE(t1.IsEqualApprox(t2));
    EXPECT_FALSE(t1.IsNotEqualApprox(t2));
}

TEST_F(Transform3DTest, OrthogonalityCheck) {
    EXPECT_FALSE(t_non_ortho.IsOrthogonal());
    EXPECT_TRUE(t_scale.IsOrthogonal());
}

TEST_F(Transform3DTest, Orthogonalization) {
    Transform3D o = t_non_ortho.Orthogonalized();
    EXPECT_TRUE(o.IsOrthogonal());

    t_non_ortho.Orthogonalize();
    EXPECT_TRUE(t_non_ortho.IsOrthogonal());
}

TEST_F(Transform3DTest, OrthonormalityCheck) {
    EXPECT_FALSE(t_scale.IsOrthonormal());
    EXPECT_TRUE(t_rot.IsOrthonormal());
}

TEST_F(Transform3DTest, Orthonormalization) {
    Transform3D n = t_non_ortho.Orthonormalized();
    EXPECT_TRUE(n.IsOrthonormal());

    t_non_ortho.Orthonormalize();
    EXPECT_TRUE(t_non_ortho.IsOrthonormal());
}

TEST_F(Transform3DTest, Scaling) {
    Transform3D t(b_yaw, Vector3(1.0_r, 2.0_r, 3.0_r));
    Matrix4x4 m_base = t.ToMatrix();

    Matrix4x4 m_scale(Vector4(2.0_r, 0.0_r, 0.0_r, 0.0_r), Vector4(0.0_r, 3.0_r, 0.0_r, 0.0_r),
                      Vector4(0.0_r, 0.0_r, 4.0_r, 0.0_r), Vector4(0.0_r, 0.0_r, 0.0_r, 1.0_r));

    Transform3D t1 = t;
    t1.Scale(Vector3(2.0_r, 3.0_r, 4.0_r));
    EXPECT_TRUE(t1.ToMatrix().IsEqualApprox(m_scale * m_base));

    t1 = t.Scaled(Vector3(2.0_r, 3.0_r, 4.0_r));
    EXPECT_TRUE(t1.ToMatrix().IsEqualApprox(m_scale * m_base));

    Matrix4x4 m_uniform(Vector4(2.0_r, 0.0_r, 0.0_r, 0.0_r), Vector4(0.0_r, 2.0_r, 0.0_r, 0.0_r),
                        Vector4(0.0_r, 0.0_r, 2.0_r, 0.0_r), Vector4(0.0_r, 0.0_r, 0.0_r, 1.0_r));

    t1 = t;
    t1.ScaleUniform(2.0_r);
    EXPECT_TRUE(t1.ToMatrix().IsEqualApprox(m_uniform * m_base));

    t1 = t.ScaledUniform(2.0_r);
    EXPECT_TRUE(t1.ToMatrix().IsEqualApprox(m_uniform * m_base));
}

TEST_F(Transform3DTest, ScalingLocal) {
    Transform3D t(b_yaw, Vector3(1.0_r, 2.0_r, 3.0_r));
    Matrix4x4 m_base = t.ToMatrix();

    Matrix4x4 m_scale(Vector4(2.0_r, 0.0_r, 0.0_r, 0.0_r), Vector4(0.0_r, 3.0_r, 0.0_r, 0.0_r),
                      Vector4(0.0_r, 0.0_r, 4.0_r, 0.0_r), Vector4(0.0_r, 0.0_r, 0.0_r, 1.0_r));

    Transform3D t1 = t;
    t1.ScaleLocal(Vector3(2.0_r, 3.0_r, 4.0_r));
    EXPECT_TRUE(t1.ToMatrix().IsEqualApprox(m_base * m_scale));

    t1 = t.ScaledLocal(Vector3(2.0_r, 3.0_r, 4.0_r));
    EXPECT_TRUE(t1.ToMatrix().IsEqualApprox(m_base * m_scale));

    Matrix4x4 m_uniform(Vector4(2.0_r, 0.0_r, 0.0_r, 0.0_r), Vector4(0.0_r, 2.0_r, 0.0_r, 0.0_r),
                        Vector4(0.0_r, 0.0_r, 2.0_r, 0.0_r), Vector4(0.0_r, 0.0_r, 0.0_r, 1.0_r));
    t1 = t;
    t1.ScaleUniformLocal(2.0_r);
    EXPECT_TRUE(t1.ToMatrix().IsEqualApprox(m_base * m_uniform));

    t1 = t.ScaledUniformLocal(2.0_r);
    EXPECT_TRUE(t1.ToMatrix().IsEqualApprox(m_base * m_uniform));
}

TEST_F(Transform3DTest, RotationAxisAngle) {
    Transform3D t(b_pitch, Vector3(1.0_r, 2.0_r, 3.0_r));
    Vector3 axis = Vector3(1.0_r, 2.0_r, 3.0_r);
    real angle = PI / 4.0_r;
    Matrix3x3 m = Matrix3x3::FromAxisAngle(axis, angle);
    Matrix4x4 m_rot = Matrix4x4(Vector4(m.row0), Vector4(m.row1), Vector4(m.row2), Vector4::UNIT_W);

    Transform3D t1 = t;
    t1.RotateAxisAngle(axis, angle);
    EXPECT_TRUE(t1.ToMatrix().IsEqualApprox(m_rot * t.ToMatrix()));

    t1 = t.RotatedAxisAngle(axis, angle);
    EXPECT_TRUE(t1.ToMatrix().IsEqualApprox(m_rot * t.ToMatrix()));
}

TEST_F(Transform3DTest, RotationAxisAngleLocal) {
    Transform3D t(b_pitch, Vector3(1.0_r, 2.0_r, 3.0_r));
    Vector3 axis = Vector3(1.0_r, 2.0_r, 3.0_r);
    real angle = PI / 4.0_r;
    Matrix3x3 m = Matrix3x3::FromAxisAngle(axis, angle);
    Matrix4x4 m_rot = Matrix4x4(Vector4(m.row0), Vector4(m.row1), Vector4(m.row2), Vector4::UNIT_W);

    Transform3D t1 = t;
    t1.RotateAxisAngleLocal(axis, angle);
    EXPECT_TRUE(t1.ToMatrix().IsEqualApprox(t.ToMatrix() * m_rot));

    t1 = t.RotatedAxisAngleLocal(axis, angle);
    EXPECT_TRUE(t1.ToMatrix().IsEqualApprox(t.ToMatrix() * m_rot));
}

TEST_F(Transform3DTest, RotationEuler) {
    Transform3D t(b_pitch, Vector3(1.0_r, 2.0_r, 3.0_r));
    real angle = PI / 4.0_r;

    Transform3D t1;

    Matrix3x3 m_rot = b_pitch.matrix * b_yaw.matrix * b_roll.matrix;
    Matrix4x4 m_rot4(Vector4(m_rot.row0), Vector4(m_rot.row1), Vector4(m_rot.row2), Vector4::UNIT_W);
    t1 = t;
    t1.RotateEuler(angle, angle, angle, XYZ);
    EXPECT_TRUE(t1.ToMatrix().IsEqualApprox(m_rot4 * t.ToMatrix()));

    t1 = t.RotatedEuler(angle, angle, angle, XYZ);
    EXPECT_TRUE(t1.ToMatrix().IsEqualApprox(m_rot4 * t.ToMatrix()));

    m_rot = b_pitch.matrix * b_roll.matrix * b_yaw.matrix;
    m_rot4 = Matrix4x4(Vector4(m_rot.row0), Vector4(m_rot.row1), Vector4(m_rot.row2), Vector4::UNIT_W);
    t1 = t;
    t1.RotateEuler(angle, angle, angle, XZY);
    EXPECT_TRUE(t1.ToMatrix().IsEqualApprox(m_rot4 * t.ToMatrix()));

    t1 = t.RotatedEuler(angle, angle, angle, XZY);
    EXPECT_TRUE(t1.ToMatrix().IsEqualApprox(m_rot4 * t.ToMatrix()));

    m_rot = b_yaw.matrix * b_roll.matrix * b_pitch.matrix;
    m_rot4 = Matrix4x4(Vector4(m_rot.row0), Vector4(m_rot.row1), Vector4(m_rot.row2), Vector4::UNIT_W);
    t1 = t;
    t1.RotateEuler(angle, angle, angle, YZX);
    EXPECT_TRUE(t1.ToMatrix().IsEqualApprox(m_rot4 * t.ToMatrix()));

    t1 = t.RotatedEuler(angle, angle, angle, YZX);
    EXPECT_TRUE(t1.ToMatrix().IsEqualApprox(m_rot4 * t.ToMatrix()));

    m_rot = b_yaw.matrix * b_pitch.matrix * b_roll.matrix;
    m_rot4 = Matrix4x4(Vector4(m_rot.row0), Vector4(m_rot.row1), Vector4(m_rot.row2), Vector4::UNIT_W);
    t1 = t;
    t1.RotateEuler(angle, angle, angle, YXZ);
    EXPECT_TRUE(t1.ToMatrix().IsEqualApprox(m_rot4 * t.ToMatrix()));

    t1 = t.RotatedEuler(angle, angle, angle, YXZ);
    EXPECT_TRUE(t1.ToMatrix().IsEqualApprox(m_rot4 * t.ToMatrix()));

    m_rot = b_roll.matrix * b_pitch.matrix * b_yaw.matrix;
    m_rot4 = Matrix4x4(Vector4(m_rot.row0), Vector4(m_rot.row1), Vector4(m_rot.row2), Vector4::UNIT_W);
    t1 = t;
    t1.RotateEuler(angle, angle, angle, ZXY);
    EXPECT_TRUE(t1.ToMatrix().IsEqualApprox(m_rot4 * t.ToMatrix()));

    t1 = t.RotatedEuler(angle, angle, angle, ZXY);
    EXPECT_TRUE(t1.ToMatrix().IsEqualApprox(m_rot4 * t.ToMatrix()));

    m_rot = b_roll.matrix * b_yaw.matrix * b_pitch.matrix;
    m_rot4 = Matrix4x4(Vector4(m_rot.row0), Vector4(m_rot.row1), Vector4(m_rot.row2), Vector4::UNIT_W);
    t1 = t;
    t1.RotateEuler(angle, angle, angle, ZYX);
    EXPECT_TRUE(t1.ToMatrix().IsEqualApprox(m_rot4 * t.ToMatrix()));

    t1 = t.RotatedEuler(angle, angle, angle, ZYX);
    EXPECT_TRUE(t1.ToMatrix().IsEqualApprox(m_rot4 * t.ToMatrix()));
}

TEST_F(Transform3DTest, RotationEulerLocal) {
    Transform3D t(b_pitch, Vector3(1.0_r, 2.0_r, 3.0_r));
    real angle = PI / 4.0_r;
    Transform3D t1;

    Matrix3x3 m_rot = b_pitch.matrix * b_yaw.matrix * b_roll.matrix;
    Matrix4x4 m_rot4(Vector4(m_rot.row0), Vector4(m_rot.row1), Vector4(m_rot.row2), Vector4::UNIT_W);
    t1 = t;
    t1.RotateEulerLocal(angle, angle, angle, XYZ);
    EXPECT_TRUE(t1.ToMatrix().IsEqualApprox(t.ToMatrix() * m_rot4));

    t1 = t.RotatedEulerLocal(angle, angle, angle, XYZ);
    EXPECT_TRUE(t1.ToMatrix().IsEqualApprox(t.ToMatrix() * m_rot4));

    m_rot = b_pitch.matrix * b_roll.matrix * b_yaw.matrix;
    m_rot4 = Matrix4x4(Vector4(m_rot.row0), Vector4(m_rot.row1), Vector4(m_rot.row2), Vector4::UNIT_W);
    t1 = t;
    t1.RotateEulerLocal(angle, angle, angle, XZY);
    EXPECT_TRUE(t1.ToMatrix().IsEqualApprox(t.ToMatrix() * m_rot4));

    t1 = t.RotatedEulerLocal(angle, angle, angle, XZY);
    EXPECT_TRUE(t1.ToMatrix().IsEqualApprox(t.ToMatrix() * m_rot4));

    m_rot = b_yaw.matrix * b_roll.matrix * b_pitch.matrix;
    m_rot4 = Matrix4x4(Vector4(m_rot.row0), Vector4(m_rot.row1), Vector4(m_rot.row2), Vector4::UNIT_W);
    t1 = t;
    t1.RotateEulerLocal(angle, angle, angle, YZX);
    EXPECT_TRUE(t1.ToMatrix().IsEqualApprox(t.ToMatrix() * m_rot4));

    t1 = t.RotatedEulerLocal(angle, angle, angle, YZX);
    EXPECT_TRUE(t1.ToMatrix().IsEqualApprox(t.ToMatrix() * m_rot4));

    m_rot = b_yaw.matrix * b_pitch.matrix * b_roll.matrix;
    m_rot4 = Matrix4x4(Vector4(m_rot.row0), Vector4(m_rot.row1), Vector4(m_rot.row2), Vector4::UNIT_W);
    t1 = t;
    t1.RotateEulerLocal(angle, angle, angle, YXZ);
    EXPECT_TRUE(t1.ToMatrix().IsEqualApprox(t.ToMatrix() * m_rot4));

    t1 = t.RotatedEulerLocal(angle, angle, angle, YXZ);
    EXPECT_TRUE(t1.ToMatrix().IsEqualApprox(t.ToMatrix() * m_rot4));

    m_rot = b_roll.matrix * b_pitch.matrix * b_yaw.matrix;
    m_rot4 = Matrix4x4(Vector4(m_rot.row0), Vector4(m_rot.row1), Vector4(m_rot.row2), Vector4::UNIT_W);
    t1 = t;
    t1.RotateEulerLocal(angle, angle, angle, ZXY);
    EXPECT_TRUE(t1.ToMatrix().IsEqualApprox(t.ToMatrix() * m_rot4));

    t1 = t.RotatedEulerLocal(angle, angle, angle, ZXY);
    EXPECT_TRUE(t1.ToMatrix().IsEqualApprox(t.ToMatrix() * m_rot4));

    m_rot = b_roll.matrix * b_yaw.matrix * b_pitch.matrix;
    m_rot4 = Matrix4x4(Vector4(m_rot.row0), Vector4(m_rot.row1), Vector4(m_rot.row2), Vector4::UNIT_W);
    t1 = t;
    t1.RotateEulerLocal(angle, angle, angle, ZYX);
    EXPECT_TRUE(t1.ToMatrix().IsEqualApprox(t.ToMatrix() * m_rot4));

    t1 = t.RotatedEulerLocal(angle, angle, angle, ZYX);
    EXPECT_TRUE(t1.ToMatrix().IsEqualApprox(t.ToMatrix() * m_rot4));
}

TEST_F(Transform3DTest, RotationQuaternion) {
    Transform3D t(b_pitch, Vector3(1.0_r, 2.0_r, 3.0_r));
    Vector3 axis(1.0_r, 2.0_r, 3.0_r);
    real angle = PI / 4.0_r;
    Quaternion q = Quaternion::FromAxisAngle(axis, angle);
    Matrix3x3 m = Matrix3x3::FromQuaternion(q);
    Matrix4x4 m_rot = Matrix4x4(Vector4(m.row0), Vector4(m.row1), Vector4(m.row2), Vector4::UNIT_W);

    Transform3D t1 = t;
    t1.RotateQuaternion(q);
    EXPECT_TRUE(t1.ToMatrix().IsEqualApprox(m_rot * t.ToMatrix()));

    t1 = t.RotatedQuaternion(q);
    EXPECT_TRUE(t1.ToMatrix().IsEqualApprox(m_rot * t.ToMatrix()));
}

TEST_F(Transform3DTest, RotationQuaternionLocal) {
    Transform3D t(b_pitch, Vector3(1.0_r, 2.0_r, 3.0_r));
    Vector3 axis(1.0_r, 2.0_r, 3.0_r);
    real angle = PI / 4.0_r;
    Quaternion q = Quaternion::FromAxisAngle(axis, angle);
    Matrix3x3 m = Matrix3x3::FromQuaternion(q);
    Matrix4x4 m_rot = Matrix4x4(Vector4(m.row0), Vector4(m.row1), Vector4(m.row2), Vector4::UNIT_W);

    Transform3D t1 = t;
    t1.RotateQuaternionLocal(q);
    EXPECT_TRUE(t1.ToMatrix().IsEqualApprox(t.ToMatrix() * m_rot));

    t1 = t.RotatedQuaternionLocal(q);
    EXPECT_TRUE(t1.ToMatrix().IsEqualApprox(t.ToMatrix() * m_rot));
}

TEST_F(Transform3DTest, Translation) {
    Transform3D t(b_scale, Vector3(1.0_r, 2.0_r, 3.0_r));
    Vector3 trans(3.0_r, 4.0_r, 5.0_r);
    Matrix4x4 m_trans(Vector4(1.0_r, 0.0_r, 0.0_r, trans.x), Vector4(0.0_r, 1.0_r, 0.0_r, trans.y),
                      Vector4(0.0_r, 0.0_r, 1.0_r, trans.z), Vector4(0.0_r, 0.0_r, 0.0_r, 1.0_r));

    Transform3D t1 = t;
    t1.Translate(trans);
    EXPECT_TRUE(t1.ToMatrix().IsEqualApprox(m_trans * t.ToMatrix()));

    t1 = t.Translated(trans);
    EXPECT_TRUE(t1.ToMatrix().IsEqualApprox(m_trans * t.ToMatrix()));
}

TEST_F(Transform3DTest, TranslationLocal) {
    Transform3D t(b_scale, Vector3(1.0_r, 2.0_r, 3.0_r));
    Vector3 trans(3.0_r, 4.0_r, 5.0_r);
    Matrix4x4 m_trans(Vector4(1.0_r, 0.0_r, 0.0_r, trans.x), Vector4(0.0_r, 1.0_r, 0.0_r, trans.y),
                      Vector4(0.0_r, 0.0_r, 1.0_r, trans.z), Vector4(0.0_r, 0.0_r, 0.0_r, 1.0_r));

    Transform3D t1 = t;
    t1.TranslateLocal(trans);
    EXPECT_TRUE(t1.ToMatrix().IsEqualApprox(t.ToMatrix() * m_trans));

    t1 = t.TranslatedLocal(trans);
    EXPECT_TRUE(t1.ToMatrix().IsEqualApprox(t.ToMatrix() * m_trans));
}

TEST_F(Transform3DTest, Inversion) {
    Transform3D t1 = t_non_ortho;
    t1.Invert();
    EXPECT_TRUE(t1.ToMatrix().IsEqualApprox(t_non_ortho.ToMatrix().Inverse()))
        << t1.Invert().ToString() << "\n"
        << t_non_ortho.ToMatrix().Inverse().ToString();

    t1 = t_non_ortho.Inverse();
    EXPECT_TRUE(t1.ToMatrix().IsEqualApprox(t_non_ortho.ToMatrix().Inverse()));
}

TEST_F(Transform3DTest, InversionFast) {
    Transform3D t1 = t_non_ortho;
    t1.InvertFast();
    EXPECT_FALSE(t1.ToMatrix().IsEqualApprox(t_non_ortho.ToMatrix().Inverse()));

    t1 = t_rot;
    t1.InvertFast();
    EXPECT_TRUE(t1.ToMatrix().IsEqualApprox(t_rot.ToMatrix().Inverse()));

    t1 = t_rot.InverseFast();
    EXPECT_TRUE(t1.ToMatrix().IsEqualApprox(t_rot.ToMatrix().Inverse()));
}

TEST_F(Transform3DTest, LookAt) {
    Vector3 at(1.0_r, 2.0_r, -3.0_r);
    Vector3 up(0.0_r, 1.0_r, 0.0_r);

    Transform3D t;
    t.LookAt(at, up, false);

    EXPECT_TRUE(t.basis.IsOrthogonal());

    Vector3 at_norm = at.Normalized();
    EXPECT_TRUE(t.basis.basis_z().IsEqualApprox(-at_norm));

    Vector3 right = t.basis.basis_x();
    Vector3 cross_xy = right.Cross(t.basis.basis_y());
    EXPECT_TRUE(cross_xy.IsEqualApprox(t.basis.basis_z()));
}

TEST_F(Transform3DTest, Multiplication) {
    Transform3D t1(b_scale, Vector3(1.0_r, 2.0_r, 3.0_r));
    Transform3D t2(b_yaw, Vector3(3.0_r, 4.0_r, 5.0_r));

    EXPECT_TRUE((t1 * t2).ToMatrix().IsEqualApprox(t1.ToMatrix() * t2.ToMatrix()));
    EXPECT_TRUE((t2 * t1).ToMatrix().IsEqualApprox(t2.ToMatrix() * t1.ToMatrix()));

    Transform3D t3 = t1;
    t3 *= t2;
    EXPECT_TRUE(t3.ToMatrix().IsEqualApprox(t1.ToMatrix() * t2.ToMatrix()));
}

TEST_F(Transform3DTest, TransformAndInvTransform) {
    Transform3D t(b_yaw, Vector3(1.0_r, 2.0_r, 3.0_r));
    Matrix4x4 m = t.ToMatrix();
    Vector3 v(1.0_r, 1.0_r, 1.0_r);

    Vector4 vh(v.x, v.y, v.z, 1.0_r);
    Vector3 expected = (m * vh).ToCartesian();

    EXPECT_TRUE(t.Transform(v).IsEqualApprox(expected));

    EXPECT_TRUE(t.InvTransform(v).IsEqualApprox((m.Inverse() * vh).ToCartesian()));

    EXPECT_FALSE(t_non_ortho.InvTransformFast(v).IsEqualApprox((t_non_ortho.ToMatrix().Inverse() * vh).ToCartesian()));

    EXPECT_TRUE(t_rot.InvTransformFast(v).IsEqualApprox((t_rot.ToMatrix().Inverse() * vh).ToCartesian()));
}

TEST_F(Transform3DTest, ToMatrix) {
    Transform3D t(b_scale, Vector3(1.0_r, 2.0_r, 3.0_r));
    Matrix4x4 m = t.ToMatrix();

    EXPECT_TRUE(m.row0.IsEqualApprox(Vector4(2.0_r, 0.0_r, 0.0_r, 1.0_r)));
    EXPECT_TRUE(m.row1.IsEqualApprox(Vector4(0.0_r, 3.0_r, 0.0_r, 2.0_r)));
    EXPECT_TRUE(m.row2.IsEqualApprox(Vector4(0.0_r, 0.0_r, 4.0_r, 3.0_r)));
    EXPECT_TRUE(m.row3.IsEqualApprox(Vector4(0.0_r, 0.0_r, 0.0_r, 1.0_r)));
}