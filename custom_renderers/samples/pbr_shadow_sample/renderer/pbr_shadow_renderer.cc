#include "pbr_shadow_renderer.h"

#include <iostream>

#include "../shader/depthmap_fs.h"
#include "../shader/depthmap_vs.h"
#include "../shader/pbr_fs.h"
#include "../shader/pbr_vs.h"
#include "core/io/resource_loader.h"
#include "core/math/frustum.h"
#include "renderer/renderer.h"
#include "resource/material.h"
#include "resource/mesh.h"
#include "resource/skeleton.h"
#include "resource/skin.h"
#include "virtual_gpu/virtual_gpu.h"

namespace ho {
    bool PBRShadowRenderer::Initialize() {
        // Set GPU state
        vgClearColor(0.3f, 0.3f, 0.3f, 1.f);
        vgDepthFunc(VG_LESS);
        vgPolygonMode(VG_FRONT_AND_BACK, VG_FILL);
        vgEnable(VG_CULL_FACE);
        vgEnable(VG_DEPTH_TEST);

        // Set Light
        light_.color = Color128(1.f, 1.f, 0.5f);
        light_.intensity = 500.f;
        light_.direction = -Vector3(10.f, 10.f, 10.f);
        light_.view = Transform3D(Basis3D(Vector3::UNIT_X, Vector3::UNIT_Y, Vector3::UNIT_Z), Vector3(10.f, 10.f, 10.f))
                          .LookedAt(Vector3::ZERO, Vector3::UNIT_Y)
                          .InverseFast();
        light_.projection = Projection::CreateOrthographic((real)width_, (real)height_, 1.f, 500.f);

        // Set framebuffer for depth map
        vgGenTextures(1, &depthmap_);
        vgActiveTexture(VG_TEXTURE0);
        vgBindTexture(VG_TEXTURE_2D, depthmap_);
        vgTexImage2D(VG_TEXTURE_2D, 0, VG_DEPTH_COMPONENT, width_, height_, 0, VG_DEPTH_COMPONENT, VG_FLOAT, nullptr);

        vgGenFramebuffers(1, &depthmap_framebuffer_);
        vgBindFramebuffer(VG_FRAMEBUFFER, depthmap_framebuffer_);
        vgFramebufferTexture2D(VG_FRAMEBUFFER, VG_DEPTH_ATTACHMENT, VG_TEXTURE_2D, depthmap_, 0);

        vgBindTexture(VG_TEXTURE_2D, 0);
        vgBindFramebuffer(VG_FRAMEBUFFER, 0);

        // Set Camera
        camera_.modeling_transform =
            Transform3D(Basis3D(Vector3::UNIT_X, Vector3::UNIT_Y, Vector3::UNIT_Z), Vector3(0.f, 3.f, 10.f))
                .LookedAt(Vector3::ZERO, Vector3::UNIT_Y);
        camera_.projection = Projection::CreatePerspective(math::PI / 2.f, (real)width_, (real)height_, 1.0f, 500.f);

        // Set Object
        real scale = 5.0_r;
        object_.modeling_transform = Transform3D(
            Basis3D(scale * Vector3::UNIT_X, scale * Vector3::UNIT_Y, scale * Vector3::UNIT_Z), Vector3(0.f, 0.f, 0.f));

        // Load Resource for object
        ResourceLoader::Model model =
            ResourceLoader::LoadModel(Path(std::string("DamagedHelmet/glTF/DamagedHelmet.gltf")), resource_manager_);

        // upload mesh to gpu
        const Mesh* mesh = resource_manager_.GetMesh(model.mesh);
        if (!mesh) {
            return false;
        }

        std::vector<VGuint> vao;
        vao.resize(mesh->GetSubMeshCount());
        vgGenVertexArrays(mesh->GetSubMeshCount(), vao.data());

        std::vector<VGuint> vbo;
        vbo.resize(mesh->GetSubMeshCount());
        vgGenBuffers(mesh->GetSubMeshCount(), vbo.data());

        std::vector<VGuint> ebo;
        ebo.resize(mesh->GetSubMeshCount());
        vgGenBuffers(mesh->GetSubMeshCount(), ebo.data());

        for (uint32_t smi = 0; smi < mesh->GetSubMeshCount(); smi++) {
            const Mesh::SubMesh& sub_mesh = mesh->sub_meshes[smi];

            vgBindVertexArray(vao[smi]);

            vgBindBuffer(VG_ARRAY_BUFFER, vbo[smi]);

            // make interleaved vertex buffer
            // | Position | normal | tangent | uv0 |
            std::vector<float> buf;
            buf.reserve(sub_mesh.positions.size() * sizeof(float) * 12);
            for (int vi = 0; vi < sub_mesh.positions.size(); vi++) {
                buf.push_back(sub_mesh.positions[vi].x);
                buf.push_back(sub_mesh.positions[vi].y);
                buf.push_back(sub_mesh.positions[vi].z);
                if (sub_mesh.normals.empty()) {
                    buf.push_back(0.f);
                    buf.push_back(0.f);
                    buf.push_back(0.f);
                } else {
                    buf.push_back(sub_mesh.normals[vi].x);
                    buf.push_back(sub_mesh.normals[vi].y);
                    buf.push_back(sub_mesh.normals[vi].z);
                }
                if (sub_mesh.tangents.empty()) {
                    buf.push_back(0.f);
                    buf.push_back(0.f);
                    buf.push_back(0.f);
                    buf.push_back(0.f);
                } else {
                    buf.push_back(sub_mesh.tangents[vi].x);
                    buf.push_back(sub_mesh.tangents[vi].y);
                    buf.push_back(sub_mesh.tangents[vi].z);
                    buf.push_back(sub_mesh.tangents[vi].w);
                }
                buf.push_back(sub_mesh.uvs[vi][0].x);
                buf.push_back(sub_mesh.uvs[vi][0].y);
            }

            // upload interleaved vertex buffer
            vgBufferData(VG_ARRAY_BUFFER, sizeof(float) * buf.size(), buf.data(), VG_STATIC_DRAW);

            vgEnableVertexAttribArray(0);  // position
            vgVertexAttribPointer(0, 3, VG_FLOAT, false, sizeof(float) * 12, 0);
            vgEnableVertexAttribArray(1);  // normal
            vgVertexAttribPointer(1, 3, VG_FLOAT, false, sizeof(float) * 12, (const void*)(sizeof(float) * 3));
            vgEnableVertexAttribArray(2);  // tangent
            vgVertexAttribPointer(2, 4, VG_FLOAT, false, sizeof(float) * 12, (const void*)(sizeof(float) * 6));
            vgEnableVertexAttribArray(3);  // uv0
            vgVertexAttribPointer(3, 2, VG_FLOAT, false, sizeof(float) * 12, (const void*)(sizeof(float) * 10));

            vgBindBuffer(VG_ELEMENT_ARRAY_BUFFER, ebo[smi]);
            vgBufferData(VG_ELEMENT_ARRAY_BUFFER, sizeof(uint32_t) * sub_mesh.indices.size(), sub_mesh.indices.data(),
                         VG_STATIC_DRAW);

            vgBindVertexArray(0);
            vgBindBuffer(VG_ARRAY_BUFFER, 0);
            vgBindBuffer(VG_ELEMENT_ARRAY_BUFFER, 0);

            UploadedSubMesh usm;
            usm.vao = vao[smi];
            usm.vbo = vbo[smi];
            usm.ebo = ebo[smi];
            usm.index_count = (uint32_t)sub_mesh.indices.size();
            const Material* material = resource_manager_.GetMaterial(sub_mesh.material);
            if (!material) {
                return false;
            }
            usm.material = material;
            usm.sphere = sub_mesh.sphere;
            usm.aabb = sub_mesh.aabb;
            object_.sub_meshes.push_back(usm);
        }

        object_.sphere = mesh->sphere;
        object_.aabb = mesh->aabb;

        // delete mesh after updloading
        resource_manager_.DeleteMesh(model.mesh);

        // upload textures to gpu
        vgActiveTexture(VG_TEXTURE0);

        for (int i = 0; i < model.materials.size(); i++) {
            const Material* material = resource_manager_.GetMaterial(model.materials[i]);
            if (!material) {
                return false;
            }
            for (ResourceID rid : material->textures) {
                const Texture* tex = resource_manager_.GetTexture(rid);
                if (tex) {
                    auto it = object_.texture_rid_to_vgid.find(rid);
                    if (it == object_.texture_rid_to_vgid.end()) {
                        VGuint tid;
                        vgGenTextures(1, &tid);
                        object_.texture_rid_to_vgid.insert({rid, tid});

                        VGenum format;
                        switch (tex->images[0]->format()) {
                            case Image::IMAGE_FORMAT_R8:
                                format = VG_RED;
                                break;
                            case Image::IMAGE_FORMAT_RG8:
                                format = VG_RG;
                                break;
                            case Image::IMAGE_FORMAT_RGB8:
                                format = VG_RGB;
                                break;
                            case Image::IMAGE_FORMAT_RGBA8:
                                format = VG_RGBA;
                                break;
                            default:
                                format = VG_RGBA;
                        }
                        vgBindTexture(VG_TEXTURE_2D, tid);
                        vgTexImage2D(VG_TEXTURE_2D, 0, format, tex->images[0]->width(), tex->images[0]->height(), 0,
                                     format, VG_UNSIGNED_BYTE, tex->images[0]->GetBitmap());

                        // delete texture after uploading
                        resource_manager_.DeleteTexture(rid);
                        vgBindTexture(VG_TEXTURE_2D, 0);
                    }
                }
            }
        }

        const Skeleton* skeleton = resource_manager_.GetSkeleton(model.skeleton);
        if (!skeleton) {
            return false;
        }
        object_.skeleton = skeleton;
        const Skin* skin = resource_manager_.GetSkin(model.skin);
        if (!skin) {
            return false;
        }
        object_.skin = skin;
        for (int i = 0; i < model.materials.size(); i++) {
            object_.materials.push_back(resource_manager_.GetMaterial(model.materials[i]));
        }

        // Set program

        // Depthmap program
        VGuint depthmap_vs = vgCreateShader(VG_VERTEX_SHADER);
        vgShaderSource(depthmap_vs, DEPTHMAP_VS);
        vgCompileShader(depthmap_vs);
        VGuint depthmap_fs = vgCreateShader(VG_FRAGMENT_SHADER);
        vgShaderSource(depthmap_fs, DEPTHMAP_FS);
        vgCompileShader(depthmap_fs);
        depthmap_program_ = vgCreateProgram();
        vgAttachShader(depthmap_program_, depthmap_vs);
        vgAttachShader(depthmap_program_, depthmap_fs);
        vgLinkProgram(depthmap_program_);

        // PBR program
        VGuint pbr_vs = vgCreateShader(VG_VERTEX_SHADER);
        vgShaderSource(pbr_vs, PBR_VS);
        vgCompileShader(pbr_vs);
        VGuint pbr_fs = vgCreateShader(VG_FRAGMENT_SHADER);
        vgShaderSource(pbr_fs, PBR_FS);
        vgCompileShader(pbr_fs);
        pbr_program_ = vgCreateProgram();
        vgAttachShader(pbr_program_, pbr_vs);
        vgAttachShader(pbr_program_, pbr_fs);
        vgLinkProgram(pbr_program_);

        if (vgGetError() != VG_NONE) {
            return false;
        }
        return true;
    }

    bool PBRShadowRenderer::PreUpdate(float delta_time) {
        static float rotate_velocity = 1.5f;
        static float sensitivity = 0.1f;
        static float zoom_speed = 2.f;

        if (input_states[INPUT_KEY_LEFT]) {
            object_.modeling_transform.RotateAxisAngle(Vector3::UNIT_Y, -rotate_velocity * delta_time);
        }
        if (input_states[INPUT_KEY_RIGHT]) {
            object_.modeling_transform.RotateAxisAngle(Vector3::UNIT_Y, rotate_velocity * delta_time);
        }

        if (input_states[INPUT_KEY_MOUSE_LEFT]) {
            if (mouse_delta_x_ != 0.0f) {
                object_.modeling_transform.RotateAxisAngle(Vector3::UNIT_Y, mouse_delta_x_ * sensitivity * delta_time);
            }

            if (mouse_delta_y_ != 0.0f) {
                object_.modeling_transform.RotateAxisAngle(Vector3::UNIT_X, mouse_delta_y_ * sensitivity * delta_time);
            }
        }

        if (mouse_wheel_delta_ != 0.0f) {
            object_.modeling_transform.Translate(mouse_wheel_delta_ * zoom_speed * delta_time * Vector3::UNIT_Z);
        }

        return true;
    }

    bool PBRShadowRenderer::Render() {
        // ===================================================================
        // 1 Pass : Make Shadow Map
        // ===================================================================
        vgUseProgram(depthmap_program_);

        vgBindFramebuffer(VG_FRAMEBUFFER, depthmap_framebuffer_);
        vgDrawBuffer(VG_NONE);

        vgClear(VG_DEPTH_BUFFER_BIT);

        Matrix4x4 light_view_projection = light_.projection.matrix * light_.view.ToMatrix();

        // traversal skeleton tree
        const Skeleton* sklt = object_.skeleton;
        std::vector<Transform3D> acc_transforms;
        acc_transforms.resize(sklt->GetBoneCount());
        for (uint32_t bi = 0; bi < sklt->GetBoneCount(); bi++) {
            int parent_idx = sklt->GetParentIndex(bi);
            // get accumulated modeling transform
            Transform3D model_t = parent_idx == -1 ? object_.modeling_transform * sklt->GetLocalTransform(bi)
                                                   : acc_transforms[parent_idx] * sklt->GetLocalTransform(bi);
            acc_transforms[bi] = model_t;

            // Prepare frustum in local space for bounding volume cullling
            Matrix4x4 PVM_mat = light_view_projection * model_t.ToMatrix();
            Frustum frustum = Frustum::FromMatrix4x4(PVM_mat);

            // draw submesh bound to bone
            auto& bound_submesh_idx = object_.skin->bind_sub_meshes[bi];
            for (int smi = 0; smi < bound_submesh_idx.size(); smi++) {
                const UploadedSubMesh& usm = object_.sub_meshes[bound_submesh_idx[smi]];
                // Bounding volume culling
                if (frustum.GetSide(usm.sphere) == math::OUTSIDE) {
                    continue;
                }

                VGuint vao = usm.vao;
                vgBindVertexArray(vao);

                VGuint u_model = vgGetUniformLocation(depthmap_program_, "u_model"_vg);
                vgUniformMatrix4fv(u_model, 1, false, (const VGfloat*)model_t.ToMatrix().data);
                VGuint u_view_projection = vgGetUniformLocation(depthmap_program_, "u_view_projection"_vg);
                vgUniformMatrix4fv(u_view_projection, 1, false, (const VGfloat*)light_view_projection.data);

                vgDrawElements(VG_TRIANGLES, usm.index_count, VG_UNSIGNED_INT, (const void*)0);
            }
        }

        // ===================================================================
        // 2 Pass : Actual rendering
        // ===================================================================
        vgUseProgram(pbr_program_);

        vgBindFramebuffer(VG_FRAMEBUFFER, 0);
        vgDrawBuffer(VG_BACK);

        vgClear(VG_COLOR_BUFFER_BIT | VG_DEPTH_BUFFER_BIT);
        // Prepare PV matrix for bounding volume culling
        Matrix4x4 view_mat = camera_.modeling_transform.InverseFast().ToMatrix();
        Matrix4x4 PV_mat = camera_.projection.matrix * view_mat;

        // traversal skeleton tree
        acc_transforms.resize(sklt->GetBoneCount());
        for (uint32_t bi = 0; bi < sklt->GetBoneCount(); bi++) {
            int parent_idx = sklt->GetParentIndex(bi);
            // get accumulated modeling transform
            Transform3D model_t = parent_idx == -1 ? object_.modeling_transform * sklt->GetLocalTransform(bi)
                                                   : acc_transforms[parent_idx] * sklt->GetLocalTransform(bi);

            // Prepare frustum in local space for bounding volume cullling
            Matrix4x4 PVM_mat = PV_mat * model_t.ToMatrix();
            Frustum frustum = Frustum::FromMatrix4x4(PVM_mat);

            // draw submesh bound to bone
            auto& bound_submesh_idx = object_.skin->bind_sub_meshes[bi];
            for (int smi = 0; smi < bound_submesh_idx.size(); smi++) {
                const UploadedSubMesh& usm = object_.sub_meshes[bound_submesh_idx[smi]];
                // Bounding volume culling
                if (frustum.GetSide(usm.sphere) == math::OUTSIDE) {
                    continue;
                }

                VGuint vao = usm.vao;
                vgBindVertexArray(vao);

                const Material* mat = usm.material;
                VGuint u_model = vgGetUniformLocation(pbr_program_, "u_model"_vg);
                vgUniformMatrix4fv(u_model, 1, false, (const VGfloat*)model_t.ToMatrix().data);
                VGuint u_view = vgGetUniformLocation(pbr_program_, "u_view"_vg);
                vgUniformMatrix4fv(u_view, 1, false, (const VGfloat*)view_mat.data);
                VGuint u_projection = vgGetUniformLocation(pbr_program_, "u_projection"_vg);
                vgUniformMatrix4fv(u_projection, 1, false, (const VGfloat*)camera_.projection.matrix.data);
                VGuint u_light_view_projection = vgGetUniformLocation(pbr_program_, "u_light_view_projection"_vg);
                vgUniformMatrix4fv(u_light_view_projection, 1, false, (const VGfloat*)light_view_projection.data);

                // unit - texture mapping
                // 0: diffuse, 1: specular, 2: shininess, 3: opacitry, 4: normal, 5: albedo, 6: emission,
                // 7: metallic/roughness, 8:ao, 9: depthmap

                // bind normal
                vgActiveTexture(VG_TEXTURE4);
                vgBindTexture(VG_TEXTURE_2D, object_.texture_rid_to_vgid[mat->textures[TEXTURE_TYPE_NORMAL]]);
                VGuint u_normal = vgGetUniformLocation(pbr_program_, "u_normal_sampler"_vg);
                vgUniform1i(u_normal, 4);

                // bind albedo
                vgActiveTexture(VG_TEXTURE5);
                vgBindTexture(VG_TEXTURE_2D, object_.texture_rid_to_vgid[mat->textures[TEXTURE_TYPE_ALBEDO]]);
                VGuint u_albedo = vgGetUniformLocation(pbr_program_, "u_albedo_sampler"_vg);
                vgUniform1i(u_albedo, 5);

                // bind metallic/roughness
                vgActiveTexture(VG_TEXTURE7);
                vgBindTexture(VG_TEXTURE_2D,
                              object_.texture_rid_to_vgid[mat->textures[TEXTURE_TYPE_METALLIC_ROUGHNESS]]);
                VGuint u_metallic_roughness = vgGetUniformLocation(pbr_program_, "u_metallic_roughness_sampler"_vg);
                vgUniform1i(u_metallic_roughness, 7);

                // bind emission
                vgActiveTexture(VG_TEXTURE6);
                vgBindTexture(VG_TEXTURE_2D, object_.texture_rid_to_vgid[mat->textures[TEXTURE_TYPE_EMISSION]]);
                VGuint u_emission = vgGetUniformLocation(pbr_program_, "u_emission_sampler"_vg);
                vgUniform1i(u_emission, 6);

                // bind ao
                vgActiveTexture(VG_TEXTURE8);
                vgBindTexture(VG_TEXTURE_2D,
                              object_.texture_rid_to_vgid[mat->textures[TEXTURE_TYPE_AMBIENT_OCCLUSION]]);
                VGuint u_ao = vgGetUniformLocation(pbr_program_, "u_ao_sampler"_vg);
                vgUniform1i(u_ao, 8);

                // bind depthmap
                vgActiveTexture(VG_TEXTURE9);
                vgBindTexture(VG_TEXTURE_2D, depthmap_);
                VGuint u_depthmap = vgGetUniformLocation(pbr_program_, "u_depthmap_sampler"_vg);
                vgUniform1i(u_depthmap, 9);

                VGuint u_light_positions = vgGetUniformLocation(pbr_program_, "u_light_positions"_vg);
                Vector3 light_pos[1];
                light_pos[0] = Vector3(5.f, 5.f, 5.f);
                vgUniform3fv(u_light_positions, 1, (const VGfloat*)(light_pos));

                VGuint u_lightColors = vgGetUniformLocation(pbr_program_, "u_light_colors"_vg);
                Vector3 light_colors[1];
                Color128 lc = light_.color * light_.intensity;
                light_colors[0] = Vector3(lc.r, lc.g, lc.b);
                vgUniform3fv(u_lightColors, 1, (const VGfloat*)(light_colors));

                VGuint u_eye_position = vgGetUniformLocation(pbr_program_, "u_eye_position"_vg);
                vgUniform3f(u_eye_position, camera_.modeling_transform.origin.x, camera_.modeling_transform.origin.y,
                            camera_.modeling_transform.origin.z);

                vgDrawElements(VG_TRIANGLES, usm.index_count, VG_UNSIGNED_INT, (const void*)0);
            }
        }
        if (vgGetError() != VG_NONE) {
            return false;
        }
        return true;
    }

    bool PBRShadowRenderer::PostUpdate(float delta_time) {
        (void)delta_time;
        mouse_delta_x_ = 0.0f;
        mouse_delta_y_ = 0.0f;
        mouse_wheel_delta_ = 0.0f;
        return true;
    }

    bool PBRShadowRenderer::Quit() { return true; }

}  // namespace ho