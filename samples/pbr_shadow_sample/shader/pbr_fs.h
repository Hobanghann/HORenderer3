#pragma once

#include "virtual_gpu/shader_api.h"

namespace ho {
    ALWAYS_INLINE float CalculateShadow(uint32_t tex_slot, Vector4 lightSpacePosition, Vector3 normal,
                                        Vector3 lightDirection) {
        Vector3 projCoords = lightSpacePosition.ToCartesian();
        projCoords = projCoords * 0.5f + Vector3(0.5f, 0.5f, 0.5f);
        Vector2 texCoord = Vector2(projCoords.x, projCoords.y);
        float d = projCoords.z;

        float bias = math::Max(0.0005f * (1.f - normal.Dot(lightDirection)), 0.005f);

        float shadow = 0.f;

        Vector2 invTexelSize = 1.f / TextureSize2D(tex_slot, 0);
        for (int x = -1; x <= 1; x++) {
            for (int y = -1; y <= 1; y++) {
                Vector2 nearCoord = Vector2(texCoord.x + x * invTexelSize.x, texCoord.y + y * invTexelSize.y);
                float pcfDepth = Texture2D<Color128>(tex_slot, nearCoord).r;
                shadow += d > pcfDepth + bias ? 1.f : 0.f;
            }
        }
        shadow /= 9.f;

        shadow = d > 1.f ? 0.f : shadow;
        shadow = (projCoords.x > 1.f || projCoords.x < 0.f || projCoords.y > 1.f || projCoords.y < 0.f) ? 0.f : shadow;
        return shadow;
    }

    ALWAYS_INLINE float DistributionGGX(Vector3 N, Vector3 H, float roughness) {
        float a2 = roughness * roughness * roughness * roughness;
        float NdotH = math::Max(N.Dot(H), 0.f);
        float denom = (NdotH * NdotH * (a2 - 1.f) + 1.f);
        return a2 / (math::PI * denom * denom);
    }

    ALWAYS_INLINE float GeometrySchlickGGX(float NdotV, float roughness) {
        float r = (roughness + 1.f);
        float k = (r * r) / 8.f;
        return NdotV / (NdotV * (1.f - k) + k);
    }

    ALWAYS_INLINE float GeometrySmith(Vector3 N, Vector3 V, Vector3 L, float roughness) {
        return GeometrySchlickGGX(math::Max(N.Dot(L), 0.f), roughness) *
               GeometrySchlickGGX(math::Max(N.Dot(V), 0.f), roughness);
    }

    ALWAYS_INLINE Vector3 FresnelSchlick(float cosTheta, Vector3 F0) {
        return F0 + (Vector3(1.f, 1.f, 1.f) - F0) * math::Pow(1.f - cosTheta, 5.f);
    }

    ALWAYS_INLINE void PBR_FS(const VirtualGPU::Fragment& in, VirtualGPU::FSOutputs& out) {
        Vector3 v_world_pos = in.In<Vector3>("world_pos"_vg);
        Vector3 v_tangent = in.In<Vector3>("tangent"_vg);
        float v_handedness = in.InFlat<float>("handedness"_vg);
        Vector3 v_normal = in.In<Vector3>("normal"_vg);
        Vector2 v_uv = in.In<Vector2>("uv"_vg);
        Vector4 v_light_space_pos = in.In<Vector4>("light_space_pos"_vg);

        int u_normal_sampler = FetchUniform<int>("u_normal_sampler"_vg);
        int u_albedo_sampler = FetchUniform<int>("u_albedo_sampler"_vg);
        int u_metallic_roughness_sampler = FetchUniform<int>("u_metallic_roughness_sampler"_vg);
        int u_emission_sampler = FetchUniform<int>("u_emission_sampler"_vg);
        int u_ao_sampler = FetchUniform<int>("u_ao_sampler"_vg);
        int u_depthmap_sampler = FetchUniform<int>("u_depthmap_sampler"_vg);
        // sample normal
        Vector3 bitangent = v_handedness * v_normal.Cross(Vector3(v_tangent));
        Matrix3x3 tbn = Matrix3x3(Vector3(v_tangent), bitangent, v_normal).Transpose();
        Vector3 normal = Texture2D<Color128>(u_normal_sampler, v_uv).ToVector3();
        normal = tbn * normal;

        // sample albedo
        Color128 albedo = Texture2D<Color128>(u_albedo_sampler, v_uv);
        albedo = albedo.sRGBToLinear();

        // sample metallic/roughness
        Color128 metallic_roughness = Texture2D<Color128>(u_metallic_roughness_sampler, v_uv);

        // sample emission
        Color128 emission = Texture2D<Color128>(u_emission_sampler, v_uv);
        emission = emission.sRGBToLinear();

        // sample AO
        Color128 ao = Texture2D<Color128>(u_ao_sampler, v_uv);

        Vector3 u_lightPositions[1];
        u_lightPositions[0] = FetchUniform<Vector3>("u_lightPositions"_vg, 0);

        Vector3 u_lightColors[1];
        u_lightColors[0] = FetchUniform<Vector3>("u_lightColors"_vg, 0);

        Vector3 u_eyePosition = FetchUniform<Vector3>("u_eyePosition"_vg);

        Vector3 N = normal.Normalized();
        Vector3 V = (u_eyePosition - v_world_pos).Normalized();

        Vector3 F0 = Vector3(0.04f, 0.04f, 0.04f);
        F0 = F0 * (1.f - metallic_roughness.b) + Vector3(albedo.r, albedo.g, albedo.b) * metallic_roughness.b;

        // reflectance equation
        Vector3 Lo = Vector3(0.f, 0.f, 0.f);

        for (int i = 0; i < 1; i++) {
            // calculate per-light radiance
            Vector3 L = (u_lightPositions[i] - v_world_pos).Normalized();
            Vector3 H = (V + L).Normalized();
            float distance = (u_lightPositions[i] - v_world_pos).Magnitude();
            float attenuation = 1.f / (distance * distance);
            Vector3 radiance = u_lightColors[i] * attenuation;

            // cook-torrance brdf
            float NDF = DistributionGGX(N, H, metallic_roughness.g);
            float G = GeometrySmith(N, V, L, metallic_roughness.g);
            Vector3 F = FresnelSchlick(math::Max(H.Dot(V), 0.f), F0);

            Vector3 kS = F;
            Vector3 kD = Vector3(1.f, 1.f, 1.f) - kS;
            kD *= 1.f - metallic_roughness.b;

            Vector3 numerator = NDF * G * F;
            float denominator = 4.f * math::Max(N.Dot(V), 0.f) * math::Max(N.Dot(L), 0.f) + 0.0001f;
            Vector3 specular = numerator / denominator;

            // add to outgoing radiance Lo
            float NdotL = math::Max(N.Dot(L), 0.f);

            // calculate shadow
            float shadow = CalculateShadow(u_depthmap_sampler, v_light_space_pos, N, L);

            Lo +=
                (kD * Vector3(albedo.r, albedo.g, albedo.b) / math::PI + specular) * radiance * NdotL * (1.f - shadow);
        }

        Vector3 up = Vector3(0.0f, 1.0f, 0.0f);

        float skyWeight = math::Max(N.Dot(up) * 0.5f + 0.5f, 0.0f);

        Vector3 groundColor = Vector3(0.1f, 0.05f, 0.03f); 
        Vector3 skyColor = Vector3(0.1f, 0.2f, 0.4f);    

        Vector3 hemiAmbient = groundColor * (1.f - skyWeight) + skyColor * skyWeight;

        Vector3 ambient = hemiAmbient * Vector3(albedo.r, albedo.g, albedo.b) * ao.r;

        Vector3 color = ambient + Lo + Vector3(emission.r, emission.g, emission.b);

        Vector3 R = (N * 2.0f * math::Max(N.Dot(V), 0.0f) - V).Normalized();  
        float reflectWeight = math::Max(R.Dot(up) * 0.5f + 0.5f, 0.0f);
        Vector3 fakeReflection = (groundColor * (1.f - reflectWeight) + skyColor * reflectWeight) * 2.0f;

        color += fakeReflection * F0 * (1.0f - metallic_roughness.g);

        color = color / (color + Vector3(1.f, 1.f, 1.f));

        Color128 final_color;
        final_color.r = color.x;
        final_color.g = color.y;
        final_color.b = color.z;
        final_color.a = 1.f;

        out.Out(0, final_color.LinearTosRGB());
    }

}  // namespace ho