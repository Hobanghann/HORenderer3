#pragma once

#include <string>

#include "core/templates/id_owner.h"

namespace ho {
    struct Skeleton;
    struct Animation;
    struct Mesh;
    struct Material;
    struct Texture;
    struct Skin;

    class ResourceManager {
       public:
        ResourceManager();
        ResourceManager(const ResourceManager&) = delete;
        ResourceManager& operator=(const ResourceManager&) = delete;
        ~ResourceManager();

        ResourceID AddMesh(Mesh&& mesh);
        ResourceID AddSkeleton(Skeleton&& skeleton);
        ResourceID AddAnimation(Animation&& anim);
        ResourceID AddSkin(Skin&& skin);
        ResourceID AddMaterial(Material&& material);
        ResourceID AddTexture(Texture&& texture);

        bool DeleteMesh(ResourceID id);
        bool DeleteSkeleton(ResourceID id);
        bool DeleteAnimation(ResourceID id);
        bool DeleteSkin(ResourceID id);
        bool DeleteMaterial(ResourceID id);
        bool DeleteTexture(ResourceID id);

        const Mesh* GetMesh(ResourceID id) const;
        const Skeleton* GetSkeleton(ResourceID id) const;
        const Animation* GetAnimation(ResourceID id) const;
        const Skin* GetSkin(ResourceID id) const;
        const Material* GetMaterial(ResourceID id) const;
        const Texture* GetTexture(ResourceID id) const;

        bool HasMesh(ResourceID id) const;
        bool HasSkeleton(ResourceID id) const;
        bool HasAnimation(ResourceID id) const;
        bool HasSkin(ResourceID id) const;
        bool HasMaterial(ResourceID id) const;
        bool HasTexture(ResourceID id) const;

        ResourceID GetMeshID(const std::string& name) const;
        ResourceID GetSkeletonID(const std::string& name) const;
        ResourceID GetAnimationID(const std::string& name) const;
        ResourceID GetSkinID(const std::string& name) const;
        ResourceID GetMaterialID(const std::string& name) const;
        ResourceID GetTextureID(const std::string& name) const;

        int GetMeshCount() const;
        int GetSkeletonCount() const;
        int GetAnimationCount() const;
        int GetSkinCount() const;
        int GetMaterialCount() const;
        int GetTextureCount() const;

       private:
        ID_Owner<ResourceID, Mesh> mesh_pool_;
        ID_Owner<ResourceID, Skeleton> skeleton_pool_;
        ID_Owner<ResourceID, Skin> skin_pool_;
        ID_Owner<ResourceID, Animation> animation_pool_;
        ID_Owner<ResourceID, Material> material_pool_;
        ID_Owner<ResourceID, Texture> texture_pool_;

        std::unordered_map<std::string, ResourceID> name_to_mesh_;
        std::unordered_map<std::string, ResourceID> name_to_skeleton_;
        std::unordered_map<std::string, ResourceID> name_to_anim_;
        std::unordered_map<std::string, ResourceID> name_to_skin_;
        std::unordered_map<std::string, ResourceID> name_to_material_;
        std::unordered_map<std::string, ResourceID> name_to_texture_;
    };
}  // namespace ho
