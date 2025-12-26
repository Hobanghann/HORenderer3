#include <gtest/gtest.h>

#include "resource/animation.h"
#include "resource/material.h"
#include "resource/mesh.h"
#include "resource/resource_manager.h"
#include "resource/skeleton.h"
#include "resource/skin.h"
#include "resource/texture.h"

using namespace ho;

class ResourceManagerTest : public ::testing::Test {
   protected:
    ResourceManager rm;

    Mesh MakeMesh(const std::string& name) {
        Mesh m;
        m.name = name;
        return m;
    }

    Skeleton MakeSkeleton(const std::string& name) {
        Skeleton s;
        s.name = name;
        return s;
    }

    Animation MakeAnimation(const std::string& name) {
        Animation a;
        a.name = name;
        a.duration = 1.0f;
        return a;
    }

    Skin MakeSkin(const std::string& name) {
        Skin s;
        s.name = name;
        return s;
    }

    Material MakeMaterial(const std::string& name) {
        Material m;
        m.name = name;
        return m;
    }

    Texture MakeTexture(const std::string& name) {
        Texture t;
        t.name = name;
        return t;
    }
};

TEST_F(ResourceManagerTest, DefaultAddTest) {
    auto mesh_id = rm.AddMesh(MakeMesh("mesh"));
    auto sk_id = rm.AddSkeleton(MakeSkeleton("skeleton"));
    auto anim_id = rm.AddAnimation(MakeAnimation("animation"));
    auto skin_id = rm.AddSkin(MakeSkin("skin"));
    auto mat_id = rm.AddMaterial(MakeMaterial("material"));
    auto tex_id = rm.AddTexture(MakeTexture("texture"));

    EXPECT_FALSE(mesh_id.IsNULL());
    EXPECT_FALSE(sk_id.IsNULL());
    EXPECT_FALSE(anim_id.IsNULL());
    EXPECT_FALSE(skin_id.IsNULL());
    EXPECT_FALSE(mat_id.IsNULL());
    EXPECT_FALSE(tex_id.IsNULL());

    EXPECT_EQ(rm.GetMeshID("mesh"), mesh_id);
    EXPECT_EQ(rm.GetSkeletonID("skeleton"), sk_id);
    EXPECT_EQ(rm.GetAnimationID("animation"), anim_id);
    EXPECT_EQ(rm.GetSkinID("skin"), skin_id);
    EXPECT_EQ(rm.GetMaterialID("material"), mat_id);
    EXPECT_EQ(rm.GetTextureID("texture"), tex_id);
}

TEST_F(ResourceManagerTest, DuplicateAddTest) {
    auto mesh_id = rm.AddMesh(MakeMesh("mesh"));
    auto sk_id = rm.AddSkeleton(MakeSkeleton("skeleton"));
    auto anim_id = rm.AddAnimation(MakeAnimation("animation"));
    auto skin_id = rm.AddSkin(MakeSkin("skin"));
    auto mat_id = rm.AddMaterial(MakeMaterial("material"));
    auto tex_id = rm.AddTexture(MakeTexture("texture"));

    EXPECT_TRUE(rm.AddMesh(MakeMesh("mesh")).IsNULL());
    EXPECT_TRUE(rm.AddSkeleton(MakeSkeleton("skeleton")).IsNULL());
    EXPECT_TRUE(rm.AddAnimation(MakeAnimation("animation")).IsNULL());
    EXPECT_TRUE(rm.AddSkin(MakeSkin("skin")).IsNULL());
    EXPECT_TRUE(rm.AddMaterial(MakeMaterial("material")).IsNULL());
    EXPECT_TRUE(rm.AddTexture(MakeTexture("texture")).IsNULL());

    EXPECT_EQ(rm.GetMeshID("mesh"), mesh_id);
    EXPECT_EQ(rm.GetSkeletonID("skeleton"), sk_id);
    EXPECT_EQ(rm.GetAnimationID("animation"), anim_id);
    EXPECT_EQ(rm.GetSkinID("skin"), skin_id);
    EXPECT_EQ(rm.GetMaterialID("material"), mat_id);
    EXPECT_EQ(rm.GetTextureID("texture"), tex_id);
}

TEST_F(ResourceManagerTest, DefaultDeleteTest) {
    auto mesh_id = rm.AddMesh(MakeMesh("mesh"));
    auto sk_id = rm.AddSkeleton(MakeSkeleton("skeleton"));
    auto anim_id = rm.AddAnimation(MakeAnimation("animation"));
    auto skin_id = rm.AddSkin(MakeSkin("skin"));
    auto mat_id = rm.AddMaterial(MakeMaterial("material"));
    auto tex_id = rm.AddTexture(MakeTexture("texture"));

    EXPECT_TRUE(rm.DeleteMesh(mesh_id));
    EXPECT_TRUE(rm.DeleteSkeleton(sk_id));
    EXPECT_TRUE(rm.DeleteAnimation(anim_id));
    EXPECT_TRUE(rm.DeleteSkin(skin_id));
    EXPECT_TRUE(rm.DeleteMaterial(mat_id));
    EXPECT_TRUE(rm.DeleteTexture(tex_id));

    EXPECT_FALSE(rm.HasMesh(mesh_id));
    EXPECT_FALSE(rm.HasSkeleton(sk_id));
    EXPECT_FALSE(rm.HasAnimation(anim_id));
    EXPECT_FALSE(rm.HasSkin(skin_id));
    EXPECT_FALSE(rm.HasMaterial(mat_id));
    EXPECT_FALSE(rm.HasTexture(tex_id));

    EXPECT_EQ(rm.GetMesh(mesh_id), nullptr);
    EXPECT_EQ(rm.GetSkeleton(sk_id), nullptr);
    EXPECT_EQ(rm.GetAnimation(anim_id), nullptr);
    EXPECT_EQ(rm.GetSkin(skin_id), nullptr);
    EXPECT_EQ(rm.GetMaterial(mat_id), nullptr);
    EXPECT_EQ(rm.GetTexture(tex_id), nullptr);

    EXPECT_TRUE(rm.GetMeshID("mesh").IsNULL());
    EXPECT_TRUE(rm.GetSkeletonID("skeleton").IsNULL());
    EXPECT_TRUE(rm.GetAnimationID("animation").IsNULL());
    EXPECT_TRUE(rm.GetSkinID("skin").IsNULL());
    EXPECT_TRUE(rm.GetMaterialID("material").IsNULL());
    EXPECT_TRUE(rm.GetTextureID("texture").IsNULL());
}

TEST_F(ResourceManagerTest, NonExistingDeleteTest) {
    ResourceID invalid;  // NULL ID

    EXPECT_FALSE(rm.DeleteMesh(invalid));
    EXPECT_FALSE(rm.DeleteSkeleton(invalid));
    EXPECT_FALSE(rm.DeleteAnimation(invalid));
    EXPECT_FALSE(rm.DeleteSkin(invalid));
    EXPECT_FALSE(rm.DeleteMaterial(invalid));
    EXPECT_FALSE(rm.DeleteTexture(invalid));
}

TEST_F(ResourceManagerTest, DefaultGetTest) {
    auto mesh_id = rm.AddMesh(MakeMesh("mesh"));
    auto sk_id = rm.AddSkeleton(MakeSkeleton("skeleton"));
    auto anim_id = rm.AddAnimation(MakeAnimation("animation"));
    auto skin_id = rm.AddSkin(MakeSkin("skin"));
    auto mat_id = rm.AddMaterial(MakeMaterial("material"));
    auto tex_id = rm.AddTexture(MakeTexture("texture"));

    EXPECT_NE(rm.GetMesh(mesh_id), nullptr);
    EXPECT_NE(rm.GetSkeleton(sk_id), nullptr);
    EXPECT_NE(rm.GetAnimation(anim_id), nullptr);
    EXPECT_NE(rm.GetSkin(skin_id), nullptr);
    EXPECT_NE(rm.GetMaterial(mat_id), nullptr);
    EXPECT_NE(rm.GetTexture(tex_id), nullptr);
}

TEST_F(ResourceManagerTest, NonExistingGetTest) {
    ResourceID invalid;  // NULL ID

    EXPECT_EQ(rm.GetMesh(invalid), nullptr);
    EXPECT_EQ(rm.GetSkeleton(invalid), nullptr);
    EXPECT_EQ(rm.GetAnimation(invalid), nullptr);
    EXPECT_EQ(rm.GetSkin(invalid), nullptr);
    EXPECT_EQ(rm.GetMaterial(invalid), nullptr);
    EXPECT_EQ(rm.GetTexture(invalid), nullptr);
}

TEST_F(ResourceManagerTest, DefaultHasTest) {
    auto mesh_id = rm.AddMesh(MakeMesh("mesh"));
    auto sk_id = rm.AddSkeleton(MakeSkeleton("skeleton"));
    auto anim_id = rm.AddAnimation(MakeAnimation("animation"));
    auto skin_id = rm.AddSkin(MakeSkin("skin"));
    auto mat_id = rm.AddMaterial(MakeMaterial("material"));
    auto tex_id = rm.AddTexture(MakeTexture("texture"));

    EXPECT_TRUE(rm.HasMesh(mesh_id));
    EXPECT_TRUE(rm.HasSkeleton(sk_id));
    EXPECT_TRUE(rm.HasAnimation(anim_id));
    EXPECT_TRUE(rm.HasSkin(skin_id));
    EXPECT_TRUE(rm.HasMaterial(mat_id));
    EXPECT_TRUE(rm.HasTexture(tex_id));
}

TEST_F(ResourceManagerTest, NonExistingHasTest) {
    ResourceID invalid;  // NULL ID

    EXPECT_FALSE(rm.HasMesh(invalid));
    EXPECT_FALSE(rm.HasSkeleton(invalid));
    EXPECT_FALSE(rm.HasAnimation(invalid));
    EXPECT_FALSE(rm.HasSkin(invalid));
    EXPECT_FALSE(rm.HasMaterial(invalid));
    EXPECT_FALSE(rm.HasTexture(invalid));
}

TEST_F(ResourceManagerTest, DefaultGetIDTest) {
    auto mesh_id = rm.AddMesh(MakeMesh("mesh"));
    auto sk_id = rm.AddSkeleton(MakeSkeleton("skeleton"));
    auto anim_id = rm.AddAnimation(MakeAnimation("animation"));
    auto skin_id = rm.AddSkin(MakeSkin("skin"));
    auto mat_id = rm.AddMaterial(MakeMaterial("material"));
    auto tex_id = rm.AddTexture(MakeTexture("texture"));

    EXPECT_EQ(rm.GetMeshID("mesh"), mesh_id);
    EXPECT_EQ(rm.GetSkeletonID("skeleton"), sk_id);
    EXPECT_EQ(rm.GetAnimationID("animation"), anim_id);
    EXPECT_EQ(rm.GetSkinID("skin"), skin_id);
    EXPECT_EQ(rm.GetMaterialID("material"), mat_id);
    EXPECT_EQ(rm.GetTextureID("texture"), tex_id);
}

TEST_F(ResourceManagerTest, NonExistingGetIDtest) {
    EXPECT_TRUE(rm.GetMeshID("mesh").IsNULL());
    EXPECT_TRUE(rm.GetSkeletonID("skeleton").IsNULL());
    EXPECT_TRUE(rm.GetAnimationID("animation").IsNULL());
    EXPECT_TRUE(rm.GetSkinID("skin").IsNULL());
    EXPECT_TRUE(rm.GetMaterialID("material").IsNULL());
    EXPECT_TRUE(rm.GetTextureID("texture").IsNULL());
}

TEST_F(ResourceManagerTest, GetCountTest) {
    EXPECT_EQ(rm.GetMeshCount(), 0);
    EXPECT_EQ(rm.GetSkeletonCount(), 0);
    EXPECT_EQ(rm.GetAnimationCount(), 0);
    EXPECT_EQ(rm.GetSkinCount(), 0);
    EXPECT_EQ(rm.GetMaterialCount(), 0);
    EXPECT_EQ(rm.GetTextureCount(), 0);

    rm.AddMesh(MakeMesh("mesh"));
    rm.AddSkeleton(MakeSkeleton("skeleton"));
    rm.AddSkeleton(MakeSkeleton("skeleton1"));
    rm.AddAnimation(MakeAnimation("animation"));
    rm.AddAnimation(MakeAnimation("animation1"));
    rm.AddAnimation(MakeAnimation("animation2"));
    rm.AddSkin(MakeSkin("skin"));
    rm.AddSkin(MakeSkin("skin1"));
    rm.AddSkin(MakeSkin("skin2"));
    rm.AddSkin(MakeSkin("skin3"));
    rm.AddMaterial(MakeMaterial("material"));
    rm.AddMaterial(MakeMaterial("material1"));
    rm.AddMaterial(MakeMaterial("material2"));
    rm.AddMaterial(MakeMaterial("material3"));
    rm.AddMaterial(MakeMaterial("material4"));
    rm.AddTexture(MakeTexture("texture"));
    rm.AddTexture(MakeTexture("texture1"));
    rm.AddTexture(MakeTexture("texture2"));
    rm.AddTexture(MakeTexture("texture3"));
    rm.AddTexture(MakeTexture("texture4"));
    rm.AddTexture(MakeTexture("texture5"));

    EXPECT_EQ(rm.GetMeshCount(), 1);
    EXPECT_EQ(rm.GetSkeletonCount(), 2);
    EXPECT_EQ(rm.GetAnimationCount(), 3);
    EXPECT_EQ(rm.GetSkinCount(), 4);
    EXPECT_EQ(rm.GetMaterialCount(), 5);
    EXPECT_EQ(rm.GetTextureCount(), 6);
}
