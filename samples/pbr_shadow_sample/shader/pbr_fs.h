#pragma once

#include "virtual_gpu/shader_api.h"

namespace ho {
    ALWAYS_INLINE float CalculateShadow(uint32_t tex_slot, Vector4 light_space_position, Vector3 normal,
                                        Vector3 light_direction) {
        Vector3 proj_coord = light_space_position.ToCartesian();
        proj_coord = proj_coord * 0.5f + Vector3(0.5f, 0.5f, 0.5f);
        Vector2 tex_coord = Vector2(proj_coord.x, proj_coord.y);
        float d = proj_coord.z;

        float bias = math::Max(0.0005f * (1.f - normal.Dot(light_direction)), 0.005f);

        float shadow = 0.f;

        Vector2 inv_txel_size = 1.f / TextureSize2D(tex_slot, 0);
        for (int x = -1; x <= 1; x++) {
            for (int y = -1; y <= 1; y++) {
                Vector2 near_coord = Vector2(tex_coord.x + x * inv_txel_size.x, tex_coord.y + y * inv_txel_size.y);
                float pcf_depth = Texture2D<Color128>(tex_slot, near_coord).r;
                shadow += d > pcf_depth + bias ? 1.f : 0.f;
            }
        }
        shadow /= 9.f;

        shadow = d > 1.f ? 0.f : shadow;
        shadow = (proj_coord.x > 1.f || proj_coord.x < 0.f || proj_coord.y > 1.f || proj_coord.y < 0.f) ? 0.f : shadow;
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

    ALWAYS_INLINE Vector3 FresnelSchlick(float cos_theta, Vector3 F0) {
        return F0 + (Vector3(1.f, 1.f, 1.f) - F0) * math::Pow(1.f - cos_theta, 5.f);
    }

    ALWAYS_INLINE void PBR_FS(const VirtualGPU::Fragment& in, VirtualGPU::FSOutputs& out) {
        Vector3 v_world_pos = in.In<Vector3>("world_pos"_vg);
        Vector3 v_tangent = in.In<Vector3>("tangent"_vg).Normalized();
        float v_handedness = in.InFlat<float>("handedness"_vg);
        Vector3 v_normal = in.In<Vector3>("normal"_vg).Normalized();
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
        normal = (tbn * normal).Normalized();

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

        Vector3 u_light_positions[1];
        u_light_positions[0] = FetchUniform<Vector3>("u_light_positions"_vg, 0);

        Vector3 u_light_colors[1];
        u_light_colors[0] = FetchUniform<Vector3>("u_light_colors"_vg, 0);

        Vector3 u_eye_position = FetchUniform<Vector3>("u_eye_position"_vg);

        Vector3 N = normal.Normalized();
        Vector3 V = (u_eye_position - v_world_pos).Normalized();

        Vector3 F0 = Vector3(0.04f, 0.04f, 0.04f);
        F0 = F0 * (1.f - metallic_roughness.b) + Vector3(albedo.r, albedo.g, albedo.b) * metallic_roughness.b;

        // reflectance equation
        Vector3 Lo = Vector3(0.f, 0.f, 0.f);

        for (int i = 0; i < 1; i++) {
            // calculate per-light radiance
            Vector3 L = (u_light_positions[i] - v_world_pos).Normalized();
            Vector3 H = (V + L).Normalized();
            float distance = (u_light_positions[i] - v_world_pos).Magnitude();
            float attenuation = 1.f / (distance * distance);
            Vector3 radiance = u_light_colors[i] * attenuation;

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

        float sky_weight = math::Max(N.Dot(up) * 0.5f + 0.5f, 0.0f);

        Vector3 ground_color = Vector3(0.2f, 0.12f, 0.08f);
        Vector3 sky_color = Vector3(0.2f, 0.4f, 0.8f);

        Vector3 hemi_ambient = ground_color * (1.f - sky_weight) + sky_color * sky_weight;

        Vector3 ambient = hemi_ambient * Vector3(albedo.r, albedo.g, albedo.b) * ao.r;

        Vector3 color = ambient + Lo + Vector3(emission.r, emission.g, emission.b);

        Vector3 R = (N * 2.0f * math::Max(N.Dot(V), 0.0f) - V).Normalized();
        float reflect_weight = math::Max(R.Dot(up) * 0.5f + 0.5f, 0.0f);
        Vector3 fake_reflection = (ground_color * (1.f - reflect_weight) + sky_color * reflect_weight) * 2.0f;

        color += fake_reflection * F0 * (1.0f - metallic_roughness.g);

        color = color / (color + Vector3(1.f, 1.f, 1.f));

        Color128 final_color;
        final_color.r = color.x;
        final_color.g = color.y;
        final_color.b = color.z;
        final_color.a = 1.f;

        out.Out(0, final_color.LinearTosRGB());
    }

}  // namespace ho