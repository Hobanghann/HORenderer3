#include <gtest/gtest.h>

#include "resource/skeleton.h"

class SkeletonTest : public ::testing::Test {
   protected:
    ho::Skeleton MakeTestSkeleton() {
        std::vector<std::string> names = {"root", "hip", "leg", "spine"};

        std::vector<ho::Transform3D> transforms(names.size());

        std::vector<int> parents = {
            -1,  // root
            0,   // hip
            1,   // leg
            0    // spine
        };

        return ho::Skeleton("test_skeleton", std::move(names), std::move(transforms), std::move(parents));
    }
};

TEST_F(SkeletonTest, BoneAccessors) {
    auto sk = MakeTestSkeleton();
    EXPECT_EQ(sk.GetBoneCount(), 4u);
    EXPECT_EQ(sk.GetBoneIndex("root"), 0u);
    EXPECT_EQ(sk.GetBoneIndex("hip"), 1u);
    EXPECT_EQ(sk.GetBoneIndex("leg"), 2u);
    EXPECT_EQ(sk.GetBoneIndex("spine"), 3u);
    EXPECT_EQ(sk.GetBoneName(0), "root");
    EXPECT_EQ(sk.GetBoneName(1), "hip");
    EXPECT_EQ(sk.GetBoneName(2), "leg");
    EXPECT_EQ(sk.GetBoneName(3), "spine");
}

TEST_F(SkeletonTest, LocalTransformAccessors) {
    auto sk = MakeTestSkeleton();

    const auto& t1 = sk.GetLocalTransform("hip");
    const auto& t2 = sk.GetLocalTransform(sk.GetBoneIndex("hip"));

    EXPECT_EQ(&t1, &t2);
}

TEST_F(SkeletonTest, ParentAccessors) {
    auto sk = MakeTestSkeleton();

    EXPECT_EQ(sk.GetParentIndex("root"), -1);
    EXPECT_EQ(sk.GetParentIndex("hip"), 0);
    EXPECT_EQ(sk.GetParentIndex("leg"), 1);
    EXPECT_EQ(sk.GetParentIndex("spine"), 0);
}

TEST_F(SkeletonTest, ChildAccessors) {
    auto sk = MakeTestSkeleton();

    EXPECT_EQ(sk.GetChildCount("root"), 2u);
    EXPECT_EQ(sk.GetChildCount("hip"), 1u);
    EXPECT_EQ(sk.GetChildCount("leg"), 0u);
    EXPECT_EQ(sk.GetChildCount("spine"), 0u);

    uint32_t root_child0 = sk.GetChildIndex("root", 0);
    uint32_t root_child1 = sk.GetChildIndex("root", 1);

    EXPECT_TRUE((sk.GetBoneName(root_child0) == "hip" && sk.GetBoneName(root_child1) == "spine") ||
                (sk.GetBoneName(root_child0) == "spine" && sk.GetBoneName(root_child1) == "hip"));

    EXPECT_EQ(sk.GetChildIndex("hip", 0), sk.GetBoneIndex("leg"));
}
