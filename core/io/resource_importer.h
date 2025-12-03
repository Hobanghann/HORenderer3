#pragma once

#include <deque>
#include <string>
#include <unordered_map>

#include "assimp/Importer.hpp"
#include "assimp/scene.h"

namespace ho {
    struct ImportedResource {
        const aiScene* scene;
        std::deque<const aiNode*> flatted_scene;
        std::unordered_map<const aiNode*, int> node_to_index;
    };

    class ResourceImporter {
       public:
        ResourceImporter() = default;
        ImportedResource ImportResource(const std::string& path);

       private:
        void TopologicalSort(const aiNode* root, std::deque<const aiNode*>& flatted_scene);

        Assimp::Importer importer_;
    };

}  // namespace ho