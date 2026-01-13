#include <gtest/gtest.h>

#include "core/math/math_funcs.h"
#include "core/math/matrix4x4.h"
#include "core/math/projection.h"
#include "core/math/vector4.h"

using namespace ho;
using namespace ho::math;

class ProjectionTest : public ::testing::Test {
   protected:
    void SetUp() override {
        fov = math::DegToRad(60.0_r);
        width = 1920.0_r;
        height = 1080.0_r;
        aspect = width / height;
        near_z = 0.1_r;
        far_z = 100.0_r;
    }

    real fov, width, height, aspect, near_z, far_z;
    Projection proj;
};

TEST_F(ProjectionTest, CreationOrthographicCheck) {
    aspect = width / height;

    proj = Projection::CreateOrthographic(width, height, near_z, far_z);

    real inv_range = 1.0_r / (far_z - near_z);

    Matrix4x4 expected_matrix({2.0_r / width, 0.0_r, 0.0_r, 0.0_r}, {0.0_r, 2.0_r / height, 0.0_r, 0.0_r},
                              {0.0_r, 0.0_r, -2.0_r * inv_range, -(far_z + near_z) * inv_range},
                              {0.0_r, 0.0_r, 0.0_r, 1.0_r});

    EXPECT_TRUE(proj.matrix.IsEqualApprox(expected_matrix));
    EXPECT_NEAR(proj.get_fov(), 0.0_r, 1e-3);
    EXPECT_NEAR(proj.get_aspect_ratio(), aspect, 1e-3);
    EXPECT_NEAR(proj.get_near_dist(), near_z, 1e-3);
    EXPECT_NEAR(proj.get_far_dist(), far_z, 1e-3);

    Projection proj_m = Projection::CreateOrthographic(expected_matrix);
    EXPECT_TRUE(proj.IsEqualApprox(proj_m));
}

TEST_F(ProjectionTest, CreationPerspectiveCheck) {
    proj = Projection::CreatePerspective(fov, width, height, near_z, far_z);

    real tan_half_fov = math::Tan(fov * 0.5_r);
    real inv_nf = 1.0_r / (near_z - far_z);

    Matrix4x4 expected_matrix(
        {1.0_r / (aspect * tan_half_fov), 0.0_r, 0.0_r, 0.0_r}, {0.0_r, 1.0_r / tan_half_fov, 0.0_r, 0.0_r},
        {0.0_r, 0.0_r, (near_z + far_z) * inv_nf, (2.0_r * near_z * far_z) * inv_nf}, {0.0_r, 0.0_r, -1.0_r, 0.0_r});

    EXPECT_TRUE(proj.matrix.IsEqualApprox(expected_matrix));
    EXPECT_NEAR(proj.get_fov(), fov, 1e-3);
    EXPECT_NEAR(proj.get_aspect_ratio(), aspect, 1e-3);
    EXPECT_NEAR(proj.get_near_dist(), near_z, 1e-3);
    EXPECT_NEAR(proj.get_far_dist(), far_z, 1e-3);

    Projection proj_m = Projection::CreatePerspective(expected_matrix);
    EXPECT_TRUE(proj.IsEqualApprox(proj_m));
}

TEST_F(ProjectionTest, Assignment_Orthographic) {
    Projection dst = Projection::CreateOrthographic(100.0_r, 100.0_r, 0.0_r, 100.0_r);

    Projection src_same = Projection::CreateOrthographic(200.0_r, 200.0_r, 10.0_r, 50.0_r);

    Projection src_diff = Projection::CreatePerspective(math::DegToRad(60.0_r), 1920.0_r, 1080.0_r, 0.1_r, 1000.0_r);

    dst = src_same;

    EXPECT_EQ(dst, src_same);

    dst = src_diff;

    EXPECT_EQ(dst, src_diff);
}

TEST_F(ProjectionTest, Assignment_Perspective) {
    Projection dst = Projection::CreatePerspective(math::DegToRad(45.0_r), 800.0_r, 600.0_r, 0.1_r, 100.0_r);

    Projection src_same = Projection::CreatePerspective(math::DegToRad(90.0_r), 1920.0_r, 1080.0_r, 1.0_r, 500.0_r);

    Projection src_diff = Projection::CreateOrthographic(100.0_r, 100.0_r, 0.0_r, 100.0_r);

    dst = src_same;

    EXPECT_EQ(dst, src_same);

    dst = src_diff;

    EXPECT_EQ(dst, src_diff);
}

TEST_F(ProjectionTest, Accessors_Orthographic) {
    proj = Projection::CreateOrthographic(width, height, near_z, far_z);

    EXPECT_NEAR(proj.get_aspect_ratio(), aspect, 1e-3);
    EXPECT_NEAR(proj.get_near_dist(), near_z, 1e-3);
    EXPECT_NEAR(proj.get_far_dist(), far_z, 1e-3);
    EXPECT_NEAR(proj.get_fov(), 0.0_r, 1e-3);

    real original_m11 = proj.matrix.data[1][1];

    proj.set_aspect_ratio(200.0_r, 100.0_r);

    EXPECT_NEAR(proj.get_aspect_ratio(), 2.0_r, 1e-3);
    EXPECT_EQ(proj.matrix.data[1][1], original_m11);
    EXPECT_NEAR(proj.matrix.data[0][0], original_m11 / 2.0_r, 1e-3);

    real new_near = 10.0_r;
    real new_far = 50.0_r;
    proj.set_near_dist(new_near);
    proj.set_far_dist(new_far);

    EXPECT_NEAR(proj.get_near_dist(), new_near, 1e-3);
    EXPECT_NEAR(proj.get_far_dist(), new_far, 1e-3);

    real expected_m22 = -2.0_r / (new_far - new_near);
    EXPECT_NEAR(proj.matrix.data[2][2], expected_m22, 1e-3);
}

TEST_F(ProjectionTest, Accessors_Perspective) {
    proj = Projection::CreatePerspective(fov, width, height, near_z, far_z);

    EXPECT_NEAR(proj.get_fov(), fov, 1e-3);
    EXPECT_NEAR(proj.get_aspect_ratio(), aspect, 1e-3);
    EXPECT_NEAR(proj.get_near_dist(), near_z, 1e-3);
    EXPECT_NEAR(proj.get_far_dist(), far_z, 1e-3);

    real new_fov = math::DegToRad(90.0_r);
    proj.set_fov(new_fov);

    EXPECT_NEAR(proj.get_fov(), new_fov, 1e-3);
    EXPECT_NEAR(proj.matrix.data[1][1], 1.0_r / math::Tan(new_fov * 0.5_r), 1e-3);

    real m11_fixed = proj.matrix.data[1][1];

    proj.set_aspect_ratio(800.0_r, 600.0_r);

    EXPECT_NEAR(proj.get_aspect_ratio(), 4.0_r / 3.0_r, 1e-3);
    EXPECT_EQ(proj.matrix.data[1][1], m11_fixed);

    EXPECT_NEAR(proj.matrix.data[0][0], m11_fixed / (4.0_r / 3.0_r), 1e-3);

    real new_near = 1.0_r;
    real new_far = 500.0_r;
    proj.set_near_dist(new_near);
    proj.set_far_dist(new_far);

    EXPECT_NEAR(proj.get_near_dist(), new_near, 1e-3);
    EXPECT_NEAR(proj.get_far_dist(), new_far, 1e-3);

    real expected_m22 = (new_near + new_far) / (new_near - new_far);
    EXPECT_NEAR(proj.matrix.data[2][2], expected_m22, 1e-3);
}

TEST_F(ProjectionTest, ComparisonOperators_Orthographic) {
    Projection p1 = Projection::CreateOrthographic(100.0_r, 100.0_r, 0.0_r, 100.0_r);

    Projection p2 = Projection::CreateOrthographic(100.0_r, 100.0_r, 0.0_r, 100.0_r);

    Projection p3 = Projection::CreateOrthographic(100.0_r, 200.0_r, 0.0_r, 100.0_r);

    Projection p_persp = Projection::CreatePerspective(math::DegToRad(60.0_r), 100.0_r, 100.0_r, 0.1_r, 100.0_r);

    EXPECT_TRUE(p1 == p2);
    EXPECT_FALSE(p1 != p2);

    EXPECT_FALSE(p1 == p3);
    EXPECT_TRUE(p1 != p3);

    EXPECT_FALSE(p1 == p_persp);
    EXPECT_TRUE(p1 != p_persp);
}

TEST_F(ProjectionTest, ComparisonOperators_Perspective) {
    Projection p1 = Projection::CreatePerspective(fov, width, height, near_z, far_z);

    Projection p2 = Projection::CreatePerspective(fov, width, height, near_z, far_z);

    Projection p3 = Projection::CreatePerspective(math::DegToRad(90.0_r), width, height, near_z, far_z);

    Projection p_ortho = Projection::CreateOrthographic(width, height, near_z, far_z);

    EXPECT_TRUE(p1 == p2);
    EXPECT_FALSE(p1 != p2);

    EXPECT_FALSE(p1 == p3);
    EXPECT_TRUE(p1 != p3);

    EXPECT_FALSE(p1 == p_ortho);
    EXPECT_TRUE(p1 != p_ortho);
}

TEST_F(ProjectionTest, ApproximateEqualityCheck_Orthographic) {
    Projection p1 = Projection::CreateOrthographic(100.0_r, 100.0_r, 0.0_r, 100.0_r);

    real epsilon = 0.000001_r;
    Projection p_approx = Projection::CreateOrthographic(100.0_r, 100.0_r + epsilon, 0.0_r, 100.0_r);

    EXPECT_TRUE(p1.IsEqualApprox(p_approx));
    EXPECT_FALSE(p1.IsNotEqualApprox(p_approx));

    Projection p_diff = Projection::CreateOrthographic(100.0_r, 100.1_r, 0.0_r, 100.0_r);

    EXPECT_FALSE(p1.IsEqualApprox(p_diff));
    EXPECT_TRUE(p1.IsNotEqualApprox(p_diff));

    Projection p_persp = Projection::CreatePerspective(math::DegToRad(60.0_r), 100.0_r, 100.0_r, 0.1_r, 100.0_r);

    EXPECT_FALSE(p1.IsEqualApprox(p_persp));
}

TEST_F(ProjectionTest, ApproximateEqualityCheck_Perspective) {
    Projection p1 = Projection::CreatePerspective(fov, width, height, near_z, far_z);

    real epsilon = 0.000001_r;
    Projection p_approx = Projection::CreatePerspective(fov + epsilon, width, height, near_z, far_z);

    EXPECT_TRUE(p1.IsEqualApprox(p_approx));
    EXPECT_FALSE(p1.IsNotEqualApprox(p_approx));

    Projection p_diff = Projection::CreatePerspective(fov + 0.1_r, width, height, near_z, far_z);

    EXPECT_FALSE(p1.IsEqualApprox(p_diff));
    EXPECT_TRUE(p1.IsNotEqualApprox(p_diff));

    Projection p_ortho = Projection::CreateOrthographic(width, height, near_z, far_z);

    EXPECT_FALSE(p1.IsEqualApprox(p_ortho));
}