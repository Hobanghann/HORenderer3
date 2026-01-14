#pragma once

#include <string>
#include <unordered_map>
#include <vector>

#include "core/math/transform_3d.h"

namespace ho {
    struct Skeleton {
        Skeleton() = default;
        Skeleton(std::string&& p_name, std::vector<std::string>&& p_bone_names,
                 std::vector<Transform3D>&& p_local_transforms, std::vector<int>&& p_parents) noexcept
            : name(std::move(p_name)),
              bone_names(std::move(p_bone_names)),
              local_transforms(std::move(p_local_transforms)),
              parents(std::move(p_parents)) {
            const std::size_t count = bone_names.size();

            children.resize(count);
            for (std::size_t i = 0; i < parents.size(); i++) {
                int parent = parents[i];
                if (parent >= 0) {
                    children[static_cast<size_t>(parent)].push_back(static_cast<size_t>(i));
                }
            }

            for (std::size_t i = 0; i < bone_names.size(); i++) {
                name_to_index[bone_names[i]] = i;
            }
        }

        Skeleton(Skeleton&& rhs) noexcept = default;
        Skeleton& operator=(Skeleton&& rhs) noexcept = default;

        Skeleton(const Skeleton&) = delete;
        Skeleton& operator=(const Skeleton&) = delete;

        ALWAYS_INLINE int GetBoneCount() const { return static_cast<int>(bone_names.size()); }

        ALWAYS_INLINE size_t GetBoneIndex(const std::string& p_name) const {
            auto it = name_to_index.find(p_name);
            return it->second;
        }
        ALWAYS_INLINE const std::string& GetBoneName(size_t index) const { return bone_names[index]; }

        ALWAYS_INLINE const Transform3D& GetLocalTransform(const std::string& p_name) const {
            size_t index = GetBoneIndex(p_name);
            return GetLocalTransform(index);
        }

        ALWAYS_INLINE const Transform3D& GetLocalTransform(size_t index) const { return local_transforms[index]; }

        ALWAYS_INLINE int GetParentIndex(const std::string& p_name) const {
            return GetParentIndex(GetBoneIndex(p_name));
        }
        ALWAYS_INLINE int GetParentIndex(size_t index) const { return parents[index]; }

        ALWAYS_INLINE int GetChildCount(const std::string& p_name) const { return GetChildCount(GetBoneIndex(p_name)); }
        ALWAYS_INLINE int GetChildCount(size_t index) const { return static_cast<int>(children[index].size()); }

        ALWAYS_INLINE size_t GetChildIndex(const std::string& parent_name, size_t child_index) const {
            return GetChildIndex(GetBoneIndex(parent_name), child_index);
        }
        ALWAYS_INLINE size_t GetChildIndex(size_t parent_index, size_t child_index) const {
            return children[parent_index][child_index];
        }

        std::string name;
        std::vector<std::string> bone_names;
        std::vector<Transform3D> local_transforms;
        std::vector<int> parents;
        std::vector<std::vector<size_t>> children;
        std::unordered_map<std::string, size_t> name_to_index;
    };
}  // namespace ho