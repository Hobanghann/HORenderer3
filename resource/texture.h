#pragma once

#include <cstdint>
#include <memory>
#include <string>
#include <vector>

#include "core/io/image.h"
#include "core/templates/id.h"

namespace ho {

struct Texture {
  enum Type {
    TEXTURE_TYPE_2D,
    TEXTURE_TYPE_LAYERED,
    TEXTURE_TYPE_3D,
  };

  enum LayeredType : uint8_t {
    TEXTURE_LAYERED_2D_ARRAY,
    TEXTURE_LAYERED_CUBEMAP,
    TEXTURE_LAYERED_CUBEMAP_ARRAY,
    TEXTURE_LAYERED_NONE
  };

  enum CubeMapLayer : uint8_t {
    CUBEMAP_LAYER_LEFT,
    CUBEMAP_LAYER_RIGHT,
    CUBEMAP_LAYER_BOTTOM,
    CUBEMAP_LAYER_TOP,
    CUBEMAP_LAYER_FRONT,
    CUBEMAP_LAYER_BACK,
  };

  std::string name;
  std::vector<std::unique_ptr<Image>> images;
  Type type;
  LayeredType layered_type = TEXTURE_LAYERED_NONE;
};
}  // namespace ho