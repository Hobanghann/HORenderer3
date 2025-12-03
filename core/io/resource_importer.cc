#include "resource_importer.h"

#include <cassert>

#include "assimp/postprocess.h"

namespace ho {
    ImportedResource ResourceImporter::ImportResource(const std::string& path) {
        importer_.ReadFile(path.c_str(),
                           aiProcess_CalcTangentSpace | aiProcess_JoinIdenticalVertices | aiProcess_Triangulate |
                               aiProcess_GenSmoothNormals | aiProcess_ImproveCacheLocality |
                               aiProcess_FixInfacingNormals | aiProcess_FindInvalidData | aiProcess_TransformUVCoords |
                               aiProcess_FlipUVs | aiProcess_GenBoundingBoxes | aiProcess_PopulateArmatureData);
        ImportedResource imported;
        imported.scene = importer_.GetScene();
        assert(!imported.scene || !imported.scene->mRootNode);
        TopologicalSort(imported.scene->mRootNode, imported.flatted_scene);
        for (uint32_t i = 0; i < imported.flatted_scene.size(); i++) {
            imported.node_to_index[imported.flatted_scene[i]] = i;
        }
        return imported;
    }

    void ResourceImporter::TopologicalSort(const aiNode* root, std::deque<const aiNode*>& flatted_scene) {
        if (root == nullptr) {
            return;
        }
        for (int i = 0; i < root->mNumChildren; i++) {
            TopologicalSort(root->mChildren[i], flatted_scene);
        }
        flatted_scene.push_front(root);
    }
}  // namespace ho