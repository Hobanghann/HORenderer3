#include "resource_importer.h"

#include <cassert>
#include <iostream>

#include "assimp/postprocess.h"
#include "image.h"
#include "path.h"
#include "stb_image.h"

namespace ho {
    std::unique_ptr<Image> ResourceImporter::ImportImage(const std::string& path) {
        std::string file_name = Path::ResolveFileName(path);
        Image::Format format;
        int width;
        int height;
        int num_color_channels;

        // optional: set flip
        // stbi_set_flip_vertically_on_load(true);

        std::uint8_t* stbi_bitmap =
            (std::uint8_t*)(stbi_load(Path::ResolveAssetPath(path).c_str(), &width, &height, &num_color_channels, 0));

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

        std::unique_ptr<Image> img = std::make_unique<Image>(Path::ResolveAssetPath(path).c_str(), file_name, format,
                                                             width, height, stbi_bitmap);
        stbi_image_free(stbi_bitmap);
        return img;
    }

    std::unique_ptr<ImportedModel> ResourceImporter::ImportModel(const std::string& path) {
        std::cerr << Path::ResolveAssetPath(path).c_str() << std::endl;
        std::unique_ptr<ImportedModel> imported = std::make_unique<ImportedModel>();
        imported->importer_.ReadFile(
            Path::ResolveAssetPath(path).c_str(),
            static_cast<unsigned int>(aiProcess_CalcTangentSpace | aiProcess_JoinIdenticalVertices |
                                      aiProcess_Triangulate | aiProcess_GenSmoothNormals |
                                      aiProcess_ImproveCacheLocality | aiProcess_FixInfacingNormals |
                                      aiProcess_FindInvalidData | aiProcess_TransformUVCoords | aiProcess_FlipUVs |
                                      aiProcess_GenBoundingBoxes | aiProcess_PopulateArmatureData));
        imported->scene = imported->importer_.GetScene();
        if (!imported->scene) {
            return nullptr;
        }
        TopologicalSort(imported->scene->mRootNode, imported->flatted_scene);
        for (uint32_t i = 0; i < imported->flatted_scene.size(); i++) {
            imported->node_to_index[imported->flatted_scene[i]] = i;
        }
        return imported;
    }

    void ResourceImporter::TopologicalSort(const aiNode* root, std::deque<const aiNode*>& flatted_scene) {
        if (root == nullptr) {
            return;
        }
        for (unsigned int i = 0; i < root->mNumChildren; i++) {
            TopologicalSort(root->mChildren[i], flatted_scene);
        }
        flatted_scene.push_front(root);
    }
}  // namespace ho