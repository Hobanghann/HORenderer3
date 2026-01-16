#pragma once

#include <string>
#include <vector>

#include "core/math/aabb.h"
#include "core/math/color128.h"
#include "core/math/projection.h"
#include "core/math/sphere.h"
#include "core/math/transform_3d.h"
#include "renderer/renderer.h"
#include "virtual_gpu/vg.h"

namespace ho {
    struct Skeleton;
    struct Skin;
    struct Material;

    class BlinnPhongRenderer : public Renderer {
       public:
        struct Light {
            Color128 color;
            real intensity;
            Vector3 direction;
        };

        struct Camera {
            Transform3D modeling_transform;
            Projection projection;
        };

        struct UploadedSubMesh {
            VGuint vao;
            VGuint vbo;
            VGuint ebo;
            const Material* material;
            uint32_t index_count;
            Sphere sphere;
            AABB aabb;
        };

        struct RenderObject {
            Transform3D modeling_transform;
            const Skeleton* skeleton;
            const Skin* skin;
            std::vector<const Material*> materials;
            std::vector<UploadedSubMesh> sub_meshes;
            std::unordered_map<ResourceID, VGuint> texture_rid_to_vgid;
            Sphere sphere;
            AABB aabb;
        };
        bool Initialize() override;

        bool PreUpdate(float delta_time) override;

        bool Render() override;

        bool PostUpdate(float delta_time) override;

        bool Quit() override;

       private:
        Light light_;
        Camera camera_;
        RenderObject object_;

        VGuint program_;
    };

}  // namespace ho