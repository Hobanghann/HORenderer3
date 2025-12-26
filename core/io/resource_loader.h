#pragma once

#include <memory>

#include "core/templates/id.h"
#include "resource/resource_manager.h"
#include "resource/texture.h"
#include "resource_importer.h"

namespace ho {
    class ResourceLoader {
       public:
        struct Model {
            ResourceID mesh;
            std::vector<ResourceID> materials;
            ResourceID skeleton;
            std::vector<ResourceID> animations;
            ResourceID skin;
        };

        static Model LoadModel(const Path& path, ResourceManager& manager);

       private:
        static ResourceID LoadTexture(const std::string& name, Texture::Type type, Texture::LayeredType layered_type,
                                      std::vector<std::unique_ptr<Image>>&& images, ResourceManager& manager);
        static ResourceID LoadMaterial(const std::string& name, const aiMaterial* aimat, aiTexture** emb_textures,
                                       const Path& parent_path, ResourceManager& manager);
        static ResourceID LoadSkeleton(const std::string& name, const ImportedModel& imp_model,
                                       ResourceManager& manager);
        static ResourceID LoadMesh(const std::string& name, const ImportedModel& imp_model, const Skeleton* skeleton,
                                   const std::vector<ResourceID>& materials, ResourceManager& manager);
        static ResourceID LoadAnimation(const std::string& name, const aiAnimation* aianim, const Skeleton* skeleton,
                                        ResourceManager& manager);
        static ResourceID LoadSkin(const std::string& name, const ImportedModel& imp_model, const Skeleton* skeleton,
                                   ResourceManager& manager);
    };
}  // namespace ho