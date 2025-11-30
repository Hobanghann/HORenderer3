#include <gtest/gtest.h>

#include "core/math/aabb.h"
#include "core/math/frustum.h"
#include "core/math/matrix4x4.h"
#include "core/math/plane.h"
#include "core/math/sphere.h"
#include "core/math/vector3.h"

using namespace ho;
using namespace math;

class FrustumTest : public ::testing::Test {
   protected:
    void SetUp() override {
        p_default = Plane();
        left = Plane(Vector3(-1.0_r, 0.0_r, 0.0_r), Vector3(-1.0_r, 0.0_r, 0.0_r));
        right = Plane(Vector3(1.0_r, 0.0_r, 0.0_r), Vector3(1.0_r, 0.0_r, 0.0_r));
        bottom = Plane(Vector3(0.0_r, -1.0_r, 0.0_r), Vector3(0.0_r, -1.0_r, 0.0_r));
        top = Plane(Vector3(0.0_r, 1.0_r, 0.0_r), Vector3(0.0_r, 1.0_r, 0.0_r));
        near = Plane(Vector3(0.0_r, 0.0_r, -1.0_r), Vector3(0.0_r, 0.0_r, -1.0_r));
        far = Plane(Vector3(0.0_r, 0.0_r, 1.0_r), Vector3(0.0_r, 0.0_r, 1.0_r));

        f_default = Frustum();
        f1 = Frustum(left, right, bottom, top, near, far);
    }

    Frustum f_default, f1;
    Plane p_default, left, right, bottom, top, near, far;
};

TEST_F(FrustumTest, Constructors) {
    EXPECT_EQ(f_default.planes[PLANE_POS_LEFT], p_default);
    EXPECT_EQ(f_default.planes[PLANE_POS_RIGHT], p_default);
    EXPECT_EQ(f_default.planes[PLANE_POS_BOTTOM], p_default);
    EXPECT_EQ(f_default.planes[PLANE_POS_TOP], p_default);
    EXPECT_EQ(f_default.planes[PLANE_POS_NEAR], p_default);
    EXPECT_EQ(f_default.planes[PLANE_POS_FAR], p_default);

    EXPECT_EQ(f1.planes[PLANE_POS_LEFT], left);
    EXPECT_EQ(f1.planes[PLANE_POS_RIGHT], right);
    EXPECT_EQ(f1.planes[PLANE_POS_BOTTOM], bottom);
    EXPECT_EQ(f1.planes[PLANE_POS_TOP], top);
    EXPECT_EQ(f1.planes[PLANE_POS_NEAR], near);
    EXPECT_EQ(f1.planes[PLANE_POS_FAR], far);
}

TEST_F(FrustumTest, Assignment) {
    f_default = f1;

    for (int i = 0; i < 6; ++i) {
        EXPECT_TRUE(f_default.planes[static_cast<PlanePos>(i)].IsEqualApprox(f1.planes[static_cast<PlanePos>(i)]));
    }
    EXPECT_EQ(f_default, f1);
}

TEST_F(FrustumTest, ComparisonOperators) {
    Frustum f2 = f1;

    EXPECT_TRUE(f1 == f2);
    EXPECT_FALSE(f1 != f2);
}

TEST_F(FrustumTest, ApproximateEqualityCheck) {
    Plane far2(Vector3(0.0_r, 0.0_r, 1.0_r), Vector3(0.0_r, 0.0_r, 1.000001_r));
    Frustum f2(left, right, bottom, top, near, far2);

    EXPECT_TRUE(f1.IsEqualApprox(f2));
    Plane far3(Vector3(0.0_r, 0.0_r, 1.0_r), Vector3(0.0_r, 0.0_r, 5.0_r));
    Frustum f3(left, right, bottom, top, near, far3);

    EXPECT_FALSE(f1.IsEqualApprox(f3));
    EXPECT_TRUE(f1.IsNotEqualApprox(f3));
}

TEST_F(FrustumTest, GetSideSinglePoint) {
    Vector3 inside(0.0_r, 0.0_r, 0.0_r);
    Vector3 outsideX(2.0_r, 0.0_r, 0.0_r);
    Vector3 outsideY(0.0_r, 2.0_r, 0.0_r);
    Vector3 outsideZ(0.0_r, 0.0_r, 2.0_r);

    EXPECT_EQ(f1.GetSide(inside), math::INSIDE);
    EXPECT_EQ(f1.GetSide(outsideX), math::OUTSIDE);
    EXPECT_EQ(f1.GetSide(outsideY), math::OUTSIDE);
    EXPECT_EQ(f1.GetSide(outsideZ), math::OUTSIDE);
}

TEST_F(FrustumTest, GetSideTwoPoints) {
    Vector3 inside1(0.0_r, 0.0_r, 0.0_r);
    Vector3 inside2(0.5_r, -0.2_r, 0.7_r);
    Vector3 outside_posZ(0.0_r, 0.0_r, 2.0_r);
    Vector3 outside_posX(2.0_r, 0.0_r, 0.0_r);

    EXPECT_EQ(f1.GetSide(inside1, inside2), math::INSIDE);

    EXPECT_EQ(f1.GetSide(inside1, outside_posZ), math::INTERSECT);
    EXPECT_EQ(f1.GetSide(inside2, outside_posX), math::INTERSECT);

    EXPECT_EQ(f1.GetSide(outside_posZ, Vector3(0.0_r, 0.0_r, 3.0_r)), math::OUTSIDE);
    EXPECT_EQ(f1.GetSide(outside_posX, Vector3(3.0_r, 0.0_r, 0.0_r)), math::OUTSIDE);
}

TEST_F(FrustumTest, GetSideThreePoints) {
    Vector3 inA(0.0_r, 0.0_r, 0.0_r);
    Vector3 inB(0.2_r, 0.3_r, -0.7_r);
    Vector3 inC(-0.8_r, 0.1_r, 0.9_r);

    Vector3 outZ1(0.0_r, 0.0_r, 2.0_r);
    Vector3 outZ2(0.5_r, 0.2_r, 3.0_r);
    Vector3 outZ3(-0.4_r, -0.3_r, 2.5_r);

    Vector3 mixOutX(2.0_r, 0.0_r, 0.0_r);

    EXPECT_EQ(f1.GetSide(inA, inB, inC), math::INSIDE);
    EXPECT_EQ(f1.GetSide(inA, inB, mixOutX), math::INTERSECT);
    EXPECT_EQ(f1.GetSide(inA, outZ1, inC), math::INTERSECT);
    EXPECT_EQ(f1.GetSide(outZ1, outZ2, outZ3), math::OUTSIDE);
}

TEST_F(FrustumTest, GetSideAABB) {
    AABB inside(Vector3(-0.5_r, -0.5_r, -0.5_r), Vector3(0.5_r, 0.5_r, 0.5_r));
    EXPECT_EQ(f1.GetSide(inside), math::INSIDE);

    AABB intersect(Vector3(0.5_r, -0.5_r, -0.5_r), Vector3(1.5_r, 0.5_r, 0.5_r));
    EXPECT_EQ(f1.GetSide(intersect), math::INTERSECT);

    AABB outside(Vector3(2.0_r, 2.0_r, 2.0_r), Vector3(3.0_r, 3.0_r, 3.0_r));
    EXPECT_EQ(f1.GetSide(outside), math::OUTSIDE);
}

TEST_F(FrustumTest, GetSideSphere) {
    Sphere s_inside(Vector3(0.0_r, 0.0_r, 0.0_r), 0.5_r);
    EXPECT_EQ(f1.GetSide(s_inside), math::INSIDE);

    Sphere s_inter(Vector3(1.2_r, 0.0_r, 0.0_r), 0.5_r);
    EXPECT_EQ(f1.GetSide(s_inter), math::INTERSECT);

    Sphere s_out(Vector3(2.0_r, 0.0_r, 0.0_r), 0.2_r);
    EXPECT_EQ(f1.GetSide(s_out), math::OUTSIDE);
}

TEST_F(FrustumTest, FromMatrix4x4) {
    Matrix4x4 m = Matrix4x4::IDENTITY;
    Frustum f = Frustum::FromMatrix4x4(m);

    for (int i = 0; i < 6; ++i) {
        auto n = f.planes[static_cast<PlanePos>(i)].normal;
        EXPECT_NEAR(n.Magnitude(), 1.0_r, math::EPSILON_CMP);
    }

    EXPECT_FALSE(f.planes[PLANE_POS_LEFT].IsEqualApprox(f.planes[PLANE_POS_RIGHT]));
    EXPECT_FALSE(f.planes[PLANE_POS_TOP].IsEqualApprox(f.planes[PLANE_POS_BOTTOM]));
    EXPECT_FALSE(f.planes[PLANE_POS_NEAR].IsEqualApprox(f.planes[PLANE_POS_FAR]));

    EXPECT_TRUE(f1.IsNotEqualApprox(f));
}
