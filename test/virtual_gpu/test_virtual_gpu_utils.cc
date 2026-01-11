#include <gtest/gtest.h>

#include <cstring>

#include "virtual_gpu/virtual_gpu_utils.h"

using namespace ho;
using namespace ho::vg;

TEST(VirtualGPUUtilsTest, FNV1aHash) {
    constexpr uint32_t h1 = fnv1a_32("test", 4);
    constexpr uint32_t h2 = fnv1a_32("test", 4);
    constexpr uint32_t h3 = fnv1a_32("test2", 5);

    EXPECT_EQ(h1, h2);
    EXPECT_NE(h1, h3);
}

TEST(VirtualGPUUtilsTest, LiteralHash) {
    VGint a = "POSITION"_vg;
    VGint b = "POSITION"_vg;
    VGint c = "NORMAL"_vg;

    EXPECT_EQ(a, b);
    EXPECT_NE(a, c);
}

TEST(VirtualGPUUtilsTest, GetBufferSlot) {
    EXPECT_EQ(GetBufferSlot(VG_ARRAY_BUFFER), 0u);
    EXPECT_EQ(GetBufferSlot(VG_ELEMENT_ARRAY_BUFFER), 1u);
    EXPECT_EQ(GetBufferSlot(VG_TEXTURE_BUFFER), 2u);
    EXPECT_EQ(GetBufferSlot(VG_UNIFORM_BUFFER), 3u);

    EXPECT_EQ(GetBufferSlot(0xdead), VG_INVALID_SLOT);
}

TEST(VirtualGPUUtilsTest, GetTextureSlot) {
    EXPECT_EQ(GetTextureSlot(VG_TEXTURE_1D), 0u);
    EXPECT_EQ(GetTextureSlot(VG_PROXY_TEXTURE_1D), 0u);

    EXPECT_EQ(GetTextureSlot(VG_TEXTURE_2D), 1u);
    EXPECT_EQ(GetTextureSlot(VG_PROXY_TEXTURE_2D), 1u);

    EXPECT_EQ(GetTextureSlot(VG_TEXTURE_3D), 2u);
    EXPECT_EQ(GetTextureSlot(VG_PROXY_TEXTURE_3D), 2u);

    EXPECT_EQ(GetTextureSlot(0xdead), VG_INVALID_SLOT);
}

TEST(VirtualGPUUtilsTest, IsProxyTexture) {
    EXPECT_TRUE(IsProxyTexture(VG_PROXY_TEXTURE_1D));
    EXPECT_TRUE(IsProxyTexture(VG_PROXY_TEXTURE_2D));
    EXPECT_TRUE(IsProxyTexture(VG_PROXY_TEXTURE_3D));

    EXPECT_FALSE(IsProxyTexture(VG_TEXTURE_1D));
    EXPECT_FALSE(IsProxyTexture(VG_TEXTURE_2D));
    EXPECT_FALSE(IsProxyTexture(VG_TEXTURE_3D));

    EXPECT_FALSE(IsProxyTexture(0xdead));
}

TEST(VirtualGPUUtilsTest, IsColorFormat) {
    EXPECT_TRUE(IsColorFormat(VG_RED));
    EXPECT_TRUE(IsColorFormat(VG_RG));
    EXPECT_TRUE(IsColorFormat(VG_RGB));
    EXPECT_TRUE(IsColorFormat(VG_RGBA));

    EXPECT_FALSE(IsColorFormat(VG_DEPTH_COMPONENT));
    EXPECT_FALSE(IsColorFormat(VG_DEPTH_STENCIL));

    EXPECT_FALSE(IsColorFormat(0xdead));
}

TEST(VirtualGPUUtilsTest, GetTypeSize) {
    EXPECT_EQ(GetTypeSize(VG_UNSIGNED_BYTE), 1u);
    EXPECT_EQ(GetTypeSize(VG_BYTE), 1u);

    EXPECT_EQ(GetTypeSize(VG_UNSIGNED_SHORT), 2u);
    EXPECT_EQ(GetTypeSize(VG_SHORT), 2u);
    EXPECT_EQ(GetTypeSize(VG_HALF_FLOAT), 2u);

    EXPECT_EQ(GetTypeSize(VG_UNSIGNED_INT), 4u);
    EXPECT_EQ(GetTypeSize(VG_INT), 4u);
    EXPECT_EQ(GetTypeSize(VG_FLOAT), 4u);

    EXPECT_EQ(GetTypeSize(0xdead), 0u);
}

TEST(VirtualGPUUtilsTest, GetChannelCount) {
    EXPECT_EQ(GetChannelCount(VG_RED), 1u);
    EXPECT_EQ(GetChannelCount(VG_DEPTH_COMPONENT), 1u);

    EXPECT_EQ(GetChannelCount(VG_RG), 2u);
    EXPECT_EQ(GetChannelCount(VG_DEPTH_STENCIL), 2u);

    EXPECT_EQ(GetChannelCount(VG_RGB), 3u);
    EXPECT_EQ(GetChannelCount(VG_RGBA), 4u);

    EXPECT_EQ(GetChannelCount(0xdead), 0u);
}

TEST(VirtualGPUUtilsTest, GetPixelSize) {
    EXPECT_EQ(GetPixelSize(VG_RED, VG_UNSIGNED_BYTE), 1u);
    EXPECT_EQ(GetPixelSize(VG_RED, VG_FLOAT), 4u);

    EXPECT_EQ(GetPixelSize(VG_RG, VG_UNSIGNED_BYTE), 2u);
    EXPECT_EQ(GetPixelSize(VG_RG, VG_FLOAT), 8u);

    EXPECT_EQ(GetPixelSize(VG_RGB, VG_UNSIGNED_BYTE), 3u);
    EXPECT_EQ(GetPixelSize(VG_RGB, VG_FLOAT), 12u);

    EXPECT_EQ(GetPixelSize(VG_RGBA, VG_UNSIGNED_BYTE), 4u);
    EXPECT_EQ(GetPixelSize(VG_RGBA, VG_FLOAT), 16u);

    EXPECT_EQ(GetPixelSize(VG_DEPTH_COMPONENT, VG_FLOAT), 4u);

    EXPECT_EQ(GetPixelSize(VG_DEPTH_STENCIL, VG_UNSIGNED_INT), 4u);

    EXPECT_EQ(GetPixelSize(0xdead, VG_FLOAT), 0u);
}

TEST(VirtualGPUUtilsTest, HalfFloatRoundtrip) {
    float v = 0.5f;
    uint16_t h = FloatToHalf(v);
    float f = HalfToFloat(h);

    EXPECT_NEAR(v, f, 1e-3f);
}

static float QuantizeAndRecover(float value, VGenum dst_type) {
    switch (dst_type) {
        case VG_UNSIGNED_BYTE: {
            value = math::Clamp(value, 0.0f, 1.0f);
            uint8_t q = static_cast<uint8_t>(value * 255.0f + 0.5f);
            return q / 255.0f;
        }

        case VG_BYTE: {
            value = math::Clamp(value, -1.0f, 1.0f);
            int8_t q;
            if (value == -1.0f) {
                q = -128;
            } else {
                float t = value * 127.0f;
                t += (t >= 0.0f) ? 0.5f : -0.5f;
                q = static_cast<int8_t>(t);
            }
            return q / 127.0f;
        }

        case VG_UNSIGNED_SHORT: {
            value = math::Clamp(value, 0.0f, 1.0f);
            uint16_t q = static_cast<uint16_t>(value * 65535.0f + 0.5f);
            return q / 65535.0f;
        }

        case VG_SHORT: {
            value = math::Clamp(value, -1.0f, 1.0f);
            int16_t q;
            if (value == -1.0f) {
                q = -32768;
            } else {
                float t = value * 32767.0f;
                t += (t >= 0.0f) ? 0.5f : -0.5f;
                q = static_cast<int16_t>(t);
            }
            return q / 32767.0f;
        }

        case VG_UNSIGNED_INT: {
            value = math::Clamp(value, 0.0f, 1.0f);
            uint32_t q = static_cast<uint32_t>(value * 4294967295.0f + 0.5f);
            return q / 4294967295.0f;
        }

        case VG_INT: {
            value = math::Clamp(value, -1.0f, 1.0f);
            int32_t q;
            if (value == -1.0f) {
                q = std::numeric_limits<int32_t>::min();
            } else {
                float t = value * 2147483647.0f;
                t += (t >= 0.0f) ? 0.5f : -0.5f;
                q = static_cast<int32_t>(t);
            }
            return q / 2147483647.0f;
        }

        case VG_FLOAT:
            return value;

        case VG_HALF_FLOAT:
            return HalfToFloat(FloatToHalf(value));

        default:
            assert(false);
            return 0.0f;
    }
}

static constexpr VGenum kColorFormats[] = {VG_RED, VG_RG, VG_RGB, VG_RGBA, VG_BGR, VG_BGRA};

static constexpr VGenum kTypes[] = {VG_UNSIGNED_BYTE, VG_BYTE, VG_UNSIGNED_SHORT, VG_SHORT,
                                    VG_UNSIGNED_INT,  VG_INT,  VG_HALF_FLOAT,     VG_FLOAT};

TEST(VirtualGPUUtilsTest, CopyPixelColor) {
    const float kValues[4] = {0.2f, 0.4f, 0.6f, 0.8f};

    for (VGenum dst_format : kColorFormats) {
        for (VGenum src_format : kColorFormats) {
            for (VGenum dst_type : kTypes) {
                for (VGenum src_type : kTypes) {
                    const uint32_t src_channels = GetChannelCount(src_format);
                    const uint32_t dst_channels = GetChannelCount(dst_format);
                    const uint32_t src_stride = GetTypeSize(src_type);
                    const uint32_t dst_stride = GetTypeSize(dst_type);

                    std::vector<uint8_t> src(src_channels * src_stride);
                    std::vector<uint8_t> dst(dst_channels * dst_stride, 0);

                    for (uint32_t c = 0; c < src_channels; ++c) {
                        uint32_t idx = c;
                        if ((src_format == VG_BGR || src_format == VG_BGRA)) {
                            if (c == 0)
                                idx = 2;
                            else if (c == 2)
                                idx = 0;
                        }

                        uint8_t* p = src.data() + idx * src_stride;
                        double val = kValues[c];

                        switch (src_type) {
                            case VG_UNSIGNED_BYTE:
                                p[0] = (uint8_t)(val * 255.0 + 0.5);
                                break;
                            case VG_BYTE:
                                *(int8_t*)p = (int8_t)(val * 127.0 + 0.5);
                                break;
                            case VG_UNSIGNED_SHORT:
                                *(uint16_t*)p = (uint16_t)(val * 65535.0 + 0.5);
                                break;
                            case VG_SHORT:
                                *(int16_t*)p = (int16_t)(val * 32767.0 + 0.5);
                                break;
                            case VG_UNSIGNED_INT:
                                *(uint32_t*)p = (uint32_t)(val * 4294967295.0 + 0.5);
                                break;
                            case VG_INT:
                                *(int32_t*)p = (int32_t)(val * 2147483647.0 + 0.5);
                                break;
                            case VG_FLOAT:
                                *(float*)p = (float)val;
                                break;
                            case VG_HALF_FLOAT:
                                *(uint16_t*)p = FloatToHalf((float)val);
                                break;
                        }
                    }

                    CopyPixel(dst.data(), src.data(), dst_format, dst_type, src_format, src_type);

                    for (uint32_t c = 0; c < dst_channels; ++c) {
                        uint32_t idx = c;
                        if ((dst_format == VG_BGR || dst_format == VG_BGRA)) {
                            if (c == 0)
                                idx = 2;
                            else if (c == 2)
                                idx = 0;
                        }

                        uint8_t* p = dst.data() + idx * dst_stride;
                        float actual = 0.0f;

                        switch (dst_type) {
                            case VG_UNSIGNED_BYTE:
                                actual = p[0] / 255.0f;
                                break;
                            case VG_BYTE:
                                actual = *(int8_t*)p / 127.0f;
                                break;
                            case VG_UNSIGNED_SHORT:
                                actual = *(uint16_t*)p / 65535.0f;
                                break;
                            case VG_SHORT:
                                actual = *(int16_t*)p / 32767.0f;
                                break;
                            case VG_UNSIGNED_INT:
                                actual = *(uint32_t*)p / 4294967295.0f;
                                break;
                            case VG_INT:
                                actual = *(int32_t*)p / 2147483647.0f;
                                break;
                            case VG_FLOAT:
                                actual = *(float*)p;
                                break;
                            case VG_HALF_FLOAT:
                                actual = HalfToFloat(*(uint16_t*)p);
                                break;
                        }

                        float expected = 0.0f;
                        if (c < src_channels) {
                            expected = kValues[c];
                            if (src_type == VG_UNSIGNED_BYTE)
                                expected = (int)(expected * 255.0 + 0.5) / 255.0f;
                            else if (src_type == VG_BYTE)
                                expected = (int)(expected * 127.0 + 0.5) / 127.0f;
                        } else {
                            expected = (c == 3) ? 1.0f : 0.0f;
                        }

                        if (dst_type == VG_UNSIGNED_BYTE)
                            expected = (int)(expected * 255.0 + 0.5) / 255.0f;
                        else if (dst_type == VG_BYTE)
                            expected = (int)(expected * 127.0 + 0.5) / 127.0f;

                        EXPECT_NEAR(actual, expected, 0.02f)
                            << "Fail: " << dst_format << "/" << src_format << " type: " << dst_type << "/" << src_type
                            << " chan: " << c;
                    }
                }
            }
        }
    }
}

TEST(VirtualGPUUtilsTest, CopyPixelDepth) {
    float src = 0.6f;
    float dst = 0.f;

    CopyPixel(reinterpret_cast<uint8_t*>(&dst), reinterpret_cast<uint8_t*>(&src), VG_DEPTH_COMPONENT, VG_FLOAT,
              VG_DEPTH_COMPONENT, VG_FLOAT);

    EXPECT_NEAR(dst, 0.6f, 1e-6f);
}

TEST(VirtualGPUUtilsTest, CopyPixelDepthStencil) {
    uint32_t src = 0xDEADBEEF;
    uint32_t dst = 0;

    CopyPixel(reinterpret_cast<uint8_t*>(&dst), reinterpret_cast<uint8_t*>(&src), VG_DEPTH_STENCIL, VG_UNSIGNED_INT,
              VG_DEPTH_STENCIL, VG_UNSIGNED_INT);

    EXPECT_EQ(dst, src);
}

TEST(VirtualGPUUtilsTest, CopyPixelDepthStencilAssertion) {
#ifndef NDEBUG
    uint32_t src = 0;
    uint32_t dst = 0;

    EXPECT_DEATH(CopyPixel(reinterpret_cast<uint8_t*>(&dst), reinterpret_cast<uint8_t*>(&src), VG_DEPTH_STENCIL,
                           VG_FLOAT, VG_DEPTH_STENCIL, VG_UNSIGNED_INT),
                 "");
#endif
}

TEST(VirtualGPUUtilsTest, EncodeDecodeColor) {
    const Color128 src_color(0.25f, 0.5f, 0.75f, 0.125f);

    for (VGenum format : kColorFormats) {
        for (VGenum type : kTypes) {
            const uint32_t channels = GetChannelCount(format);
            const uint32_t stride = GetTypeSize(type);

            std::vector<uint8_t> buffer(channels * stride, 0xCD);

            ASSERT_NO_FATAL_FAILURE(EncodeColor(buffer.data(), src_color, format, type));

            Color128 decoded;
            ASSERT_NO_FATAL_FAILURE(DecodeColor(&decoded, buffer.data(), format, type));
            Color128 expected;

            expected.r = QuantizeAndRecover(src_color.r, type);

            expected.g = (channels >= 2) ? QuantizeAndRecover(src_color.g, type) : 0.0f;

            expected.b = (channels >= 3) ? QuantizeAndRecover(src_color.b, type) : 0.0f;

            if (channels == 4) {
                expected.a = QuantizeAndRecover(src_color.a, type);
            } else {
                expected.a = 1.0f;
            }

            EXPECT_NEAR(decoded.r, expected.r, 1e-6f) << "R mismatch: format=" << format << " type=" << type;
            EXPECT_NEAR(decoded.g, expected.g, 1e-6f) << "G mismatch: format=" << format << " type=" << type;
            EXPECT_NEAR(decoded.b, expected.b, 1e-6f) << "B mismatch: format=" << format << " type=" << type;
            EXPECT_NEAR(decoded.a, expected.a, 1e-6f) << "A mismatch: format=" << format << " type=" << type;
        }
    }
}

TEST(VirtualGPUUtilsTest, EncodeDecodeDepthStencil) {
    uint8_t buf[4];

    EncodeDepthStencil(buf, 0.5f, 123);

    float depth;
    uint8_t stencil;
    DecodeDepthStencil(&depth, &stencil, buf);

    EXPECT_EQ(stencil, 123);
    EXPECT_NEAR(depth, 0.5f, 1e-5f);
}
