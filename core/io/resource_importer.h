#pragma once

#include <deque>
#include <memory>
#include <string>
#include <unordered_map>

#include "assimp/Importer.hpp"
#include "assimp/scene.h"

namespace ho {
    class Path;
    class Image;

    struct ImportedModel {
        const aiScene* scene;
        std::deque<aiNode*> flatted_scene;
        std::unordered_map<aiNode*, uint32_t> node_to_index;
        Assimp::Importer importer;
    };

    class ResourceImporter {
        friend class ResourceLoader;

       public:
        ResourceImporter() = default;

        static std::unique_ptr<Image> ImportImage(const Path& path);
        static std::unique_ptr<ImportedModel> ImportModel(const Path& path);

       private:
        static std::unique_ptr<Image> ImportEmbeddedTexture(const aiTexture* aitex);
        static void TopologicalSort(aiNode* root, std::deque<aiNode*>& flatted_scene);
    };

}  // namespace ho