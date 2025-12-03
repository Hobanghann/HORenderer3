#pragma once

#include <memory>
#include <string>

namespace ho {
    class Image;
    namespace ImageImporter {
        std::unique_ptr<Image> ImportImage(const std::string& absolute_image_path);
    }
}  // namespace ho