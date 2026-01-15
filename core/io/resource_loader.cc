#include "resource_loader.h"

#include <unordered_map>

#include "core/math/transform_3d.h"
#include "path.h"
#include "resource/animation.h"
#include "resource/material.h"
#include "resource/mesh.h"
#include "resource/skeleton.h"
#include "resource/skin.h"
#include "resource_importer.h"

namespace ho {
    ResourceLoader::Model ResourceLoader::LoadModel(const Path& path, ResourceManager& manager) {
        std::unique_ptr<ImportedModel> imp_model = ResourceImporter::ImportModel(path);
        Path parent_path = path.GetParentPath();
        std::string file_name = path.GetFileName().ToString();

        Model model;

        // Load materials
        std::unordered_map<std::string, int> mat_name_count;
        for (size_t mi = 0; mi < static_cast<size_t>(imp_model->scene->mNumMaterials); mi++) {
            const aiMaterial* aimat = imp_model->scene->mMaterials[mi];
            aiTexture** emb_textures = imp_model->scene->mTextures;
            std::string mat_name;
            if (aimat->GetName().Empty()) {
                mat_name = file_name + "_" + "unnamed_material_" + std::to_string(mi);
            } else {
                mat_name = aimat->GetName().C_Str();
                auto it = mat_name_count.find(mat_name);
                if (it == mat_name_count.end()) {
                    mat_name_count.insert({mat_name, 1});
                } else {
                    mat_name += "_" + std::to_string(it->second);
                    it->second++;
                }
            }
            ResourceID mat_id = manager.GetMaterialID(mat_name);
            if (mat_id.IsNULL()) {
                mat_id = LoadMaterial(mat_name, aimat, emb_textures, parent_path, manager);
                if (mat_id.IsNULL()) {
                    // loading failed
                }
            }
            model.materials.push_back(mat_id);
        }

        // Load skeleton
        ResourceID skeleton_id = manager.GetSkeletonID(file_name);
        if (skeleton_id.IsNULL()) {
            skeleton_id = LoadSkeleton(file_name, *imp_model, manager);
            if (skeleton_id.IsNULL()) {
                // loading failed
            }
        }
        model.skeleton = skeleton_id;
        const Skeleton* skeleton = manager.GetSkeleton(model.skeleton);

        // Load mesh
        ResourceID mesh_id = manager.GetMeshID(file_name);
        if (mesh_id.IsNULL()) {
            mesh_id = LoadMesh(file_name, *imp_model, skeleton, model.materials, manager);
            if (mesh_id.IsNULL()) {
                // loading failed
            }
        }
        model.mesh = mesh_id;

        // Load animations
        std::unordered_map<std::string, int> anim_name_count;
        for (size_t ai = 0; ai < static_cast<size_t>(imp_model->scene->mNumAnimations); ai++) {
            const aiAnimation* aianim = imp_model->scene->mAnimations[ai];
            std::string anim_name;
            if (aianim->mName.Empty()) {
                anim_name = file_name + "_" + "unnamed_animation_" + std::to_string(ai);
            } else {
                anim_name = aianim->mName.C_Str();
                auto it = anim_name_count.find(anim_name);
                if (it == anim_name_count.end()) {
                    anim_name_count.insert({anim_name, 1});
                } else {
                    anim_name += "_" + std::to_string(it->second);
                    it->second++;
                }
            }
            ResourceID animation_id = manager.GetAnimationID(anim_name);
            if (animation_id.IsNULL()) {
                animation_id = LoadAnimation(anim_name, aianim, skeleton, manager);
                if (animation_id.IsNULL()) {
                    // loading failed
                }
            }
            model.animations.push_back(animation_id);
        }

        // Load Skin
        ResourceID skin_id = manager.GetSkinID(file_name);
        if (skin_id.IsNULL()) {
            skin_id = LoadSkin(file_name, *imp_model, skeleton, manager);
            if (skin_id.IsNULL()) {
                // loading failed
            }
        }
        model.skin = skin_id;

        return model;
    }

    ResourceID ResourceLoader::LoadTexture(const std::string& name, Texture::Type type,
                                           Texture::LayeredType layered_type,
                                           std::vector<std::unique_ptr<Image>>&& images, ResourceManager& manager) {
        Texture tex;
        tex.name = name;
        tex.type = type;
        tex.layered_type = layered_type;
        for (std::unique_ptr<Image>& img : images) {
            tex.images.push_back(std::move(img));
        }
        return manager.AddTexture(std::move(tex));
    }

    ResourceID ResourceLoader::LoadMaterial(const std::string& name, const aiMaterial* aimat, aiTexture** emb_textures,
                                            const Path& parent_path, ResourceManager& manager) {
        if (!aimat) {
            return ResourceID();
        }

        Material mat;

        mat.name = name;

        aiColor3D c3;
        aiColor4D c4;
        float fval = 0.0f;
        int ival = 0;
        // attributes
        if (AI_SUCCESS == aimat->Get(AI_MATKEY_COLOR_AMBIENT, c3)) mat.ambient = Color128(c3.r, c3.g, c3.b);
        if (AI_SUCCESS == aimat->Get(AI_MATKEY_COLOR_DIFFUSE, c3)) mat.diffuse = Color128(c3.r, c3.g, c3.b);
        if (AI_SUCCESS == aimat->Get(AI_MATKEY_COLOR_SPECULAR, c3)) mat.specular = Color128(c3.r, c3.g, c3.b);
        if (AI_SUCCESS == aimat->Get(AI_MATKEY_SHININESS, fval)) mat.shininess = fval;

        if (AI_SUCCESS == aimat->Get(AI_MATKEY_BASE_COLOR, c4)) mat.albedo = Color128(c4.r, c4.g, c4.b, c4.a);
        if (AI_SUCCESS == aimat->Get(AI_MATKEY_METALLIC_FACTOR, fval)) mat.metallic = fval;
        if (AI_SUCCESS == aimat->Get(AI_MATKEY_ROUGHNESS_FACTOR, fval)) mat.roughness = fval;

        if (AI_SUCCESS == aimat->Get(AI_MATKEY_OPACITY, fval)) mat.opacity = fval;
        if (AI_SUCCESS == aimat->Get(AI_MATKEY_COLOR_EMISSIVE, c3)) mat.emissive = Color128(c3.r, c3.g, c3.b);
        if (AI_SUCCESS == aimat->Get(AI_MATKEY_EMISSIVE_INTENSITY, fval)) mat.emissive_intensity = fval;

        if (AI_SUCCESS == aimat->Get(AI_MATKEY_ENABLE_WIREFRAME, ival)) mat.wireframe_enabled = (ival != 0);
        if (AI_SUCCESS == aimat->Get(AI_MATKEY_TWOSIDED, ival))
            mat.backface_culling_enabled = (ival == 0);  // ai two-sided = disable culling

        if (AI_SUCCESS == aimat->Get(AI_MATKEY_BLEND_FUNC, ival)) {
            if (static_cast<aiBlendMode>(ival) == aiBlendMode::aiBlendMode_Default) {
                mat.blend_mode = Material::BLEND_MODE_DEFAULT;
            }
            if (static_cast<aiBlendMode>(ival) == aiBlendMode::aiBlendMode_Additive) {
                mat.blend_mode = Material::BLEND_MODE_ADDITIVE;
            }
        }

        // texture load helper
        auto LoadMaterialTexture = [&](aiTextureType ai_type, TextureType engine_type) {
            aiString tex_path;
            unsigned int uv = 0;
            if (AI_SUCCESS != aimat->GetTexture(ai_type, 0, &tex_path, nullptr, &uv)) return;

            std::string tex_name = tex_path.C_Str();
            ResourceID tex_id;
            unsigned int tex_idx = 0;

            const bool is_embedded = !tex_name.empty() && tex_name[0] == '*';

            if (is_embedded) {
                tex_idx = static_cast<unsigned int>(std::atoi(tex_name.c_str() + 1));
                tex_name = emb_textures[tex_idx]->mFilename.C_Str();
            }

            tex_id = manager.GetTextureID(tex_name);

            if (tex_id.IsNULL()) {
                std::unique_ptr<Image> img;

                if (is_embedded) {
                    img = ResourceImporter::ImportEmbeddedTexture(emb_textures[tex_idx]);
                } else if (!tex_name.empty()) {
                    Path img_path = parent_path / Path(tex_name);
                    img = ResourceImporter::ImportImage(img_path);
                }

                if (!img) return;

                std::vector<std::unique_ptr<Image>> imgs;
                imgs.push_back(std::move(img));

                tex_id = LoadTexture(tex_name, Texture::TEXTURE_TYPE_2D, Texture::TEXTURE_LAYERED_NONE, std::move(imgs),
                                     manager);
            }

            mat.textures[engine_type] = tex_id;
            mat.uv_channels[engine_type] = static_cast<int>(uv);
        };

        // Texture in obj
        LoadMaterialTexture(aiTextureType_DIFFUSE, TEXTURE_TYPE_DIFFUSE);
        LoadMaterialTexture(aiTextureType_SPECULAR, TEXTURE_TYPE_SPECULAR);
        LoadMaterialTexture(aiTextureType_SHININESS, TEXTURE_TYPE_SHININESS);
        LoadMaterialTexture(aiTextureType_OPACITY, TEXTURE_TYPE_OPACITY);
        LoadMaterialTexture(aiTextureType_HEIGHT, TEXTURE_TYPE_NORMAL);

        // Texture in gltf
        LoadMaterialTexture(aiTextureType_BASE_COLOR, TEXTURE_TYPE_ALBEDO);
        LoadMaterialTexture(aiTextureType_EMISSIVE, TEXTURE_TYPE_EMISSION);
        LoadMaterialTexture(aiTextureType_EMISSION_COLOR, TEXTURE_TYPE_EMISSION);
        LoadMaterialTexture(aiTextureType_METALNESS, TEXTURE_TYPE_METALLIC_ROUGHNESS);
        LoadMaterialTexture(aiTextureType_LIGHTMAP, TEXTURE_TYPE_AMBIENT_OCCLUSION);
        LoadMaterialTexture(aiTextureType_NORMALS, TEXTURE_TYPE_NORMAL);

        return manager.AddMaterial(std::move(mat));
    }

    ResourceID ResourceLoader::LoadSkeleton(const std::string& name, const ImportedModel& imp_model,
                                            ResourceManager& manager) {
        std::string sk_name = name;
        std::vector<std::string> bone_names;
        std::vector<Transform3D> local_transforms;
        std::vector<int> parents;
        std::unordered_map<std::string, size_t> bone_name_to_index;

        bone_names.reserve(imp_model.flatted_scene.size());
        local_transforms.reserve(imp_model.flatted_scene.size());
        parents.reserve(imp_model.flatted_scene.size());

        std::unordered_map<std::string, int> bone_name_count;  // for handling duplicated bone name
        for (size_t i = 0; i < imp_model.flatted_scene.size(); i++) {
            const aiNode* node = imp_model.flatted_scene[i];

            // bone name
            std::string bone_name;

            if (node->mName.Empty()) {
                bone_name = sk_name + "_" + "unnamed_bone_" + std::to_string(i);
            } else {
                bone_name = node->mName.C_Str();
                auto it = bone_name_count.find(bone_name);
                if (it == bone_name_count.end()) {
                    bone_name_count.insert({bone_name, 1});
                } else {
                    bone_name += "_" + std::to_string(it->second);
                    it->second++;
                }
            }
            bone_names.push_back(bone_name);

            // bone local transform
            aiMatrix4x4 aim = node->mTransformation;
            Matrix4x4 m(Vector4(aim.a1, aim.a2, aim.a3, aim.a4), Vector4(aim.b1, aim.b2, aim.b3, aim.b4),
                        Vector4(aim.c1, aim.c2, aim.c3, aim.c4), Vector4(aim.d1, aim.d2, aim.d3, aim.d4));
            local_transforms.emplace_back(Transform3D(m));

            bone_name_to_index[bone_name] = i;

            // parent bone
            if (node->mParent) {
                parents.push_back(static_cast<int>(imp_model.node_to_index.at(node->mParent)));
            } else {
                parents.push_back(-1);  // root
            }
        }

        Skeleton skeleton(std::move(sk_name), std::move(bone_names), std::move(local_transforms), std::move(parents));

        return manager.AddSkeleton(std::move(skeleton));
    }

    ResourceID ResourceLoader::LoadMesh(const std::string& name, const ImportedModel& imp_model,
                                        const Skeleton* skeleton, const std::vector<ResourceID>& materials,
                                        ResourceManager& manager) {
        std::string m_name = name;

        std::vector<Mesh::SubMesh> sub_meshes;
        sub_meshes.reserve(static_cast<size_t>(imp_model.scene->mNumMeshes));

        // submeshes
        std::unordered_map<std::string, int> submesh_name_count;  // for handling duplicated submesh name
        for (size_t mi = 0; mi < static_cast<size_t>(imp_model.scene->mNumMeshes); mi++) {
            const aiMesh* aimesh = imp_model.scene->mMeshes[mi];
            if (!aimesh) {
                continue;
            }

            // name
            std::string sm_name;
            if (aimesh->mName.Empty()) {
                sm_name = m_name + "_" + "unnamed_submesh_" + std::to_string(mi);
            } else {
                sm_name = aimesh->mName.C_Str();
                auto it = submesh_name_count.find(sm_name);
                if (it == submesh_name_count.end()) {
                    submesh_name_count.insert({sm_name, 1});
                } else {
                    sm_name += "_" + std::to_string(it->second);
                    it->second++;
                }
            }

            // primitive type
            Mesh::PrimitiveType prim_type;
            switch (aimesh->mPrimitiveTypes) {
                case aiPrimitiveType_POINT:
                    prim_type = Mesh::PRIMITIVE_TYPE_POINT;
                    break;
                case aiPrimitiveType_LINE:
                    prim_type = Mesh::PRIMITIVE_TYPE_LINE;
                    break;
                case aiPrimitiveType_TRIANGLE:
                    prim_type = Mesh::PRIMITIVE_TYPE_TRIANGLE;
                    break;
                default:
                    prim_type = Mesh::PRIMITIVE_TYPE_TRIANGLE;
            }

            std::vector<Vector3> positions;
            positions.reserve(static_cast<size_t>(aimesh->mNumVertices));
            std::vector<Vector3> normals;
            normals.reserve(static_cast<size_t>(aimesh->mNumVertices));
            std::vector<Vector4> tangents;
            tangents.reserve(static_cast<size_t>(aimesh->mNumVertices));

            std::vector<std::array<Vector2, Mesh::MAX_UV_CHANNEL>> uvs(static_cast<size_t>(aimesh->mNumVertices));
            std::vector<std::array<Color32, Mesh::MAX_COLOR_CHANNEL>> colors(static_cast<size_t>(aimesh->mNumVertices));
            std::vector<std::array<Mesh::BoneWeight, Mesh::MAX_BONE_CHANNEL>> bone_weights(
                static_cast<size_t>(aimesh->mNumVertices));

            std::vector<uint32_t> indices;
            indices.reserve(static_cast<size_t>(aimesh->mNumFaces) * 3);
            std::vector<Mesh::MorphTarget> morph_targets;
            morph_targets.reserve(static_cast<size_t>(aimesh->mNumAnimMeshes));

            const bool has_pos = aimesh->HasPositions();
            const bool has_normal = aimesh->HasNormals();
            const bool has_tan = aimesh->HasTangentsAndBitangents();

            for (size_t vi = 0; vi < static_cast<size_t>(aimesh->mNumVertices); vi++) {
                // Positions
                if (has_pos) {
                    const aiVector3D& pos = aimesh->mVertices[vi];
                    positions.emplace_back(static_cast<real>(pos.x), static_cast<real>(pos.y),
                                           static_cast<real>(pos.z));
                }

                // Normals
                if (has_normal) {
                    const aiVector3D& normal = aimesh->mNormals[vi];
                    normals.emplace_back(static_cast<real>(normal.x), static_cast<real>(normal.y),
                                         static_cast<real>(normal.z));
                }

                // Tangents (handedness in w component)
                if (has_tan) {
                    const aiVector3D& tan = aimesh->mTangents[vi];
                    const aiVector3D& bitan = aimesh->mBitangents[vi];
                    Vector3 tangent(static_cast<real>(tan.x), static_cast<real>(tan.y), static_cast<real>(tan.z));
                    Vector3 bitangent(static_cast<real>(bitan.x), static_cast<real>(bitan.y),
                                      static_cast<real>(bitan.z));

                    // calcuate handedness
                    real h = 1.0_r;
                    if (has_normal) {
                        const Vector3& normal = normals[vi];
                        h = (normal.Cross(tangent).Dot(bitangent) > 0.0_r) ? 1.0_r : -1.0_r;
                    }
                    tangents.emplace_back(tangent.x, tangent.y, tangent.z, h);
                }
            }

            // UVs
            for (size_t vi = 0; vi < static_cast<size_t>(aimesh->mNumVertices); vi++) {
                for (size_t ui = 0; ui < static_cast<size_t>(Mesh::MAX_UV_CHANNEL); ui++) {
                    if (!aimesh->HasTextureCoords(static_cast<unsigned int>(ui))) {
                        continue;
                    }
                    const aiVector3D uv = aimesh->mTextureCoords[ui][vi];
                    uvs[vi][ui] = Vector2(static_cast<real>(uv.x), static_cast<real>(uv.y));
                }
            }

            // Colors
            for (size_t vi = 0; vi < static_cast<size_t>(aimesh->mNumVertices); vi++) {
                for (size_t ci = 0; ci < static_cast<size_t>(Mesh::MAX_UV_CHANNEL); ci++) {
                    if (!aimesh->HasVertexColors(static_cast<unsigned int>(ci))) {
                        continue;
                    }
                    const aiColor4D& color = aimesh->mColors[ci][vi];
                    const Color128 color128(static_cast<float>(color.r), static_cast<float>(color.g),
                                            static_cast<float>(color.b), static_cast<float>(color.a));
                    colors[vi][ci] = Color32(color128);
                }
            }

            // Bone weights
            std::vector<std::vector<Mesh::BoneWeight>> temp_weights;
            temp_weights.resize(static_cast<size_t>(aimesh->mNumVertices));

            if (aimesh->HasBones()) {
                for (size_t bi = 0; bi < static_cast<size_t>(aimesh->mNumBones); bi++) {
                    const aiBone* aibone = aimesh->mBones[bi];
                    const size_t bone_index = skeleton->GetBoneIndex(aibone->mName.C_Str());

                    for (size_t wi = 0; wi < static_cast<size_t>(aibone->mNumWeights); wi++) {
                        const aiVertexWeight& w = aibone->mWeights[wi];
                        if (static_cast<size_t>(w.mVertexId) < temp_weights.size()) {
                            temp_weights[w.mVertexId].push_back({bone_index, static_cast<real>(w.mWeight)});
                        }
                    }
                }
            }

            for (size_t v = 0; v < temp_weights.size(); v++) {
                auto& src = temp_weights[v];
                auto& dst = bone_weights[v];

                if (src.empty()) {
                    continue;
                }

                std::sort(src.begin(), src.end(),
                          [](const Mesh::BoneWeight& a, const Mesh::BoneWeight& b) { return a.weight > b.weight; });

                const size_t count = math::Min(src.size(), static_cast<size_t>(Mesh::MAX_BONE_CHANNEL));

                real weight_sum = 0.0_r;

                for (size_t i = 0; i < count; i++) {
                    dst[i] = src[i];
                    weight_sum += src[i].weight;
                }

                if (weight_sum > 0.0_r) {
                    for (size_t i = 0; i < count; i++) {
                        dst[i].weight /= weight_sum;
                    }
                }
            }

            // Indices
            for (size_t fi = 0; fi < static_cast<size_t>(aimesh->mNumFaces); fi++) {
                const aiFace& face = aimesh->mFaces[fi];
                for (size_t ii = 0; ii < static_cast<size_t>(face.mNumIndices); ii++) {
                    indices.push_back(static_cast<uint32_t>(face.mIndices[ii]));
                }
            }

            // Morph targets
            std::unordered_map<std::string, int> target_name_count;  // for handling duplicated morph target name
            for (size_t mti = 0; mti < static_cast<size_t>(aimesh->mNumAnimMeshes); mti++) {
                const aiAnimMesh* anim_mesh = aimesh->mAnimMeshes[mti];

                std::string mt_name;
                if (anim_mesh->mName.Empty()) {
                    mt_name =
                        m_name.append("_").append(sm_name).append("_unnamed_morph_target_").append(std::to_string(mti));
                } else {
                    mt_name = anim_mesh->mName.C_Str();
                    auto it = target_name_count.find(mt_name);
                    if (it == target_name_count.end()) {
                        target_name_count.insert({mt_name, 1});
                    } else {
                        mt_name += "_" + std::to_string(it->second);
                        it->second++;
                    }
                }

                std::vector<Vector3> mt_positions;
                mt_positions.reserve(static_cast<size_t>(anim_mesh->mNumVertices));
                std::vector<Vector3> mt_normals;
                mt_normals.reserve(static_cast<size_t>(anim_mesh->mNumVertices));
                std::vector<Vector4> mt_tangents;
                mt_tangents.reserve(static_cast<size_t>(anim_mesh->mNumVertices));

                const bool mt_has_pos = anim_mesh->HasPositions();
                const bool mt_has_normal = anim_mesh->HasNormals();
                const bool mt_has_tan = anim_mesh->HasTangentsAndBitangents();

                for (size_t vi = 0; vi < static_cast<size_t>(anim_mesh->mNumVertices); vi++) {
                    // Morph target positions
                    if (mt_has_pos) {
                        const aiVector3D& pos = anim_mesh->mVertices[vi];
                        mt_positions.emplace_back(static_cast<real>(pos.x), static_cast<real>(pos.y),
                                                  static_cast<real>(pos.z));
                    }

                    // Morph target Normals
                    if (mt_has_normal) {
                        const aiVector3D& normal = anim_mesh->mNormals[vi];
                        mt_normals.emplace_back(static_cast<real>(normal.x), static_cast<real>(normal.y),
                                                static_cast<real>(normal.z));
                    }

                    // Morph target tangents (handedness in w component)
                    if (mt_has_tan) {
                        const aiVector3D& tan = anim_mesh->mTangents[vi];
                        const aiVector3D& bitan = anim_mesh->mBitangents[vi];
                        Vector3 tangent(static_cast<real>(tan.x), static_cast<real>(tan.y), static_cast<real>(tan.z));
                        Vector3 bitangent(static_cast<real>(bitan.x), static_cast<real>(bitan.y),
                                          static_cast<real>(bitan.z));

                        // calcuate handedness
                        real h = 1.0_r;
                        if (has_normal) {
                            const Vector3& normal = normals[vi];
                            h = (normal.Cross(tangent).Dot(bitangent) > 0.0_r) ? 1.0_r : -1.0_r;
                        }
                        mt_tangents.emplace_back(tangent.x, tangent.y, tangent.z, h);
                    }
                }

                real weight = anim_mesh->mWeight;

                Mesh::MorphTarget morph_target(std::move(mt_name), std::move(mt_positions), std::move(mt_normals),
                                               std::move(mt_tangents), weight);

                morph_targets.push_back(std::move(morph_target));
            }

            ResourceID mat_id = materials[static_cast<size_t>(aimesh->mMaterialIndex)];

            Mesh::SubMesh sub_mesh(std::move(sm_name), prim_type, std::move(positions), std::move(normals),
                                   std::move(tangents), std::move(uvs), std::move(colors), std::move(bone_weights),
                                   std::move(indices), std::move(morph_targets), mat_id);

            sub_meshes.push_back(std::move(sub_mesh));
        }

        Mesh mesh(std::move(m_name), std::move(sub_meshes));

        return manager.AddMesh(std::move(mesh));
    }

    ResourceID ResourceLoader::LoadAnimation(const std::string& name, const aiAnimation* aianim,
                                             const Skeleton* skeleton, ResourceManager& manager) {
        std::string anim_name = name;

        real duration = (aianim->mDuration != 0.0) ? static_cast<real>(aianim->mDuration) : 1.0_r;
        real ticks_per_sec =
            (aianim->mTicksPerSecond != 0.0) ? static_cast<real>(aianim->mTicksPerSecond) : 25.0_r;  // fallback

        // Skeletal tracks
        std::vector<Animation::SkeletalTrack> skeletal_tracks;
        skeletal_tracks.reserve(aianim->mNumChannels);
        for (size_t ci = 0; ci < static_cast<size_t>(aianim->mNumChannels); ci++) {
            const aiNodeAnim* channel = aianim->mChannels[ci];
            if (!channel) {
                continue;
            }

            std::string bone_name = channel->mNodeName.C_Str();
            size_t bone_idx = skeleton->GetBoneIndex(bone_name);

            std::vector<Animation::TranslationKey> translations;
            translations.reserve(static_cast<size_t>(channel->mNumPositionKeys));
            Animation::InterpolationMode translation_interp_mode = Animation::INTERP_MODE_STEP;
            if (channel->mNumPositionKeys > 0) {
                switch (channel->mPositionKeys[0].mInterpolation) {
                    case aiAnimInterpolation_Step:
                        translation_interp_mode = Animation::INTERP_MODE_STEP;
                        break;
                    case aiAnimInterpolation_Linear:
                        translation_interp_mode = Animation::INTERP_MODE_LINEAR;
                        break;
                    case aiAnimInterpolation_Spherical_Linear:
                        translation_interp_mode = Animation::INTERP_MODE_SPHERICAL_LINEAR;
                        break;
                    case aiAnimInterpolation_Cubic_Spline:
                        translation_interp_mode = Animation::INTERP_MODE_CUBIC_SPLINE;
                        break;
                    default:
                        translation_interp_mode = Animation::INTERP_MODE_STEP;
                }
            }
            for (size_t ki = 0; ki < static_cast<size_t>(channel->mNumPositionKeys); ki++) {
                const aiVectorKey& k = channel->mPositionKeys[ki];
                translations.push_back({static_cast<real>(k.mTime) / ticks_per_sec,
                                        Vector3(static_cast<real>(k.mValue.x), static_cast<real>(k.mValue.y),
                                                static_cast<real>(k.mValue.z))});
            }

            std::vector<Animation::RotationKey> rotations;
            rotations.reserve(static_cast<size_t>(channel->mNumRotationKeys));
            Animation::InterpolationMode rotation_interp_mode = Animation::INTERP_MODE_STEP;
            if (channel->mNumRotationKeys > 0) {
                switch (channel->mRotationKeys[0].mInterpolation) {
                    case aiAnimInterpolation_Step:
                        rotation_interp_mode = Animation::INTERP_MODE_STEP;
                        break;
                    case aiAnimInterpolation_Linear:
                        rotation_interp_mode = Animation::INTERP_MODE_LINEAR;
                        break;
                    case aiAnimInterpolation_Spherical_Linear:
                        rotation_interp_mode = Animation::INTERP_MODE_SPHERICAL_LINEAR;
                        break;
                    case aiAnimInterpolation_Cubic_Spline:
                        rotation_interp_mode = Animation::INTERP_MODE_CUBIC_SPLINE;
                        break;
                    default:
                        rotation_interp_mode = Animation::INTERP_MODE_STEP;
                }
            }
            for (size_t ki = 0; ki < static_cast<size_t>(channel->mNumRotationKeys); ki++) {
                const aiQuatKey& k = channel->mRotationKeys[ki];
                rotations.push_back({static_cast<real>(k.mTime) / ticks_per_sec,
                                     Quaternion(static_cast<real>(k.mValue.x), static_cast<real>(k.mValue.y),
                                                static_cast<real>(k.mValue.z), static_cast<real>(k.mValue.w))});
            }

            std::vector<Animation::ScalingKey> scalings;
            scalings.reserve(static_cast<size_t>(channel->mNumScalingKeys));
            Animation::InterpolationMode scaling_interp_mode = Animation::INTERP_MODE_STEP;
            if (channel->mNumScalingKeys > 0) {
                switch (channel->mScalingKeys[0].mInterpolation) {
                    case aiAnimInterpolation_Step:
                        scaling_interp_mode = Animation::INTERP_MODE_STEP;
                        break;
                    case aiAnimInterpolation_Linear:
                        scaling_interp_mode = Animation::INTERP_MODE_LINEAR;
                        break;
                    case aiAnimInterpolation_Spherical_Linear:
                        scaling_interp_mode = Animation::INTERP_MODE_SPHERICAL_LINEAR;
                        break;
                    case aiAnimInterpolation_Cubic_Spline:
                        scaling_interp_mode = Animation::INTERP_MODE_CUBIC_SPLINE;
                        break;
                    default:
                        scaling_interp_mode = Animation::INTERP_MODE_STEP;
                }
            }
            for (size_t ki = 0; ki < static_cast<size_t>(channel->mNumScalingKeys); ki++) {
                const aiVectorKey& k = channel->mScalingKeys[ki];
                scalings.push_back({static_cast<real>(k.mTime) / ticks_per_sec,
                                    Vector3(static_cast<real>(k.mValue.x), static_cast<real>(k.mValue.y),
                                            static_cast<real>(k.mValue.z))});
            }

            Animation::ExtrapolationMode pre_mode;
            switch (channel->mPreState) {
                case aiAnimBehaviour_DEFAULT:
                    pre_mode = Animation::EXTRAP_MODE_DEFAULT;
                    break;
                case aiAnimBehaviour_CONSTANT:
                    pre_mode = Animation::EXTRAP_MODE_CONSTANT;
                    break;
                case aiAnimBehaviour_LINEAR:
                    pre_mode = Animation::EXTRAP_MODE_LINEAR;
                    break;
                case aiAnimBehaviour_REPEAT:
                    pre_mode = Animation::EXTRAP_MODE_REPEAT;
                    break;
                default:
                    pre_mode = Animation::EXTRAP_MODE_DEFAULT;
            }
            Animation::ExtrapolationMode post_mode;
            switch (channel->mPostState) {
                case aiAnimBehaviour_DEFAULT:
                    post_mode = Animation::EXTRAP_MODE_DEFAULT;
                    break;
                case aiAnimBehaviour_CONSTANT:
                    post_mode = Animation::EXTRAP_MODE_CONSTANT;
                    break;
                case aiAnimBehaviour_LINEAR:
                    post_mode = Animation::EXTRAP_MODE_LINEAR;
                    break;
                case aiAnimBehaviour_REPEAT:
                    post_mode = Animation::EXTRAP_MODE_REPEAT;
                    break;
                default:
                    post_mode = Animation::EXTRAP_MODE_DEFAULT;
            }

            skeletal_tracks.emplace_back(bone_idx, std::move(translations), translation_interp_mode,
                                         std::move(rotations), rotation_interp_mode, std::move(scalings),
                                         scaling_interp_mode, pre_mode, post_mode);
        }

        // Morph Target tracks
        std::vector<Animation::MorphTargetTrack> morph_target_tracks;
        morph_target_tracks.reserve(static_cast<size_t>(aianim->mNumMorphMeshChannels));
        for (size_t ci = 0; ci < static_cast<size_t>(aianim->mNumMorphMeshChannels); ci++) {
            const aiMeshMorphAnim* channel = aianim->mMorphMeshChannels[ci];
            if (!channel) continue;

            std::string bone_name = channel->mName.C_Str();
            size_t bone_idx = skeleton->GetBoneIndex(bone_name);

            std::vector<Animation::MorphingKey> keys;
            keys.reserve(static_cast<size_t>(channel->mNumKeys));
            for (size_t ki = 0; ki < static_cast<size_t>(channel->mNumKeys); ki++) {
                const aiMeshMorphKey& k = channel->mKeys[ki];
                std::vector<size_t> target_indices;
                std::vector<real> weights;
                target_indices.reserve(static_cast<size_t>(k.mNumValuesAndWeights));
                weights.reserve(static_cast<size_t>(k.mNumValuesAndWeights));
                for (size_t vi = 0; vi < static_cast<size_t>(k.mNumValuesAndWeights); vi++) {
                    target_indices.push_back(static_cast<size_t>(k.mValues[vi]));
                    weights.push_back(static_cast<real>(k.mWeights[vi]));
                }
                keys.emplace_back(static_cast<real>(k.mTime) / ticks_per_sec, std::move(target_indices),
                                  std::move(weights));
            }

            morph_target_tracks.emplace_back(bone_idx, std::move(keys));
        }

        Animation animation(std::move(anim_name), duration / ticks_per_sec, std::move(skeletal_tracks),
                            std::move(morph_target_tracks));
        return manager.AddAnimation(std::move(animation));
    }

    ResourceID ResourceLoader::LoadSkin(const std::string& name, const ImportedModel& imp_model,
                                        const Skeleton* skeleton, ResourceManager& manager) {
        std::string skin_name = name;

        // Inverse bine poses
        std::vector<Transform3D> inverse_bind_poses(skeleton->bone_names.size());

        // inverse bind transform
        for (size_t mi = 0; mi < imp_model.scene->mNumMeshes; mi++) {
            const aiMesh* aimesh = imp_model.scene->mMeshes[mi];
            for (size_t bi = 0; bi < aimesh->mNumBones; bi++) {
                aiBone* aibone = aimesh->mBones[bi];
                std::string bone_name = aibone->mName.C_Str();
                size_t bone_index = skeleton->name_to_index.at(bone_name);
                const aiMatrix4x4& om = aibone->mOffsetMatrix;
                Matrix4x4 m(Vector4(om.a1, om.a2, om.a3, om.a4), Vector4(om.b1, om.b2, om.b3, om.b4),
                            Vector4(om.c1, om.c2, om.c3, om.c4), Vector4(om.d1, om.d2, om.d3, om.d4));
                inverse_bind_poses[bone_index] = Transform3D(m);
            }
        }

        std::vector<std::vector<size_t>> bind_sub_meshes(imp_model.flatted_scene.size());
        for (size_t bi = 0; bi < imp_model.flatted_scene.size(); bi++) {
            const aiNode* ainode = imp_model.flatted_scene[bi];
            for (uint32_t mi = 0; mi < ainode->mNumMeshes; mi++) {
                bind_sub_meshes[bi].push_back(static_cast<size_t>(ainode->mMeshes[mi]));
            }
        }

        Skin skin(std::move(skin_name), std::move(inverse_bind_poses), std::move(bind_sub_meshes));

        return manager.AddSkin(std::move(skin));
    }
}  // namespace ho