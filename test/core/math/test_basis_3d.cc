#include <gtest/gtest.h>

#include "core/math/basis_3d.h"
#include "core/math/math_funcs.h"
#include "core/math/matrix3x3.h"
#include "core/math/quaternion.h"
#include "core/math/vector3.h"

using namespace ho;
using namespace ho::math;

class Basis3DTest : public ::testing::Test {
 protected:
  void SetUp() override {
    b_default = Basis3D();
    b_scale =
        Basis3D(Vector3(2.0_r, 0.0_r, 0.0_r), Vector3(0.0_r, 3.0_r, 0.0_r),
                Vector3(0.0_r, 0.0_r, 4.0_r));

    real angle = PI / 4.0_r;
    real s = std::sin(angle), c = std::cos(angle);
    b_pitch = Basis3D(Matrix3x3(Vector3(1.0_r, 0.0_r, 0.0_r),
                                Vector3(0.0_r, c, -s), Vector3(0.0_r, s, c)));
    b_yaw =
        Basis3D(Matrix3x3(Vector3(c, 0.0_r, s), Vector3(0.0_r, 1.0_r, 0.0_r),
                          Vector3(-s, 0.0_r, c)));
    b_roll = Basis3D(Matrix3x3(Vector3(c, -s, 0.0_r), Vector3(s, c, 0.0_r),
                               Vector3(0.0_r, 0.0_r, 1.0_r)));

    b_non_ortho =
        Basis3D(Vector3(1.0_r, 0.2_r, 0.3_r), Vector3(0.5_r, 1.0_r, 0.0_r),
                Vector3(0.0_r, 0.5_r, 1.0_r));
  }

  Basis3D b_default, b_scale, b_pitch, b_yaw, b_roll, b_non_ortho;
};

TEST_F(Basis3DTest, Constructors) {
  EXPECT_EQ(b_default.row0, Vector3(1.0_r, 0.0_r, 0.0_r));
  EXPECT_EQ(b_default.row1, Vector3(0.0_r, 1.0_r, 0.0_r));
  EXPECT_EQ(b_default.row2, Vector3(0.0_r, 0.0_r, 1.0_r));

  EXPECT_EQ(b_scale.row0, Vector3(2.0_r, 0.0_r, 0.0_r));
  EXPECT_EQ(b_scale.row1, Vector3(0.0_r, 3.0_r, 0.0_r));
  EXPECT_EQ(b_scale.row2, Vector3(0.0_r, 0.0_r, 4.0_r));

  Matrix3x3 m(Vector3(1.0_r, 2.0_r, 3.0_r), Vector3(4.0_r, 5.0_r, 6.0_r),
              Vector3(7.0_r, 8.0_r, 9.0_r));
  Basis3D b_from_m(m);
  EXPECT_EQ(b_from_m.matrix, m);

  Quaternion q = Quaternion::FromAxisAngle(Vector3::UNIT_Y, PI / 4.0_r);
  Basis3D b_from_q(q);
  EXPECT_TRUE(b_from_q.IsEqualApprox(b_yaw));
}

TEST_F(Basis3DTest, Assignment) {
  Basis3D b = b_scale;
  EXPECT_EQ(b, b_scale);
}

TEST_F(Basis3DTest, Accessors) {
  Basis3D b = b_scale;
  EXPECT_EQ(b.basis_x(), Vector3(2.0_r, 0.0_r, 0.0_r));
  EXPECT_EQ(b.basis_y(), Vector3(0.0_r, 3.0_r, 0.0_r));
  EXPECT_EQ(b.basis_z(), Vector3(0.0_r, 0.0_r, 4.0_r));

  b.set_basis_x(Vector3(1.0_r, 2.0_r, 3.0_r));
  b.set_basis_y(Vector3(4.0_r, 5.0_r, 6.0_r));
  b.set_basis_z(Vector3(7.0_r, 8.0_r, 9.0_r));
  EXPECT_EQ(b.basis_x(), Vector3(1.0_r, 2.0_r, 3.0_r));
  EXPECT_EQ(b.basis_y(), Vector3(4.0_r, 5.0_r, 6.0_r));
  EXPECT_EQ(b.basis_z(), Vector3(7.0_r, 8.0_r, 9.0_r));

  EXPECT_NEAR(b.scale_x(), b.basis_x().Magnitude(), math::EPSILON_CMP);
  EXPECT_NEAR(b.scale_y(), b.basis_y().Magnitude(), math::EPSILON_CMP);
  EXPECT_NEAR(b.scale_z(), b.basis_z().Magnitude(), math::EPSILON_CMP);
}

TEST_F(Basis3DTest, ComparisonOperators) {
  Basis3D b1(Vector3(1.0_r, 0.0_r, 0.0_r), Vector3(0.0_r, 1.0_r, 0.0_r),
             Vector3(0.0_r, 0.0_r, 1.0_r));
  Basis3D b2 = b1;
  Basis3D b3 = b_scale;

  EXPECT_TRUE(b1 == b2);
  EXPECT_FALSE(b1 != b2);
  EXPECT_TRUE(b1 != b3);
}

TEST_F(Basis3DTest, ApproximateEqualityCheck) {
  Basis3D b1(Vector3(1.0_r, 0.0_r, 0.0_r), Vector3(0.0_r, 1.0_r, 0.0_r),
             Vector3(0.0_r, 0.0_r, 1.0_r));
  Basis3D b2(Vector3(1.000001_r, 0.0_r, 0.0_r),
             Vector3(0.0_r, 1.000001_r, 0.0_r),
             Vector3(0.0_r, 0.0_r, 0.999999_r));
  EXPECT_TRUE(b1.IsEqualApprox(b2));
  EXPECT_FALSE(b1.IsNotEqualApprox(b2));
}

TEST_F(Basis3DTest, OrthogonalityCheck) {
  EXPECT_TRUE(b_scale.IsOrthogonal());
  EXPECT_FALSE(b_non_ortho.IsOrthogonal());
}

TEST_F(Basis3DTest, Orthogonalization) {
  Basis3D o = b_non_ortho.Orthogonalized();
  EXPECT_TRUE(o.IsOrthogonal());

  b_non_ortho.Orthogonalize();
  EXPECT_TRUE(b_non_ortho.IsOrthogonal());
}

TEST_F(Basis3DTest, OrthonormalityCheck) {
  EXPECT_TRUE(b_yaw.IsOrthonormal());
  EXPECT_FALSE(b_scale.IsOrthonormal());
}

TEST_F(Basis3DTest, Orthonormalization) {
  Basis3D n = b_non_ortho.Orthonormalized();
  EXPECT_TRUE(n.IsOrthonormal());

  b_non_ortho.Orthonormalize();
  EXPECT_TRUE(b_non_ortho.IsOrthonormal());
}

TEST_F(Basis3DTest, Scaling) {
  Basis3D b(Vector3(1.0_r, 2.0_r, 3.0_r), Vector3(0.0_r, 1.0_r, 1.0_r),
            Vector3(1.0_r, 0.0_r, 1.0_r));

  Basis3D b1 = b;
  b1.Scale(Vector3(2.0_r, 3.0_r, 4.0_r));
  EXPECT_TRUE(b1.matrix.IsEqualApprox(b_scale.matrix * b.matrix));

  b1 = b.Scaled(Vector3(2.0_r, 3.0_r, 4.0_r));
  EXPECT_TRUE(b1.matrix.IsEqualApprox(b_scale.matrix * b.matrix));

  Matrix3x3 m_scale(Vector3(2.0_r, 0.0_r, 0.0_r), Vector3(0.0_r, 2.0_r, 0.0_r),
                    Vector3(0.0_r, 0.0_r, 2.0_r));

  b1 = b;
  b1.ScaleUniform(2.0_r);
  EXPECT_TRUE(b1.matrix.IsEqualApprox(m_scale * b.matrix));

  b1 = b.ScaledUniform(2.0_r);
  EXPECT_TRUE(b1.matrix.IsEqualApprox(m_scale * b.matrix));
}

TEST_F(Basis3DTest, ScalingLocal) {
  Basis3D b(Vector3(1.0_r, 2.0_r, 3.0_r), Vector3(0.0_r, 1.0_r, 1.0_r),
            Vector3(1.0_r, 0.0_r, 1.0_r));
  Basis3D b1;

  b1 = b;
  b1.ScaleLocal(Vector3(2.0_r, 3.0_r, 4.0_r));
  EXPECT_TRUE(b1.matrix.IsEqualApprox(b.matrix * b_scale.matrix));

  b1 = b.ScaledLocal(Vector3(2.0_r, 3.0_r, 4.0_r));
  EXPECT_TRUE(b1.matrix.IsEqualApprox(b.matrix * b_scale.matrix));

  Matrix3x3 m_scale(Vector3(2.0_r, 0.0_r, 0.0_r), Vector3(0.0_r, 2.0_r, 0.0_r),
                    Vector3(0.0_r, 0.0_r, 2.0_r));

  b1 = b;
  b1.ScaleUniformLocal(2.0_r);
  EXPECT_TRUE(b1.matrix.IsEqualApprox(b.matrix * m_scale));

  b1 = b.ScaledUniformLocal(2.0_r);
  EXPECT_TRUE(b1.matrix.IsEqualApprox(b.matrix * m_scale));
}

TEST_F(Basis3DTest, RotationEuler) {
  Basis3D b(Vector3(1.0_r, 2.0_r, 3.0_r), Vector3(4.0_r, 5.0_r, 6.0_r),
            Vector3(7.0_r, 8.0_r, 9.0_r));
  Basis3D b1;

  b1 = b;
  b1.RotateEuler(PI / 4.0_r, PI / 4.0_r, PI / 4.0_r, XYZ);
  EXPECT_TRUE(b1.matrix.IsEqualApprox(b_pitch.matrix * b_yaw.matrix *
                                      b_roll.matrix * b.matrix));

  b1 = b;
  b1.RotateEuler(PI / 4.0_r, PI / 4.0_r, PI / 4.0_r, XZY);
  EXPECT_TRUE(b1.matrix.IsEqualApprox(b_pitch.matrix * b_roll.matrix *
                                      b_yaw.matrix * b.matrix));

  b1 = b;
  b1.RotateEuler(PI / 4.0_r, PI / 4.0_r, PI / 4.0_r, YZX);
  EXPECT_TRUE(b1.matrix.IsEqualApprox(b_yaw.matrix * b_roll.matrix *
                                      b_pitch.matrix * b.matrix));

  b1 = b;
  b1.RotateEuler(PI / 4.0_r, PI / 4.0_r, PI / 4.0_r, YXZ);
  EXPECT_TRUE(b1.matrix.IsEqualApprox(b_yaw.matrix * b_pitch.matrix *
                                      b_roll.matrix * b.matrix));

  b1 = b;
  b1.RotateEuler(PI / 4.0_r, PI / 4.0_r, PI / 4.0_r, ZXY);
  EXPECT_TRUE(b1.matrix.IsEqualApprox(b_roll.matrix * b_pitch.matrix *
                                      b_yaw.matrix * b.matrix));

  b1 = b;
  b1.RotateEuler(PI / 4.0_r, PI / 4.0_r, PI / 4.0_r, ZYX);
  EXPECT_TRUE(b1.matrix.IsEqualApprox(b_roll.matrix * b_yaw.matrix *
                                      b_pitch.matrix * b.matrix));

  b1 = b.RotatedEuler(PI / 4.0_r, PI / 4.0_r, PI / 4.0_r, XYZ);
  EXPECT_TRUE(b1.matrix.IsEqualApprox(b_pitch.matrix * b_yaw.matrix *
                                      b_roll.matrix * b.matrix));

  b1 = b.RotatedEuler(PI / 4.0_r, PI / 4.0_r, PI / 4.0_r, XZY);
  EXPECT_TRUE(b1.matrix.IsEqualApprox(b_pitch.matrix * b_roll.matrix *
                                      b_yaw.matrix * b.matrix));

  b1 = b.RotatedEuler(PI / 4.0_r, PI / 4.0_r, PI / 4.0_r, YZX);
  EXPECT_TRUE(b1.matrix.IsEqualApprox(b_yaw.matrix * b_roll.matrix *
                                      b_pitch.matrix * b.matrix));

  b1 = b.RotatedEuler(PI / 4.0_r, PI / 4.0_r, PI / 4.0_r, YXZ);
  EXPECT_TRUE(b1.matrix.IsEqualApprox(b_yaw.matrix * b_pitch.matrix *
                                      b_roll.matrix * b.matrix));

  b1 = b.RotatedEuler(PI / 4.0_r, PI / 4.0_r, PI / 4.0_r, ZXY);
  EXPECT_TRUE(b1.matrix.IsEqualApprox(b_roll.matrix * b_pitch.matrix *
                                      b_yaw.matrix * b.matrix));

  b1 = b.RotatedEuler(PI / 4.0_r, PI / 4.0_r, PI / 4.0_r, ZYX);
  EXPECT_TRUE(b1.matrix.IsEqualApprox(b_roll.matrix * b_yaw.matrix *
                                      b_pitch.matrix * b.matrix));
}

TEST_F(Basis3DTest, RotationAxisAngle) {
  Basis3D b(Vector3(1.0_r, 2.0_r, 3.0_r), Vector3(4.0_r, 3.0_r, 6.0_r),
            Vector3(7.0_r, 6.0_r, 9.0_r));

  Vector3 axis = Vector3(1.0_r, 2.0_r, 3.0_r);
  real angle = PI / 4.0_r;

  Matrix3x3 m_rot = Matrix3x3::FromAxisAngle(axis, angle);

  Basis3D b1;
  b1 = b;
  b1.RotateAxisAngle(axis, angle);
  EXPECT_TRUE(b1.matrix.IsEqualApprox(m_rot * b.matrix));

  b1 = b.RotatedAxisAngle(axis, angle);
  EXPECT_TRUE(b1.matrix.IsEqualApprox(m_rot * b.matrix));
}

TEST_F(Basis3DTest, RotationQuaternion) {
  Basis3D b(Vector3(1.0_r, 2.0_r, 3.0_r), Vector3(4.0_r, 5.0_r, 6.0_r),
            Vector3(7.0_r, 8.0_r, 9.0_r));

  Vector3 axis = Vector3(1.0_r, 2.0_r, 3.0_r);
  real angle = PI / 4.0_r;
  Quaternion q = Quaternion::FromAxisAngle(axis, angle);
  Matrix3x3 m_rot = Matrix3x3::FromQuaternion(q);

  Basis3D b1;
  b1 = b;
  b1.RotateQuaternion(q);
  EXPECT_TRUE(b1.matrix.IsEqualApprox(m_rot * b.matrix));

  b1 = b.RotatedQuaternion(q);
  EXPECT_TRUE(b1.matrix.IsEqualApprox(m_rot * b.matrix));
}

TEST_F(Basis3DTest, RotationEulerLocal) {
  Basis3D b(Vector3(1.0_r, 2.0_r, 3.0_r), Vector3(4.0_r, 5.0_r, 6.0_r),
            Vector3(7.0_r, 8.0_r, 9.0_r));
  Basis3D b1;

  b1 = b;
  b1.RotateEulerLocal(PI / 4.0_r, PI / 4.0_r, PI / 4.0_r, XYZ);
  EXPECT_TRUE(b1.matrix.IsEqualApprox(b.matrix * b_pitch.matrix * b_yaw.matrix *
                                      b_roll.matrix));

  b1 = b;
  b1.RotateEulerLocal(PI / 4.0_r, PI / 4.0_r, PI / 4.0_r, XZY);
  EXPECT_TRUE(b1.matrix.IsEqualApprox(b.matrix * b_pitch.matrix *
                                      b_roll.matrix * b_yaw.matrix));

  b1 = b;
  b1.RotateEulerLocal(PI / 4.0_r, PI / 4.0_r, PI / 4.0_r, YZX);
  EXPECT_TRUE(b1.matrix.IsEqualApprox(b.matrix * b_yaw.matrix * b_roll.matrix *
                                      b_pitch.matrix));

  b1 = b;
  b1.RotateEulerLocal(PI / 4.0_r, PI / 4.0_r, PI / 4.0_r, YXZ);
  EXPECT_TRUE(b1.matrix.IsEqualApprox(b.matrix * b_yaw.matrix * b_pitch.matrix *
                                      b_roll.matrix));

  b1 = b;
  b1.RotateEulerLocal(PI / 4.0_r, PI / 4.0_r, PI / 4.0_r, ZXY);
  EXPECT_TRUE(b1.matrix.IsEqualApprox(b.matrix * b_roll.matrix *
                                      b_pitch.matrix * b_yaw.matrix));

  b1 = b;
  b1.RotateEulerLocal(PI / 4.0_r, PI / 4.0_r, PI / 4.0_r, ZYX);
  EXPECT_TRUE(b1.matrix.IsEqualApprox(b.matrix * b_roll.matrix * b_yaw.matrix *
                                      b_pitch.matrix));

  b1 = b.RotatedEulerLocal(PI / 4.0_r, PI / 4.0_r, PI / 4.0_r, XYZ);
  EXPECT_TRUE(b1.matrix.IsEqualApprox(b.matrix * b_pitch.matrix * b_yaw.matrix *
                                      b_roll.matrix ));

  b1 = b.RotatedEulerLocal(PI / 4.0_r, PI / 4.0_r, PI / 4.0_r, XZY);
  EXPECT_TRUE(b1.matrix.IsEqualApprox(b.matrix * b_pitch.matrix *
                                      b_roll.matrix * b_yaw.matrix));

  b1 = b.RotatedEulerLocal(PI / 4.0_r, PI / 4.0_r, PI / 4.0_r, YZX);
  EXPECT_TRUE(b1.matrix.IsEqualApprox(b.matrix * b_yaw.matrix * b_roll.matrix *
                                      b_pitch.matrix));

  b1 = b.RotatedEulerLocal(PI / 4.0_r, PI / 4.0_r, PI / 4.0_r, YXZ);
  EXPECT_TRUE(b1.matrix.IsEqualApprox(b.matrix * b_yaw.matrix * b_pitch.matrix *
                                      b_roll.matrix));

  b1 = b.RotatedEulerLocal(PI / 4.0_r, PI / 4.0_r, PI / 4.0_r, ZXY);
  EXPECT_TRUE(b1.matrix.IsEqualApprox(b.matrix * b_roll.matrix *
                                      b_pitch.matrix * b_yaw.matrix));

  b1 = b.RotatedEulerLocal(PI / 4.0_r, PI / 4.0_r, PI / 4.0_r, ZYX);
  EXPECT_TRUE(b1.matrix.IsEqualApprox(b.matrix * b_roll.matrix * b_yaw.matrix *
                                      b_pitch.matrix));
}

TEST_F(Basis3DTest, RotationAxisAngleLocal) {
  Basis3D b(Vector3(1.0_r, 2.0_r, 3.0_r), Vector3(4.0_r, 5.0_r, 6.0_r),
            Vector3(7.0_r, 8.0_r, 9.0_r));

  Vector3 axis = Vector3(1.0_r, 2.0_r, 3.0_r);
  real angle = PI / 4.0_r;

  Matrix3x3 m_rot = Matrix3x3::FromAxisAngle(axis, angle);

  Basis3D b1;
  b1 = b;
  b1.RotateAxisAngleLocal(axis, angle);
  EXPECT_TRUE(b1.matrix.IsEqualApprox(b.matrix * m_rot));

  b1 = b.RotatedAxisAngleLocal(axis, angle);
  EXPECT_TRUE(b1.matrix.IsEqualApprox(b.matrix * m_rot));
}

TEST_F(Basis3DTest, RotationQuaternionLocal) {
  Basis3D b(Vector3(1.0_r, 2.0_r, 3.0_r), Vector3(4.0_r, 5.0_r, 6.0_r),
            Vector3(7.0_r, 8.0_r, 9.0_r));

  Vector3 axis = Vector3(1.0_r, 2.0_r, 3.0_r);
  real angle = PI / 4.0_r;
  Quaternion q = Quaternion::FromAxisAngle(axis, angle);
  Matrix3x3 m_rot = Matrix3x3::FromQuaternion(q);

  Basis3D b1;
  b1 = b;
  b1.RotateQuaternionLocal(q);
  EXPECT_TRUE(b1.matrix.IsEqualApprox(b.matrix * m_rot));

  b1 = b.RotatedQuaternionLocal(q);
  EXPECT_TRUE(b1.matrix.IsEqualApprox(b.matrix * m_rot));
}

TEST_F(Basis3DTest, Inversion) {
  Basis3D b1 = b_non_ortho;
  b1.Invert();
  EXPECT_TRUE(b1.matrix.IsEqualApprox(b_non_ortho.matrix.Inverse()));

  b1 = b_non_ortho.Inverse();
  EXPECT_TRUE(b1.matrix.IsEqualApprox(b_non_ortho.matrix.Inverse()));
}

TEST_F(Basis3DTest, InversionFast) {
  Basis3D b1 = b_non_ortho;
  b1.InvertFast();
  EXPECT_FALSE(b1.matrix.IsEqualApprox(b_non_ortho.matrix.Inverse()));

  b1 = b_pitch;
  b1.InvertFast();
  EXPECT_TRUE(b1.matrix.IsEqualApprox(b_pitch.matrix.Inverse()));

  b1 = b_pitch.InverseFast();
  EXPECT_TRUE(b1.matrix.IsEqualApprox(b_pitch.matrix.Inverse()));
}

TEST_F(Basis3DTest, LookAt) {
  Vector3 at(1.0_r, 2.0_r, -3.0_r);
  Vector3 up(0.0_r, 1.0_r, 0.0_r);

  Basis3D b;
  b.LookAt(at, up);

  EXPECT_TRUE(b.IsOrthogonal());

  Vector3 at_norm = at.Normalized();
  EXPECT_FLOAT_EQ(b.basis_z().Normalized().Dot(at_norm), -1.0_r);

  real dot_up_y = up.Normalized().Dot(b.basis_y().Normalized());
  EXPECT_GT(dot_up_y, 0.0_r);

  Vector3 right = b.basis_x();
  Vector3 cross_xy_norm = right.Cross(b.basis_y()).Normalized();
  EXPECT_FLOAT_EQ(cross_xy_norm.Dot(at_norm), -1.0_r);
}

TEST_F(Basis3DTest, LookAtFacingTarget) {
  Vector3 at(1.0_r, 2.0_r, -3.0_r);
  Vector3 up(0.0_r, 1.0_r, 0.0_r);

  Basis3D b;
  b.LookAt(at, up, true);

  EXPECT_TRUE(b.IsOrthogonal());

  Vector3 at_norm = at.Normalized();
  EXPECT_FLOAT_EQ(b.basis_z().Normalized().Dot(at_norm), 1.0_r);

  real dot_up_y = up.Normalized().Dot(b.basis_y().Normalized());
  EXPECT_GT(dot_up_y, 0.0_r);

  Vector3 right = b.basis_x();
  Vector3 cross_xy_norm = right.Cross(b.basis_y()).Normalized();
  EXPECT_TRUE(cross_xy_norm.IsEqualApprox(b.basis_z().Normalized()));
}
TEST_F(Basis3DTest, Multiplication) {
  Basis3D b1(Vector3(1.0_r, 2.0_r, 3.0_r), Vector3(0.0_r, 1.0_r, 2.0_r),
             Vector3(3.0_r, 0.0_r, 1.0_r));

  Basis3D b2(Vector3(2.0_r, 0.0_r, 1.0_r), Vector3(1.0_r, 2.0_r, 0.0_r),
             Vector3(0.0_r, 1.0_r, 3.0_r));

  EXPECT_TRUE((b1 * b2).matrix.IsEqualApprox(b1.matrix * b2.matrix));
  EXPECT_TRUE((b2 * b1).matrix.IsEqualApprox(b2.matrix * b1.matrix));
}

TEST_F(Basis3DTest, TransformAndInvTransform) {
  Basis3D b1(Vector3(1.0_r, 2.0_r, 3.0_r), Vector3(0.0_r, 1.0_r, 2.0_r),
             Vector3(3.0_r, 0.0_r, 1.0_r));

  Matrix3x3 m1 = b1.matrix;
  Vector3 v(1.0_r, 1.0_r, 1.0_r);

  EXPECT_TRUE(b1.Transform(v).IsEqualApprox(m1 * v));
  EXPECT_TRUE(b1.InvTransform(v).IsEqualApprox(m1.Inverse() * v));
  EXPECT_FALSE(b1.InvTransformFast(v).IsEqualApprox(m1.Inverse() * v));
  EXPECT_TRUE(
      b_pitch.InvTransformFast(v).IsEqualApprox(b_pitch.matrix.Inverse() * v));
}
