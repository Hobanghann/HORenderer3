#include <gtest/gtest.h>

#include "resource/animation.h"

using namespace ho;

class AnimationTestFixture : public ::testing::Test {
   protected:
    void SetUp() override {
        std::vector<Animation::SkeletalTrack> skeletal_tracks;

        skeletal_tracks.emplace_back(
            0, std::vector<Animation::TranslationKey>{{0.0_r, Vector3(0, 0, 0)}}, Animation::INTERP_MODE_LINEAR,
            std::vector<Animation::RotationKey>{{0.0_r, Quaternion::IDENTITY}}, Animation::INTERP_MODE_SPHERICAL_LINEAR,
            std::vector<Animation::ScalingKey>{{0.0_r, Vector3(1, 1, 1)}}, Animation::INTERP_MODE_LINEAR,
            Animation::EXTRAP_MODE_DEFAULT, Animation::EXTRAP_MODE_DEFAULT);

        skeletal_tracks.emplace_back(
            5, std::vector<Animation::TranslationKey>{{0.0_r, Vector3(1, 2, 3)}}, Animation::INTERP_MODE_LINEAR,
            std::vector<Animation::RotationKey>{{0.0_r, Quaternion::IDENTITY}}, Animation::INTERP_MODE_SPHERICAL_LINEAR,
            std::vector<Animation::ScalingKey>{{0.0_r, Vector3(1, 1, 1)}}, Animation::INTERP_MODE_LINEAR,
            Animation::EXTRAP_MODE_CONSTANT, Animation::EXTRAP_MODE_REPEAT);

        std::vector<Animation::MorphTargetTrack> morph_tracks;

        {
            std::vector<Animation::MorphingKey> keys;
            keys.emplace_back(0.0_r, std::vector<uint32_t>{0, 1}, std::vector<real>{0.5_r, 0.5_r});

            morph_tracks.emplace_back(0u, std::move(keys));
        }

        {
            std::vector<Animation::MorphingKey> keys;
            keys.emplace_back(0.0_r, std::vector<uint32_t>{2}, std::vector<real>{1.0_r});

            morph_tracks.emplace_back(3u, std::move(keys));
        }

        animation =
            std::make_unique<Animation>("test_animation", 2.0_r, std::move(skeletal_tracks), std::move(morph_tracks));
    }

    std::unique_ptr<ho::Animation> animation;
};

TEST_F(AnimationTestFixture, SkeletalTrackAccessors) {
    EXPECT_EQ(animation->GetSkeletalTrackCount(), 2u);

    EXPECT_TRUE(animation->HasSkeletalTrack(0));
    EXPECT_TRUE(animation->HasSkeletalTrack(5));
    EXPECT_FALSE(animation->HasSkeletalTrack(1));
    EXPECT_FALSE(animation->HasSkeletalTrack(999));

    const Animation::SkeletalTrack& track0 = animation->GetSkeletalTrack(0);

    EXPECT_EQ(track0.bone_index, 0u);
    EXPECT_EQ(track0.translation_key_sequence.size(), 1u);
    EXPECT_EQ(track0.rotation_key_sequence.size(), 1u);
    EXPECT_EQ(track0.scaling_key_sequence.size(), 1u);
    EXPECT_EQ(track0.translation_interp_mode, Animation::INTERP_MODE_LINEAR);
    EXPECT_EQ(track0.rotation_interp_mode, Animation::INTERP_MODE_SPHERICAL_LINEAR);

    const Animation::SkeletalTrack& track5 = animation->GetSkeletalTrack(5);

    EXPECT_EQ(track5.bone_index, 5u);
    EXPECT_EQ(track5.pre_mode, Animation::EXTRAP_MODE_CONSTANT);
    EXPECT_EQ(track5.post_mode, Animation::EXTRAP_MODE_REPEAT);
}

TEST_F(AnimationTestFixture, MorphTargetTrackAccessors) {
    EXPECT_EQ(animation->GetMorphTargetTrackCount(), 2u);

    EXPECT_TRUE(animation->HasMorphTargetTrack(0));
    EXPECT_TRUE(animation->HasMorphTargetTrack(3));
    EXPECT_FALSE(animation->HasMorphTargetTrack(1));
    EXPECT_FALSE(animation->HasMorphTargetTrack(999));

    const Animation::MorphTargetTrack& track0 = animation->GetMorphTargetTrack(0);

    EXPECT_EQ(track0.sub_mesh_index, 0u);
    EXPECT_EQ(track0.key_sequence.size(), 1u);
    EXPECT_EQ(track0.key_sequence[0].weights.size(), 2u);

    const Animation::MorphTargetTrack& track3 = animation->GetMorphTargetTrack(3);

    EXPECT_EQ(track3.sub_mesh_index, 3u);
    EXPECT_EQ(track3.key_sequence.size(), 1u);
    EXPECT_EQ(track3.key_sequence[0].weights[0], 1.0_r);
}
