#include <gtest/gtest.h>

#include "core/math/basis_2d.h"
#include "core/math/math_funcs.h"
#include "core/math/matrix3x3.h"
#include "core/math/transform_2d.h"
#include "core/math/vector2.h"

using namespace ho;
using namespace ho::math;

class Transform2DTest : public ::testing::Test {
 protected:
  void SetUp() override {
    b_default = Basis2D();
    b_scale = Basis2D(Vector2(2.0_r, 0.0_r), Vector2(0.0_r, 3.0_r));

    real angle = PI / 4.0_r;
    real s = std::sin(angle), c = std::cos(angle);
    b_rot = Basis2D(Matrix2x2(Vector2(c, -s), Vector2(s, c)));

    t_default = Transform2D();
    t_scale = Transform2D(b_scale, Vector2(1.0_r, 2.0_r));
    t_rot = Transform2D(b_rot, Vector2(3.0_r, 4.0_r));
    t_non_ortho =
        Transform2D(Basis2D(Vector2(1.0_r, 0.2_r), Vector2(0.3_r, 1.0_r)),
                    Vector2(2.0_r, 1.0_r));
  }

  Basis2D b_default, b_scale, b_rot;
  Transform2D t_default, t_scale, t_rot, t_non_ortho;
};

TEST_F(Transform2DTest, Constructors) {
  EXPECT_EQ(t_default.basis, b_default);
  EXPECT_EQ(t_default.origin, Vector2(0.0_r, 0.0_r));

  EXPECT_EQ(t_scale.basis, b_scale);
  EXPECT_EQ(t_scale.origin, Vector2(1.0_r, 2.0_r));

  Matrix3x3 m = t_scale.ToMatrix();
  Transform2D t_from_m(m);
  EXPECT_TRUE(t_from_m.ToMatrix().IsEqualApprox(m));
}

TEST_F(Transform2DTest, Assignment) {
  Transform2D t = t_scale;
  EXPECT_EQ(t, t_scale);
}

TEST_F(Transform2DTest, Accessors) {
  Transform2D t = t_scale;
  EXPECT_EQ(t.basis_x(), Vector2(2.0_r, 0.0_r));
  EXPECT_EQ(t.basis_y(), Vector2(0.0_r, 3.0_r));

  t.set_basis_x(Vector2(5.0_r, 1.0_r));
  t.set_basis_y(Vector2(3.0_r, 4.0_r));
  EXPECT_EQ(t.basis_x(), Vector2(5.0_r, 1.0_r));
  EXPECT_EQ(t.basis_y(), Vector2(3.0_r, 4.0_r));

  EXPECT_NEAR(t.scale_x(), t.basis_x().Magnitude(), math::EPSILON_CMP);
  EXPECT_NEAR(t.scale_y(), t.basis_y().Magnitude(), math::EPSILON_CMP);
}

TEST_F(Transform2DTest, ComparisonOperators) {
  Transform2D t1(b_scale, Vector2(1.0_r, 2.0_r));
  Transform2D t2(b_scale, Vector2(1.0_r, 2.0_r));
  Transform2D t3(b_rot, Vector2(3.0_r, 4.0_r));

  EXPECT_TRUE(t1 == t2);
  EXPECT_FALSE(t1 != t2);
  EXPECT_TRUE(t1 != t3);
}

TEST_F(Transform2DTest, ApproximateEqualityCheck) {
  Transform2D t1(b_scale, Vector2(1.0_r, 2.0_r));
  Transform2D t2(
      Basis2D(Vector2(2.000001_r, 0.0_r), Vector2(0.0_r, 3.000001_r)),
      Vector2(1.000001_r, 2.000001_r));
  EXPECT_TRUE(t1.IsEqualApprox(t2));
  EXPECT_FALSE(t1.IsNotEqualApprox(t2));
}

TEST_F(Transform2DTest, OrthogonalityCheck) {
  EXPECT_TRUE(t_rot.IsOrthogonal());
  EXPECT_FALSE(t_non_ortho.IsOrthogonal());
}

TEST_F(Transform2DTest, Orthogonalization) {
  Transform2D ortho = t_non_ortho.Orthogonalized();
  EXPECT_TRUE(ortho.IsOrthogonal());

  t_non_ortho.Orthogonalize();
  EXPECT_TRUE(t_non_ortho.IsOrthogonal());
}

TEST_F(Transform2DTest, OrthonormalityCheck) {
  EXPECT_FALSE(t_scale.IsOrthonormal());
  EXPECT_TRUE(t_rot.IsOrthonormal());
}

TEST_F(Transform2DTest, Orthonormalization) {
  EXPECT_FALSE(t_scale.IsOrthonormal());
  EXPECT_TRUE(t_rot.IsOrthonormal());

  Transform2D normed = t_non_ortho.Orthonormalized();
  EXPECT_TRUE(normed.IsOrthonormal());

  t_non_ortho.Orthonormalize();
  EXPECT_TRUE(t_non_ortho.IsOrthonormal());
}

TEST_F(Transform2DTest, Scaling) {
  Transform2D t(b_rot, Vector2(1.0_r, 2.0_r));
  Matrix3x3 m_base = t.ToMatrix();
  Matrix3x3 m_scale(Vector3(2.0_r, 0.0_r, 0.0_r), Vector3(0.0_r, 3.0_r, 0.0_r),
                    Vector3(0.0_r, 0.0_r, 1.0_r));

  Transform2D t1 = t;
  t1.Scale(Vector2(2.0_r, 3.0_r));
  EXPECT_TRUE(t1.ToMatrix().IsEqualApprox(m_scale * m_base));

  t1 = t.Scaled(Vector2(2.0_r, 3.0_r));
  EXPECT_TRUE(t1.ToMatrix().IsEqualApprox(m_scale * m_base));

  t1 = t;
  t1.ScaleUniform(2.0_r);
  Matrix3x3 m_uniform(Vector3(2.0_r, 0.0_r, 0.0_r),
                      Vector3(0.0_r, 2.0_r, 0.0_r),
                      Vector3(0.0_r, 0.0_r, 1.0_r));
  EXPECT_TRUE(t1.ToMatrix().IsEqualApprox(m_uniform * m_base));

  t1 = t.ScaledUniform(2.0_r);
  EXPECT_TRUE(t1.ToMatrix().IsEqualApprox(m_uniform * m_base));
}

TEST_F(Transform2DTest, ScalingLocal) {
  Transform2D t(b_rot, Vector2(1.0_r, 2.0_r));
  Matrix3x3 m_base = t.ToMatrix();
  Matrix3x3 m_scale(Vector3(2.0_r, 0.0_r, 0.0_r), Vector3(0.0_r, 3.0_r, 0.0_r),
                    Vector3(0.0_r, 0.0_r, 1.0_r));

  Transform2D t1 = t;
  t1.ScaleLocal(Vector2(2.0_r, 3.0_r));
  EXPECT_TRUE(t1.ToMatrix().IsEqualApprox(m_base * m_scale));

  t1 = t.ScaledLocal(Vector2(2.0_r, 3.0_r));
  EXPECT_TRUE(t1.ToMatrix().IsEqualApprox(m_base * m_scale));

  Matrix3x3 m_uniform(Vector3(2.0_r, 0.0_r, 0.0_r),
                      Vector3(0.0_r, 2.0_r, 0.0_r),
                      Vector3(0.0_r, 0.0_r, 1.0_r));
  t1 = t;
  t1.ScaleUniformLocal(2.0_r);
  EXPECT_TRUE(t1.ToMatrix().IsEqualApprox(m_base * m_uniform));

  t1 = t.ScaledUniformLocal(2.0_r);
  EXPECT_TRUE(t1.ToMatrix().IsEqualApprox(m_base * m_uniform));
}

TEST_F(Transform2DTest, Rotation) {
  Transform2D t(b_scale, Vector2(1.0_r, 2.0_r));
  Matrix3x3 m_base = t.ToMatrix();
  real angle = PI / 4.0_r;
  real s = std::sin(angle), c = std::cos(angle);
  Matrix3x3 m_rot(Vector3(c, -s, 0.0_r), Vector3(s, c, 0.0_r),
                  Vector3(0.0_r, 0.0_r, 1.0_r));

  Transform2D t1 = t;
  t1.Rotate(angle);
  EXPECT_TRUE(t1.ToMatrix().IsEqualApprox(m_rot * m_base))
      << t1.ToMatrix().ToString() << "\n"
      << (m_rot * m_base).ToString();

  t1 = t.Rotated(angle);
  EXPECT_TRUE(t1.ToMatrix().IsEqualApprox(m_rot * m_base));
}

TEST_F(Transform2DTest, RotationLocal) {
  Transform2D t(b_scale, Vector2(1.0_r, 2.0_r));
  Matrix3x3 m_base = t.ToMatrix();
  real angle = PI / 4.0_r;
  real s = std::sin(angle), c = std::cos(angle);
  Matrix3x3 m_rot(Vector3(c, -s, 0.0_r), Vector3(s, c, 0.0_r),
                  Vector3(0.0_r, 0.0_r, 1.0_r));

  Transform2D t1 = t;
  t1.RotateLocal(angle);
  EXPECT_TRUE(t1.ToMatrix().IsEqualApprox(m_base * m_rot));

  t1 = t.RotatedLocal(angle);
  EXPECT_TRUE(t1.ToMatrix().IsEqualApprox(m_base * m_rot));
}

TEST_F(Transform2DTest, Translation) {
  Transform2D t(b_scale, Vector2(1.0_r, 2.0_r));
  Matrix3x3 m_base = t.ToMatrix();

  Vector2 trans(3.0_r, 4.0_r);
  Matrix3x3 m_trans(Vector3(1.0_r, 0.0_r, trans.x),
                    Vector3(0.0_r, 1.0_r, trans.y),
                    Vector3(0.0_r, 0.0_r, 1.0_r));

  Transform2D t1 = t;
  t1.Translate(trans);
  EXPECT_TRUE(t1.ToMatrix().IsEqualApprox(m_trans * m_base));

  t1 = t.Translated(trans);
  EXPECT_TRUE(t1.ToMatrix().IsEqualApprox(m_trans * m_base));
}

TEST_F(Transform2DTest, TranslationLocal) {
  Transform2D t(b_scale, Vector2(1.0_r, 2.0_r));
  Matrix3x3 m_base = t.ToMatrix();

  Vector2 trans(3.0_r, 4.0_r);
  Matrix3x3 m_trans(Vector3(1.0_r, 0.0_r, trans.x),
                    Vector3(0.0_r, 1.0_r, trans.y),
                    Vector3(0.0_r, 0.0_r, 1.0_r));

  Transform2D t1 = t;
  t1.TranslateLocal(trans);
  EXPECT_TRUE(t1.ToMatrix().IsEqualApprox(m_base * m_trans));

  t1 = t.TranslatedLocal(trans);
  EXPECT_TRUE(t1.ToMatrix().IsEqualApprox(m_base * m_trans));
}

TEST_F(Transform2DTest, Inversion) {
  Transform2D t = t_non_ortho;
  Transform2D t1 = t;
  t1.Invert();
  EXPECT_TRUE(t1.ToMatrix().IsEqualApprox(t.ToMatrix().Inverse()))
      << t1.ToMatrix().ToString() << t.ToMatrix().Inverse().ToString();

  t1 = t.Inverse();
  EXPECT_TRUE(t1.ToMatrix().IsEqualApprox(t.ToMatrix().Inverse()));
}

TEST_F(Transform2DTest, InversionFast) {
  Transform2D t = t_non_ortho;
  Transform2D t1 = t;
  t1.InvertFast();
  EXPECT_FALSE(t1.ToMatrix().IsEqualApprox(t.ToMatrix().Inverse()));

  t1 = t_rot;
  t1.InvertFast();
  EXPECT_TRUE(t1.ToMatrix().IsEqualApprox(t_rot.ToMatrix().Inverse()))
      << t1.ToMatrix().ToString() << "\n" << t_rot.ToMatrix().Inverse().ToString();

  t1 = t_rot.InverseFast();
  EXPECT_TRUE(t1.ToMatrix().IsEqualApprox(t_rot.ToMatrix().Inverse()));
}

TEST_F(Transform2DTest, LookAt) {
  Vector2 at(1.0_r, 2.0_r);
  Transform2D t;
  t.LookAt(at);

  EXPECT_TRUE(t.basis.IsOrthonormal());
  Vector2 dir = at.Normalized();
  EXPECT_TRUE(t.basis.basis_y().IsEqualApprox(dir));
  EXPECT_NEAR(t.basis.basis_x().Dot(dir), 0.0_r, EPSILON_CMP);
}

TEST_F(Transform2DTest, Multiplication) {
  Transform2D t1(b_scale, Vector2(1.0_r, 2.0_r));
  Transform2D t2(b_rot, Vector2(3.0_r, 4.0_r));

  EXPECT_TRUE(
      (t1 * t2).ToMatrix().IsEqualApprox(t1.ToMatrix() * t2.ToMatrix()));
  EXPECT_TRUE(
      (t2 * t1).ToMatrix().IsEqualApprox(t2.ToMatrix() * t1.ToMatrix()));
}

TEST_F(Transform2DTest, TransformAndInvTransform) {
  Transform2D t(b_scale, Vector2(1.0_r, 2.0_r));
  Matrix3x3 m = t.ToMatrix();
  Vector2 v(2.0_r, 3.0_r);

  EXPECT_TRUE(
      t.Transform(v).IsEqualApprox(Vector2(m * Vector3(v.x, v.y, 1.0_r))));
  EXPECT_TRUE(t.InvTransform(v).IsEqualApprox(
      Vector2(m.Inverse() * Vector3(v.x, v.y, 1.0_r))));
  EXPECT_FALSE(t.InvTransformFast(v).IsEqualApprox(
      Vector2(m.Inverse() * Vector3(v.x, v.y, 1.0_r))));

  Transform2D t_ortho(b_rot, Vector2(1.0_r, 2.0_r));
  EXPECT_TRUE(t_ortho.InvTransformFast(v).IsEqualApprox(
      Vector2(t_ortho.ToMatrix().Inverse() * Vector3(v.x, v.y, 1.0_r))));
}
