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
    aspect = 16.0_r / 9.0_r;
    near_z = 0.1_r;
    far_z = 100.0_r;

    p_default = Projection();
    p_169 = Projection(fov, aspect, near_z, far_z);
    p_wh = Projection(fov, 1920.0_r, 1080.0_r, near_z, far_z);
    proj_m = Matrix4x4(
        {1.0_r / math::Tan(fov * 0.5_r) / aspect, 0.0_r, 0.0_r, 0.0_r},
        {0.0_r, 1.0_r / math::Tan(fov * 0.5_r), 0.0_r, 0.0_r},
        {0.0_r, 0.0_r, -(near_z + far_z) * (1.0_r / (near_z - far_z)),
         -(2.0_r * near_z * far_z) * (1.0_r / (near_z - far_z))},
        {0.0_r, 0.0_r, -1.0_r, 0.0_r});
    p_from_m = Projection(proj_m);
  }

  real fov, aspect, near_z, far_z;
  Projection p_default, p_169, p_wh, p_from_m;
  Matrix4x4 proj_m;
};

TEST_F(ProjectionTest, Constructors) {
  EXPECT_EQ(p_default, Projection(Matrix4x4()));
  EXPECT_EQ(p_169.matrix, proj_m);
  EXPECT_EQ(p_wh.matrix, proj_m);
  EXPECT_EQ(p_from_m.matrix, proj_m);
  EXPECT_NEAR(p_from_m.get_fov(), fov, 1e-3);
  EXPECT_NEAR(p_from_m.get_aspect_ratio(), aspect, 1e-3);
  EXPECT_NEAR(p_from_m.get_near_dist(), near_z, 1e-3);
  EXPECT_NEAR(p_from_m.get_far_dist(), far_z, 1e-3);
}

TEST_F(ProjectionTest, Assignment) {
  Projection p = p_169;
  EXPECT_EQ(p, p_169);
}

TEST_F(ProjectionTest, AccessorFOV) {
  EXPECT_FLOAT_EQ(p_169.get_fov(), fov);
  p_169.set_fov(math::DegToRad(90.0_r));
  EXPECT_FLOAT_EQ(p_169.get_fov(), PI / 2.0_r);
}

TEST_F(ProjectionTest, AccessorAspectRatio) {
  EXPECT_FLOAT_EQ(p_169.get_aspect_ratio(), aspect);
  p_169.set_aspect_ratio(4.0_r / 3.0_r);
  EXPECT_FLOAT_EQ(p_169.get_aspect_ratio(), 4.0_r / 3.0_r);
  p_169.set_aspect_ratio(1920.0_r, 1080.0_r);
  EXPECT_FLOAT_EQ(p_169.get_aspect_ratio(), aspect);
}

TEST_F(ProjectionTest, AccessorNearDistance) {
  EXPECT_FLOAT_EQ(p_169.get_near_dist(), 0.1_r);
  p_169.set_near_dist(5.0_r);
  EXPECT_FLOAT_EQ(p_169.get_near_dist(), 5.0_r);
}

TEST_F(ProjectionTest, AccessorFarDistance) {
  EXPECT_FLOAT_EQ(p_169.get_far_dist(), 100.0_r);
  p_169.set_far_dist(1000.0_r);
  EXPECT_FLOAT_EQ(p_169.get_far_dist(), 1000.0_r);
}

TEST_F(ProjectionTest, ComparisonOperators) {
  Projection p1(fov, aspect, near_z, far_z);
  Projection p2(fov, aspect, near_z, far_z);
  Projection p3(math::DegToRad(70.0_r), aspect, near_z, far_z);

  EXPECT_TRUE(p1 == p2);
  EXPECT_FALSE(p1 != p2);
  EXPECT_TRUE(p1 != p3);
}

TEST_F(ProjectionTest, IsEqualApproxCheck) {
  Projection p1(fov, aspect, near_z, far_z);
  Projection p2(fov + 0.000001_r, aspect, near_z, far_z);
  EXPECT_TRUE(p1.IsEqualApprox(p2));
  EXPECT_FALSE(p1.IsNotEqualApprox(p2));
}

TEST_F(ProjectionTest, Projection) {
  Projection p(fov, aspect, near_z, far_z);

  Vector4 point(0.0_r, 0.0_r, -1.0_r, 1.0_r);
  Vector4 projected = p.Project(point);

  EXPECT_FALSE(math::IsZeroApprox(projected.w));

  Vector4 ndc = projected / projected.w;
  EXPECT_GE(ndc.z, -1.0_r - math::EPSILON_CMP);
  EXPECT_LE(ndc.z, 1.0_r + math::EPSILON_CMP);
}
