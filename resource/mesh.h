#pragma once

#include <array>
#include <cassert>
#include <cstdint>
#include <string>
#include <unordered_map>
#include <vector>

#include "core/math/aabb.h"
#include "core/math/color32.h"
#include "core/math/sphere.h"
#include "core/math/transform_3d.h"
#include "core/math/vector2.h"
#include "core/templates/id.h"

namespace ho {
    struct Mesh {
       public:
        static const uint32_t MAX_UV_CHANNEL = 2;
        static const uint32_t MAX_COLOR_CHANNEL = 1;
        static const uint32_t MAX_BONE_CHANNEL = 4;

        enum PrimitiveType : uint8_t {
            PRIMITIVE_TYPE_POINT,
            PRIMITIVE_TYPE_LINE,
            PRIMITIVE_TYPE_LINE_STRIP,
            PRIMITIVE_TYPE_TRIANGLE,
            PRIMITIVE_TYPE_TRIANGLE_STRIP
        };

        struct BoneWeight {
            size_t bone_index;
            real weight;
        };

        struct MorphTarget {
            MorphTarget() = default;
            MorphTarget(std::string&& p_name, std::vector<Vector3>&& p_positions, std::vector<Vector3>&& p_normals,
                        std::vector<Vector4>&& p_tangents, real p_weight) noexcept
                : name(std::move(p_name)),
                  positions(std::move(p_positions)),
                  normals(std::move(p_normals)),
                  tangents(std::move(p_tangents)),
                  weight(p_weight) {}

            MorphTarget(MorphTarget&& rhs) noexcept = default;
            MorphTarget& operator=(MorphTarget&& rhs) noexcept = default;

            MorphTarget(const MorphTarget&) = delete;
            MorphTarget& operator=(const MorphTarget&) = delete;

            ~MorphTarget() = default;

            std::string name;
            std::vector<Vector3> positions;
            std::vector<Vector3> normals;
            std::vector<Vector4> tangents;
            real weight;
        };

        struct SubMesh {
            SubMesh() = default;
            SubMesh(std::string&& p_name, PrimitiveType p_primitive_type, std::vector<Vector3>&& p_positions,
                    std::vector<Vector3>&& p_normals, std::vector<Vector4>&& p_tangents,
                    std::vector<std::array<Vector2, MAX_UV_CHANNEL>>&& p_uvs,
                    std::vector<std::array<Color32, MAX_COLOR_CHANNEL>>&& p_colors,
                    std::vector<std::array<BoneWeight, MAX_BONE_CHANNEL>>&& p_bone_weights,
                    std::vector<uint32_t>&& p_indices, std::vector<MorphTarget>&& p_morph_targets,
                    ResourceID p_material) noexcept
                : name(std::move(p_name)),
                  primitive_type(p_primitive_type),
                  positions(std::move(p_positions)),
                  normals(std::move(p_normals)),
                  tangents(std::move(p_tangents)),
                  uvs(std::move(p_uvs)),
                  colors(std::move(p_colors)),
                  bone_weights(std::move(p_bone_weights)),
                  indices(std::move(p_indices)),
                  morph_targets(std::move(p_morph_targets)),
                  material(p_material) {
                if (!positions.empty()) {
                    aabb = AABB::FromPositions(positions);
                    sphere = Sphere::FromPositionsRitter(positions);
                }
                for (size_t i = 0; i < morph_targets.size(); i++) {
                    morph_name_to_index[morph_targets[i].name] = i;
                }
            }

            SubMesh(SubMesh&& rhs) noexcept = default;
            SubMesh& operator=(SubMesh&& rhs) noexcept = default;

            SubMesh(const SubMesh&) = delete;
            SubMesh& operator=(const SubMesh&) = delete;

            ~SubMesh() = default;

            ALWAYS_INLINE const MorphTarget& GetMorphTarget(const std::string& p_name) const {
                auto it = morph_name_to_index.find(p_name);
                assert(it != morph_name_to_index.end());
                return morph_targets[it->second];
            }
            ALWAYS_INLINE size_t GetMorphTargetIndex(const std::string& p_name) const {
                auto it = morph_name_to_index.find(p_name);
                assert(it != morph_name_to_index.end());
                return it->second;
            }
            ALWAYS_INLINE int GetMorphTargetCount() const { return static_cast<int>(morph_targets.size()); }

            std::string name;
            PrimitiveType primitive_type;

            std::vector<Vector3> positions;
            std::vector<Vector3> normals;
            std::vector<Vector4> tangents;

            std::vector<std::array<Vector2, MAX_UV_CHANNEL>> uvs;
            std::vector<std::array<Color32, MAX_COLOR_CHANNEL>> colors;
            std::vector<std::array<BoneWeight, MAX_BONE_CHANNEL>> bone_weights;

            std::vector<uint32_t> indices;

            std::vector<MorphTarget> morph_targets;

            ResourceID material;

            AABB aabb;
            Sphere sphere;

            std::unordered_map<std::string, size_t> morph_name_to_index;
        };

        Mesh() = default;

        Mesh(std::string&& p_name, std::vector<SubMesh>&& p_sub_meshes) noexcept
            : name(std::move(p_name)), sub_meshes(std::move(p_sub_meshes)) {
            if (!sub_meshes.empty()) {
                // Merge all submesh AABBs to form mesh AABB
                aabb = sub_meshes[0].aabb;
                for (size_t i = 1; i < sub_meshes.size(); i++) {
                    aabb = AABB::Merge(aabb, sub_meshes[i].aabb);
                }

                // For sphere, simple approach: build Ritter from all vertices
                std::vector<Vector3> all_positions;
                for (const auto& sm : sub_meshes) {
                    all_positions.insert(all_positions.end(), sm.positions.begin(), sm.positions.end());
                }
                if (!all_positions.empty()) {
                    sphere = Sphere::FromPositionsRitter(all_positions);
                }
            }
            for (size_t i = 0; i < sub_meshes.size(); i++) {
                sub_mesh_name_to_index[sub_meshes[i].name] = i;
            }
        }
        Mesh(Mesh&& rhs) noexcept = default;
        Mesh& operator=(Mesh&& rhs) noexcept = default;

        Mesh(const Mesh&) = delete;
        Mesh& operator=(const Mesh&) = delete;

        ALWAYS_INLINE const SubMesh& GetSubMesh(const std::string& p_name) const {
            auto it = sub_mesh_name_to_index.find(p_name);
            assert(it != sub_mesh_name_to_index.end());
            return sub_meshes[it->second];
        }
        ALWAYS_INLINE size_t GetSubMeshIndex(const std::string& p_name) const {
            auto it = sub_mesh_name_to_index.find(p_name);
            assert(it != sub_mesh_name_to_index.end());
            return it->second;
        }

        ALWAYS_INLINE int GetSubMeshCount() const { return static_cast<int>(sub_meshes.size()); }

        std::string name;

        std::vector<SubMesh> sub_meshes;

        AABB aabb;
        Sphere sphere;

        std::unordered_map<std::string, size_t> sub_mesh_name_to_index;
    };

}  // namespace ho
