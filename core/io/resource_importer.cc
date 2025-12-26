#include "resource_importer.h"

#include "assimp/postprocess.h"
#include "image.h"
#include "path.h"
#include "stb_image.h"

namespace ho {
    std::unique_ptr<Image> ResourceImporter::ImportImage(const Path& path) {
        std::string file_name = path.GetFileName().ToString();
        Image::Format format;
        int width;
        int height;
        int num_color_channels;

        // optional: set flip
        // stbi_set_flip_vertically_on_load(true);

        std::uint8_t* stbi_bitmap = (std::uint8_t*)(stbi_load(path.ResolvedAssetPath().ToString().c_str(), &width,
                                                              &height, &num_color_channels, 0));

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
            std::make_unique<Image>(path.ResolvedAssetPath(), file_name, format, width, height, stbi_bitmap);
        stbi_image_free(stbi_bitmap);
        return img;
    }

    std::unique_ptr<ImportedModel> ResourceImporter::ImportModel(const Path& path) {
        std::unique_ptr<ImportedModel> imported = std::make_unique<ImportedModel>();
        imported->importer.ReadFile(
            path.ResolvedAssetPath().ToString(),
            static_cast<unsigned int>(aiProcess_CalcTangentSpace | aiProcess_JoinIdenticalVertices |
                                      aiProcess_Triangulate | aiProcess_GenSmoothNormals |
                                      aiProcess_ImproveCacheLocality | aiProcess_FixInfacingNormals |
                                      aiProcess_FindInvalidData | aiProcess_TransformUVCoords | aiProcess_FlipUVs |
                                      aiProcess_GenBoundingBoxes | aiProcess_PopulateArmatureData));
        imported->scene = imported->importer.GetScene();
        if (!imported->scene) {
            return nullptr;
        }
        TopologicalSort(imported->scene->mRootNode, imported->flatted_scene);
        for (uint32_t i = 0; i < imported->flatted_scene.size(); i++) {
            imported->node_to_index[imported->flatted_scene[i]] = i;
        }
        return imported;
    }

    std::unique_ptr<Image> ResourceImporter::ImportEmbeddedTexture(const aiTexture* aitex) {
        if (!aitex) {
            return nullptr;
        }

        int width = 0;
        int height = 0;
        int num_channels = 0;
        std::uint8_t* decoded = nullptr;

        // Case 1: Compressed texture (mHeight == 0)
        if (aitex->mHeight == 0) {
            const unsigned char* compressed_data = reinterpret_cast<const unsigned char*>(aitex->pcData);
            const int compressed_size = static_cast<int>(aitex->mWidth);

            decoded = stbi_load_from_memory(compressed_data, compressed_size, &width, &height, &num_channels, 0);
            if (!decoded) {
                return nullptr;
            }
        }
        // Case 2: Raw texture (mHeight > 0)
        else {
            width = static_cast<int>(aitex->mWidth);
            height = static_cast<int>(aitex->mHeight);
            num_channels = 4;  // aiTexel = RGBA8888

            size_t size = static_cast<size_t>(width) * static_cast<size_t>(height) * static_cast<size_t>(num_channels);
            decoded = new std::uint8_t[size];

            for (int y = 0; y < height; ++y) {
                for (int x = 0; x < width; ++x) {
                    const aiTexel& texel = aitex->pcData[y * width + x];
                    int idx = (y * width + x) * num_channels;
                    decoded[idx + 0] = texel.r;
                    decoded[idx + 1] = texel.g;
                    decoded[idx + 2] = texel.b;
                    decoded[idx + 3] = texel.a;
                }
            }
        }

        // Decide format
        Image::Format format;
        switch (num_channels) {
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
                format = Image::Format::IMAGE_FORMAT_RGBA8;
                break;
        }

        std::unique_ptr<Image> img = std::make_unique<Image>(Path(std::string()), std::string(aitex->mFilename.C_Str()),
                                                             format, width, height, decoded);

        if (aitex->mHeight == 0) {
            stbi_image_free(decoded);
        } else {
            delete[] decoded;
        }

        return img;
    }

    void ResourceImporter::TopologicalSort(aiNode* root, std::deque<aiNode*>& flatted_scene) {
        if (root == nullptr) {
            return;
        }
        for (unsigned int i = 0; i < root->mNumChildren; i++) {
            TopologicalSort(root->mChildren[i], flatted_scene);
        }
        flatted_scene.push_front(root);
    }

}  // namespace ho