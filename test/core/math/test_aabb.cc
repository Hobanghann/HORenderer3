#include <gtest/gtest.h>

#include "core/math/aabb.h"
#include "core/math/vector3.h"

using namespace ho;

TEST(AABBTest, Constructors) {
    AABB dbox;
    EXPECT_EQ(dbox.min_edges, Vector3::ZERO);
    EXPECT_EQ(dbox.max_edges, Vector3::ZERO);

    Vector3 min(-1.0_r, -2.0_r, -3.0_r);
    Vector3 max(3.0_r, 2.0_r, 1.0_r);
    AABB box(min, max);
    EXPECT_EQ(box.min_edges, min);
    EXPECT_EQ(box.max_edges, max);

    Vector3 expected_center = 0.5_r * (min + max);
    EXPECT_FLOAT_EQ(box.center.x, expected_center.x);
    EXPECT_FLOAT_EQ(box.center.y, expected_center.y);
    EXPECT_FLOAT_EQ(box.center.z, expected_center.z);
}

TEST(AABBTest, Assignment) {
    AABB a(Vector3(0.0_r, 0.0_r, 0.0_r), Vector3(1.0_r, 1.0_r, 1.0_r));
    AABB b;
    b = a;
    EXPECT_EQ(a, b);
}

TEST(AABBTest, ComparisonOperators) {
    AABB a(Vector3(0.0_r, 0.0_r, 0.0_r), Vector3(1.0_r, 1.0_r, 1.0_r));
    AABB b(Vector3(0.0_r, 0.0_r, 0.0_r), Vector3(1.0_r, 1.0_r, 1.0_r));
    AABB c(Vector3(-1.0_r, 0.0_r, 0.0_r), Vector3(1.0_r, 1.0_r, 1.0_r));

    EXPECT_TRUE(a == b);
    EXPECT_FALSE(a != b);
    EXPECT_TRUE(a != c);
    EXPECT_FALSE(a == c);
}

TEST(AABBTest, ApproximateEqualityCheck) {
    AABB a(Vector3(-1.0_r, -1.0_r, -1.0_r), Vector3(1.0_r, 1.0_r, 1.0_r));
    AABB b(Vector3(-1.0000001_r, -1.0000001_r, -1.0000001_r), Vector3(1.0000001_r, 1.0000001_r, 1.0000001_r));
    AABB c(Vector3(-2.0_r, -2.0_r, -2.0_r), Vector3(2.0_r, 2.0_r, 2.0_r));

    EXPECT_TRUE(a.IsEqualApprox(b));
    EXPECT_FALSE(a.IsNotEqualApprox(b));

    EXPECT_FALSE(a.IsEqualApprox(c));
    EXPECT_TRUE(a.IsNotEqualApprox(c));
}

TEST(AABBTest, ConstructFromPositions) {
    std::vector<Vector3> positions = {
        Vector3(1.0_r, 2.0_r, 3.0_r),
        Vector3(-2.0_r, 5.0_r, 1.0_r),
        Vector3(0.0_r, -1.0_r, 4.0_r),
    };

    AABB box = AABB::ConstructFromPositions(positions);

    EXPECT_FLOAT_EQ(box.min_edges.x, -2.0_r);
    EXPECT_FLOAT_EQ(box.min_edges.y, -1.0_r);
    EXPECT_FLOAT_EQ(box.min_edges.z, 1.0_r);

    EXPECT_FLOAT_EQ(box.max_edges.x, 1.0_r);
    EXPECT_FLOAT_EQ(box.max_edges.y, 5.0_r);
    EXPECT_FLOAT_EQ(box.max_edges.z, 4.0_r);

    Vector3 expected_center = 0.5_r * (box.min_edges + box.max_edges);
    EXPECT_EQ(box.center, expected_center);

    positions = {Vector3(1.0_r, 2.0_r, 3.0_r)};
    box = AABB::ConstructFromPositions(positions);
    EXPECT_FLOAT_EQ(box.min_edges.x, 1.0_r);
    EXPECT_FLOAT_EQ(box.min_edges.y, 2.0_r);
    EXPECT_FLOAT_EQ(box.min_edges.z, 3.0_r);
    EXPECT_FLOAT_EQ(box.max_edges.x, 1.0_r);
    EXPECT_FLOAT_EQ(box.max_edges.y, 2.0_r);
    EXPECT_FLOAT_EQ(box.max_edges.z, 3.0_r);
}

TEST(AABBTest, MergeSeparatedBoxes) {
    AABB box1(Vector3(-3.0_r, -1.0_r, -1.0_r), Vector3(-1.0_r, 1.0_r, 1.0_r));
    AABB box2(Vector3(2.0_r, -2.0_r, -2.0_r), Vector3(4.0_r, 2.0_r, 2.0_r));

    AABB merged = AABB::Merge(box1, box2);

    EXPECT_FLOAT_EQ(merged.min_edges.x, -3.0_r);
    EXPECT_FLOAT_EQ(merged.min_edges.y, -2.0_r);
    EXPECT_FLOAT_EQ(merged.min_edges.z, -2.0_r);

    EXPECT_FLOAT_EQ(merged.max_edges.x, 4.0_r);
    EXPECT_FLOAT_EQ(merged.max_edges.y, 2.0_r);
    EXPECT_FLOAT_EQ(merged.max_edges.z, 2.0_r);

    Vector3 expected_center = 0.5_r * (merged.min_edges + merged.max_edges);
    EXPECT_TRUE(merged.center.IsEqualApprox(expected_center));
}

TEST(AABBTest, MergeOverlappingBoxes) {
    AABB box1(Vector3(-2.0_r, -2.0_r, -2.0_r), Vector3(1.0_r, 1.0_r, 1.0_r));
    AABB box2(Vector3(0.0_r, 0.0_r, 0.0_r), Vector3(3.0_r, 3.0_r, 3.0_r));

    AABB merged = AABB::Merge(box1, box2);

    EXPECT_FLOAT_EQ(merged.min_edges.x, -2.0_r);
    EXPECT_FLOAT_EQ(merged.min_edges.y, -2.0_r);
    EXPECT_FLOAT_EQ(merged.min_edges.z, -2.0_r);

    EXPECT_FLOAT_EQ(merged.max_edges.x, 3.0_r);
    EXPECT_FLOAT_EQ(merged.max_edges.y, 3.0_r);
    EXPECT_FLOAT_EQ(merged.max_edges.z, 3.0_r);

    Vector3 expected_center = 0.5_r * (merged.min_edges + merged.max_edges);
    EXPECT_TRUE(merged.center.IsEqualApprox(expected_center));
}

TEST(AABBTest, MergeContainedBoxes) {
    AABB outer(Vector3(-5.0_r, -5.0_r, -5.0_r), Vector3(5.0_r, 5.0_r, 5.0_r));
    AABB inner(Vector3(-2.0_r, -2.0_r, -2.0_r), Vector3(2.0_r, 2.0_r, 2.0_r));

    AABB merged = AABB::Merge(outer, inner);

    EXPECT_TRUE(merged.IsEqualApprox(outer));
}

TEST(AABBTest, MergeIdenticalBoxes) {
    AABB box(Vector3(-1.0_r, -1.0_r, -1.0_r), Vector3(1.0_r, 1.0_r, 1.0_r));
    AABB merged = AABB::Merge(box, box);

    EXPECT_TRUE(merged.IsEqualApprox(box));
}