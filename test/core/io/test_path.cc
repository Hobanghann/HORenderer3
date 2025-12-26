#include <gtest/gtest.h>

#include <string>

#include "core/io/path.h"

using namespace ho;

TEST(Path, ConstructAndToString) {
    std::string s = "a/b/c.txt";
    Path p(s);

    EXPECT_EQ(p.ToString(), std::filesystem::path(s).string());
}

TEST(Path, PathJoinOperator) {
    std::string base_str = "a/b";
    std::string file_str = "c.txt";

    Path base(base_str);
    Path file(file_str);

    Path result = base / file;

    std::filesystem::path expected = std::filesystem::path(base_str) / file_str;

    EXPECT_EQ(result.ToString(), expected.string());
}

TEST(Path, GetParentPath) {
    std::string s = "a/b/c.txt";
    Path p(s);

    EXPECT_EQ(p.GetParentPath().ToString(), std::filesystem::path(s).parent_path().string());
}

TEST(Path, GetFileName) {
    std::string s = "a/b/c.txt";
    Path p(s);

    EXPECT_EQ(p.GetFileName().ToString(), "c.txt");
}

TEST(Path, ResolveProjectPathRelative) {
    std::string rel = "src/main.cpp";
    Path p(rel);

    p.ResolveProjectPath();

    std::filesystem::path expected = std::filesystem::path(HO_PROJECT_ROOT) / rel;

    EXPECT_TRUE(std::filesystem::path(p.ToString()).is_absolute());
    EXPECT_EQ(p.ToString(), expected.lexically_normal().string());
}

TEST(Path, ResolveProjectPathAbsolute) {
    std::filesystem::path abs_path = std::filesystem::path(HO_PROJECT_ROOT) / "src/main.cpp";

    std::string abs = abs_path.string();
    Path p(abs);

    p.ResolveProjectPath();

    EXPECT_EQ(p.ToString(), abs_path.string());
}

TEST(Path, ResolveAssetPathRelative) {
    std::string rel = "textures/albedo.png";
    Path p(rel);

    p.ResolveAssetPath();

    std::filesystem::path expected = std::filesystem::path(HO_ASSET_ROOT) / rel;

    EXPECT_TRUE(std::filesystem::path(p.ToString()).is_absolute());
    EXPECT_EQ(p.ToString(), expected.lexically_normal().string());
}

TEST(Path, ResolveAssetPathAbsolute) {
    std::filesystem::path abs_path = std::filesystem::path(HO_ASSET_ROOT) / "textures/albedo.png";

    std::string abs = abs_path.string();
    Path p(abs);

    p.ResolveAssetPath();

    EXPECT_EQ(p.ToString(), abs_path.string());
}
