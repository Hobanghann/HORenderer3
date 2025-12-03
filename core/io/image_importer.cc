#include "image_importer.h"

#include <filesystem>

#include "image.h"
#include "stb_image.h"

namespace ho {
    namespace ImageImporter {
        std::unique_ptr<Image> ImportImage(const std::string& absolute_image_path) {
            std::string file_name = std::filesystem::path(absolute_image_path).filename().string();
            Image::Format format;
            int width;
            int height;
            int num_color_channels;

            // optional: set flip
            // stbi_set_flip_vertically_on_load(true);

            std::uint8_t* stbi_bitmap =
                (std::uint8_t*)(stbi_load(absolute_image_path.c_str(), &width, &height, &num_color_channels, 0));

            if (stbi_bitmap == nullptr) {
                return nullptr;
            }

            switch (num_color_channels) {
                case 1:
                    format = Image::Format::IMAGE_FORMAT_R8;
                    break;
                case 2:
                    format = Image::Format::IMAGE_FORMAT_RG8;
                    break;
                case 3:
                    format = Image::Format::IMAGE_FORMAT_RGB8;
                    break;
                case 4:
                    format = Image::Format::IMAGE_FORMAT_RGBA8;
                    break;
                default:
                    stbi_image_free(stbi_bitmap);
                    return nullptr;
            }

            std::unique_ptr<Image> img =
                std::make_unique<Image>(absolute_image_path, file_name, format, width, height, stbi_bitmap);
            stbi_image_free(stbi_bitmap);
            return img;
        }
    }  // namespace ImageImporter
}  // namespace ho