#include <gtest/gtest.h>

#include <filesystem>
#include <string>

#include "core/io/path.h"

using namespace ho;

class PathTest : public ::testing::Test {
   protected:
    void SetUp() override { Path::Initialize(); }
};

TEST_F(PathTest, ProjectRootCheck) { EXPECT_EQ(Path::ProjectRoot(), TEST_HO_PROJECT_ROOT); }

TEST_F(PathTest, AssetRootCheck) { EXPECT_EQ(Path::AssetRoot(), TEST_HO_ASSET_ROOT); }

TEST_F(PathTest, ResolveProjectPathCheck) {
    std::string relative = "core/math/vector3.h";

    std::filesystem::path expected = std::filesystem::path(TEST_HO_PROJECT_ROOT) / relative;

    EXPECT_EQ(Path::ResolveProjectPath(relative), expected.lexically_normal().string());
}

TEST_F(PathTest, ResolveAssetPathCheck) {
    std::string relative = "textures/diffuse.png";

    std::filesystem::path expected = std::filesystem::path(TEST_HO_ASSET_ROOT) / relative;

    EXPECT_EQ(Path::ResolveAssetPath(relative), expected.lexically_normal().string());
}

TEST_F(PathTest, ResolveFileNameCheck) {
    EXPECT_EQ(Path::ResolveFileName("C:/Project/HORenderer3/assets/brick/albedo.png"), "albedo.png");
    EXPECT_EQ(Path::ResolveFileName("/home/user/file.txt"), "file.txt");
    EXPECT_EQ(Path::ResolveFileName("justname.ext"), "justname.ext");
    EXPECT_EQ(Path::ResolveFileName("folder/inner/test.shader"), "test.shader");
}
