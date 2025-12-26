#include "resource_manager.h"

#include "animation.h"
#include "material.h"
#include "mesh.h"
#include "skeleton.h"
#include "skin.h"
#include "texture.h"

namespace ho {

ResourceManager::ResourceManager() = default;
ResourceManager::~ResourceManager() = default;

ResourceID ResourceManager::AddMesh(Mesh&& mesh) {
  const std::string name = mesh.name;

  if (name_to_mesh_.find(name) != name_to_mesh_.end()) {
    return ResourceID();
  }

  auto id = mesh_pool_.Enroll(std::move(mesh));
  name_to_mesh_[name] = id;
  return id;
}

ResourceID ResourceManager::AddSkeleton(Skeleton&& skeleton) {
  const std::string name = skeleton.name;

  if (name_to_skeleton_.find(name) != name_to_skeleton_.end()) {
    return ResourceID();
  }

  auto id = skeleton_pool_.Enroll(std::move(skeleton));
  name_to_skeleton_[name] = id;
  return id;
}

ResourceID ResourceManager::AddAnimation(Animation&& anim) {
  const std::string name = anim.name;

  if (name_to_anim_.find(name) != name_to_anim_.end()) {
    return ResourceID();
  }

  auto id = animation_pool_.Enroll(std::move(anim));
  name_to_anim_[name] = id;
  return id;
}

ResourceID ResourceManager::AddSkin(Skin&& skin) {
  const std::string name = skin.name;

  if (name_to_skin_.find(name) != name_to_skin_.end()) {
    return ResourceID();
  }

  auto id = skin_pool_.Enroll(std::move(skin));
  name_to_skin_[name] = id;
  return id;
}

ResourceID ResourceManager::AddMaterial(Material&& material) {
  const std::string name = material.name;

  if (name_to_material_.find(name) != name_to_material_.end()) {
    return ResourceID();
  }

  auto id = material_pool_.Enroll(std::move(material));
  name_to_material_[name] = id;
  return id;
}

ResourceID ResourceManager::AddTexture(Texture&& texture) {
  const std::string name = texture.name;

  if (name_to_texture_.find(name) != name_to_texture_.end()) {
    return ResourceID();
  }

  auto id = texture_pool_.Enroll(std::move(texture));
  name_to_texture_[name] = id;
  return id;
}

bool ResourceManager::DeleteMesh(ResourceID id) {
  const Mesh* mesh = mesh_pool_.Get(id);
  if (!mesh) return false;
  name_to_mesh_.erase(mesh->name);
  return mesh_pool_.Delete(id);
}

bool ResourceManager::DeleteSkeleton(ResourceID id) {
  const Skeleton* sk = skeleton_pool_.Get(id);
  if (!sk) return false;
  name_to_skeleton_.erase(sk->name);
  return skeleton_pool_.Delete(id);
}

bool ResourceManager::DeleteAnimation(ResourceID id) {
  const Animation* anim = animation_pool_.Get(id);
  if (!anim) return false;
  name_to_anim_.erase(anim->name);
  return animation_pool_.Delete(id);
}

bool ResourceManager::DeleteSkin(ResourceID id) {
  const Skin* skin = skin_pool_.Get(id);
  if (!skin) return false;
  name_to_skin_.erase(skin->name);
  return skin_pool_.Delete(id);
}

bool ResourceManager::DeleteMaterial(ResourceID id) {
  const Material* mat = material_pool_.Get(id);
  if (!mat) return false;
  name_to_material_.erase(mat->name);
  return material_pool_.Delete(id);
}

bool ResourceManager::DeleteTexture(ResourceID id) {
  const Texture* tex = texture_pool_.Get(id);
  if (!tex) return false;
  name_to_texture_.erase(tex->name);
  return texture_pool_.Delete(id);
}

const Mesh* ResourceManager::GetMesh(ResourceID id) const {
  return mesh_pool_.Get(id);
}

const Skeleton* ResourceManager::GetSkeleton(ResourceID id) const {
  return skeleton_pool_.Get(id);
}

const Animation* ResourceManager::GetAnimation(ResourceID id) const {
  return animation_pool_.Get(id);
}

const Skin* ResourceManager::GetSkin(ResourceID id) const {
  return skin_pool_.Get(id);
}

const Material* ResourceManager::GetMaterial(ResourceID id) const {
  return material_pool_.Get(id);
}

const Texture* ResourceManager::GetTexture(ResourceID id) const {
  return texture_pool_.Get(id);
}

bool ResourceManager::HasMesh(ResourceID id) const {
  return mesh_pool_.Has(id);
}

bool ResourceManager::HasSkeleton(ResourceID id) const {
  return skeleton_pool_.Has(id);
}

bool ResourceManager::HasAnimation(ResourceID id) const {
  return animation_pool_.Has(id);
}

bool ResourceManager::HasSkin(ResourceID id) const {
  return skin_pool_.Has(id);
}

bool ResourceManager::HasMaterial(ResourceID id) const {
  return material_pool_.Has(id);
}

bool ResourceManager::HasTexture(ResourceID id) const {
  return texture_pool_.Has(id);
}

ResourceID ResourceManager::GetMeshID(const std::string& name) const {
  auto it = name_to_mesh_.find(name);
  return it != name_to_mesh_.end() ? it->second : ResourceID();
}

ResourceID ResourceManager::GetSkeletonID(const std::string& name) const {
  auto it = name_to_skeleton_.find(name);
  return it != name_to_skeleton_.end() ? it->second : ResourceID();
}

ResourceID ResourceManager::GetAnimationID(const std::string& name) const {
  auto it = name_to_anim_.find(name);
  return it != name_to_anim_.end() ? it->second : ResourceID();
}

ResourceID ResourceManager::GetSkinID(const std::string& name) const {
  auto it = name_to_skin_.find(name);
  return it != name_to_skin_.end() ? it->second : ResourceID();
}

ResourceID ResourceManager::GetMaterialID(const std::string& name) const {
  auto it = name_to_material_.find(name);
  return it != name_to_material_.end() ? it->second : ResourceID();
}

ResourceID ResourceManager::GetTextureID(const std::string& name) const {
  auto it = name_to_texture_.find(name);
  return it != name_to_texture_.end() ? it->second : ResourceID();
}

uint32_t ResourceManager::GetMeshCount() const {
  return (uint32_t)mesh_pool_.Size();
}
uint32_t ResourceManager::GetSkeletonCount() const {
  return (uint32_t)skeleton_pool_.Size();
}
uint32_t ResourceManager::GetAnimationCount() const {
  return (uint32_t)animation_pool_.Size();
}
uint32_t ResourceManager::GetSkinCount() const {
  return (uint32_t)skin_pool_.Size();
}
uint32_t ResourceManager::GetMaterialCount() const {
  return (uint32_t)material_pool_.Size();
}
uint32_t ResourceManager::GetTextureCount() const {
  return (uint32_t)texture_pool_.Size();
}

}  // namespace ho
