#include <gtest/gtest.h>

#include "core/math/math_funcs.h"
#include "core/math/sphere.h"
#include "core/math/vector3.h"

using namespace ho;
using namespace ho::math;

TEST(SphereTest, Constructors) {
  Sphere s1;
  EXPECT_EQ(s1.center, Vector3::ZERO);
  EXPECT_FLOAT_EQ(s1.radius, 0.0_r);

  Sphere s2(Vector3(1.0_r, 2.0_r, 3.0_r), 5.0_r);
  EXPECT_EQ(s2.center, Vector3(1.0_r, 2.0_r, 3.0_r));
  EXPECT_FLOAT_EQ(s2.radius, 5.0_r);
}

TEST(SphereTest, Assignment) {
  Sphere a(Vector3(1.0_r, 2.0_r, 3.0_r), 5.0_r);
  Sphere b;
  b = a;
  EXPECT_EQ(b.center, Vector3(1.0_r, 2.0_r, 3.0_r));
  EXPECT_FLOAT_EQ(b.radius, 5.0_r);
}

TEST(SphereTest, ComparisonOperators) {
  Sphere s1(Vector3(0.0_r, 0.0_r, 0.0_r), 1.0_r);
  Sphere s2(Vector3(0.0_r, 0.0_r, 0.0_r), 1.0_r);
  Sphere s3(Vector3(0.1_r, 0.0_r, 0.0_r), 1.0_r);

  EXPECT_TRUE(s1 == s2);
  EXPECT_FALSE(s1 != s2);

  EXPECT_FALSE(s1 == s3);
  EXPECT_TRUE(s1 != s3);
}

TEST(SphereTest, ApproximateEqualityCheck) {
  Sphere a(Vector3(1.0_r, 1.0_r, 1.0_r), 2.0_r);
  Sphere b(Vector3(1.0000001_r, 1.0000001_r, 1.0000001_r), 2.0000001_r);
  Sphere c(Vector3(2.0_r, 2.0_r, 2.0_r), 3.0_r);

  EXPECT_TRUE(a.IsEqualApprox(b));
  EXPECT_FALSE(a.IsNotEqualApprox(b));

  EXPECT_FALSE(a.IsEqualApprox(c));
  EXPECT_TRUE(a.IsNotEqualApprox(c));
}

TEST(SphereTest, MergeSeparatedSpheres) {
  Sphere s1(Vector3(-2.0_r, 0.0_r, 0.0_r), 1.0_r);
  Sphere s2(Vector3(2.0_r, 0.0_r, 0.0_r), 1.0_r);

  Sphere merged = Sphere::Merge(s1, s2);

  EXPECT_NEAR(merged.center.x, 0.0_r, math::EPSILON_CMP);
  EXPECT_NEAR(merged.center.y, 0.0_r, math::EPSILON_CMP);
  EXPECT_NEAR(merged.center.z, 0.0_r, math::EPSILON_CMP);

  EXPECT_NEAR(merged.radius, 3.0_r, math::EPSILON_CMP);
}

TEST(SphereTest, MergeOverlappingSpheres) {
  Sphere s1(Vector3(0.0_r, 0.0_r, 0.0_r), 3.0_r);
  Sphere s2(Vector3(4.0_r, 0.0_r, 0.0_r), 3.0_r);

  Sphere merged = Sphere::Merge(s1, s2);
  EXPECT_NEAR(merged.center.x, 2.0_r, math::EPSILON_CMP);
  EXPECT_NEAR(merged.radius, 5.0_r, math::EPSILON_CMP);
}

TEST(SphereTest, MergeContainedSpheres) {
  Sphere s_big(Vector3(0.0_r, 0.0_r, 0.0_r), 5.0_r);
  Sphere s_small(Vector3(1.0_r, 0.0_r, 0.0_r), 1.0_r);

  Sphere merged = Sphere::Merge(s_big, s_small);

  EXPECT_TRUE(merged.IsEqualApprox(s_big));
}

TEST(SphereTest, MergeIdenticalSpheres) {
  Sphere s(Vector3(0.0_r, 0.0_r, 0.0_r), 2.0_r);
  Sphere merged = Sphere::Merge(s, s);

  EXPECT_TRUE(merged.IsEqualApprox(s));
}

TEST(SphereTest, ConstructCentroidFromPositions) {
  std::vector<Vector3> points = {
      Vector3(-1.0_r, 0.0_r, 0.0_r), Vector3(1.0_r, 0.0_r, 0.0_r),
      Vector3(0.0_r, -1.0_r, 0.0_r), Vector3(0.0_r, 1.0_r, 0.0_r)};
  Sphere sphere = Sphere::ConstructCentroidFromPositions(points);

  EXPECT_NEAR(sphere.center.x, 0.0_r, math::EPSILON_CMP);
  EXPECT_NEAR(sphere.center.y, 0.0_r, math::EPSILON_CMP);
  EXPECT_NEAR(sphere.center.z, 0.0_r, math::EPSILON_CMP);
  EXPECT_NEAR(sphere.radius, 1.0_r, math::EPSILON_CMP);

  points = {Vector3(1.0_r, 1.0_r, 1.0_r)};
  sphere = Sphere::ConstructCentroidFromPositions(points);
  EXPECT_NEAR(sphere.center.x, 1.0_r, math::EPSILON_CMP);
  EXPECT_NEAR(sphere.center.y, 1.0_r, math::EPSILON_CMP);
  EXPECT_NEAR(sphere.center.z, 1.0_r, math::EPSILON_CMP);
  EXPECT_NEAR(sphere.radius, 0.0_r, math::EPSILON_CMP);
}

TEST(SphereTest, ConstructRitterFromPositions) {
  std::vector<Vector3> points = {
      Vector3(-1.0_r, 0.0_r, 0.0_r), Vector3(1.0_r, 0.0_r, 0.0_r),
      Vector3(0.0_r, 0.0_r, -1.0_r), Vector3(0.0_r, 0.0_r, 1.0_r)};
  Sphere sphere = Sphere::ConstructRitterFromPositions(points);

  EXPECT_NEAR(sphere.center.x, 0.0_r, math::EPSILON_CMP);
  EXPECT_NEAR(sphere.center.y, 0.0_r, math::EPSILON_CMP);
  EXPECT_NEAR(sphere.center.z, 0.0_r, math::EPSILON_CMP);
  EXPECT_NEAR(sphere.radius, 1.0_r, math::EPSILON_CMP);

  points = {Vector3(1.0_r, 1.0_r, 1.0_r)};
  sphere = Sphere::ConstructRitterFromPositions(points);
  EXPECT_NEAR(sphere.center.x, 1.0_r, math::EPSILON_CMP);
  EXPECT_NEAR(sphere.center.y, 1.0_r, math::EPSILON_CMP);
  EXPECT_NEAR(sphere.center.z, 1.0_r, math::EPSILON_CMP);
  EXPECT_NEAR(sphere.radius, 0.0_r, math::EPSILON_CMP);
}