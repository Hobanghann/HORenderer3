#pragma once

#include <deque>
#include <memory>
#include <string>
#include <unordered_map>

#include "assimp/Importer.hpp"
#include "assimp/scene.h"

namespace ho {
    class Image;

    struct ImportedModel {
        const aiScene* scene;
        std::deque<const aiNode*> flatted_scene;
        std::unordered_map<const aiNode*, int> node_to_index;
        Assimp::Importer importer_;
    };

    class ResourceImporter {
       public:
        ResourceImporter() = default;

        static std::unique_ptr<Image> ImportImage(const std::string& path);
        static std::unique_ptr<ImportedModel> ImportModel(const std::string& path);

       private:
        static void TopologicalSort(const aiNode* root, std::deque<const aiNode*>& flatted_scene);
    };

}  // namespace ho