#pragma once

#include <cstdint>
#include <iterator>
#include <string>
#include <vector>

#include "core/math/color128.h"
#include "core/math/color32.h"
#include "path.h"

namespace ho {
    class Image {
       public:
        enum Format : std::uint8_t {
            // --- Uncompressed formats (8-bit normalized) ---
            IMAGE_FORMAT_R8,    // Single channel, 8-bit (e.g. masks, heightmaps)
            IMAGE_FORMAT_RG8,   // Two channels, 8-bit each (e.g. UV data, masks)
            IMAGE_FORMAT_RGB8,  // Three channels, 8-bit each, no alpha
            IMAGE_FORMAT_BGR8,
            IMAGE_FORMAT_RGBA8,  // Four channels, 8-bit each, standard color + alpha
            IMAGE_FORMAT_BGRA8,  // BGRA channel order, common in DirectX/Windows
            IMAGE_FORMAT_ARGB8,  // ARGB channel order, used by some toolchains
            IMAGE_FORMAT_ABGR8,  // ABGR channel order, sometimes used in OpenGL
        };

        ALWAYS_INLINE Image()
            : path_(std::string()),
              name_(std::string()),
              format_(IMAGE_FORMAT_RGB8),
              width_(0),
              height_(0),
              bitmap_(std::vector<uint8_t>()) {}
        Image(const Image& img) = default;
        ALWAYS_INLINE Image(const Path& path, const std::string& name, Format format, std::uint32_t width,
                            std::uint32_t height, const std::uint8_t* bitmap);

        ALWAYS_INLINE Image(Image&& other) noexcept
            : path_(std::move(other.path_)),
              name_(std::move(other.name_)),
              format_(other.format_),
              width_(other.width_),
              height_(other.height_),
              bitmap_(std::move(other.bitmap_)) {}

        ALWAYS_INLINE Image& operator=(Image&& other) noexcept {
            if (this != &other) {
                path_ = std::move(other.path_);
                name_ = std::move(other.name_);
                format_ = other.format_;
                width_ = other.width_;
                height_ = other.height_;
                bitmap_ = std::move(other.bitmap_);
            }
            return *this;
        }
        Image& operator=(const Image& img) = delete;
        ALWAYS_INLINE ~Image() = default;

        ALWAYS_INLINE const Path& path() const;
        ALWAYS_INLINE const std::string& name() const;
        ALWAYS_INLINE Format format() const;
        ALWAYS_INLINE std::uint32_t width() const;
        ALWAYS_INLINE std::uint32_t height() const;

        ALWAYS_INLINE Color32 GetColor32(std::uint32_t x, std::uint32_t y) const;
        ALWAYS_INLINE Color128 GetColor128(std::uint32_t x, std::uint32_t y) const;

        ALWAYS_INLINE const uint8_t* GetBitmap() const;

       private:
        Path path_;
        std::string name_;
        Format format_;
        std::uint32_t width_;
        std::uint32_t height_;
        std::vector<uint8_t> bitmap_;
    };

    ALWAYS_INLINE uint32_t GetPixelBytes(Image::Format format) {
        switch (format) {
            case Image::Format::IMAGE_FORMAT_R8:
                return 1;

            case Image::Format::IMAGE_FORMAT_RG8:
                return 2;

            case Image::Format::IMAGE_FORMAT_RGB8:
            case Image::Format::IMAGE_FORMAT_BGR8:
                return 3;

            case Image::Format::IMAGE_FORMAT_RGBA8:
            case Image::Format::IMAGE_FORMAT_BGRA8:
            case Image::Format::IMAGE_FORMAT_ARGB8:
            case Image::Format::IMAGE_FORMAT_ABGR8:
                return 4;

            default:
                return 0;
        }
    }

    Image::Image(const Path& path, const std::string& name, Format format, std::uint32_t width, std::uint32_t height,
                 const std::uint8_t* bitmap)
        : path_(path),
          name_(name),
          format_(format),
          width_(width),
          height_(height),
          bitmap_(bitmap, bitmap + static_cast<size_t>(width * height * GetPixelBytes(format))) {}

    const Path& Image::path() const { return path_; }
    const std::string& Image::name() const { return name_; }
    Image::Format Image::format() const { return format_; }
    std::uint32_t Image::width() const { return width_; }
    std::uint32_t Image::height() const { return height_; }

    Color32 Image::GetColor32(std::uint32_t x, std::uint32_t y) const {
        assert(x < width_ && y < height_);
        std::size_t idx = (y * width_ + x);

        const std::uint8_t* px = bitmap_.data() + idx * GetPixelBytes(format_);
        switch (format_) {
            case Image::Format::IMAGE_FORMAT_R8:
                return Color32(px[0], px[0], px[0], 255);
            case Image::Format::IMAGE_FORMAT_RG8:
                return Color32(px[0], px[1], 0, 255);
            case Image::Format::IMAGE_FORMAT_RGB8:
                return Color32(px[0], px[1], px[2], 255);
            case Image::Format::IMAGE_FORMAT_BGR8:
                return Color32(px[2], px[1], px[0], 255);
            case Image::Format::IMAGE_FORMAT_RGBA8:
                return Color32(px[0], px[1], px[2], px[3]);
            case Image::Format::IMAGE_FORMAT_BGRA8:
                return Color32(px[2], px[1], px[0], px[3]);
            case Image::Format::IMAGE_FORMAT_ARGB8:
                return Color32(px[1], px[2], px[3], px[0]);
            case Image::Format::IMAGE_FORMAT_ABGR8:
                return Color32(px[3], px[2], px[1], px[0]);
            default:
                return Color32(255, 0, 255, 255);  // magenta = unsupported
        }
    }

    Color128 Image::GetColor128(std::uint32_t x, std::uint32_t y) const { return Color128(GetColor32(x, y)); }
    const uint8_t* Image::GetBitmap() const { return bitmap_.data(); }

}  // namespace ho
