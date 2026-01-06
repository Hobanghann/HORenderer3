#pragma once

#include "virtual_gpu.h"
#include "virtual_gpu_utils.h"

namespace ho {
ALWAYS_INLINE float LoadAsFloat(const uint8_t* p, VGenum type,
                                bool normalized) {
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
ALWAYS_INLINE T FetchAttribute(VGuint location, VGuint index = 0) {
  static uint8_t default_zero[16] = {0};

  VirtualGPU& vg = VirtualGPU::GetInstance();

  const uint8_t* src = nullptr;
  VGenum type;
  uint32_t type_size = 0;
  uint32_t comp_count = 0;
  bool normalized = false;
  bool is_pure_integer = false;

  auto ait = vg.bound_vertex_array_->index_to_attrib.find(location);
  if (ait != vg.bound_vertex_array_->index_to_attrib.end() &&
      ait->second.enabled) {
    VirtualGPU::Attribute& attr = ait->second;
    type = attr.type;
    type_size = vg::GetTypeSize(attr.type);
    comp_count = attr.size;
    normalized = attr.normalized;
    is_pure_integer = attr.is_pure_integer;
    uint32_t stride = attr.stride == 0 ? attr.size * type_size : attr.stride;
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
    for (int i = 0; i < comp_count; ++i) {
      comps[i] = LoadAsFloat(src + i * type_size, type, normalized);
    }
    return comps[0];
  } else if constexpr (std::is_same_v<T, Vector2>) {
    float comps[4] = {0.f, 0.f, 0.f, 1.f};
    for (int i = 0; i < comp_count; ++i) {
      comps[i] = LoadAsFloat(src + i * type_size, type, normalized);
    }
    return Vector2(comps[0], comps[1]);
  } else if constexpr (std::is_same_v<T, Vector3>) {
    float comps[4] = {0.f, 0.f, 0.f, 1.f};
    for (int i = 0; i < comp_count; ++i) {
      comps[i] = LoadAsFloat(src + i * type_size, type, normalized);
    }
    return Vector3(comps[0], comps[1], comps[2]);
  } else if constexpr (std::is_same_v<T, Vector4>) {
    float comps[4] = {0.f, 0.f, 0.f, 1.f};
    for (int i = 0; i < comp_count; ++i) {
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
ALWAYS_INLINE T FetchUniform(VGint name_hash, VGuint index = 0) {
  VirtualGPU& vg = VirtualGPU::GetInstance();
  assert(vg.using_program_);
  auto loc_it = vg.using_program_->location_to_uniform_index.find(location);
  assert(loc_it != vg.using_program_->location_to_uniform_index.end());
  VirtualGPU::Uniform& u = vg.using_program_->uniforms[loc_it->second];
  assert(index < u.count);
  const uint8_t* src =
      u.data.data() + index * (u.size * vg::GetTypeSize(u.type));
  T dst;
  std::memcpy(&dst, src, sizeof(T));
  return dst;
}

// T can be  float, Vector2, Vector3, Vector4, Matrix2x2, Matrix3x3, Matrix4x4,
// uint32_t, int32_t
template <typename T>
ALWAYS_INLINE T FetchUniformBlock(VGuint binding, VGuint offset) {
  VirtualGPU& vg = VirtualGPU::GetInstance();
  auto it = vg.uniform_buffer_bindings_.find(binding);
  assert(it != vg.uniform_buffer_bindings_.end());
  VirtualGPU::BufferBinding& b = it->second;
  const uint8_t* src = b.buffer->memory->data() + b.offset + offset;
  T dst;
  std::memcpy(&dst, src, sizeof(T));
  return dst;
}

VGfloat ApplyWrap(VGint wrap_mode, VGfloat coord) {
  switch (wrap_mode) {
    case VG_REPEAT:
      return coord - math::Floor(coord);
    case VG_MIRRORED_REPEAT:
      real t = math::Floor(coord);
      real frac = coord - t;
      return (static_cast<int>(t) & 1) ? (1.0_r - frac) : frac;
    case VG_CLAMP_TO_EDGE:
      return math::Clamp(coord, 0.0f, nextafter(1.0f, 0.0f));
    case VG_CLAMP_TO_BORDER:
      return -1.f;
    default:
      return 0.f;
  }
}

Color128 ApplyFilter(VGint filter, const VirtualGPU::TextureObject& tex,
                     uint32_t level, VGfloat u) {
  const auto& lvl = tex.mipmap[level];
  const uint8_t* base = lvl.memory->data();
  const uint32_t pixel_size =
      vg::GetPixelSize(tex.internal_format, tex.component_type);

  const float x = u * (float)(lvl.width - 1);

  Color128 c0, c1;

  switch (filter) {
    case VG_NEAREST: {
      int px = (int)(math::Round(x));
      const uint8_t* pixel = base + px * pixel_size;
      vg::DecodeColor(&c0, pixel, tex.internal_format, tex.component_type);
      return c0;
    }

    case VG_LINEAR: {
      int x0 = (int)(math::Floor(x));
      int x1 = x0 + 1;
      float t = x - x0;

      vg::DecodeColor(&c0, base + x0 * pixel_size, tex.internal_format,
                      tex.component_type);
      vg::DecodeColor(&c1, base + x1 * pixel_size, tex.internal_format,
                      tex.component_type);

      return c0 * (1.f - t) + c1 * t;
    }

    default:
      return Color128(0.f, 0.f, 0.f, 1.f);
  }
}

Color128 ApplyFilter(VGint filter, const VirtualGPU::TextureObject& tex,
                     uint32_t level, VGfloat u, VGfloat v) {
  const auto& lvl = tex.mipmap[level];
  const uint8_t* base = lvl.memory->data();
  const uint32_t pixel_size =
      vg::GetPixelSize(tex.internal_format, tex.component_type);

  const float x = u * (lvl.width - 1);
  const float y = v * (lvl.height - 1);

  Color128 c00, c10, c01, c11;

  switch (filter) {
    case VG_NEAREST: {
      int px = (int)(math::Round(x));
      int py = (int)(math::Round(y));
      const uint8_t* p = base + (py * lvl.width + px) * pixel_size;
      vg::DecodeColor(&c00, p, tex.internal_format, tex.component_type);
      return c00;
    }

    case VG_LINEAR: {
      int x0 = (int)(math::Floor(x));
      int y0 = (int)(math::Floor(y));
      int x1 = x0 + 1;
      int y1 = y0 + 1;

      float tx = x - x0;
      float ty = y - y0;

      vg::DecodeColor(&c00, base + (y0 * lvl.width + x0) * pixel_size,
                      tex.internal_format, tex.component_type);
      vg::DecodeColor(&c10, base + (y0 * lvl.width + x1) * pixel_size,
                      tex.internal_format, tex.component_type);
      vg::DecodeColor(&c01, base + (y1 * lvl.width + x0) * pixel_size,
                      tex.internal_format, tex.component_type);
      vg::DecodeColor(&c11, base + (y1 * lvl.width + x1) * pixel_size,
                      tex.internal_format, tex.component_type);

      Color128 cx0 = c00 * (1.f - tx) + c10 * tx;
      Color128 cx1 = c01 * (1.f - tx) + c11 * tx;
      return cx0 * (1.f - ty) + cx1 * ty;
    }

    default:
      return Color128(0.f, 0.f, 0.f, 1.f);
  }
}

Color128 ApplyFilter(VGint filter, const VirtualGPU::TextureObject& tex,
                     uint32_t level, VGfloat u, VGfloat v, VGfloat w) {
  const auto& lvl = tex.mipmap[level];
  const uint8_t* base = lvl.memory->data();
  const uint32_t pixel_size =
      vg::GetPixelSize(tex.internal_format, tex.component_type);

  const float x = u * (lvl.width - 1);
  const float y = v * (lvl.height - 1);
  const float z = w * (lvl.depth - 1);

  Color128 c000, c100, c010, c110;
  Color128 c001, c101, c011, c111;

  switch (filter) {
    case VG_NEAREST: {
      int ix = (int)(math::Round(x));
      int iy = (int)(math::Round(y));
      int iz = (int)(math::Round(z));
      const uint8_t* p =
          base + ((iz * lvl.height + iy) * lvl.width + ix) * pixel_size;
      vg::DecodeColor(&c000, p, tex.internal_format, tex.component_type);
      return c000;
    }

    case VG_LINEAR: {
      int x0 = (int)(math::Floor(x));
      int y0 = (int)(math::Floor(y));
      int z0 = (int)(math::Floor(z));
      int x1 = x0 + 1;
      int y1 = y0 + 1;
      int z1 = z0 + 1;

      float tx = x - x0;
      float ty = y - y0;
      float tz = z - z0;

      vg::DecodeColor(
          &c000, base + ((z0 * lvl.height + y0) * lvl.width + x0) * pixel_size,
          tex.internal_format, tex.component_type);
      vg::DecodeColor(
          &c100, base + ((z0 * lvl.height + y0) * lvl.width + x1) * pixel_size,
          tex.internal_format, tex.component_type);
      vg::DecodeColor(
          &c010, base + ((z0 * lvl.height + y1) * lvl.width + x0) * pixel_size,
          tex.internal_format, tex.component_type);
      vg::DecodeColor(
          &c110, base + ((z0 * lvl.height + y1) * lvl.width + x1) * pixel_size,
          tex.internal_format, tex.component_type);

      vg::DecodeColor(
          &c001, base + ((z1 * lvl.height + y0) * lvl.width + x0) * pixel_size,
          tex.internal_format, tex.component_type);
      vg::DecodeColor(
          &c101, base + ((z1 * lvl.height + y0) * lvl.width + x1) * pixel_size,
          tex.internal_format, tex.component_type);
      vg::DecodeColor(
          &c011, base + ((z1 * lvl.height + y1) * lvl.width + x0) * pixel_size,
          tex.internal_format, tex.component_type);
      vg::DecodeColor(
          &c111, base + ((z1 * lvl.height + y1) * lvl.width + x1) * pixel_size,
          tex.internal_format, tex.component_type);

      Color128 c00 = c000 * (1 - tx) + c100 * tx;
      Color128 c10 = c010 * (1 - tx) + c110 * tx;
      Color128 c01 = c001 * (1 - tx) + c101 * tx;
      Color128 c11 = c011 * (1 - tx) + c111 * tx;

      Color128 c0 = c00 * (1 - ty) + c10 * ty;
      Color128 c1 = c01 * (1 - ty) + c11 * ty;

      return c0 * (1 - tz) + c1 * tz;
    }

    default:
      return Color128(0.f, 0.f, 0.f, 1.f);
  }
}

Color128 SampleTexture1D(VGuint unit_slot, VGfloat u) {
  VirtualGPU& vg = VirtualGPU::GetInstance();
  VirtualGPU::TextureUnit& unit = vg.texture_units_[unit_slot];
  VirtualGPU::TextureObject* tex =
      unit.bound_texture_targets[vg::GetTextureSlot(VG_TEXTURE_1D)];
  if (!tex) {
    return Color128(0.f, 0.f, 0.f, 1.f);
  }
  VirtualGPU::Sampler* sam = unit.bound_sampler == nullptr
                                 ? &tex->default_sampler
                                 : unit.bound_sampler;

  // 1) wrap
  VGfloat wrap_u = ApplyWrap(sam->wrap_s, u);
  if (wrap_u < 0.f) {
    return sam->border_color;
  }
  // 2) LOD -> no op

  // 3) base/max level clamp -> no op

  // 4) choose mip level -> level 0 only

  // 5) min/mag -> always mag filter
  Color128 filtered = ApplyFilter(sam->mag_filter, *tex, 0, wrap_u);

  // 6) depth compare -> no op

  // 7) swizzle -> no op

  return filtered;
}
Color128 SampleTexture2D(VGuint unit_slot, VGfloat u, VGfloat v) {
  VirtualGPU& vg = VirtualGPU::GetInstance();
  VirtualGPU::TextureUnit& unit = vg.texture_units_[unit_slot];

  VirtualGPU::TextureObject* tex =
      unit.bound_texture_targets[vg::GetTextureSlot(VG_TEXTURE_2D)];
  if (!tex) {
    return Color128(0.f, 0.f, 0.f, 1.f);
  }

  VirtualGPU::Sampler* sam =
      unit.bound_sampler ? unit.bound_sampler : &tex->default_sampler;

  // 1) wrap
  VGfloat wrap_u = ApplyWrap(sam->wrap_s, u);
  if (wrap_u < 0.f) {
    return sam->border_color;
  }

  VGfloat wrap_v = ApplyWrap(sam->wrap_t, v);
  if (wrap_v < 0.f) {
    return sam->border_color;
  }

  // 2) LOD -> no op

  // 3) base/max level clamp -> no op

  // 4) choose mip level -> level 0 only

  // 5) min/mag -> always mag filter
  Color128 filtered = ApplyFilter(sam->mag_filter, *tex, 0, wrap_u, wrap_v);

  // 6) depth compare -> no op

  // 7) swizzle -> no op

  return filtered;
}

Color128 SampleTexture3D(VGuint unit_slot, VGfloat u, VGfloat v, VGfloat w) {
  VirtualGPU& vg = VirtualGPU::GetInstance();
  VirtualGPU::TextureUnit& unit = vg.texture_units_[unit_slot];

  VirtualGPU::TextureObject* tex =
      unit.bound_texture_targets[vg::GetTextureSlot(VG_TEXTURE_3D)];
  if (!tex) {
    return Color128(0.f, 0.f, 0.f, 1.f);
  }

  VirtualGPU::Sampler* sam =
      unit.bound_sampler ? unit.bound_sampler : &tex->default_sampler;

  // 1) wrap
  VGfloat wrap_u = ApplyWrap(sam->wrap_s, u);
  if (wrap_u < 0.f) {
    return sam->border_color;
  }

  VGfloat wrap_v = ApplyWrap(sam->wrap_t, v);
  if (wrap_v < 0.f) {
    return sam->border_color;
  }

  VGfloat wrap_w = ApplyWrap(sam->wrap_r, w);
  if (wrap_w < 0.f) {
    return sam->border_color;
  }

  // 2) LOD -> no op

  // 3) base/max level clamp -> no op

  // 4) choose mip level -> level 0 only

  // 5) min/mag -> always mag filter
  Color128 filtered =
      ApplyFilter(sam->mag_filter, *tex, 0, wrap_u, wrap_v, wrap_w);

  // 6) depth compare -> no op

  // 7) swizzle -> no op

  return filtered;
}

}  // namespace ho