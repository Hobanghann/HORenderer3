#include "blinn_phong_renderer.h"

#include <iostream>

#include "../shader/blinn_phong_fs.h"
#include "../shader/blinn_phong_vs.h"
#include "core/io/resource_loader.h"
#include "core/math/frustum.h"
#include "renderer/renderer.h"
#include "resource/material.h"
#include "resource/mesh.h"
#include "resource/skeleton.h"
#include "resource/skin.h"
#include "virtual_gpu/virtual_gpu.h"

namespace ho {
    bool BlinnPhongRenderer::Initialize() {
        // Set GPU state
        vgClearColor(0.3f, 0.3f, 0.3f, 1.f);
        vgDepthFunc(VG_LESS);
        vgPolygonMode(VG_FRONT_AND_BACK, VG_FILL);
        vgEnable(VG_CULL_FACE);
        vgEnable(VG_DEPTH_TEST);

        // Set Light
        light_.color = Color128(1.f, 1.f, 1.f);
        light_.intensity = 1.f;
        light_.direction = -Vector3(10.f, 10.f, 10.f);

        // Set Camera
        camera_.modeling_transform =
            Transform3D(Basis3D(Vector3::UNIT_X, Vector3::UNIT_Y, Vector3::UNIT_Z), Vector3(0.f, 3.f, 10.f))
                .LookedAt(Vector3::ZERO, Vector3::UNIT_Y);
        camera_.projection = Projection::CreatePerspective(math::PI / 2.f, (real)width_, (real)height_, 1.0f, 500.f);

        // Set Object
        real scale = 3.0_r;
        object_.modeling_transform = Transform3D(
            Basis3D(scale * Vector3::UNIT_X, scale * Vector3::UNIT_Y, scale * Vector3::UNIT_Z), Vector3(0.f, -3.f, 0.f));

        // Load Resource for object
        ResourceLoader::Model model =
            ResourceLoader::LoadModel(Path(std::string("Simple Barrel/model_barrel.obj")), resource_manager_);

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
        VGuint vs = vgCreateShader(VG_VERTEX_SHADER);
        vgShaderSource(vs, BLINN_PHONG_VS);
        vgCompileShader(vs);
        VGuint fs = vgCreateShader(VG_FRAGMENT_SHADER);
        vgShaderSource(fs, BLINN_PHONG_FS);
        vgCompileShader(fs);
        program_ = vgCreateProgram();
        vgAttachShader(program_, vs);
        vgAttachShader(program_, fs);
        vgLinkProgram(program_);

        if (vgGetError() != VG_NONE) {
            return false;
        }
        return true;
    }

    bool BlinnPhongRenderer::PreUpdate(float delta_time) {
        if (input_states[INPUT_KEY_MOUSE_LEFT]) {
            static float sensitivity = 0.1f;

            if (mouse_delta_x_ != 0.0f) {
                object_.modeling_transform.RotateAxisAngleLocal(Vector3::UNIT_Y,
                                                                mouse_delta_x_ * sensitivity * delta_time);
            }

            if (mouse_delta_y_ != 0.0f) {
                object_.modeling_transform.RotateAxisAngleLocal(Vector3::UNIT_X,
                                                                mouse_delta_y_ * sensitivity * delta_time);
            }
        }

        if (mouse_wheel_delta_ != 0.0f) {
            float zoom_speed = 2.f;
            object_.modeling_transform.Translate(mouse_wheel_delta_ * zoom_speed * delta_time * Vector3::UNIT_Z);
        }

        return true;
    }

    bool BlinnPhongRenderer::Render() {
        vgUseProgram(program_);

        vgBindFramebuffer(VG_FRAMEBUFFER, 0);
        vgDrawBuffer(VG_BACK);

        vgClear(VG_COLOR_BUFFER_BIT | VG_DEPTH_BUFFER_BIT);

        Matrix4x4 view_mat = camera_.modeling_transform.InverseFast().ToMatrix();
        Matrix4x4 PV_mat = camera_.projection.matrix * view_mat;

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
                VGuint u_model = vgGetUniformLocation(program_, "u_model"_vg);
                vgUniformMatrix4fv(u_model, 1, false, (const VGfloat*)model_t.ToMatrix().data);
                VGuint u_view_projection = vgGetUniformLocation(program_, "u_view_projection"_vg);
                vgUniformMatrix4fv(u_view_projection, 1, false, (const VGfloat*)PV_mat.data);

                // unit - texture mapping
                // 0: diffuse, 1: specular, 2: shininess, 3: opacitry, 4: normal, 5: albedo, 6: emission,
                // 7: metallic/roughness, 8:ao, 9: depthmap

                // bind diffuse
                vgActiveTexture(VG_TEXTURE0);
                vgBindTexture(VG_TEXTURE_2D, object_.texture_rid_to_vgid[mat->textures[TEXTURE_TYPE_DIFFUSE]]);
                VGuint u_diffuse = vgGetUniformLocation(program_, "u_diffuse_sampler"_vg);
                vgUniform1i(u_diffuse, 0);

                // bind specular
                vgActiveTexture(VG_TEXTURE1);
                vgBindTexture(VG_TEXTURE_2D, object_.texture_rid_to_vgid[mat->textures[TEXTURE_TYPE_SPECULAR]]);
                VGuint u_specular = vgGetUniformLocation(program_, "u_specular_sampler"_vg);
                vgUniform1i(u_specular, 1);

                // bind normal
                vgActiveTexture(VG_TEXTURE4);
                vgBindTexture(VG_TEXTURE_2D, object_.texture_rid_to_vgid[mat->textures[TEXTURE_TYPE_NORMAL]]);
                VGuint u_normal = vgGetUniformLocation(program_, "u_normal_sampler"_vg);
                vgUniform1i(u_normal, 4);

                VGuint u_light_directions = vgGetUniformLocation(program_, "u_light_directions"_vg);
                vgUniform3fv(u_light_directions, 1, (const VGfloat*)(light_.direction.data));

                VGuint u_light_colors = vgGetUniformLocation(program_, "u_light_colors"_vg);
                Vector3 light_colors[1];
                light_colors[0] = Vector3(light_.intensity * light_.color.r, light_.intensity * light_.color.g,
                                          light_.intensity * light_.color.b);
                vgUniform3fv(u_light_colors, 1, (const VGfloat*)(light_colors));

                VGuint u_eyePosition = vgGetUniformLocation(program_, "u_eyePosition"_vg);
                vgUniform3f(u_eyePosition, camera_.modeling_transform.origin.x, camera_.modeling_transform.origin.y,
                            camera_.modeling_transform.origin.z);

                vgDrawElements(VG_TRIANGLES, usm.index_count, VG_UNSIGNED_INT, (const void*)0);
            }
        }
        if (vgGetError() != VG_NONE) {
            return false;
        }
        return true;
    }

    bool BlinnPhongRenderer::PostUpdate(float delta_time) {
        (void)delta_time;
        mouse_delta_x_ = 0.0f;
        mouse_delta_y_ = 0.0f;
        mouse_wheel_delta_ = 0.0f;
        return true;
    }

    bool BlinnPhongRenderer::Quit() { return true; }

}  // namespace ho