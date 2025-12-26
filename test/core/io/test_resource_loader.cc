#include <gtest/gtest.h>

#include "core/io/resource_loader.h"
#include "resource/animation.h"
#include "resource/material.h"
#include "resource/mesh.h"
#include "resource/resource_manager.h"
#include "resource/skeleton.h"
#include "resource/skin.h"
#include "resource/texture.h"

using namespace ho;

class ResourceLoaderTest : public ::testing::Test {
   protected:
    ResourceManager resource_manager;
};

TEST_F(ResourceLoaderTest, LoadModelObjAllAttributeAndTexture) {
    Path obj_path(std::string("test_assets/test_obj/obj_cube/cube.obj"));
    ResourceLoader::Model model = ResourceLoader::LoadModel(obj_path, resource_manager);
    const Mesh* mesh = resource_manager.GetMesh(model.mesh);

    // Check Mesh
    for (const Mesh::SubMesh& sm : mesh->sub_meshes) {
        EXPECT_GT(sm.positions.size(), 0);
        if (!sm.normals.empty()) {
            EXPECT_EQ(sm.positions.size(), sm.normals.size());
        }
        if (!sm.tangents.empty()) {
            EXPECT_EQ(sm.positions.size(), sm.tangents.size());
        }
        if (!sm.uvs.empty()) {
            EXPECT_EQ(sm.positions.size(), sm.uvs.size());
        }
        if (!sm.colors.empty()) {
            EXPECT_EQ(sm.positions.size(), sm.colors.size());
        }
        if (!sm.bone_weights.empty()) {
            EXPECT_EQ(sm.positions.size(), sm.bone_weights.size());
        }
        for (uint32_t idx : sm.indices) {
            EXPECT_LT(idx, sm.positions.size());
        }

        EXPECT_EQ(sm.primitive_type, Mesh::PRIMITIVE_TYPE_TRIANGLE);
        EXPECT_EQ(sm.indices.size() % 3, 0);

        for (const Mesh::MorphTarget& mt : sm.morph_targets) {
            EXPECT_GT(mt.positions.size(), 0);
            if (!mt.normals.empty()) {
                EXPECT_EQ(mt.positions.size(), mt.normals.size());
            }
            if (!mt.tangents.empty()) {
                EXPECT_EQ(mt.positions.size(), mt.tangents.size());
            }
        }
        EXPECT_EQ(sm.morph_targets.size(), sm.morph_name_to_index.size());

        EXPECT_TRUE(sm.material.IsValid());
        EXPECT_LE(sm.aabb.min_edges.x, sm.aabb.max_edges.x);
        EXPECT_LE(sm.aabb.min_edges.y, sm.aabb.max_edges.y);
        EXPECT_LE(sm.aabb.min_edges.z, sm.aabb.max_edges.z);

        EXPECT_TRUE(sm.sphere.radius > 0.0_r);
    }

    EXPECT_EQ(mesh->sub_meshes.size(), mesh->sub_mesh_name_to_index.size());

    EXPECT_LE(mesh->aabb.min_edges.x, mesh->aabb.max_edges.x);
    EXPECT_LE(mesh->aabb.min_edges.y, mesh->aabb.max_edges.y);
    EXPECT_LE(mesh->aabb.min_edges.z, mesh->aabb.max_edges.z);

    EXPECT_TRUE(mesh->sphere.radius > 0.0_r);

    // Check Material
    const Material* tex_material = resource_manager.GetMaterial(resource_manager.GetMaterialID("texture"));
    EXPECT_EQ(tex_material->ambient, Color128(0.f, 0.f, 0.f));
    EXPECT_TRUE(tex_material->diffuse.IsEqualApprox(Color128(0.6f, 0.6f, 0.6f)));
    EXPECT_EQ(tex_material->specular, Color128(0.f, 0.f, 0.f));
    EXPECT_EQ(tex_material->shininess, 0.0_r);
    EXPECT_EQ(tex_material->albedo, Color128(0.f, 0.f, 0.f));
    EXPECT_EQ(tex_material->metallic, 1.0_r);
    EXPECT_EQ(tex_material->roughness, 1.0_r);
    EXPECT_EQ(tex_material->opacity, 1.0_r);
    EXPECT_EQ(tex_material->emissive, Color128(0.f, 0.f, 0.f));
    EXPECT_EQ(tex_material->emissive_intensity, 1.0_r);
    EXPECT_EQ(tex_material->blend_mode, Material::BLEND_MODE_DEFAULT);
    EXPECT_EQ(tex_material->wireframe_enabled, false);
    EXPECT_EQ(tex_material->backface_culling_enabled, true);

    const Texture* diffuse = resource_manager.GetTexture(tex_material->textures[TEXTURE_TYPE_DIFFUSE]);
    EXPECT_EQ(diffuse->name, std::string("test_diffuse.png"));
    EXPECT_EQ(diffuse->type, Texture::TEXTURE_TYPE_2D);
    EXPECT_EQ(diffuse->layered_type, Texture::TEXTURE_LAYERED_NONE);
    const Texture* specular = resource_manager.GetTexture(tex_material->textures[TEXTURE_TYPE_SPECULAR]);
    EXPECT_EQ(specular->name, std::string("test_specular.png"));
    EXPECT_EQ(specular->type, Texture::TEXTURE_TYPE_2D);
    EXPECT_EQ(specular->layered_type, Texture::TEXTURE_LAYERED_NONE);
    const Texture* specular_highlight = resource_manager.GetTexture(tex_material->textures[TEXTURE_TYPE_SHININESS]);
    EXPECT_EQ(specular_highlight->name, std::string("test_specular_highlight.png"));
    EXPECT_EQ(specular_highlight->type, Texture::TEXTURE_TYPE_2D);
    EXPECT_EQ(specular_highlight->layered_type, Texture::TEXTURE_LAYERED_NONE);
    const Texture* normal = resource_manager.GetTexture(tex_material->textures[TEXTURE_TYPE_NORMAL]);
    EXPECT_EQ(normal->name, std::string("test_bump.png"));
    EXPECT_EQ(normal->type, Texture::TEXTURE_TYPE_2D);
    EXPECT_EQ(normal->layered_type, Texture::TEXTURE_LAYERED_NONE);
    const Texture* alpha = resource_manager.GetTexture(tex_material->textures[TEXTURE_TYPE_OPACITY]);
    EXPECT_EQ(alpha->name, std::string("test_alpha.png"));
    EXPECT_EQ(alpha->type, Texture::TEXTURE_TYPE_2D);
    EXPECT_EQ(alpha->layered_type, Texture::TEXTURE_LAYERED_NONE);
    EXPECT_TRUE(tex_material->textures[TEXTURE_TYPE_ALBEDO].IsNULL());
    EXPECT_TRUE(tex_material->textures[TEXTURE_TYPE_EMISSION].IsNULL());
    EXPECT_TRUE(tex_material->textures[TEXTURE_TYPE_METALLIC_ROUGHNESS].IsNULL());
    EXPECT_TRUE(tex_material->textures[TEXTURE_TYPE_AMBIENT_OCCLUSION].IsNULL());

    const Material* attr_material = resource_manager.GetMaterial(resource_manager.GetMaterialID("attribute"));
    EXPECT_EQ(attr_material->ambient, Color128(0.2f, 0.2f, 0.2f));
    EXPECT_EQ(attr_material->diffuse, Color128(0.1f, 0.1f, 0.1f));
    EXPECT_EQ(attr_material->specular, Color128(1.f, 1.f, 1.f));
    EXPECT_EQ(attr_material->shininess, 10.0_r);
    EXPECT_EQ(attr_material->albedo, Color128(0.f, 0.f, 0.f));
    EXPECT_EQ(attr_material->metallic, 1.0_r);
    EXPECT_EQ(attr_material->roughness, 1.0_r);
    EXPECT_EQ(attr_material->opacity, 0.5_r);
    EXPECT_EQ(attr_material->emissive, Color128(0.f, 0.f, 0.f));
    EXPECT_EQ(attr_material->emissive_intensity, 1.0_r);
    EXPECT_EQ(attr_material->blend_mode, Material::BLEND_MODE_DEFAULT);
    EXPECT_EQ(attr_material->wireframe_enabled, false);
    EXPECT_EQ(attr_material->backface_culling_enabled, true);
    EXPECT_TRUE(attr_material->textures[TEXTURE_TYPE_DIFFUSE].IsNULL());
    EXPECT_TRUE(attr_material->textures[TEXTURE_TYPE_SPECULAR].IsNULL());
    EXPECT_TRUE(attr_material->textures[TEXTURE_TYPE_SHININESS].IsNULL());
    EXPECT_TRUE(attr_material->textures[TEXTURE_TYPE_OPACITY].IsNULL());
    EXPECT_TRUE(attr_material->textures[TEXTURE_TYPE_NORMAL].IsNULL());
    EXPECT_TRUE(attr_material->textures[TEXTURE_TYPE_ALBEDO].IsNULL());
    EXPECT_TRUE(attr_material->textures[TEXTURE_TYPE_EMISSION].IsNULL());
    EXPECT_TRUE(attr_material->textures[TEXTURE_TYPE_METALLIC_ROUGHNESS].IsNULL());
    EXPECT_TRUE(attr_material->textures[TEXTURE_TYPE_AMBIENT_OCCLUSION].IsNULL());
}

TEST_F(ResourceLoaderTest, LoadModelObjDuplicatedTexture) {
    Path obj_path(std::string("test_assets/test_obj/obj_cube_dup_texture/cube.obj"));
    ResourceLoader::Model model = ResourceLoader::LoadModel(obj_path, resource_manager);
    EXPECT_EQ(resource_manager.GetTextureCount(), 1);
    const Material* material = resource_manager.GetMaterial(resource_manager.GetMaterialID("texture"));

    const Texture* diffuse = resource_manager.GetTexture(material->textures[TEXTURE_TYPE_DIFFUSE]);
    const Texture* specular = resource_manager.GetTexture(material->textures[TEXTURE_TYPE_SPECULAR]);
    const Texture* specular_highlight = resource_manager.GetTexture(material->textures[TEXTURE_TYPE_SHININESS]);
    const Texture* normal = resource_manager.GetTexture(material->textures[TEXTURE_TYPE_NORMAL]);
    const Texture* alpha = resource_manager.GetTexture(material->textures[TEXTURE_TYPE_OPACITY]);

    EXPECT_EQ(diffuse, specular);
    EXPECT_EQ(diffuse, specular_highlight);
    EXPECT_EQ(diffuse, normal);
    EXPECT_EQ(diffuse, alpha);
}

TEST_F(ResourceLoaderTest, LoadModelObjMultipleMesh) {
    Path obj_path(std::string("test_assets/test_obj/obj_multiple_mesh/multi_mesh.obj"));
    ResourceLoader::Model model = ResourceLoader::LoadModel(obj_path, resource_manager);
    const Mesh* mesh = resource_manager.GetMesh(model.mesh);

    EXPECT_GE(mesh->sub_meshes.size(), 3);

    EXPECT_EQ(mesh->sub_meshes.size(), mesh->sub_mesh_name_to_index.size());

    std::vector<ResourceID> metal_materials;
    std::vector<ResourceID> plastic_materials;

    for (const auto& sm : mesh->sub_meshes) {
        const Material* mat = resource_manager.GetMaterial(sm.material);
        ASSERT_NE(mat, nullptr);

        if (mat->name == "metal") {
            metal_materials.push_back(sm.material);
        } else if (mat->name == "plastic") {
            plastic_materials.push_back(sm.material);
        }
    }

    EXPECT_GE(metal_materials.size(), 2);  // A + B
    EXPECT_GE(plastic_materials.size(), 1);

    for (size_t i = 1; i < metal_materials.size(); ++i) {
        EXPECT_EQ(metal_materials[0], metal_materials[i]);
    }

    EXPECT_EQ(resource_manager.GetTextureCount(), 1);

    const Material* metal = resource_manager.GetMaterial(resource_manager.GetMaterialID("metal"));
    const Material* plastic = resource_manager.GetMaterial(resource_manager.GetMaterialID("plastic"));

    const Texture* metal_diffuse = resource_manager.GetTexture(metal->textures[TEXTURE_TYPE_DIFFUSE]);
    const Texture* plastic_diffuse = resource_manager.GetTexture(plastic->textures[TEXTURE_TYPE_DIFFUSE]);

    EXPECT_EQ(metal_diffuse, plastic_diffuse);
    EXPECT_EQ(metal_diffuse->name, "test_diffuse.png");

    EXPECT_NE(metal->shininess, plastic->shininess);
    EXPECT_GT(metal->shininess, plastic->shininess);

    EXPECT_NE(metal->specular, plastic->specular);
}

TEST_F(ResourceLoaderTest, LoadModelglTFBox) {
    Path obj_path(std::string("test_assets/test_gltf/Box/glTF/Box.gltf"));
    ResourceLoader::Model model = ResourceLoader::LoadModel(obj_path, resource_manager);
    const Mesh* mesh = resource_manager.GetMesh(model.mesh);

    // Check Mesh
    EXPECT_EQ(mesh->sub_meshes.size(), 1);
    EXPECT_EQ(mesh->sub_meshes[0].name, std::string("Mesh"));

    for (const Mesh::SubMesh& sm : mesh->sub_meshes) {
        EXPECT_GT(sm.positions.size(), 0);
        if (!sm.normals.empty()) {
            EXPECT_EQ(sm.positions.size(), sm.normals.size());
        }
        if (!sm.tangents.empty()) {
            EXPECT_EQ(sm.positions.size(), sm.tangents.size());
        }
        if (!sm.uvs.empty()) {
            EXPECT_EQ(sm.positions.size(), sm.uvs.size());
        }
        if (!sm.colors.empty()) {
            EXPECT_EQ(sm.positions.size(), sm.colors.size());
        }
        if (!sm.bone_weights.empty()) {
            EXPECT_EQ(sm.positions.size(), sm.bone_weights.size());
        }
        for (uint32_t idx : sm.indices) {
            EXPECT_LT(idx, sm.positions.size());
        }

        EXPECT_EQ(sm.primitive_type, Mesh::PRIMITIVE_TYPE_TRIANGLE);
        EXPECT_EQ(sm.indices.size() % 3, 0);

        for (const Mesh::MorphTarget& mt : sm.morph_targets) {
            EXPECT_GT(mt.positions.size(), 0);
            if (!mt.normals.empty()) {
                EXPECT_EQ(mt.positions.size(), mt.normals.size());
            }
            if (!mt.tangents.empty()) {
                EXPECT_EQ(mt.positions.size(), mt.tangents.size());
            }
        }
        EXPECT_EQ(sm.morph_targets.size(), sm.morph_name_to_index.size());

        EXPECT_TRUE(sm.material.IsValid());
        EXPECT_LT(sm.material.id(), model.materials.size());
        EXPECT_LE(sm.aabb.min_edges.x, sm.aabb.max_edges.x);
        EXPECT_LE(sm.aabb.min_edges.y, sm.aabb.max_edges.y);
        EXPECT_LE(sm.aabb.min_edges.z, sm.aabb.max_edges.z);

        EXPECT_TRUE(sm.sphere.radius > 0.0_r);
    }

    EXPECT_EQ(mesh->sub_meshes.size(), mesh->sub_mesh_name_to_index.size());

    EXPECT_LE(mesh->aabb.min_edges.x, mesh->aabb.max_edges.x);
    EXPECT_LE(mesh->aabb.min_edges.y, mesh->aabb.max_edges.y);
    EXPECT_LE(mesh->aabb.min_edges.z, mesh->aabb.max_edges.z);

    EXPECT_TRUE(mesh->sphere.radius > 0.0_r);

    // Check Material
    const Material* material = resource_manager.GetMaterial(resource_manager.GetMaterialID("Red"));
    EXPECT_EQ(material->ambient, Color128(0.f, 0.f, 0.f));
    EXPECT_TRUE(material->diffuse.IsEqualApprox(Color128(0.800000011920929f, 0.f, 0.f)));
    EXPECT_EQ(material->specular, Color128(0.f, 0.f, 0.f));
    EXPECT_EQ(material->shininess, 0.0_r);
    EXPECT_TRUE(material->albedo.IsEqualApprox(Color128(0.800000011920929f, 0.f, 0.f)));
    EXPECT_EQ(material->metallic, 0.0_r);
    EXPECT_EQ(material->roughness, 1.0_r);
    EXPECT_EQ(material->opacity, 1.0_r);
    EXPECT_EQ(material->emissive, Color128(0.f, 0.f, 0.f));
    EXPECT_EQ(material->emissive_intensity, 1.0_r);
    EXPECT_EQ(material->blend_mode, Material::BLEND_MODE_DEFAULT);
    EXPECT_EQ(material->wireframe_enabled, false);
    EXPECT_EQ(material->backface_culling_enabled, true);
    EXPECT_TRUE(material->textures[TEXTURE_TYPE_DIFFUSE].IsNULL());
    EXPECT_TRUE(material->textures[TEXTURE_TYPE_SPECULAR].IsNULL());
    EXPECT_TRUE(material->textures[TEXTURE_TYPE_SHININESS].IsNULL());
    EXPECT_TRUE(material->textures[TEXTURE_TYPE_OPACITY].IsNULL());
    EXPECT_TRUE(material->textures[TEXTURE_TYPE_NORMAL].IsNULL());
    EXPECT_TRUE(material->textures[TEXTURE_TYPE_ALBEDO].IsNULL());
    EXPECT_TRUE(material->textures[TEXTURE_TYPE_EMISSION].IsNULL());
    EXPECT_TRUE(material->textures[TEXTURE_TYPE_METALLIC_ROUGHNESS].IsNULL());
    EXPECT_TRUE(material->textures[TEXTURE_TYPE_AMBIENT_OCCLUSION].IsNULL());

    // Check Skeleton
    const Skeleton* skeleton = resource_manager.GetSkeleton(model.skeleton);
    EXPECT_EQ(skeleton->bone_names.size(), 2);
    EXPECT_EQ(skeleton->local_transforms.size(), 2);
    EXPECT_EQ(skeleton->parents.size(), 2);
    EXPECT_EQ(skeleton->children.size(), 2);
    EXPECT_EQ(skeleton->name_to_index.size(), 2);

    EXPECT_EQ(skeleton->parents[0], -1);
    EXPECT_EQ(skeleton->children[0].size(), 1);
    EXPECT_EQ(skeleton->children[0][0], 1);
    Matrix4x4 local_t(Vector4(1.0_r, 0.0_r, 0.0_r, 0.0_r), Vector4(0.0_r, 0.0_r, 1.0_r, 0.0_r),
                      Vector4(0.0_r, -1.0_r, 0.0_r, 0.0_r), Vector4(0.0_r, 0.0_r, 0.0_r, 1.0_r));
    EXPECT_EQ(skeleton->local_transforms[0], Transform3D(local_t));
    EXPECT_EQ(skeleton->parents[1], 0);
    EXPECT_EQ(skeleton->children[1].size(), 0);
    EXPECT_EQ(skeleton->local_transforms[1], Transform3D());

    // Check Skin
    const Skin* skin = resource_manager.GetSkin(model.skin);
    EXPECT_EQ(skin->inverse_bind_poses.size(), 2);
    EXPECT_EQ(skin->bind_sub_meshes.size(), 2);
    EXPECT_EQ(skin->inverse_bind_poses[0], Transform3D());
    EXPECT_EQ(skin->inverse_bind_poses[1], Transform3D());
    EXPECT_EQ(skin->bind_sub_meshes[0].size(), 0);
    EXPECT_EQ(skin->bind_sub_meshes[1].size(), 1);
    EXPECT_EQ(skin->bind_sub_meshes[1][0], 0);
}

TEST_F(ResourceLoaderTest, LoadModelglTFBoxInterleaved) {
    Path obj_path(std::string("test_assets/test_gltf/BoxInterleaved/glTF/BoxInterleaved.gltf"));
    ResourceLoader::Model model = ResourceLoader::LoadModel(obj_path, resource_manager);
    const Mesh* mesh = resource_manager.GetMesh(model.mesh);

    // Check Mesh
    EXPECT_EQ(mesh->sub_meshes.size(), 1);
    EXPECT_EQ(mesh->sub_meshes[0].name, std::string("Mesh"));

    for (const Mesh::SubMesh& sm : mesh->sub_meshes) {
        EXPECT_GT(sm.positions.size(), 0);
        if (!sm.normals.empty()) {
            EXPECT_EQ(sm.positions.size(), sm.normals.size());
        }
        if (!sm.tangents.empty()) {
            EXPECT_EQ(sm.positions.size(), sm.tangents.size());
        }
        if (!sm.uvs.empty()) {
            EXPECT_EQ(sm.positions.size(), sm.uvs.size());
        }
        if (!sm.colors.empty()) {
            EXPECT_EQ(sm.positions.size(), sm.colors.size());
        }
        if (!sm.bone_weights.empty()) {
            EXPECT_EQ(sm.positions.size(), sm.bone_weights.size());
        }
        for (uint32_t idx : sm.indices) {
            EXPECT_LT(idx, sm.positions.size());
        }

        EXPECT_EQ(sm.primitive_type, Mesh::PRIMITIVE_TYPE_TRIANGLE);
        EXPECT_EQ(sm.indices.size() % 3, 0);

        for (const Mesh::MorphTarget& mt : sm.morph_targets) {
            EXPECT_GT(mt.positions.size(), 0);
            if (!mt.normals.empty()) {
                EXPECT_EQ(mt.positions.size(), mt.normals.size());
            }
            if (!mt.tangents.empty()) {
                EXPECT_EQ(mt.positions.size(), mt.tangents.size());
            }
        }
        EXPECT_EQ(sm.morph_targets.size(), sm.morph_name_to_index.size());

        EXPECT_TRUE(sm.material.IsValid());
        EXPECT_LT(sm.material.id(), model.materials.size());
        EXPECT_LE(sm.aabb.min_edges.x, sm.aabb.max_edges.x);
        EXPECT_LE(sm.aabb.min_edges.y, sm.aabb.max_edges.y);
        EXPECT_LE(sm.aabb.min_edges.z, sm.aabb.max_edges.z);

        EXPECT_TRUE(sm.sphere.radius > 0.0_r);
    }

    EXPECT_EQ(mesh->sub_meshes.size(), mesh->sub_mesh_name_to_index.size());

    EXPECT_LE(mesh->aabb.min_edges.x, mesh->aabb.max_edges.x);
    EXPECT_LE(mesh->aabb.min_edges.y, mesh->aabb.max_edges.y);
    EXPECT_LE(mesh->aabb.min_edges.z, mesh->aabb.max_edges.z);

    EXPECT_TRUE(mesh->sphere.radius > 0.0_r);

    // Check Material
    const Material* material =
        resource_manager.GetMaterial(resource_manager.GetMaterialID("BoxInterleaved.gltf_unnamed_material_0"));
    EXPECT_EQ(material->ambient, Color128(0.f, 0.f, 0.f));
    EXPECT_TRUE(material->diffuse.IsEqualApprox(Color128(0.800000011920929f, 0.f, 0.f)));
    EXPECT_EQ(material->specular, Color128(0.f, 0.f, 0.f));
    EXPECT_EQ(material->shininess, 0.0_r);
    EXPECT_TRUE(material->albedo.IsEqualApprox(Color128(0.800000011920929f, 0.f, 0.f)));
    EXPECT_EQ(material->metallic, 1.0_r);
    EXPECT_EQ(material->roughness, 1.0_r);
    EXPECT_EQ(material->opacity, 1.0_r);
    EXPECT_EQ(material->emissive, Color128(0.f, 0.f, 0.f));
    EXPECT_EQ(material->emissive_intensity, 1.0_r);
    EXPECT_EQ(material->blend_mode, Material::BLEND_MODE_DEFAULT);
    EXPECT_EQ(material->wireframe_enabled, false);
    EXPECT_EQ(material->backface_culling_enabled, true);
    EXPECT_TRUE(material->textures[TEXTURE_TYPE_DIFFUSE].IsNULL());
    EXPECT_TRUE(material->textures[TEXTURE_TYPE_SPECULAR].IsNULL());
    EXPECT_TRUE(material->textures[TEXTURE_TYPE_SHININESS].IsNULL());
    EXPECT_TRUE(material->textures[TEXTURE_TYPE_OPACITY].IsNULL());
    EXPECT_TRUE(material->textures[TEXTURE_TYPE_NORMAL].IsNULL());
    EXPECT_TRUE(material->textures[TEXTURE_TYPE_ALBEDO].IsNULL());
    EXPECT_TRUE(material->textures[TEXTURE_TYPE_EMISSION].IsNULL());
    EXPECT_TRUE(material->textures[TEXTURE_TYPE_METALLIC_ROUGHNESS].IsNULL());
    EXPECT_TRUE(material->textures[TEXTURE_TYPE_AMBIENT_OCCLUSION].IsNULL());

    // Check Skeleton
    const Skeleton* skeleton = resource_manager.GetSkeleton(model.skeleton);
    EXPECT_EQ(skeleton->bone_names.size(), 2);
    EXPECT_EQ(skeleton->local_transforms.size(), 2);
    EXPECT_EQ(skeleton->parents.size(), 2);
    EXPECT_EQ(skeleton->children.size(), 2);
    EXPECT_EQ(skeleton->name_to_index.size(), 2);

    EXPECT_EQ(skeleton->parents[0], -1);
    EXPECT_EQ(skeleton->children[0].size(), 1);
    EXPECT_EQ(skeleton->children[0][0], 1);
    Matrix4x4 local_t(Vector4(1.0_r, 0.0_r, 0.0_r, 0.0_r), Vector4(0.0_r, 0.0_r, 1.0_r, 0.0_r),
                      Vector4(0.0_r, -1.0_r, 0.0_r, 0.0_r), Vector4(0.0_r, 0.0_r, 0.0_r, 1.0_r));
    EXPECT_EQ(skeleton->local_transforms[0], Transform3D(local_t));
    EXPECT_EQ(skeleton->parents[1], 0);
    EXPECT_EQ(skeleton->children[1].size(), 0);
    EXPECT_EQ(skeleton->local_transforms[1], Transform3D());

    // Check Skin
    const Skin* skin = resource_manager.GetSkin(model.skin);
    EXPECT_EQ(skin->inverse_bind_poses.size(), 2);
    EXPECT_EQ(skin->bind_sub_meshes.size(), 2);
    EXPECT_EQ(skin->inverse_bind_poses[0], Transform3D());
    EXPECT_EQ(skin->inverse_bind_poses[1], Transform3D());
    EXPECT_EQ(skin->bind_sub_meshes[0].size(), 0);
    EXPECT_EQ(skin->bind_sub_meshes[1].size(), 1);
    EXPECT_EQ(skin->bind_sub_meshes[1][0], 0);
}

TEST_F(ResourceLoaderTest, LoadModelglTFBoxVertexColors) {
    Path path(std::string("test_assets/test_gltf/BoxVertexColors/glTF/BoxVertexColors.gltf"));
    ResourceLoader::Model model = ResourceLoader::LoadModel(path, resource_manager);
    const Mesh* mesh = resource_manager.GetMesh(model.mesh);

    ASSERT_EQ(mesh->sub_meshes.size(), 1);
    const auto& sm = mesh->sub_meshes[0];

    EXPECT_GT(sm.positions.size(), 0);
    EXPECT_FALSE(sm.colors.empty());
    EXPECT_EQ(sm.positions.size(), sm.colors.size());

    EXPECT_TRUE(sm.material.IsValid());
}

TEST_F(ResourceLoaderTest, LoadModelglTFBoxTextured) {
    Path path(std::string("test_assets/test_gltf/BoxTextured/glTF/BoxTextured.gltf"));
    ResourceLoader::Model model = ResourceLoader::LoadModel(path, resource_manager);
    const Mesh* mesh = resource_manager.GetMesh(model.mesh);

    ASSERT_EQ(mesh->sub_meshes.size(), 1);
    const auto& sm = mesh->sub_meshes[0];

    // Check Mesh
    EXPECT_GT(sm.positions.size(), 0);
    EXPECT_FALSE(sm.uvs.empty());
    EXPECT_EQ(sm.positions.size(), sm.uvs.size());

    // Check Material
    EXPECT_TRUE(sm.material.IsValid());
    const Material* mat = resource_manager.GetMaterial(sm.material);

    const Texture* albedo = resource_manager.GetTexture(mat->textures[TEXTURE_TYPE_ALBEDO]);
    EXPECT_EQ(albedo->name, std::string("test_albedo.png"));
    EXPECT_EQ(albedo->type, Texture::TEXTURE_TYPE_2D);
    EXPECT_EQ(albedo->layered_type, Texture::TEXTURE_LAYERED_NONE);
    const Texture* metallic_roughness = resource_manager.GetTexture(mat->textures[TEXTURE_TYPE_METALLIC_ROUGHNESS]);
    EXPECT_EQ(metallic_roughness->name, std::string("test_metallic_roughness.png"));
    EXPECT_EQ(metallic_roughness->type, Texture::TEXTURE_TYPE_2D);
    EXPECT_EQ(metallic_roughness->layered_type, Texture::TEXTURE_LAYERED_NONE);
    const Texture* normal = resource_manager.GetTexture(mat->textures[TEXTURE_TYPE_NORMAL]);
    EXPECT_EQ(normal->name, std::string("test_normal.png"));
    EXPECT_EQ(normal->type, Texture::TEXTURE_TYPE_2D);
    EXPECT_EQ(normal->layered_type, Texture::TEXTURE_LAYERED_NONE);
    const Texture* occlusion = resource_manager.GetTexture(mat->textures[TEXTURE_TYPE_AMBIENT_OCCLUSION]);
    EXPECT_EQ(occlusion->name, std::string("test_occlusion.png"));
    EXPECT_EQ(occlusion->type, Texture::TEXTURE_TYPE_2D);
    EXPECT_EQ(occlusion->layered_type, Texture::TEXTURE_LAYERED_NONE);
    const Texture* emissive = resource_manager.GetTexture(mat->textures[TEXTURE_TYPE_EMISSION]);
    EXPECT_EQ(emissive->name, std::string("test_emissive.png"));
    EXPECT_EQ(emissive->type, Texture::TEXTURE_TYPE_2D);
    EXPECT_EQ(emissive->layered_type, Texture::TEXTURE_LAYERED_NONE);
}

TEST_F(ResourceLoaderTest, LoadModelglTFRiggidFiture) {
    Path path(std::string("test_assets/test_gltf/RiggedFigure/glTF/RiggedFigure.gltf"));
    ResourceLoader::Model model = ResourceLoader::LoadModel(path, resource_manager);
    const Mesh* mesh = resource_manager.GetMesh(model.mesh);
    std::vector<const Material*> materials;
    for (ResourceID id : model.materials) {
        materials.push_back(resource_manager.GetMaterial(id));
    }
    const Skeleton* skeleton = resource_manager.GetSkeleton(model.skeleton);
    std::vector<const Animation*> animations;
    for (ResourceID id : model.animations) {
        animations.push_back(resource_manager.GetAnimation(id));
    }
    const Skin* skin = resource_manager.GetSkin(model.skin);

    const auto& sm = mesh->sub_meshes[0];
    EXPECT_LT(0, sm.positions.size());
    EXPECT_EQ(sm.positions.size(), sm.bone_weights.size());
    for (const auto& bws : sm.bone_weights) {
        real sum = 0.0_r;
        for (const Mesh::BoneWeight& bw : bws) {
            EXPECT_GE(bw.bone_index, 0);
            EXPECT_LT(bw.bone_index, skeleton->bone_names.size());
            sum += bw.weight;
        }
        EXPECT_NEAR(sum, 1.0_r, 1e-4_r);
    }

    EXPECT_LT(0, skeleton->bone_names.size());
    EXPECT_EQ(skeleton->bone_names.size(), skeleton->local_transforms.size());
    EXPECT_EQ(skeleton->bone_names.size(), skeleton->parents.size());
    EXPECT_EQ(skeleton->bone_names.size(), skeleton->children.size());
    EXPECT_EQ(skeleton->bone_names.size(), skeleton->name_to_index.size());
    EXPECT_EQ(skeleton->bone_names.size(), skin->inverse_bind_poses.size());
    EXPECT_EQ(skeleton->bone_names.size(), skin->bind_sub_meshes.size());

    // Check Bone Hierarchy

    // ========================
    // Torso
    // ========================
    EXPECT_EQ(skeleton->GetChildCount("torso_joint_1"), 3u);
    {
        std::unordered_set<std::string> expected_children = {"torso_joint_2", "leg_joint_L_1", "leg_joint_R_1"};

        for (uint32_t i = 0; i < 3; ++i) {
            const std::string& child_name = skeleton->bone_names[skeleton->GetChildIndex("torso_joint_1", i)];
            EXPECT_TRUE(expected_children.find(child_name) != expected_children.end());
        }
    }

    EXPECT_EQ(skeleton->GetParentIndex("torso_joint_2"), skeleton->GetBoneIndex("torso_joint_1"));
    EXPECT_EQ(skeleton->GetChildCount("torso_joint_2"), 1u);
    EXPECT_EQ(skeleton->GetChildIndex("torso_joint_2", 0), skeleton->GetBoneIndex("torso_joint_3"));

    EXPECT_EQ(skeleton->GetParentIndex("torso_joint_3"), skeleton->GetBoneIndex("torso_joint_2"));
    EXPECT_EQ(skeleton->GetChildCount("torso_joint_3"), 3u);
    {
        std::unordered_set<std::string> expected_children = {"arm_joint_R_1", "arm_joint_L_1", "neck_joint_1"};

        for (uint32_t i = 0; i < 3; ++i) {
            const std::string& child_name = skeleton->bone_names[skeleton->GetChildIndex("torso_joint_3", i)];
            EXPECT_TRUE(expected_children.find(child_name) != expected_children.end());
        }
    }

    // Neck
    EXPECT_EQ(skeleton->GetParentIndex("neck_joint_1"), skeleton->GetBoneIndex("torso_joint_3"));
    EXPECT_EQ(skeleton->GetChildCount("neck_joint_1"), 1u);
    EXPECT_EQ(skeleton->GetChildIndex("neck_joint_1", 0), skeleton->GetBoneIndex("neck_joint_2"));

    EXPECT_EQ(skeleton->GetParentIndex("neck_joint_2"), skeleton->GetBoneIndex("neck_joint_1"));
    EXPECT_EQ(skeleton->GetChildCount("neck_joint_2"), 0u);

    // Right Arm
    EXPECT_EQ(skeleton->GetParentIndex("arm_joint_R_1"), skeleton->GetBoneIndex("torso_joint_3"));
    EXPECT_EQ(skeleton->GetChildCount("arm_joint_R_1"), 1u);
    EXPECT_EQ(skeleton->GetChildIndex("arm_joint_R_1", 0), skeleton->GetBoneIndex("arm_joint_R_2"));

    EXPECT_EQ(skeleton->GetParentIndex("arm_joint_R_2"), skeleton->GetBoneIndex("arm_joint_R_1"));
    EXPECT_EQ(skeleton->GetChildCount("arm_joint_R_2"), 1u);
    EXPECT_EQ(skeleton->GetChildIndex("arm_joint_R_2", 0), skeleton->GetBoneIndex("arm_joint_R_3"));

    EXPECT_EQ(skeleton->GetParentIndex("arm_joint_R_3"), skeleton->GetBoneIndex("arm_joint_R_2"));
    EXPECT_EQ(skeleton->GetChildCount("arm_joint_R_3"), 0u);

    // Left Arm
    EXPECT_EQ(skeleton->GetParentIndex("arm_joint_L_1"), skeleton->GetBoneIndex("torso_joint_3"));
    EXPECT_EQ(skeleton->GetChildCount("arm_joint_L_1"), 1u);
    EXPECT_EQ(skeleton->GetChildIndex("arm_joint_L_1", 0), skeleton->GetBoneIndex("arm_joint_L_2"));

    EXPECT_EQ(skeleton->GetParentIndex("arm_joint_L_2"), skeleton->GetBoneIndex("arm_joint_L_1"));
    EXPECT_EQ(skeleton->GetChildCount("arm_joint_L_2"), 1u);
    EXPECT_EQ(skeleton->GetChildIndex("arm_joint_L_2", 0), skeleton->GetBoneIndex("arm_joint_L_3"));

    EXPECT_EQ(skeleton->GetParentIndex("arm_joint_L_3"), skeleton->GetBoneIndex("arm_joint_L_2"));
    EXPECT_EQ(skeleton->GetChildCount("arm_joint_L_3"), 0u);

    // Right Leg
    EXPECT_EQ(skeleton->GetParentIndex("leg_joint_R_1"), skeleton->GetBoneIndex("torso_joint_1"));
    EXPECT_EQ(skeleton->GetChildCount("leg_joint_R_1"), 1u);
    EXPECT_EQ(skeleton->GetChildIndex("leg_joint_R_1", 0), skeleton->GetBoneIndex("leg_joint_R_2"));

    EXPECT_EQ(skeleton->GetParentIndex("leg_joint_R_2"), skeleton->GetBoneIndex("leg_joint_R_1"));
    EXPECT_EQ(skeleton->GetChildCount("leg_joint_R_2"), 1u);
    EXPECT_EQ(skeleton->GetChildIndex("leg_joint_R_2", 0), skeleton->GetBoneIndex("leg_joint_R_3"));

    EXPECT_EQ(skeleton->GetParentIndex("leg_joint_R_3"), skeleton->GetBoneIndex("leg_joint_R_2"));
    EXPECT_EQ(skeleton->GetChildCount("leg_joint_R_3"), 1u);
    EXPECT_EQ(skeleton->GetChildIndex("leg_joint_R_3", 0), skeleton->GetBoneIndex("leg_joint_R_5"));

    EXPECT_EQ(skeleton->GetParentIndex("leg_joint_R_5"), skeleton->GetBoneIndex("leg_joint_R_3"));
    EXPECT_EQ(skeleton->GetChildCount("leg_joint_R_5"), 0u);

    // Left Leg
    EXPECT_EQ(skeleton->GetParentIndex("leg_joint_L_1"), skeleton->GetBoneIndex("torso_joint_1"));
    EXPECT_EQ(skeleton->GetChildCount("leg_joint_L_1"), 1u);
    EXPECT_EQ(skeleton->GetChildIndex("leg_joint_L_1", 0), skeleton->GetBoneIndex("leg_joint_L_2"));

    EXPECT_EQ(skeleton->GetParentIndex("leg_joint_L_2"), skeleton->GetBoneIndex("leg_joint_L_1"));
    EXPECT_EQ(skeleton->GetChildCount("leg_joint_L_2"), 1u);
    EXPECT_EQ(skeleton->GetChildIndex("leg_joint_L_2", 0), skeleton->GetBoneIndex("leg_joint_L_3"));

    EXPECT_EQ(skeleton->GetParentIndex("leg_joint_L_3"), skeleton->GetBoneIndex("leg_joint_L_2"));
    EXPECT_EQ(skeleton->GetChildCount("leg_joint_L_3"), 1u);
    EXPECT_EQ(skeleton->GetChildIndex("leg_joint_L_3", 0), skeleton->GetBoneIndex("leg_joint_L_5"));

    EXPECT_EQ(skeleton->GetParentIndex("leg_joint_L_5"), skeleton->GetBoneIndex("leg_joint_L_3"));
    EXPECT_EQ(skeleton->GetChildCount("leg_joint_L_5"), 0u);

    // Check Bone Local Transform
    Matrix3x3 s;
    Matrix3x3 r;
    Matrix3x3 rs;
    Vector3 t;

    Matrix4x4 lt;

    // torso_joint_1
    s = Matrix3x3(Vector3(1.0_r, 0.0_r, 0.0_r), Vector3(0.0_r, 1.0_r, 0.0_r),
                  Vector3(0.0_r, 0.0_r, 1.0000001192092896_r));

    r = Matrix3x3::FromQuaternion(
        Quaternion(0.03792940452694893_r, 0.002913428470492363_r, -0.00011058452219003811_r, -0.9992762207984924_r));

    rs = r * s;
    t = Vector3(2.7939699442924854e-9_r, -1.4156600514070309e-7_r, 0.6860002279281616_r);

    lt = skeleton->GetLocalTransform("torso_joint_1").ToMatrix();
    EXPECT_TRUE(lt.ToMatrix3x3().IsEqualApprox(rs));
    EXPECT_TRUE(Vector3(lt.col3().x, lt.col3().y, lt.col3().z).IsEqualApprox(t));

    // leg_joint_R_1
    s = Matrix3x3(Vector3(1.0_r, 0.0_r, 0.0_r), Vector3(0.0_r, 1.000000238418579_r, 0.0_r),
                  Vector3(0.0_r, 0.0_r, 0.9999999403953552_r));

    r = Matrix3x3::FromQuaternion(
        Quaternion(-0.02339874766767025_r, -0.6542636752128601_r, 0.754464864730835_r, 0.046630214899778369_r));

    rs = r * s;
    t = Vector3(-0.06845717132091522_r, 0.004460853058844805_r, -0.07147114723920822_r);

    lt = skeleton->GetLocalTransform("leg_joint_R_1").ToMatrix();
    EXPECT_TRUE(lt.ToMatrix3x3().IsEqualApprox(rs));
    EXPECT_TRUE(Vector3(lt.col3().x, lt.col3().y, lt.col3().z).IsEqualApprox(t));

    // leg_joint_R_2
    s = Matrix3x3(Vector3(1.0000001192092896_r, 0.0_r, 0.0_r), Vector3(0.0_r, 1.0_r, 0.0_r),
                  Vector3(0.0_r, 0.0_r, 1.0_r));

    r = Matrix3x3::FromQuaternion(
        Quaternion(0.21606147289276124_r, 0.08108008652925492_r, -0.010079992935061457_r, -0.9729552268981934_r));

    rs = r * s;
    t = Vector3(0.0_r, 0.2661120891571045_r, 1.4901200273698124e-8_r);

    lt = skeleton->GetLocalTransform("leg_joint_R_2").ToMatrix();
    EXPECT_TRUE(lt.ToMatrix3x3().IsEqualApprox(rs));
    EXPECT_TRUE(Vector3(lt.col3().x, lt.col3().y, lt.col3().z).IsEqualApprox(t));

    // leg_joint_R_3
    s = Matrix3x3(Vector3(0.9999999403953552_r, 0.0_r, 0.0_r), Vector3(0.0_r, 1.0_r, 0.0_r),
                  Vector3(0.0_r, 0.0_r, 1.0_r));

    r = Matrix3x3::FromQuaternion(
        Quaternion(0.8471670746803284_r, -0.03204828128218651_r, -0.024840382859110833_r, -0.5297772288322449_r));

    rs = r * s;
    t = Vector3(-7.450579708745408e-9_r, 0.27582409977912905_r, -3.725289854372704e-9_r);

    lt = skeleton->GetLocalTransform("leg_joint_R_3").ToMatrix();
    EXPECT_TRUE(lt.ToMatrix3x3().IsEqualApprox(rs));
    EXPECT_TRUE(Vector3(lt.col3().x, lt.col3().y, lt.col3().z).IsEqualApprox(t));

    // leg_joint_R_5
    s = Matrix3x3(Vector3(0.9999999403953552_r, 0.0_r, 0.0_r), Vector3(0.0_r, 0.9999997615814208_r, 0.0_r),
                  Vector3(0.0_r, 0.0_r, 0.9999991655349731_r));

    r = Matrix3x3::FromQuaternion(
        Quaternion(-0.03414325416088104_r, -0.3191778957843781_r, 0.9461711049079896_r, -0.04146831855177879_r));

    rs = r * s;
    t = Vector3(-0.0014585329918190837_r, -0.06619873642921448_r, 0.027856800705194474_r);

    lt = skeleton->GetLocalTransform("leg_joint_R_5").ToMatrix();
    EXPECT_TRUE(lt.ToMatrix3x3().IsEqualApprox(rs));
    EXPECT_TRUE(Vector3(lt.col3().x, lt.col3().y, lt.col3().z).IsEqualApprox(t));

    // leg_joint_L_1
    s = Matrix3x3(Vector3(1.0000001192092896_r, 0.0_r, 0.0_r), Vector3(0.0_r, 1.000000238418579_r, 0.0_r),
                  Vector3(0.0_r, 0.0_r, 1.0000001192092896_r));

    r = Matrix3x3::FromQuaternion(
        Quaternion(0.21088078618049625_r, -0.6243308186531067_r, 0.724772036075592_r, -0.2011117935180664_r));

    rs = r * s;
    t = Vector3(0.06761927157640457_r, 0.004461091011762619_r, -0.07226461172103882_r);

    lt = skeleton->GetLocalTransform("leg_joint_L_1").ToMatrix();
    EXPECT_TRUE(lt.ToMatrix3x3().IsEqualApprox(rs));
    EXPECT_TRUE(Vector3(lt.col3().x, lt.col3().y, lt.col3().z).IsEqualApprox(t));

    // leg_joint_L_2
    s = Matrix3x3(Vector3(1.0000001192092896_r, 0.0_r, 0.0_r), Vector3(0.0_r, 1.0000001192092896_r, 0.0_r),
                  Vector3(0.0_r, 0.0_r, 0.9999999403953552_r));

    r = Matrix3x3::FromQuaternion(
        Quaternion(0.2111543715000153_r, -0.29843246936798098_r, -0.04688597097992897_r, -0.92959862947464_r));

    rs = r * s;
    t = Vector3(0.0_r, 0.2661122083663941_r, 0.0_r);

    lt = skeleton->GetLocalTransform("leg_joint_L_2").ToMatrix();
    EXPECT_TRUE(lt.ToMatrix3x3().IsEqualApprox(rs));
    EXPECT_TRUE(Vector3(lt.col3().x, lt.col3().y, lt.col3().z).IsEqualApprox(t));

    // leg_joint_L_3
    s = Matrix3x3(Vector3(1.0_r, 0.0_r, 0.0_r), Vector3(0.0_r, 1.0_r, 0.0_r),
                  Vector3(0.0_r, 0.0_r, 1.0000001192092896_r));

    r = Matrix3x3::FromQuaternion(
        Quaternion(0.8477688431739807_r, -0.002281580353155732_r, -0.006338709034025669_r, -0.530323326587677_r));

    rs = r * s;
    t = Vector3(0.0_r, 0.27582401037216189_r, 0.0_r);

    lt = skeleton->GetLocalTransform("leg_joint_L_3").ToMatrix();
    EXPECT_TRUE(lt.ToMatrix3x3().IsEqualApprox(rs));
    EXPECT_TRUE(Vector3(lt.col3().x, lt.col3().y, lt.col3().z).IsEqualApprox(t));

    // leg_joint_L_5
    s = Matrix3x3(Vector3(1.0_r, 0.0_r, 0.0_r), Vector3(0.0_r, 1.0000007152557374_r, 0.0_r),
                  Vector3(0.0_r, 0.0_r, 1.0000009536743165_r));

    r = Matrix3x3::FromQuaternion(
        Quaternion(0.024532083421945573_r, -0.3199966549873352_r, 0.9446004033088684_r, 0.06878151744604111_r));

    rs = r * s;
    t = Vector3(-0.0023464928381145_r, -0.06617330759763718_r, 0.02785675972700119_r);

    lt = skeleton->GetLocalTransform("leg_joint_L_5").ToMatrix();
    EXPECT_TRUE(lt.ToMatrix3x3().IsEqualApprox(rs));
    EXPECT_TRUE(Vector3(lt.col3().x, lt.col3().y, lt.col3().z).IsEqualApprox(t));

    // torso_joint_2
    s = Matrix3x3(Vector3(1.0000001192092896_r, 0.0_r, 0.0_r), Vector3(0.0_r, 0.9999999403953552_r, 0.0_r),
                  Vector3(0.0_r, 0.0_r, 1.0_r));

    r = Matrix3x3::FromQuaternion(
        Quaternion(-0.7380747199058533_r, -0.001967150717973709_r, 0.0021518853027373554_r, -0.6747126579284668_r));

    rs = r * s;
    t = Vector3(0.0009999809553846717_r, -4.842879874900064e-8_r, 0.1714905947446823_r);

    lt = skeleton->GetLocalTransform("torso_joint_2").ToMatrix();
    EXPECT_TRUE(lt.ToMatrix3x3().IsEqualApprox(rs));
    EXPECT_TRUE(Vector3(lt.col3().x, lt.col3().y, lt.col3().z).IsEqualApprox(t));

    // torso_joint_3
    s = Matrix3x3(Vector3(1.0_r, 0.0_r, 0.0_r), Vector3(0.0_r, 1.0000001192092896_r, 0.0_r),
                  Vector3(0.0_r, 0.0_r, 1.0_r));

    r = Matrix3x3::FromQuaternion(
        Quaternion(0.6378589272499085_r, -4.2587172677244209e-10_r, -3.5271871534625629e-10_r, -0.770153284072876_r));

    rs = r * s;
    t = Vector3(0.0_r, 0.21801769733428956_r, 3.725289854372704e-9_r);

    lt = skeleton->GetLocalTransform("torso_joint_3").ToMatrix();
    EXPECT_TRUE(lt.ToMatrix3x3().IsEqualApprox(rs));
    EXPECT_TRUE(Vector3(lt.col3().x, lt.col3().y, lt.col3().z).IsEqualApprox(t));

    // arm_joint_R_1
    s = Matrix3x3(Vector3(1.0_r, 0.0_r, 0.0_r), Vector3(0.0_r, 0.9999999403953552_r, 0.0_r),
                  Vector3(0.0_r, 0.0_r, 0.9999999403953552_r));

    r = Matrix3x3::FromQuaternion(
        Quaternion(0.27643078565597536_r, 0.05186379700899124_r, -0.665187418460846_r, -0.6916804909706116_r));

    rs = r * s;
    t = Vector3(-0.08800055086612702_r, -0.0001992879988392815_r, -0.0009773969650268557_r);

    lt = skeleton->GetLocalTransform("arm_joint_R_1").ToMatrix();
    EXPECT_TRUE(lt.ToMatrix3x3().IsEqualApprox(rs));
    EXPECT_TRUE(Vector3(lt.col3().x, lt.col3().y, lt.col3().z).IsEqualApprox(t));

    // arm_joint_R_2
    s = Matrix3x3(Vector3(0.9999999403953552_r, 0.0_r, 0.0_r), Vector3(0.0_r, 0.9999995827674866_r, 0.0_r),
                  Vector3(0.0_r, 0.0_r, 0.9999998211860656_r));

    r = Matrix3x3::FromQuaternion(
        Quaternion(-0.2280062586069107_r, 0.9096477627754213_r, -0.1480233669281006_r, -0.3140748739242554_r));

    rs = r * s;
    t = Vector3(-7.450579708745408e-9_r, 0.24452559649944304_r, -5.96045985901128e-8_r);

    lt = skeleton->GetLocalTransform("arm_joint_R_2").ToMatrix();
    EXPECT_TRUE(lt.ToMatrix3x3().IsEqualApprox(rs));
    EXPECT_TRUE(Vector3(lt.col3().x, lt.col3().y, lt.col3().z).IsEqualApprox(t));

    // arm_joint_R_3
    s = Matrix3x3(Vector3(1.0000001192092896_r, 0.0_r, 0.0_r), Vector3(0.0_r, 1.0_r, 0.0_r),
                  Vector3(0.0_r, 0.0_r, 0.9999999403953552_r));

    r = Matrix3x3::FromQuaternion(
        Quaternion(-0.07854889333248139_r, 0.14253509044647218_r, -0.014102266170084477_r, -0.9865672588348388_r));

    rs = r * s;
    t = Vector3(-5.96045985901128e-8_r, 0.1855168044567108_r, 0.0_r);

    lt = skeleton->GetLocalTransform("arm_joint_R_3").ToMatrix();
    EXPECT_TRUE(lt.ToMatrix3x3().IsEqualApprox(rs));
    EXPECT_TRUE(Vector3(lt.col3().x, lt.col3().y, lt.col3().z).IsEqualApprox(t));

    // arm_joint_L_1
    s = Matrix3x3(Vector3(1.0000001192092896_r, 0.0_r, 0.0_r), Vector3(0.0_r, 1.0000004768371585_r, 0.0_r),
                  Vector3(0.0_r, 0.0_r, 1.0_r));

    r = Matrix3x3::FromQuaternion(
        Quaternion(0.6789423823356628_r, 0.6879449486732483_r, -0.24067795276641849_r, -0.08856399357318878_r));

    rs = r * s;
    t = Vector3(0.08800055086612702_r, -0.0001992879988392815_r, -0.0009773969650268557_r);

    lt = skeleton->GetLocalTransform("arm_joint_L_1").ToMatrix();
    EXPECT_TRUE(lt.ToMatrix3x3().IsEqualApprox(rs));
    EXPECT_TRUE(Vector3(lt.col3().x, lt.col3().y, lt.col3().z).IsEqualApprox(t));

    // arm_joint_L_2
    s = Matrix3x3::IDENTITY;

    r = Matrix3x3::FromQuaternion(
        Quaternion(0.0024000618141144516_r, -0.13981154561042789_r, -0.2718312442302704_r, -0.9521317481994628_r));

    rs = r * s;
    t = Vector3(1.8626500342122655e-9_r, 0.24452590942382813_r, -5.96045985901128e-8_r);

    lt = skeleton->GetLocalTransform("arm_joint_L_2").ToMatrix();
    EXPECT_TRUE(lt.ToMatrix3x3().IsEqualApprox(rs));
    EXPECT_TRUE(Vector3(lt.col3().x, lt.col3().y, lt.col3().z).IsEqualApprox(t));

    //  arm_joint_L_3
    s = Matrix3x3(Vector3(1.0_r, 0.0_r, 0.0_r), Vector3(0.0_r, 1.0000001192092896_r, 0.0_r),
                  Vector3(0.0_r, 0.0_r, 1.0000001192092896_r));

    r = Matrix3x3::FromQuaternion(
        Quaternion(-0.05729063600301743_r, -0.02822729200124741_r, -0.0555599257349968_r, -0.996410608291626_r));

    rs = r * s;
    t = Vector3(0.0_r, 0.1855167001485825_r, 0.0_r);

    lt = skeleton->GetLocalTransform("arm_joint_L_3").ToMatrix();
    EXPECT_TRUE(lt.ToMatrix3x3().IsEqualApprox(rs));
    EXPECT_TRUE(Vector3(lt.col3().x, lt.col3().y, lt.col3().z).IsEqualApprox(t));

    // neck_joint_1
    s = Matrix3x3::IDENTITY;

    r = Matrix3x3::FromQuaternion(
        Quaternion(-0.635452151298523_r, -1.0367853739773274e-15_r, -8.512669473202695e-16_r, -0.7721402645111084_r));

    rs = r * s;
    t = Vector3(0.0_r, 7.450579886381094e-8_r, 0.05255972966551781_r);

    lt = skeleton->GetLocalTransform("neck_joint_1").ToMatrix();
    EXPECT_TRUE(lt.ToMatrix3x3().IsEqualApprox(rs));
    EXPECT_TRUE(Vector3(lt.col3().x, lt.col3().y, lt.col3().z).IsEqualApprox(t));

    // neck_joint_2
    s = Matrix3x3(Vector3(1.0_r, 0.0_r, 0.0_r), Vector3(0.0_r, 1.0000001192092896_r, 0.0_r),
                  Vector3(0.0_r, 0.0_r, 1.0_r));

    r = Matrix3x3::FromQuaternion(
        Quaternion(4.2157907720330458e-10_r, 0.9999844431877136_r, -0.005583992227911949_r, -7.549667913053783e-8_r));

    rs = r * s;
    t = Vector3(0.0_r, 0.06650590896606446_r, 0.0_r);

    lt = skeleton->GetLocalTransform("neck_joint_2").ToMatrix();
    EXPECT_TRUE(lt.ToMatrix3x3().IsEqualApprox(rs));
    EXPECT_TRUE(Vector3(lt.col3().x, lt.col3().y, lt.col3().z).IsEqualApprox(t));

    // Check Animation
    EXPECT_EQ(model.animations.size(), 1);
    EXPECT_LE(animations[0]->GetSkeletalTrackCount(), skeleton->GetBoneCount());

    auto TestSkeletalTrack = [&](const char* bone_name) {
        EXPECT_TRUE(animations[0]->HasSkeletalTrack(skeleton->GetBoneIndex(bone_name)));
        const Animation::SkeletalTrack& torso_joint_1_track =
            animations[0]->GetSkeletalTrack(skeleton->GetBoneIndex(bone_name));
        EXPECT_EQ(torso_joint_1_track.translation_interp_mode, Animation::INTERP_MODE_LINEAR);
        EXPECT_GT(torso_joint_1_track.translation_key_sequence.size(), 0);
        EXPECT_EQ(torso_joint_1_track.rotation_interp_mode, Animation::INTERP_MODE_LINEAR);
        EXPECT_GT(torso_joint_1_track.rotation_key_sequence.size(), 0);
        EXPECT_EQ(torso_joint_1_track.scaling_interp_mode, Animation::INTERP_MODE_LINEAR);
        EXPECT_GT(torso_joint_1_track.scaling_key_sequence.size(), 0);
    };

    TestSkeletalTrack("torso_joint_1");
    TestSkeletalTrack("torso_joint_2");
    TestSkeletalTrack("torso_joint_3");
    TestSkeletalTrack("neck_joint_1");
    TestSkeletalTrack("neck_joint_2");
    TestSkeletalTrack("arm_joint_R_1");
    TestSkeletalTrack("arm_joint_R_2");
    TestSkeletalTrack("arm_joint_R_3");
    TestSkeletalTrack("arm_joint_L_1");
    TestSkeletalTrack("arm_joint_L_2");
    TestSkeletalTrack("arm_joint_L_3");
    TestSkeletalTrack("leg_joint_R_1");
    TestSkeletalTrack("leg_joint_R_2");
    TestSkeletalTrack("leg_joint_R_3");
    TestSkeletalTrack("leg_joint_R_5");
    TestSkeletalTrack("leg_joint_L_1");
    TestSkeletalTrack("leg_joint_L_2");
    TestSkeletalTrack("leg_joint_L_3");
    TestSkeletalTrack("leg_joint_L_5");
}

TEST_F(ResourceLoaderTest, LoadModelglTFAnimatedMorphCube) {
    Path path(std::string("test_assets/test_gltf/AnimatedMorphCube/glTF/AnimatedMorphCube.gltf"));
    ResourceLoader::Model model = ResourceLoader::LoadModel(path, resource_manager);
    const Mesh* mesh = resource_manager.GetMesh(model.mesh);
    const Skeleton* skeleton = resource_manager.GetSkeleton(model.skeleton);
    std::vector<const Animation*> animations;
    for (ResourceID id : model.animations) {
        animations.push_back(resource_manager.GetAnimation(id));
    }

    // Check Morph Target
    const Mesh::SubMesh& sub_mesh = mesh->GetSubMesh("Cube");
    EXPECT_EQ(sub_mesh.GetMorphTargetCount(), 2);
    for (const Mesh::MorphTarget& mt : sub_mesh.morph_targets) {
        EXPECT_LE(0, sub_mesh.positions.size());
        EXPECT_EQ(sub_mesh.positions.size(), mt.positions.size());
        EXPECT_EQ(sub_mesh.positions.size(), mt.normals.size());
        EXPECT_EQ(sub_mesh.positions.size(), mt.tangents.size());
    }

    // Check Animation
    EXPECT_EQ(model.animations.size(), 1);
    EXPECT_EQ(animations[0]->GetMorphTargetTrackCount(), 1);
    EXPECT_TRUE(animations[0]->HasMorphTargetTrack(0));
    const Animation::MorphTargetTrack& track =
        animations[0]->GetMorphTargetTrack(skeleton->GetBoneIndex("AnimatedMorphCube"));
    EXPECT_EQ(track.key_sequence.size(), 127);
    for (const Animation::MorphingKey& key : track.key_sequence) {
        EXPECT_EQ(key.target_indices.size(), key.weights.size());
        EXPECT_EQ(key.weights.size(), 2);
    }
}