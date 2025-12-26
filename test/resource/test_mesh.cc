#include <gtest/gtest.h>

#include "resource/mesh.h"
using namespace ho;
class MeshTest : public ::testing::Test {
   protected:
    ho::Mesh MakeTestMesh() {
        // Morph target
        std::vector<Vector3> mt_pos = {{0.0_r, 0.0_r, 0.0_r}, {1.0_r, 0.0_r, 0.0_r}};
        std::vector<Vector3> mt_nrm = {{0.0_r, 1.0_r, 0.0_r}, {0.0_r, 1.0_r, 0.0_r}};
        std::vector<Vector4> mt_tan = {{1.0_r, 0.0_r, 0.0_r, 1.0_r}, {1.0_r, 0.0_r, 0.0_r, 1.0_r}};

        Mesh::MorphTarget morph("morph_0", std::move(mt_pos), std::move(mt_nrm), std::move(mt_tan), 1.0_r);

        // SubMesh
        std::vector<Vector3> positions = {{0.0_r, 0.0_r, 0.0_r}, {1.0_r, 0.0_r, 0.0_r}, {0.0_r, 1.0_r, 0.0_r}};
        std::vector<Vector3> normals(3);
        std::vector<Vector4> tangents(3);

        std::vector<std::array<Vector2, Mesh::MAX_UV_CHANNEL>> uvs(3);
        std::vector<std::array<Color32, Mesh::MAX_COLOR_CHANNEL>> colors(3);
        std::vector<std::array<Mesh::BoneWeight, Mesh::MAX_BONE_CHANNEL>> bone_weights(3);

        std::vector<uint32_t> indices = {0, 1, 2};

        std::vector<Mesh::MorphTarget> morphs;
        morphs.push_back(std::move(morph));

        Mesh::SubMesh sm("submesh_0", Mesh::PRIMITIVE_TYPE_TRIANGLE, std::move(positions), std::move(normals),
                         std::move(tangents), std::move(uvs), std::move(colors), std::move(bone_weights),
                         std::move(indices), std::move(morphs), ResourceID());

        std::vector<Mesh::SubMesh> subs;
        subs.push_back(std::move(sm));

        return Mesh("test_mesh", std::move(subs));
    }
};

TEST_F(MeshTest, GetMorphTargetTest) {
    auto mesh = MakeTestMesh();
    const auto& sm = mesh.sub_meshes[0];

    const auto& mt = sm.GetMorphTarget("morph_0");
    EXPECT_EQ(mt.name, "morph_0");
    EXPECT_FLOAT_EQ(mt.weight, 1.0_r);
}

TEST_F(MeshTest, GetMorphTargetIndexTest) {
    auto mesh = MakeTestMesh();
    const auto& sm = mesh.sub_meshes[0];

    EXPECT_EQ(sm.GetMorphTargetIndex("morph_0"), 0u);
}

#ifndef NDEBUG
TEST_F(MeshTest, MorphTargetInvalidNameAsserts) {
    auto mesh = MakeTestMesh();
    const auto& sm = mesh.sub_meshes[0];

    EXPECT_DEATH(sm.GetMorphTarget("no_such_morph"), "");
}
#endif

TEST_F(MeshTest, SubMeshBoundingVolumeExistanceCheck) {
    auto mesh = MakeTestMesh();
    const auto& sm = mesh.sub_meshes[0];

    EXPECT_LE(sm.aabb.min_edges.x, sm.aabb.max_edges.x);
    EXPECT_LE(sm.aabb.min_edges.y, sm.aabb.max_edges.y);
    EXPECT_LE(sm.aabb.min_edges.z, sm.aabb.max_edges.z);

    EXPECT_TRUE(sm.sphere.radius > 0.0f);
}

TEST_F(MeshTest, SubMeshAccessByName) {
    auto mesh = MakeTestMesh();

    const auto& sm = mesh.GetSubMesh("submesh_0");
    EXPECT_EQ(sm.name, "submesh_0");
}

TEST_F(MeshTest, SubMeshIndexByName) {
    auto mesh = MakeTestMesh();
    EXPECT_EQ(mesh.GetSubMeshIndex("submesh_0"), 0u);
}

#ifndef NDEBUG
TEST_F(MeshTest, SubMeshInvalidNameAsserts) {
    auto mesh = MakeTestMesh();
    EXPECT_DEATH(mesh.GetSubMesh("invalid"), "");
}
#endif

TEST_F(MeshTest, MeshBoundingVolumesExistanceCheck) {
    auto mesh = MakeTestMesh();

    EXPECT_LE(mesh.aabb.min_edges.x, mesh.aabb.max_edges.x);
    EXPECT_LE(mesh.aabb.min_edges.y, mesh.aabb.max_edges.y);
    EXPECT_LE(mesh.aabb.min_edges.z, mesh.aabb.max_edges.z);

    EXPECT_TRUE(mesh.sphere.radius > 0.0f);
}
