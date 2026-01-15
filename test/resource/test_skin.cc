#include <gtest/gtest.h>

#include "resource/skin.h"

using namespace ho;

class SkinTest : public ::testing::Test {
   protected:
    ho::Skin MakeSkin() {
        std::vector<Transform3D> inverse_bind{Transform3D(), Transform3D(), Transform3D()};

        std::vector<std::vector<size_t>> bind_submeshes{
            {0, 1},  // bone 0 binds to submesh 0,1
            {1},     // bone 1 binds to submesh 1
            {}       // bone 2 binds to nothing
        };

        return Skin(std::string("test_skin"), std::move(inverse_bind), std::move(bind_submeshes));
    }
};

TEST_F(SkinTest, InverseBindTransformAccessor) {
    auto skin = MakeSkin();

    const auto& t = skin.GetInverseBindTransform(0);
    EXPECT_EQ(t, Transform3D());
}

TEST_F(SkinTest, BindSubMeshAccessors) {
    auto skin = MakeSkin();

    EXPECT_EQ(skin.GetBindSubMeshCount(0), 2u);
    EXPECT_EQ(skin.GetBindSubMeshCount(1), 1u);
    EXPECT_EQ(skin.GetBindSubMeshCount(2), 0u);

    EXPECT_EQ(skin.GetBindSubMeshIndex(0, 0), 0u);
    EXPECT_EQ(skin.GetBindSubMeshIndex(0, 1), 1u);
    EXPECT_EQ(skin.GetBindSubMeshIndex(1, 0), 1u);
}
