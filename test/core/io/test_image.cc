#include <gtest/gtest.h>

#include "core/io/image.h"
#include "core/math/math_defs.h"

using namespace ho;

// Helper: Make tiny test bitmap
static std::vector<uint8_t> MakeBitmap(std::initializer_list<uint8_t> values) { return std::vector<uint8_t>(values); }

TEST(ImageTest, PixelBytes) {
    EXPECT_EQ(GetPixelBytes(Image::IMAGE_FORMAT_R8), 1);
    EXPECT_EQ(GetPixelBytes(Image::IMAGE_FORMAT_RG8), 2);
    EXPECT_EQ(GetPixelBytes(Image::IMAGE_FORMAT_RGB8), 3);
    EXPECT_EQ(GetPixelBytes(Image::IMAGE_FORMAT_BGR8), 3);
    EXPECT_EQ(GetPixelBytes(Image::IMAGE_FORMAT_RGBA8), 4);
    EXPECT_EQ(GetPixelBytes(Image::IMAGE_FORMAT_BGRA8), 4);
    EXPECT_EQ(GetPixelBytes(Image::IMAGE_FORMAT_ARGB8), 4);
    EXPECT_EQ(GetPixelBytes(Image::IMAGE_FORMAT_ABGR8), 4);
}

TEST(ImageTest, ConstructorAndAccessors) {
    uint8_t data[3] = {10, 20, 30};

    Image img(Path(std::string("test/path")), "myimg", Image::IMAGE_FORMAT_RGB8, 1, 1, data);

    EXPECT_EQ(img.path(), Path(std::string("test/path")));
    EXPECT_EQ(img.name(), "myimg");
    EXPECT_EQ(img.format(), Image::IMAGE_FORMAT_RGB8);
    EXPECT_EQ(img.width(), 1u);
    EXPECT_EQ(img.height(), 1u);

    const uint8_t* bmp = img.GetBitmap();
    EXPECT_EQ(bmp[0], 10);
    EXPECT_EQ(bmp[1], 20);
    EXPECT_EQ(bmp[2], 30);
}

TEST(ImageTest, MoveConstructor) {
    uint8_t data[3] = {1, 2, 3};
    Image src(Path(std::string("p")), "n", Image::IMAGE_FORMAT_RGB8, 1, 1, data);

    Image dst(std::move(src));

    EXPECT_EQ(dst.path(), Path(std::string("p")));
    EXPECT_EQ(dst.name(), "n");
    EXPECT_EQ(dst.format(), Image::IMAGE_FORMAT_RGB8);

    const uint8_t* bmp = dst.GetBitmap();
    EXPECT_EQ(bmp[0], 1);
    EXPECT_EQ(bmp[1], 2);
    EXPECT_EQ(bmp[2], 3);
}

TEST(ImageTest, MoveAssignment) {
    uint8_t data[3] = {4, 5, 6};
    Image src(Path(std::string("pathX")), "nameX", Image::IMAGE_FORMAT_RGB8, 1, 1, data);

    uint8_t empty[3] = {0, 0, 0};
    Image dst(Path(std::string("old")), "old2", Image::IMAGE_FORMAT_RGB8, 1, 1, empty);

    dst = std::move(src);

    EXPECT_EQ(dst.path(), Path(std::string("pathX")));
    EXPECT_EQ(dst.name(), "nameX");

    const uint8_t* bmp = dst.GetBitmap();
    EXPECT_EQ(bmp[0], 4);
    EXPECT_EQ(bmp[1], 5);
    EXPECT_EQ(bmp[2], 6);
}

TEST(ImageTest, GetColor32_R8) {
    auto bmp = MakeBitmap({128});
    Image img(Path(std::string()), "", Image::IMAGE_FORMAT_R8, 1, 1, bmp.data());

    Color32 c = img.GetColor32(0, 0);
    EXPECT_EQ(c.r, 128);
    EXPECT_EQ(c.g, 128);
    EXPECT_EQ(c.b, 128);
    EXPECT_EQ(c.a, 255);
}

TEST(ImageTest, GetColor32_RG8) {
    auto bmp = MakeBitmap({10, 20});
    Image img(Path(std::string()), "", Image::IMAGE_FORMAT_RG8, 1, 1, bmp.data());

    Color32 c = img.GetColor32(0, 0);
    EXPECT_EQ(c.r, 10);
    EXPECT_EQ(c.g, 20);
    EXPECT_EQ(c.b, 0);
    EXPECT_EQ(c.a, 255);
}

TEST(ImageTest, GetColor32_RGB8) {
    auto bmp = MakeBitmap({1, 2, 3});
    Image img(Path(std::string()), "", Image::IMAGE_FORMAT_RGB8, 1, 1, bmp.data());

    Color32 c = img.GetColor32(0, 0);
    EXPECT_EQ(c.r, 1);
    EXPECT_EQ(c.g, 2);
    EXPECT_EQ(c.b, 3);
    EXPECT_EQ(c.a, 255);
}

TEST(ImageTest, GetColor32_BGR8) {
    auto bmp = MakeBitmap({3, 2, 1});
    Image img(Path(std::string()), "", Image::IMAGE_FORMAT_BGR8, 1, 1, bmp.data());

    Color32 c = img.GetColor32(0, 0);
    EXPECT_EQ(c.r, 1);
    EXPECT_EQ(c.g, 2);
    EXPECT_EQ(c.b, 3);
    EXPECT_EQ(c.a, 255);
}

TEST(ImageTest, GetColor32_RGBA8) {
    auto bmp = MakeBitmap({10, 20, 30, 40});
    Image img(Path(std::string()), "", Image::IMAGE_FORMAT_RGBA8, 1, 1, bmp.data());

    Color32 c = img.GetColor32(0, 0);
    EXPECT_EQ(c.r, 10);
    EXPECT_EQ(c.g, 20);
    EXPECT_EQ(c.b, 30);
    EXPECT_EQ(c.a, 40);
}

TEST(ImageTest, GetColor32_BGRA8) {
    auto bmp = MakeBitmap({30, 20, 10, 40});
    Image img(Path(std::string()), "", Image::IMAGE_FORMAT_BGRA8, 1, 1, bmp.data());

    Color32 c = img.GetColor32(0, 0);
    EXPECT_EQ(c.r, 10);
    EXPECT_EQ(c.g, 20);
    EXPECT_EQ(c.b, 30);
    EXPECT_EQ(c.a, 40);
}

TEST(ImageTest, GetColor32_ARGB8) {
    auto bmp = MakeBitmap({40, 10, 20, 30});
    Image img(Path(std::string()), "", Image::IMAGE_FORMAT_ARGB8, 1, 1, bmp.data());

    Color32 c = img.GetColor32(0, 0);
    EXPECT_EQ(c.r, 10);
    EXPECT_EQ(c.g, 20);
    EXPECT_EQ(c.b, 30);
    EXPECT_EQ(c.a, 40);
}

TEST(ImageTest, GetColor32_ABGR8) {
    auto bmp = MakeBitmap({40, 30, 20, 10});
    Image img(Path(std::string()), "", Image::IMAGE_FORMAT_ABGR8, 1, 1, bmp.data());

    Color32 c = img.GetColor32(0, 0);
    EXPECT_EQ(c.r, 10);
    EXPECT_EQ(c.g, 20);
    EXPECT_EQ(c.b, 30);
    EXPECT_EQ(c.a, 40);
}

TEST(ImageTest, GetColor128) {
    auto bmp = MakeBitmap({100, 150, 200, 255});
    Image img(Path(std::string()), "", Image::IMAGE_FORMAT_RGBA8, 1, 1, bmp.data());

    Color128 c = img.GetColor128(0, 0);
    EXPECT_NEAR(c.r, 100.0f / 255.0f, math::EPSILON_CMP);
    EXPECT_NEAR(c.g, 150.0f / 255.0f, math::EPSILON_CMP);
    EXPECT_NEAR(c.b, 200.0f / 255.0f, math::EPSILON_CMP);
    EXPECT_NEAR(c.a, 1.0f, math::EPSILON_CMP);
}