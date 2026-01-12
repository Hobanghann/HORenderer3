#pragma once

#include "virtual_gpu.h"
#include "virtual_gpu_utils.h"

namespace ho {
    ALWAYS_INLINE float LoadAsFloat(const uint8_t* p, VGenum type, bool normalized) {
        switch (type) {
            case VG_FLOAT:
                return *reinterpret_cast<const VGfloat*>(p);

            case VG_HALF_FLOAT:
                return vg::HalfToFloat(*reinterpret_cast<const uint16_t*>(p));

            case VG_BYTE: {
                int v = *reinterpret_cast<const VGbyte*>(p);
                if (!normalized) {
                    return float(v);
                }
                return math::Clamp(float(v) / 127.f, -1.f, 1.f);
            }

            case VG_UNSIGNED_BYTE: {
                unsigned v = *reinterpret_cast<const VGubyte*>(p);
                if (!normalized) {
                    return float(v);
                }
                return math::Clamp(float(v) / 255.f, 0.f, 1.f);
            }

            case VG_SHORT: {
                int v = *reinterpret_cast<const VGshort*>(p);
                if (!normalized) {
                    return float(v);
                }
                return math::Clamp(float(v) / 32767.f, -1.f, 1.f);
            }

            case VG_UNSIGNED_SHORT: {
                unsigned v = *reinterpret_cast<const VGushort*>(p);
                if (!normalized) {
                    return float(v);
                }
                return math::Clamp(float(v) / 65535.f, 0.f, 1.f);
            }

            case VG_INT: {
                int v = *reinterpret_cast<const VGint*>(p);
                if (!normalized) {
                    return float(v);
                }
                return math::Clamp(float(double(v) / 2147483647.0), -1.f, 1.f);
            }

            case VG_UNSIGNED_INT: {
                unsigned v = *reinterpret_cast<const VGuint*>(p);
                if (!normalized) {
                    return float(v);
                }
                return math::Clamp(float(double(v) / 4294967295.0), 0.f, 1.f);
            }

            default:
                assert(false);
                return 0.f;
        }
    }

    ALWAYS_INLINE int64_t LoadAsInt(const uint8_t* p, VGenum type) {
        switch (type) {
            case VG_BYTE:
                return *reinterpret_cast<const VGbyte*>(p);

            case VG_UNSIGNED_BYTE:
                return *reinterpret_cast<const VGubyte*>(p);

            case VG_SHORT:
                return *reinterpret_cast<const VGshort*>(p);

            case VG_UNSIGNED_SHORT:
                return *reinterpret_cast<const VGushort*>(p);

            case VG_INT:
                return *reinterpret_cast<const VGint*>(p);

            case VG_UNSIGNED_INT:
                return *reinterpret_cast<const VGuint*>(p);

            default:
                assert(false);
                return 0;
        }
    }

    // T can be  float, Vector2, Vector3, Vector4, uint8_t, int8_t, uint16_t,
    // int16_t,uint32_t,int32_t
    template <typename T>
    ALWAYS_INLINE T FetchAttribute(VGuint location, size_t index = 0) {
        static uint8_t default_zero[16] = {0};

        VirtualGPU& vg = VirtualGPU::GetInstance();

        const uint8_t* src = nullptr;
        VGenum type;
        int type_size = 0;
        int comp_count = 0;
        bool normalized = false;
        bool is_pure_integer = false;

        auto ait = vg.bound_vertex_array_->index_to_attrib.find(location);
        if (ait != vg.bound_vertex_array_->index_to_attrib.end() && ait->second.enabled) {
            VirtualGPU::Attribute& attr = ait->second;
            type = attr.type;
            type_size = vg::GetTypeSize(attr.type);
            comp_count = attr.size;
            normalized = attr.normalized;
            is_pure_integer = attr.is_pure_integer;
            int stride = attr.stride == 0 ? attr.size * type_size : attr.stride;
            src = attr.buffer->memory->data() + attr.offset + (stride * index);
        } else {
            auto cit = vg.constant_attributes_.find(location);
            if (cit != vg.constant_attributes_.end()) {
                VirtualGPU::ConstantAttribute& cattr = cit->second;
                src = reinterpret_cast<const uint8_t*>(cattr.f);
                type = cattr.type;
                type_size = vg::GetTypeSize(cattr.type);
                comp_count = 4;
                normalized = cattr.normalized;
                is_pure_integer = cattr.is_pure_integer;
            } else {
                src = default_zero;
                type = VG_FLOAT;
                type_size = sizeof(float);
                normalized = false;
                is_pure_integer = false;
                comp_count = 4;
            }
        }

        if constexpr (std::is_same_v<T, float>) {
            float comps[4] = {0.f, 0.f, 0.f, 1.f};
            for (int i = 0; i < comp_count; i++) {
                comps[i] = LoadAsFloat(src + i * type_size, type, normalized);
            }
            return comps[0];
        } else if constexpr (std::is_same_v<T, Vector2>) {
            float comps[4] = {0.f, 0.f, 0.f, 1.f};
            for (int i = 0; i < comp_count; i++) {
                comps[i] = LoadAsFloat(src + i * type_size, type, normalized);
            }
            return Vector2(comps[0], comps[1]);
        } else if constexpr (std::is_same_v<T, Vector3>) {
            float comps[4] = {0.f, 0.f, 0.f, 1.f};
            for (int i = 0; i < comp_count; i++) {
                comps[i] = LoadAsFloat(src + i * type_size, type, normalized);
            }
            return Vector3(comps[0], comps[1], comps[2]);
        } else if constexpr (std::is_same_v<T, Vector4>) {
            float comps[4] = {0.f, 0.f, 0.f, 1.f};
            for (int i = 0; i < comp_count; i++) {
                comps[i] = LoadAsFloat(src + i * type_size, type, normalized);
            }
            return Vector4(comps[0], comps[1], comps[2], comps[3]);
        } else if constexpr (std::is_same_v<T, uint8_t>) {
            assert(is_pure_integer);
            return (uint8_t)(LoadAsInt(src, type));
        } else if constexpr (std::is_same_v<T, int8_t>) {
            assert(is_pure_integer);
            return (int8_t)(LoadAsInt(src, type));
        } else if constexpr (std::is_same_v<T, uint16_t>) {
            assert(is_pure_integer);
            return (uint16_t)(LoadAsInt(src, type));
        } else if constexpr (std::is_same_v<T, int16_t>) {
            assert(is_pure_integer);
            return (int16_t)(LoadAsInt(src, type));
        } else if constexpr (std::is_same_v<T, uint32_t>) {
            assert(is_pure_integer);
            return (uint32_t)(LoadAsInt(src, type));
        } else if constexpr (std::is_same_v<T, int32_t>) {
            assert(is_pure_integer);
            return (int32_t)(LoadAsInt(src, type));
        } else {
            static_assert(sizeof(T) == 0, "unsupported vertex attrib type");
        }
    }

    // T can be  float, Vector2, Vector3, Vector4, Matrix2x2, Matrix3x3, Matrix4x4,
    // uint32_t, int32_t
    template <typename T>
    ALWAYS_INLINE T FetchUniform(uint32_t name_hash, size_t index = 0) {
        VirtualGPU& vg = VirtualGPU::GetInstance();
        assert(vg.using_program_);
        auto loc_it = vg.using_program_->uniform_name_hash_to_location.find(name_hash);
        assert(loc_it != vg.using_program_->uniform_name_hash_to_location.end());
        VirtualGPU::Uniform& u = vg.using_program_->uniforms[loc_it->second];
        assert((VGsizei)index < u.count);
        const uint8_t* src = u.data.data() + index * (u.size * vg::GetTypeSize(u.type));
        T dst;
        std::memcpy(&dst, src, sizeof(T));
        return dst;
    }

    // T can be  float, Vector2, Vector3, Vector4, Matrix2x2, Matrix3x3, Matrix4x4,
    // uint32_t, int32_t
    template <typename T>
    ALWAYS_INLINE T FetchUniformBlock(VGuint binding, int offset) {
        VirtualGPU& vg = VirtualGPU::GetInstance();
        auto it = vg.uniform_buffer_bindings_.find(binding);
        assert(it != vg.uniform_buffer_bindings_.end());
        VirtualGPU::BufferBinding& b = it->second;
        const uint8_t* src = b.buffer->memory->data() + b.offset + offset;
        T dst;
        std::memcpy(&dst, src, sizeof(T));
        return dst;
    }

    ALWAYS_INLINE VGfloat ApplyWrap(VGenum wrap_mode, VGfloat coord) {
        real t = 0.0_r;
        real frac = 0.0_r;
        switch (wrap_mode) {
            case VG_REPEAT:
                return coord - math::Floor(coord);
            case VG_MIRRORED_REPEAT:
                t = math::Floor(coord);
                frac = coord - t;
                return (static_cast<int>(t) & 1) ? (1.0_r - frac) : frac;
            case VG_CLAMP_TO_EDGE:
                return math::Clamp(coord, 0.0f, nextafter(1.0f, 0.0f));
            case VG_CLAMP_TO_BORDER:
                return -1.f;
            default:
                return 0.f;
        }
    }

    template <typename T>
    ALWAYS_INLINE T ApplyFilter(VGint filter, const VirtualGPU::TextureObject& tex, VGint level, VGfloat u) {
        const auto& lvl = tex.mipmap[level];
        const uint8_t* base = lvl.memory->data();
        const int pixel_size = vg::GetPixelSize(tex.internal_format, tex.component_type);

        const float x = u * static_cast<float>(lvl.width - 1);

        auto GetTexel = [&](int index) -> T {
            const uint8_t* pixel_ptr = base + index * pixel_size;

            Color128 temp_color;
            vg::DecodeColor(&temp_color, pixel_ptr, tex.internal_format, tex.component_type);

            if constexpr (std::is_same_v<T, float>) {
                return temp_color.r;
            } else if constexpr (std::is_same_v<T, Color128>) {
                return temp_color;
            } else {
                static_assert(sizeof(T) == 0, "unsupported sampling type");
            }
        };

        switch (filter) {
            case VG_NEAREST: {
                int px = static_cast<int>(math::Round(x));
                return GetTexel(px);
            }

            case VG_LINEAR: {
                int x0 = static_cast<int>(math::Floor(x));
                int x1 = x0 + 1;
                float t = x - x0;

                T c0 = GetTexel(x0);
                T c1 = GetTexel(x1);

                return c0 * (1.f - t) + c1 * t;
            }

            default:
                return T();
        }
    }

    template <typename T>
    ALWAYS_INLINE T ApplyFilter(VGint filter, const VirtualGPU::TextureObject& tex, VGint level, VGfloat u, VGfloat v) {
        const auto& lvl = tex.mipmap[level];
        const uint8_t* base = lvl.memory->data();
        const int pixel_size = vg::GetPixelSize(tex.internal_format, tex.component_type);

        const float x = u * static_cast<float>(lvl.width - 1);
        const float y = v * static_cast<float>(lvl.height - 1);

        auto GetTexel = [&](int x_idx, int y_idx) -> T {
            const uint8_t* pixel_ptr = base + (y_idx * lvl.width + x_idx) * pixel_size;

            Color128 temp_color;
            vg::DecodeColor(&temp_color, pixel_ptr, tex.internal_format, tex.component_type);

            if constexpr (std::is_same_v<T, float>) {
                return temp_color.r;
            } else if constexpr (std::is_same_v<T, Color128>) {
                return temp_color;
            } else {
                static_assert(sizeof(T) == 0, "unsupported sampling type");
            }
        };

        switch (filter) {
            case VG_NEAREST: {
                int px = static_cast<int>(math::Round(x));
                int py = static_cast<int>(math::Round(y));
                return GetTexel(px, py);
            }

            case VG_LINEAR: {
                int x0 = static_cast<int>(math::Floor(x));
                int y0 = static_cast<int>(math::Floor(y));

                int x1 = x0 + 1;
                int y1 = y0 + 1;

                float tx = x - x0;
                float ty = y - y0;

                T c00 = GetTexel(x0, y0);
                T c10 = GetTexel(x1, y0);
                T c01 = GetTexel(x0, y1);
                T c11 = GetTexel(x1, y1);

                T cx0 = c00 * (1.f - tx) + c10 * tx;
                T cx1 = c01 * (1.f - tx) + c11 * tx;

                return cx0 * (1.f - ty) + cx1 * ty;
            }

            default:
                return T();
        }
    }

    template <typename T>
    ALWAYS_INLINE T ApplyFilter(VGint filter, const VirtualGPU::TextureObject& tex, VGint level, VGfloat u, VGfloat v,
                                VGfloat w) {
        const auto& lvl = tex.mipmap[level];
        const uint8_t* base = lvl.memory->data();
        const int pixel_size = vg::GetPixelSize(tex.internal_format, tex.component_type);

        const float x = u * static_cast<float>(lvl.width - 1);
        const float y = v * static_cast<float>(lvl.height - 1);
        const float z = w * static_cast<float>(lvl.depth - 1);

        auto GetTexel = [&](int x_idx, int y_idx, int z_idx) -> T {
            const size_t index = ((size_t)z_idx * lvl.height + y_idx) * lvl.width + x_idx;
            const uint8_t* pixel_ptr = base + index * pixel_size;

            Color128 temp_color;
            vg::DecodeColor(&temp_color, pixel_ptr, tex.internal_format, tex.component_type);

            if constexpr (std::is_same_v<T, float>) {
                return temp_color.r;
            } else if constexpr (std::is_same_v<T, Color128>) {
                return temp_color;
            } else {
                static_assert(sizeof(T) == 0, "unsupported sampling type");
            }
        };

        switch (filter) {
            case VG_NEAREST: {
                int ix = static_cast<int>(math::Round(x));
                int iy = static_cast<int>(math::Round(y));
                int iz = static_cast<int>(math::Round(z));
                return GetTexel(ix, iy, iz);
            }

            case VG_LINEAR: {
                int x0 = static_cast<int>(math::Floor(x));
                int y0 = static_cast<int>(math::Floor(y));
                int z0 = static_cast<int>(math::Floor(z));

                int x1 = x0 + 1;
                int y1 = y0 + 1;
                int z1 = z0 + 1;

                float tx = x - x0;
                float ty = y - y0;
                float tz = z - z0;

                // Fetch 8 corners of the cube
                // Z=0 Plane
                T c000 = GetTexel(x0, y0, z0);
                T c100 = GetTexel(x1, y0, z0);
                T c010 = GetTexel(x0, y1, z0);
                T c110 = GetTexel(x1, y1, z0);

                // Z=1 Plane
                T c001 = GetTexel(x0, y0, z1);
                T c101 = GetTexel(x1, y0, z1);
                T c011 = GetTexel(x0, y1, z1);
                T c111 = GetTexel(x1, y1, z1);

                // Interpolate along X
                T c00 = c000 * (1.f - tx) + c100 * tx;
                T c10 = c010 * (1.f - tx) + c110 * tx;
                T c01 = c001 * (1.f - tx) + c101 * tx;
                T c11 = c011 * (1.f - tx) + c111 * tx;

                // Interpolate along Y
                T c0 = c00 * (1.f - ty) + c10 * ty;
                T c1 = c01 * (1.f - ty) + c11 * ty;

                return c0 * (1.f - tz) + c1 * tz;
            }

            default:
                return T();
        }
    }

    // T can be float, Color128
    template <typename T>
    ALWAYS_INLINE T Texture1D(VGuint unit_slot, VGfloat u) {
        VirtualGPU& vg = VirtualGPU::GetInstance();
        VirtualGPU::TextureUnit& unit = vg.texture_units_[unit_slot];
        VirtualGPU::TextureObject* tex = unit.bound_texture_targets[vg::GetTextureSlot(VG_TEXTURE_1D)];
        if (!tex) {
            return T();
        }
        VirtualGPU::Sampler* sam = unit.bound_sampler == nullptr ? &tex->default_sampler : unit.bound_sampler;

        // 1) wrap
        VGfloat wrap_u = ApplyWrap(sam->wrap_s, u);
        if (wrap_u < 0.f) {
            if constexpr (std::is_same_v<T, float>) {
                return T();
            } else if constexpr (std::is_same_v<T, Color128>) {
                return sam->border_color;
            } else {
                static_assert(sizeof(T) == 0, "unsupported sampling type");
            }
        }

        // 2) LOD -> no op

        // 3) base/max level clamp -> no op

        // 4) choose mip level -> level 0 only

        // 5) min/mag -> always mag filter
        T filtered = ApplyFilter<T>(sam->mag_filter, *tex, 0, wrap_u);

        // 6) depth compare -> no op

        // 7) swizzle -> no op

        return filtered;
    }

    // T can be float, Color128
    template <typename T>
    ALWAYS_INLINE T Texture2D(VGuint unit_slot, const Vector2& tex_coord) {
        VirtualGPU& vg = VirtualGPU::GetInstance();
        VirtualGPU::TextureUnit& unit = vg.texture_units_[unit_slot];

        VirtualGPU::TextureObject* tex = unit.bound_texture_targets[vg::GetTextureSlot(VG_TEXTURE_2D)];
        if (!tex) {
            return T();
        }

        VirtualGPU::Sampler* sam = unit.bound_sampler ? unit.bound_sampler : &tex->default_sampler;

        // 1) wrap
        VGfloat wrap_u = ApplyWrap(sam->wrap_s, tex_coord.x);
        if (wrap_u < 0.f) {
            if constexpr (std::is_same_v<T, float>) {
                return T();
            } else if constexpr (std::is_same_v<T, Color128>) {
                return sam->border_color;
            } else {
                static_assert(sizeof(T) == 0, "unsupported sampling type");
            }
        }

        VGfloat wrap_v = ApplyWrap(sam->wrap_t, tex_coord.y);
        if (wrap_v < 0.f) {
            if constexpr (std::is_same_v<T, float>) {
                return T();
            } else if constexpr (std::is_same_v<T, Color128>) {
                return sam->border_color;
            } else {
                static_assert(sizeof(T) == 0, "unsupported sampling type");
            }
        }

        // 2) LOD -> no op

        // 3) base/max level clamp -> no op

        // 4) choose mip level -> level 0 only

        // 5) min/mag -> always mag filter
        T filtered = ApplyFilter<T>(sam->mag_filter, *tex, 0, wrap_u, wrap_v);

        // 6) depth compare -> no op

        // 7) swizzle -> no op

        return filtered;
    }

    // T can be float, Color128
    template <typename T>
    ALWAYS_INLINE T Texture3D(VGuint unit_slot, const Vector3& tex_coord) {
        VirtualGPU& vg = VirtualGPU::GetInstance();
        VirtualGPU::TextureUnit& unit = vg.texture_units_[unit_slot];

        VirtualGPU::TextureObject* tex = unit.bound_texture_targets[vg::GetTextureSlot(VG_TEXTURE_3D)];
        if (!tex) {
            return T();
        }

        VirtualGPU::Sampler* sam = unit.bound_sampler ? unit.bound_sampler : &tex->default_sampler;

        // 1) wrap
        VGfloat wrap_u = ApplyWrap(sam->wrap_s, tex_coord.x);
        if (wrap_u < 0.f) {
            if constexpr (std::is_same_v<T, float>) {
                return T();
            } else if constexpr (std::is_same_v<T, Color128>) {
                return sam->border_color;
            } else {
                static_assert(sizeof(T) == 0, "unsupported sampling type");
            }
        }

        VGfloat wrap_v = ApplyWrap(sam->wrap_t, tex_coord.y);
        if (wrap_v < 0.f) {
            if constexpr (std::is_same_v<T, float>) {
                return T();
            } else if constexpr (std::is_same_v<T, Color128>) {
                return sam->border_color;
            } else {
                static_assert(sizeof(T) == 0, "unsupported sampling type");
            }
        }

        VGfloat wrap_w = ApplyWrap(sam->wrap_r, tex_coord.z);
        if (wrap_w < 0.f) {
            if constexpr (std::is_same_v<T, float>) {
                return T();
            } else if constexpr (std::is_same_v<T, Color128>) {
                return sam->border_color;
            } else {
                static_assert(sizeof(T) == 0, "unsupported sampling type");
            }
        }

        // 2) LOD -> no op

        // 3) base/max level clamp -> no op

        // 4) choose mip level -> level 0 only

        // 5) min/mag -> always mag filter
        T filtered = ApplyFilter<T>(sam->mag_filter, *tex, 0, wrap_u, wrap_v, wrap_w);

        // 6) depth compare -> no op

        // 7) swizzle -> no op

        return filtered;
    }

    ALWAYS_INLINE float TextureSize1D(VGuint unit_slot, VGint level) {
        VirtualGPU& vg = VirtualGPU::GetInstance();
        VirtualGPU::TextureUnit& unit = vg.texture_units_[unit_slot];

        VirtualGPU::TextureObject* tex = unit.bound_texture_targets[vg::GetTextureSlot(VG_TEXTURE_1D)];
        assert(tex);
        assert(tex->mipmap_count > level);
        return (float)tex->mipmap[level].width;
    }

    ALWAYS_INLINE Vector2 TextureSize2D(VGuint unit_slot, VGint level) {
        VirtualGPU& vg = VirtualGPU::GetInstance();
        VirtualGPU::TextureUnit& unit = vg.texture_units_[unit_slot];

        VirtualGPU::TextureObject* tex = unit.bound_texture_targets[vg::GetTextureSlot(VG_TEXTURE_2D)];
        assert(tex);
        assert(tex->mipmap_count > level);
        return Vector2((float)tex->mipmap[level].width, (float)tex->mipmap[level].height);
    }

    ALWAYS_INLINE Vector3 TextureSize3D(VGuint unit_slot, VGint level) {
        VirtualGPU& vg = VirtualGPU::GetInstance();
        VirtualGPU::TextureUnit& unit = vg.texture_units_[unit_slot];

        VirtualGPU::TextureObject* tex = unit.bound_texture_targets[vg::GetTextureSlot(VG_TEXTURE_3D)];
        assert(tex);
        assert(tex->mipmap_count > level);
        return Vector3((float)tex->mipmap[level].width, (float)tex->mipmap[level].height,
                       (float)tex->mipmap[level].depth);
    }

}  // namespace ho