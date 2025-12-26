#pragma once

#include <string>
#include <unordered_map>
#include <vector>

#include "core/math/transform_3d.h"

namespace ho {

struct Skin {
  Skin() = default;
  Skin(std::string&& p_name, std::vector<Transform3D>&& p_inverse_bind_poses,
       std::vector<std::vector<uint32_t>>&& p_bind_sub_meshes) noexcept
      : name(std::move(p_name)),
        inverse_bind_poses(std::move(p_inverse_bind_poses)),
        bind_sub_meshes(std::move(p_bind_sub_meshes)) {}

  Skin(Skin&& rhs) noexcept = default;
  Skin& operator=(Skin&& rhs) noexcept = default;

  Skin(const Skin&) = delete;
  Skin& operator=(const Skin&) = delete;

  ALWAYS_INLINE const Transform3D& GetInverseBindTransform(
      uint32_t index) const {
    return inverse_bind_poses[index];
  }

  ALWAYS_INLINE uint32_t GetBindSubMeshCount(uint32_t bone_index) const {
    return static_cast<uint32_t>(bind_sub_meshes[bone_index].size());
  }

  ALWAYS_INLINE uint32_t GetBindSubMeshIndex(uint32_t bone_index,
                                             uint32_t index) const {
    return bind_sub_meshes[bone_index][index];
  }

  std::string name;
  std::vector<Transform3D> inverse_bind_poses;
  std::vector<std::vector<uint32_t>> bind_sub_meshes;
};

}  // namespace ho
