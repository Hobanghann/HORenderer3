#include <gtest/gtest.h>

#include "core/math/aabb.h"
#include "core/math/math_funcs.h"
#include "core/math/plane.h"
#include "core/math/sphere.h"
#include "core/math/vector3.h"

using namespace ho;
using namespace ho::math;

TEST(PlaneTest, Constructors) {
    Plane p;
    EXPECT_FLOAT_EQ(p.normal.x, 0.0_r);
    EXPECT_FLOAT_EQ(p.normal.y, 0.0_r);
    EXPECT_FLOAT_EQ(p.normal.z, 0.0_r);
    EXPECT_FLOAT_EQ(p.d, 0.0_r);

    Plane p1(1.0_r, 2.0_r, 3.0_r, -6.0_r);
    const real len = Vector3(1.0_r, 2.0_r, 3.0_r).Magnitude();
    EXPECT_NEAR(p1.normal.x, 1.0_r / len, math::EPSILON_CMP);
    EXPECT_NEAR(p1.normal.y, 2.0_r / len, math::EPSILON_CMP);
    EXPECT_NEAR(p1.normal.z, 3.0_r / len, math::EPSILON_CMP);
    EXPECT_NEAR(p1.d, -6.0_r / len, math::EPSILON_CMP);

    Vector3 normal(0.0_r, 1.0_r, 0.0_r);
    Vector3 point(0.0_r, 5.0_r, 0.0_r);
    Plane p2(normal, point);
    EXPECT_TRUE(p2.normal.IsEqualApprox(Vector3(0.0_r, 1.0_r, 0.0_r)));
    EXPECT_NEAR(p2.d, -5.0_r, math::EPSILON_CMP);

    Vector3 v1(0.0_r, 0.0_r, 0.0_r);
    Vector3 v2(1.0_r, 0.0_r, 0.0_r);
    Vector3 v3(0.0_r, 1.0_r, 0.0_r);

    Plane p3(v1, v2, v3, math::COUNTER_CLOCK_WISE);
    EXPECT_TRUE(p3.normal.IsEqualApprox(Vector3(0.0_r, 0.0_r, 1.0_r)));
    EXPECT_NEAR(p3.d, 0.0_r, math::EPSILON_CMP);

    Plane p4(v1, v2, v3, math::CLOCK_WISE);
    EXPECT_TRUE(p4.normal.IsEqualApprox(Vector3(0.0_r, 0.0_r, -1.0_r)));
    EXPECT_NEAR(p4.d, 0.0_r, math::EPSILON_CMP);
}

TEST(PlaneTest, Assignment) {
    Plane p(1.0_r, 2.0_r, 3.0_r, -6.0_r);
    Plane p1;
    p1 = p;

    EXPECT_EQ(p, p1);
}

TEST(PlaneTest, ComparisonOperators) {
    Plane p1(0.0_r, 1.0_r, 0.0_r, 5.0_r);
    Plane p2(0.0_r, 1.0_r, 0.0_r, 5.0_r);
    Plane p3(0.0_r, 1.0_r, 0.0_r, 6.0_r);

    EXPECT_TRUE(p1 == p2);
    EXPECT_FALSE(p1 != p2);
    EXPECT_FALSE(p1 == p3);
    EXPECT_TRUE(p1 != p3);
}

TEST(PlaneTest, ApproximateEqualityCheck) {
    Plane p1(0.0_r, 1.0_r, 0.0_r, 5.0_r);
    Plane p2(0.0_r, 1.000001_r, 0.0_r, 5.000001_r);

    EXPECT_TRUE(p1.IsEqualApprox(p2));
    EXPECT_FALSE(p1.IsNotEqualApprox(p2));
}

TEST(PlaneTest, DistToPoint) {
    Plane p(Vector3(0.0_r, 1.0_r, 0.0_r), Vector3(0.0_r, 5.0_r, 0.0_r));

    Vector3 front(0.0_r, 10.0_r, 0.0_r);
    Vector3 on(0.0_r, 5.0_r, 0.0_r);
    Vector3 back(0.0_r, 0.0_r, 0.0_r);

    EXPECT_NEAR(p.DistToPoint(front), 5.0_r, math::EPSILON_CMP);
    EXPECT_NEAR(p.DistToPoint(on), 0.0_r, math::EPSILON_CMP);
    EXPECT_NEAR(p.DistToPoint(back), -5.0_r, math::EPSILON_CMP);
}

TEST(PlaneTest, Project) {
    Plane p(Vector3(0.0_r, 1.0_r, 0.0_r), Vector3(0.0_r, 5.0_r, 0.0_r));

    Vector3 front(0.0_r, 10.0_r, 0.0_r);
    Vector3 on(0.0_r, 5.0_r, 0.0_r);
    Vector3 back(0.0_r, 0.0_r, 0.0_r);

    EXPECT_TRUE(p.Project(front).IsEqualApprox(Vector3(0.0_r, 5.0_r, 0.0_r)));
    EXPECT_TRUE(p.Project(on).IsEqualApprox(Vector3(0.0_r, 5.0_r, 0.0_r)));
    EXPECT_TRUE(p.Project(back).IsEqualApprox(Vector3(0.0_r, 5.0_r, 0.0_r)));
}

TEST(PlaneTest, GetSideSinglePoint) {
    Plane p(Vector3(0.0_r, 1.0_r, 0.0_r), Vector3(0.0_r, 5.0_r, 0.0_r));

    Vector3 front(0.0_r, 10.0_r, 0.0_r);
    Vector3 on(0.0_r, 5.0_r, 0.0_r);
    Vector3 back(0.0_r, 0.0_r, 0.0_r);

    EXPECT_EQ(p.GetSide(front), math::OUTSIDE);
    EXPECT_EQ(p.GetSide(on), math::ON);
    EXPECT_EQ(p.GetSide(back), math::INSIDE);
}

TEST(PlaneTest, GetSideTwoPoints) {
    Plane p(Vector3(0.0_r, 1.0_r, 0.0_r), Vector3(0.0_r, 5.0_r, 0.0_r));

    Vector3 a(0.0_r, 10.0_r, 0.0_r);  // OUTSIDE
    Vector3 b(0.0_r, 0.0_r, 0.0_r);   // INSIDE
    Vector3 c(0.0_r, 6.0_r, 0.0_r);   // OUTSIDE
    Vector3 d(0.0_r, 4.0_r, 0.0_r);   // INSIDE

    EXPECT_EQ(p.GetSide(a, c), math::OUTSIDE);
    EXPECT_EQ(p.GetSide(b, d), math::INSIDE);
    EXPECT_EQ(p.GetSide(a, b), math::INTERSECT);
}

TEST(PlaneTest, GetSideThreePoints) {
    Plane p(Vector3(0.0_r, 1.0_r, 0.0_r), Vector3(0.0_r, 5.0_r, 0.0_r));

    Vector3 front1(0.0_r, 10.0_r, 0.0_r);
    Vector3 front2(1.0_r, 8.0_r, 0.0_r);
    Vector3 front3(-1.0_r, 6.0_r, 0.0_r);
    EXPECT_EQ(p.GetSide(front1, front2, front3), math::OUTSIDE);

    Vector3 on1(0.0_r, 5.0_r, 0.0_r);
    Vector3 on2(1.0_r, 5.0_r, 0.0_r);
    Vector3 on3(-1.0_r, 5.0_r, 0.0_r);
    EXPECT_EQ(p.GetSide(on1, on2, on3), math::INSIDE);

    Vector3 mixed1(0.0_r, 10.0_r, 0.0_r);
    Vector3 mixed2(0.0_r, 5.0_r, 0.0_r);
    Vector3 mixed3(0.0_r, 0.0_r, 0.0_r);
    EXPECT_EQ(p.GetSide(mixed1, mixed2, mixed3), math::INTERSECT);
}

TEST(PlaneTest, GetSideAABB) {
    Plane p(Vector3(0.0_r, 1.0_r, 0.0_r), Vector3(0.0_r, 5.0_r, 0.0_r));

    AABB above(Vector3(-1.0_r, 6.0_r, -1.0_r), Vector3(1.0_r, 7.0_r, 1.0_r));
    EXPECT_EQ(p.GetSide(above), math::OUTSIDE);

    AABB below(Vector3(-1.0_r, 3.0_r, -1.0_r), Vector3(1.0_r, 4.0_r, 1.0_r));
    EXPECT_EQ(p.GetSide(below), math::INSIDE);

    AABB intersect(Vector3(-1.0_r, 4.0_r, -1.0_r), Vector3(1.0_r, 6.0_r, 1.0_r));
    EXPECT_EQ(p.GetSide(intersect), math::INTERSECT);
}

TEST(PlaneTest, GetSideSphere) {
    Plane p(Vector3(0.0_r, 1.0_r, 0.0_r), Vector3(0.0_r, 5.0_r, 0.0_r));

    Sphere s_out(Vector3(0.0_r, 8.0_r, 0.0_r), 1.0_r);
    EXPECT_EQ(p.GetSide(s_out), math::OUTSIDE);

    Sphere s_in(Vector3(0.0_r, 2.0_r, 0.0_r), 1.0_r);
    EXPECT_EQ(p.GetSide(s_in), math::INSIDE);

    Sphere s_inter(Vector3(0.0_r, 6.0_r, 0.0_r), 2.0_r);
    EXPECT_EQ(p.GetSide(s_inter), math::INTERSECT);

    Sphere s_on(Vector3(0.0_r, 5.0_r, 0.0_r), 1.0_r);
    EXPECT_EQ(p.GetSide(s_on), math::INTERSECT);
}