#include <gtest/gtest.h>

#include "core/math/basis_2d.h"
#include "core/math/math_funcs.h"
#include "core/math/matrix2x2.h"
#include "core/math/vector2.h"

using namespace ho;
using namespace ho::math;

class Basis2DTest : public ::testing::Test {
   protected:
    void SetUp() override {
        b_default = Basis2D();
        b_scale = Basis2D(Vector2(2.0_r, 0.0_r), Vector2(0.0_r, 3.0_r));
        real angle = PI / 4.0_r;
        real s = std::sin(angle), c = std::cos(angle);
        b_rot = Basis2D(Matrix2x2(Vector2(c, s), Vector2(-s, c)));
        b_non_ortho = Basis2D(Vector2(1.0_r, 0.5_r), Vector2(0.3_r, 1.0_r));
    }

    Basis2D b_default, b_scale, b_rot, b_non_ortho;
};

TEST_F(Basis2DTest, Constructors) {
    EXPECT_EQ(b_default.row0, Vector2(1.0_r, 0.0_r));
    EXPECT_EQ(b_default.row1, Vector2(0.0_r, 1.0_r));

    EXPECT_EQ(b_scale.row0, Vector2(2.0_r, 0.0_r));
    EXPECT_EQ(b_scale.row1, Vector2(0.0_r, 3.0_r));

    Matrix2x2 m(Vector2(1.0_r, 2.0_r), Vector2(3.0_r, 4.0_r));
    Basis2D b_from_m(m);
    EXPECT_EQ(b_from_m.row0, Vector2(1.0_r, 2.0_r));
    EXPECT_EQ(b_from_m.row1, Vector2(3.0_r, 4.0_r));
}

TEST_F(Basis2DTest, Assignment) {
    Basis2D b = b_scale;
    EXPECT_EQ(b, b_scale);
}

TEST_F(Basis2DTest, Accessors) {
    Basis2D b = b_scale;
    EXPECT_EQ(b.basis_x(), Vector2(2.0_r, 0.0_r));
    EXPECT_EQ(b.basis_y(), Vector2(0.0_r, 3.0_r));

    b.set_basis_x(Vector2(5.0_r, 1.0_r));
    b.set_basis_y(Vector2(3.0_r, 4.0_r));
    EXPECT_EQ(b.basis_x(), Vector2(5.0_r, 1.0_r));
    EXPECT_EQ(b.basis_y(), Vector2(3.0_r, 4.0_r));

    EXPECT_NEAR(b.scale_x(), b.basis_x().Magnitude(), math::EPSILON_CMP);
    EXPECT_NEAR(b.scale_y(), b.basis_y().Magnitude(), math::EPSILON_CMP);
}

TEST_F(Basis2DTest, ComparisonOperators) {
    Basis2D b1(Vector2(1.0_r, 0.0_r), Vector2(0.0_r, 1.0_r));
    Basis2D b2(Vector2(1.0_r, 0.0_r), Vector2(0.0_r, 1.0_r));
    Basis2D b3(Vector2(2.0_r, 0.0_r), Vector2(0.0_r, 3.0_r));

    EXPECT_TRUE(b1 == b2);
    EXPECT_FALSE(b1 != b2);
    EXPECT_TRUE(b1 != b3);
}

TEST_F(Basis2DTest, ApproximateEqualityCheck) {
    Basis2D b1(Vector2(1.0_r, 0.0_r), Vector2(0.0_r, 1.0_r));
    Basis2D b2(Vector2(1.000001_r, 0.0_r), Vector2(0.0_r, 0.999999_r));
    EXPECT_TRUE(b1.IsEqualApprox(b2));
    EXPECT_FALSE(b1.IsNotEqualApprox(b2));
}

TEST_F(Basis2DTest, OrthogonalityCheck) {
    EXPECT_TRUE(b_scale.IsOrthogonal());
    EXPECT_FALSE(b_non_ortho.IsOrthogonal());
}

TEST_F(Basis2DTest, Orthogonalization) {
    Basis2D o = b_non_ortho.Orthogonalized();
    EXPECT_TRUE(o.IsOrthogonal());

    b_non_ortho.Orthogonalize();
    EXPECT_TRUE(b_non_ortho.IsOrthogonal());
}

TEST_F(Basis2DTest, OrthonormalityCheck) {
    EXPECT_TRUE(b_rot.IsOrthonormal());
    EXPECT_FALSE(b_scale.IsOrthonormal());
}

TEST_F(Basis2DTest, Orthonormalzation) {
    Basis2D n = b_scale.Orthonormalized();
    EXPECT_TRUE(n.IsOrthonormal());

    b_scale.Orthonormalize();
    EXPECT_TRUE(b_scale.IsOrthonormal());
}

TEST_F(Basis2DTest, Scaling) {
    Basis2D b(Vector2(1.0_r, 2.0_r), Vector2(3.0_r, 4.0_r));

    Basis2D b1 = b;
    b1.Scale(Vector2(2.0_r, 3.0_r));
    EXPECT_TRUE(b1.matrix.IsEqualApprox(b_scale.matrix * b.matrix));

    b1 = b;
    b1.ScaleLocal(Vector2(2.0_r, 3.0_r));
    EXPECT_TRUE(b1.matrix.IsEqualApprox(b.matrix * b_scale.matrix));

    b1 = b.Scaled(Vector2(2.0_r, 3.0_r));
    EXPECT_TRUE(b1.matrix.IsEqualApprox(b_scale.matrix * b.matrix));

    b1 = b.ScaledLocal(Vector2(2.0_r, 3.0_r));
    EXPECT_TRUE(b1.matrix.IsEqualApprox(b.matrix * b_scale.matrix));

    Matrix2x2 m_scale(Vector2(2.0_r, 0.0_r), Vector2(0.0_r, 2.0_r));

    b1 = b;
    b1.ScaleUniform(2.0_r);
    EXPECT_TRUE(b1.matrix.IsEqualApprox(m_scale * b.matrix));

    b1 = b.ScaledUniformLocal(2.0_r);
    EXPECT_TRUE(b1.matrix.IsEqualApprox(b.matrix * m_scale));
}

TEST_F(Basis2DTest, Rotation) {
    Basis2D b(Vector2(1.0_r, 2.0_r), Vector2(3.0_r, 4.0_r));

    Basis2D b1 = b;
    b1.Rotate(PI / 4.0_r);
    EXPECT_TRUE(b1.matrix.IsEqualApprox(b_rot.matrix * b.matrix));

    b1 = b;
    b1.RotateLocal(PI / 4.0_r);
    EXPECT_TRUE(b1.matrix.IsEqualApprox(b.matrix * b_rot.matrix));

    b1 = b.Rotated(PI / 4.0_r);
    EXPECT_TRUE(b1.matrix.IsEqualApprox(b_rot.matrix * b.matrix));

    b1 = b.RotatedLocal(PI / 4.0_r);
    EXPECT_TRUE(b1.matrix.IsEqualApprox(b.matrix * b_rot.matrix));
}

TEST_F(Basis2DTest, Inversion) {
    Basis2D b1 = b_non_ortho;
    b1.Invert();
    EXPECT_TRUE(b1.matrix.IsEqualApprox(b_non_ortho.matrix.Inverse()));

    b1 = b_non_ortho.Inverse();
    EXPECT_TRUE(b1.matrix.IsEqualApprox(b_non_ortho.matrix.Inverse()));
}

TEST_F(Basis2DTest, InversionFast) {
    Basis2D b1 = b_non_ortho;
    b1.InvertFast();
    EXPECT_FALSE(b1.matrix.IsEqualApprox(b_non_ortho.matrix.Inverse()));

    b1 = b_rot;
    b1.InvertFast();
    EXPECT_TRUE(b1.matrix.IsEqualApprox(b_rot.matrix.Inverse()));

    b1 = b_rot.InverseFast();
    EXPECT_TRUE(b1.matrix.IsEqualApprox(b_rot.matrix.Inverse()));
}

TEST_F(Basis2DTest, LookAt) {
    Basis2D b = b_scale;
    b.LookAt(Vector2(1.0_r, 0.0_r));

    EXPECT_TRUE(b.IsOrthogonal());
    EXPECT_TRUE(b.IsEqualApprox(Basis2D(Vector2(0.0_r, -2.0_r), Vector2(3.0_r, 0.0_r))));
}

TEST_F(Basis2DTest, Multiplication) {
    Basis2D b1(Vector2(1.0_r, 2.0_r), Vector2(3.0_r, 4.0_r));
    Basis2D b2(Vector2(4.0_r, 3.0_r), Vector2(2.0_r, 1.0_r));
    Matrix2x2 m1 = Matrix2x2(Vector2(1.0_r, 2.0_r), Vector2(3.0_r, 4.0_r)).Transposed();
    Matrix2x2 m2 = Matrix2x2(Vector2(4.0_r, 3.0_r), Vector2(2.0_r, 1.0_r)).Transposed();

    EXPECT_TRUE((b1 * b2).matrix.IsEqualApprox(m1 * m2));
    EXPECT_TRUE((b2 * b1).matrix.IsEqualApprox(m2 * m1));
}

TEST_F(Basis2DTest, TransformAndInvTransform) {
    Basis2D b1(Vector2(1.0_r, 2.0_r), Vector2(3.0_r, 4.0_r));
    Matrix2x2 m1 = Matrix2x2(Vector2(1.0_r, 2.0_r), Vector2(3.0_r, 4.0_r)).Transposed();
    Vector2 v(1.0_r, 1.0_r);

    EXPECT_TRUE(b1.Transform(v).IsEqualApprox(m1 * v));
    EXPECT_TRUE(b1.InvTransform(v).IsEqualApprox(m1.Inverse() * v));
    EXPECT_FALSE(b1.InvTransformFast(v).IsEqualApprox(m1.Inverse() * v));
    EXPECT_TRUE(b_rot.InvTransformFast(v).IsEqualApprox(b_rot.matrix.Inverse() * v));
}
