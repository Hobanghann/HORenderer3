#include <gtest/gtest.h>

#include <filesystem>

#include "core/io/image.h"
#include "core/io/path.h"
#include "core/io/resource_importer.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

using namespace ho;

// Helper: Write a PNG file with arbitrary channels using stb_image_write
static std::string WriteTempImage(const std::string& filename, int width, int height, int channels,
                                  const std::vector<uint8_t>& data) {
    std::string path = (std::filesystem::temp_directory_path() / filename).string();

    int ok = stbi_write_png(path.c_str(), width, height, channels, data.data(), width * channels);

    EXPECT_EQ(ok, 1) << "Failed to write test PNG: " << path;
    return path;
}

TEST(ResourceImporterTest, ImportImageInvalid) {
    auto img = ResourceImporter::ImportImage(Path(std::string("this_file_does_not_exist_12345.png")));
    EXPECT_EQ(img, nullptr);
}

TEST(ResourceImporterTest, ImportImageR8) {
    int width = 2, height = 2, channels = 1;
    std::vector<uint8_t> data = {10, 20, 30, 40};

    std::string path = WriteTempImage("test_r8.png", width, height, channels, data);
    auto img = ResourceImporter::ImportImage(Path(path));

    ASSERT_NE(img, nullptr);
    EXPECT_EQ(img->width(), width);
    EXPECT_EQ(img->height(), height);
    EXPECT_EQ(img->format(), Image::IMAGE_FORMAT_R8);

    EXPECT_EQ(img->GetColor32(0, 0).r, 10);
    EXPECT_EQ(img->GetColor32(0, 1).r, 30);
}

TEST(ResourceImporterTest, ImportImageRG8) {
    int width = 2, height = 1, channels = 2;
    std::vector<uint8_t> data = {10, 50, 20, 60};

    std::string path = WriteTempImage("test_rg8.png", width, height, channels, data);
    auto img = ResourceImporter::ImportImage(Path(path));

    ASSERT_NE(img, nullptr);
    EXPECT_EQ(img->format(), Image::IMAGE_FORMAT_RG8);

    Color32 c0 = img->GetColor32(0, 0);
    EXPECT_EQ(c0.r, 10);
    EXPECT_EQ(c0.g, 50);

    Color32 c1 = img->GetColor32(1, 0);
    EXPECT_EQ(c1.r, 20);
    EXPECT_EQ(c1.g, 60);
}

TEST(ResourceImporterTest, ImportImageRGB8) {
    int width = 1, height = 2, channels = 3;
    std::vector<uint8_t> data = {1, 2, 3, 4, 5, 6};

    std::string path = WriteTempImage("test_rgb8.png", width, height, channels, data);
    auto img = ResourceImporter::ImportImage(Path(path));

    ASSERT_NE(img, nullptr);
    EXPECT_EQ(img->format(), Image::IMAGE_FORMAT_RGB8);

    EXPECT_EQ(img->GetColor32(0, 0).r, 1);
    EXPECT_EQ(img->GetColor32(0, 0).g, 2);
    EXPECT_EQ(img->GetColor32(0, 0).b, 3);

    EXPECT_EQ(img->GetColor32(0, 1).r, 4);
    EXPECT_EQ(img->GetColor32(0, 1).g, 5);
    EXPECT_EQ(img->GetColor32(0, 1).b, 6);
}

TEST(ResourceImporterTest, ImportImageRGBA8) {
    int width = 2, height = 1, channels = 4;
    std::vector<uint8_t> data = {10, 20, 30, 40, 50, 60, 70, 80};

    std::string path = WriteTempImage("test_rgba8.png", width, height, channels, data);
    auto img = ResourceImporter::ImportImage(Path(path));

    ASSERT_NE(img, nullptr);
    EXPECT_EQ(img->format(), Image::IMAGE_FORMAT_RGBA8);

    Color32 c0 = img->GetColor32(0, 0);
    EXPECT_EQ(c0.r, 10);
    EXPECT_EQ(c0.g, 20);
    EXPECT_EQ(c0.b, 30);
    EXPECT_EQ(c0.a, 40);

    Color32 c1 = img->GetColor32(1, 0);
    EXPECT_EQ(c1.r, 50);
    EXPECT_EQ(c1.g, 60);
    EXPECT_EQ(c1.b, 70);
    EXPECT_EQ(c1.a, 80);
}

TEST(ResourceImporterTest, ImportImageDeepCopyCheck) {
    int width = 1, height = 1, channels = 3;
    std::vector<uint8_t> data = {100, 150, 200};

    std::string path = WriteTempImage("test_deepcopy.png", width, height, channels, data);
    auto img = ResourceImporter::ImportImage(Path(path));

    ASSERT_NE(img, nullptr);

    const uint8_t* bmp = img->GetBitmap();
    EXPECT_EQ(bmp[0], 100);
    EXPECT_EQ(bmp[1], 150);
    EXPECT_EQ(bmp[2], 200);

    data[0] = 0;
    EXPECT_EQ(img->GetBitmap()[0], 100);
}

static void RunImporterTest(const Path& file_path) {
    Path path = file_path.ResolvedAssetPath();

    std::unique_ptr<ImportedModel> resource = ResourceImporter::ImportModel(path);

    ASSERT_NE(resource, nullptr) << "Importing model failed to load: " << path.ToString();

    ASSERT_NE(resource->scene, nullptr) << "Assimp failed to load: " << path.ToString();

    ASSERT_NE(resource->scene->mRootNode, nullptr) << "Root node is null for file: " << path.ToString();

    ASSERT_FALSE(resource->flatted_scene.empty()) << "TopologicalSort produced empty result: " << path.ToString();

    const aiNode* expected_root = resource->scene->mRootNode;
    const aiNode* flat_root = resource->flatted_scene.front();
    EXPECT_EQ(expected_root, flat_root) << "Root node is not the first element in flatted_scene.";

    EXPECT_EQ(resource->flatted_scene.size(), resource->node_to_index.size()) << "node_to_index size mismatch";

    for (size_t i = 0; i < resource->flatted_scene.size(); i++) {
        aiNode* n = resource->flatted_scene[i];
        auto it = resource->node_to_index.find(n);
        ASSERT_TRUE(it != resource->node_to_index.end()) << "Node missing in node_to_index map";

        EXPECT_EQ(it->second, i) << "node_to_index index does not match flattened order";
    }

    EXPECT_GT(resource->scene->mNumMeshes, 0) << "Scene contains no meshes";

    const aiMesh* mesh = resource->scene->mMeshes[0];
    ASSERT_NE(mesh, nullptr);

    EXPECT_GT(mesh->mNumVertices, 0u) << "Mesh has no vertices";

    EXPECT_GT(mesh->mNumFaces, 0u) << "Mesh has no faces";

    EXPECT_TRUE(mesh->HasNormals()) << "Mesh has no normals (unexpected for teapot model)";

    if (path.ToString().find(".obj") == std::string::npos) {
        EXPECT_GE(resource->scene->mRootNode->mNumChildren, 0u) << "Expected child nodes in GLTF";
    }
}

TEST(ResourceImporterTest, ImportModelOBJ) {
    std::string file_path = (std::filesystem::path("test_assets") / "teapot" / "teapot.obj").string();
    RunImporterTest(Path(file_path));
}

TEST(ResourceImporterTest, ImportModelGLTF) {
    std::string file_path = (std::filesystem::path("test_assets") / "teapot" / "teapot.gltf").string();
    RunImporterTest(Path(file_path));
}
