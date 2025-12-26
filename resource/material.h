#pragma once

#include <array>
#include <string>

#include "core/math/color128.h"
#include "core/templates/id.h"
#include "texture_type.h"

namespace ho {
    struct Material {
        enum AlphaBlendMode { BLEND_MODE_DEFAULT, BLEND_MODE_ADDITIVE };

        std::string name;

        // Non-PBR Attributes
        Color128 ambient{0.f, 0.f, 0.f};
        Color128 diffuse{0.f, 0.f, 0.f};
        Color128 specular{0.f, 0.f, 0.f};
        real shininess = 0.0_r;

        // PBR attributes
        Color128 albedo{0.f, 0.f, 0.f};
        real metallic = 1.0_r;
        real roughness = 1.0_r;

        real opacity = 1.0_r;
        Color128 emissive{0.f, 0.f, 0.f};
        real emissive_intensity = 1.0_r;

        AlphaBlendMode blend_mode = BLEND_MODE_DEFAULT;
        bool wireframe_enabled = false;
        bool backface_culling_enabled = true;

        // textures
        std::array<ResourceID, TextureType::TEXTURE_TYPE_LAST> textures = {ResourceID()};
        std::array<uint32_t, TextureType::TEXTURE_TYPE_LAST> uv_channels = {0};
    };

}  // namespace ho
