#pragma once

#include "core/macros.h"
#include "core/math/color128.h"
#include "core/math/color32.h"
#include "core/math/math_funcs.h"
#include "core/math/matrix2x2.h"
#include "core/math/matrix3x3.h"
#include "core/math/matrix4x4.h"
#include "core/math/vector2.h"
#include "core/math/vector3.h"
#include "core/math/vector4.h"
#include "half.hpp"
#include "vg.h"

namespace ho {
INLINE constexpr uint32_t VG_INVALID_SLOT = 0xFFFFFFFF;

constexpr uint32_t fnv1a_32(const char* s, size_t n,
                            uint32_t hash = 2166136261u) {
  return n == 0 ? hash
                : fnv1a_32(s + 1, n - 1,
                           (hash ^ static_cast<uint8_t>(*s)) * 16777619u);
}

constexpr VGint operator"" _vg(const char* s, size_t n) {
  return static_cast<VGint>(fnv1a_32(s, n));
}

namespace vg {

ALWAYS_INLINE uint32_t GetBufferSlot(VGenum target) {
  switch (target) {
    case VG_ARRAY_BUFFER:
      return 0;
    case VG_COPY_READ_BUFFER:
      return 1;
    case VG_COPY_WRITE_BUFFER:
      return 2;
    case VG_ELEMENT_ARRAY_BUFFER:
      return 3;
    case VG_PIXEL_PACK_BUFFER:
      return 4;
    case VG_PIXEL_UNPACK_BUFFER:
      return 5;
    case VG_TEXTURE_BUFFER:
      return 6;
    case VG_TRANSFORM_FEEDBACK_BUFFER:
      return 7;
    case VG_UNIFORM_BUFFER:
      return 8;
    default:
      return VG_INVALID_SLOT;
  }
}

ALWAYS_INLINE uint32_t GetTextureSlot(VGenum target) {
  switch (target) {
    case VG_TEXTURE_1D:
    case VG_PROXY_TEXTURE_1D:
      return 0;
    case VG_TEXTURE_2D:
    case VG_PROXY_TEXTURE_2D:
      return 1;
    case VG_TEXTURE_3D:
    case VG_PROXY_TEXTURE_3D:
      return 2;
    default:
      return VG_INVALID_SLOT;
  }
}

ALWAYS_INLINE bool IsProxyTexture(VGenum target) {
  return (target == VG_PROXY_TEXTURE_1D) || (target == VG_PROXY_TEXTURE_2D) ||
         (target == VG_PROXY_TEXTURE_3D);
}

ALWAYS_INLINE bool IsColorFormat(VGenum format) {
  return (format == VG_RED) || (format == VG_RG) || (format == VG_RGB) ||
         (format == VG_RGBA);
}

ALWAYS_INLINE bool IsValidFormat(VGenum format) {
  return (format == VG_RED) || (format == VG_RG) || (format == VG_RGB) ||
         (format == VG_RGBA) || (format == VG_DEPTH_COMPONENT) ||
         (format == VG_DEPTH_STENCIL);
}

ALWAYS_INLINE bool IsValidType(VGenum type) {
  return (type == VG_UNSIGNED_BYTE) || (type == VG_BYTE) ||
         (type == VG_UNSIGNED_SHORT) || (type == VG_SHORT) ||
         (type == VG_HALF_FLOAT) || (type == VG_UNSIGNED_INT) ||
         (type == VG_INT) || (type == VG_FLOAT);
}

ALWAYS_INLINE uint32_t GetTypeSize(VGenum type) {
  switch (type) {
    case VG_UNSIGNED_BYTE:
    case VG_BYTE:
      return 1;
    case VG_UNSIGNED_SHORT:
    case VG_SHORT:
    case VG_HALF_FLOAT:
      return 2;
    case VG_UNSIGNED_INT:
    case VG_INT:
    case VG_FLOAT:
      return 4;
    default:
      return 0;
      break;
  }
}

ALWAYS_INLINE uint32_t GetChannelCount(VGenum format) {
  switch (format) {
    case VG_RED:
    case VG_DEPTH_COMPONENT:
      return 1;
    case VG_DEPTH_STENCIL:
    case VG_RG:
      return 2;
    case VG_RGB:
      return 3;
    case VG_RGBA:
      return 4;
    default:
      return 0;
  }
}

ALWAYS_INLINE uint32_t GetPixelSize(VGenum format, VGenum type) {
  const uint32_t type_size = GetTypeSize(type);
  switch (format) {
    case VG_DEPTH_COMPONENT:
    case VG_RED:
      return 1 * type_size;
    case VG_RG:
      return 2 * type_size;
    case VG_RGB:
      return 3 * type_size;
    case VG_RGBA:
      return 4 * type_size;
    case VG_DEPTH_STENCIL:
      return 4;
    default:
      return 0;
  }
}

ALWAYS_INLINE float HalfToFloat(uint16_t h) {
  half_float::half temp;
  std::memcpy(&temp, &h, sizeof(h));
  return half_float::half_cast<float, half_float::half>(temp);
}

ALWAYS_INLINE uint16_t FloatToHalf(float f) {
  half_float::half h = half_float::half_cast<half_float::half, float>(f);
  uint16_t bits;
  std::memcpy(&bits, &h, sizeof(bits));
  return bits;
}

ALWAYS_INLINE void CopyPixel(uint8_t* dst, const uint8_t* src,
                             VGenum dst_format, VGenum dst_type,
                             VGenum src_format, VGenum src_type,
                             const bool* color_mask = nullptr) {
  // Color
  if (IsColorFormat(dst_format)) {
    const uint32_t src_channels = GetChannelCount(src_format);
    const uint32_t dst_channels = GetChannelCount(dst_format);
    const uint32_t src_type_size = GetTypeSize(src_type);
    const uint32_t dst_type_size = GetTypeSize(dst_type);

    for (uint32_t c = 0; c < dst_channels; ++c) {
      if (color_mask && !color_mask[c]) {
        continue;
      }

      float value = 0.0f;

      // Read
      if (c < src_channels) {
        const uint8_t* src_comp = src + c * src_type_size;

        switch (src_type) {
          case VG_UNSIGNED_BYTE:
            value = src_comp[0] / 255.0f;
            break;
          case VG_BYTE:
            value = reinterpret_cast<const int8_t*>(src_comp)[0] / 127.0f;
            break;
          case VG_UNSIGNED_SHORT:
            value = reinterpret_cast<const uint16_t*>(src_comp)[0] / 65535.0f;
            break;
          case VG_SHORT:
            value = reinterpret_cast<const int16_t*>(src_comp)[0] / 32767.0f;
            break;
          case VG_UNSIGNED_INT:
            value =
                reinterpret_cast<const uint32_t*>(src_comp)[0] / 4294967295.0f;
            break;
          case VG_INT:
            value =
                reinterpret_cast<const int32_t*>(src_comp)[0] / 2147483647.0f;
            break;
          case VG_FLOAT:
            value = reinterpret_cast<const float*>(src_comp)[0];
            break;
          case VG_HALF_FLOAT:
            value = HalfToFloat(reinterpret_cast<const uint16_t*>(src_comp)[0]);
            break;
          default:
            assert(false);
        }
      } else {
        // missing components
        value = (c == 3) ? 1.0f : 0.0f;
      }

      uint8_t* dst_comp = dst + c * dst_type_size;

      // Write
      switch (dst_type) {
        case VG_UNSIGNED_BYTE: {
          value = math::Clamp(value, 0.0f, 1.0f);
          dst_comp[0] = static_cast<uint8_t>(value * 255.0f + 0.5f);
          break;
        }

        case VG_BYTE: {
          value = math::Clamp(value, -1.0f, 1.0f);
          if (value == -1.0f) {
            reinterpret_cast<int8_t*>(dst_comp)[0] = -128;
          } else {
            float q = value * 127.0f;
            q += (q >= 0.0f) ? 0.5f : -0.5f;
            reinterpret_cast<int8_t*>(dst_comp)[0] = static_cast<int8_t>(q);
          }
          break;
        }

        case VG_UNSIGNED_SHORT: {
          value = math::Clamp(value, 0.0f, 1.0f);
          reinterpret_cast<uint16_t*>(dst_comp)[0] =
              static_cast<uint16_t>(value * 65535.0f + 0.5f);
          break;
        }

        case VG_SHORT: {
          value = math::Clamp(value, -1.0f, 1.0f);
          if (value == -1.0f) {
            reinterpret_cast<int16_t*>(dst_comp)[0] = -32768;
          } else {
            float q = value * 32767.0f;
            q += (q >= 0.0f) ? 0.5f : -0.5f;
            reinterpret_cast<int16_t*>(dst_comp)[0] = static_cast<int16_t>(q);
          }
          break;
        }

        case VG_UNSIGNED_INT: {
          value = math::Clamp(value, 0.0f, 1.0f);
          reinterpret_cast<uint32_t*>(dst_comp)[0] =
              static_cast<uint32_t>(value * 4294967295.0f + 0.5f);
          break;
        }

        case VG_INT: {
          value = math::Clamp(value, -1.0f, 1.0f);
          if (value == -1.0f) {
            reinterpret_cast<int32_t*>(dst_comp)[0] =
                std::numeric_limits<int32_t>::min();
          } else {
            float q = value * 2147483647.0f;
            q += (q >= 0.0f) ? 0.5f : -0.5f;
            reinterpret_cast<int32_t*>(dst_comp)[0] = static_cast<int32_t>(q);
          }
          break;
        }

        case VG_FLOAT:
          reinterpret_cast<float*>(dst_comp)[0] = value;
          break;

        case VG_HALF_FLOAT:
          reinterpret_cast<uint16_t*>(dst_comp)[0] = FloatToHalf(value);
          break;

        default:
          assert(false);
      }
    }
    return;
  }

  // Depth
  if (dst_format == VG_DEPTH_COMPONENT && src_format == VG_DEPTH_COMPONENT) {
    assert(dst_type == VG_FLOAT);
    assert(src_type == VG_FLOAT);
    *reinterpret_cast<float*>(dst) = *reinterpret_cast<const float*>(src);
    return;
  }

  // Depth Stencil
  if (dst_format == VG_DEPTH_STENCIL && src_format == VG_DEPTH_STENCIL) {
    assert(src_type == VG_UNSIGNED_INT);
    assert(dst_type == VG_UNSIGNED_INT);
    *reinterpret_cast<uint32_t*>(dst) = *reinterpret_cast<const uint32_t*>(src);
    return;
  }

  assert(false);
}

ALWAYS_INLINE void EncodeColor(uint8_t* dst, const Color128& src, VGenum format,
                               VGenum type) {
  alignas(16) float src_pixel[4] = {src.r, src.g, src.b, src.a};

  CopyPixel(dst, reinterpret_cast<const uint8_t*>(src_pixel), format, type,
            VG_RGBA, VG_FLOAT);
}

ALWAYS_INLINE void DecodeColor(Color128* dst, const uint8_t* src, VGenum format,
                               VGenum type) {
  alignas(16) float dst_pixel[4];

  CopyPixel(reinterpret_cast<uint8_t*>(dst_pixel), src, VG_RGBA, VG_FLOAT,
            format, type);

  dst->r = dst_pixel[0];
  dst->g = dst_pixel[1];
  dst->b = dst_pixel[2];
  dst->a = dst_pixel[3];
}

ALWAYS_INLINE void EncodeDepthStencil(uint8_t* dst, VGfloat depth,
                                      VGint stencil) {
  depth = math::Clamp(depth, 0.f, 1.f);
  uint32_t qd = static_cast<uint32_t>(depth * 16777215.0f);
  dst[0] = (uint8_t)stencil;
  dst[1] = (uint8_t)((qd >> 0) & 0xFF);
  dst[2] = (uint8_t)((qd >> 8) & 0xFF);
  dst[3] = (uint8_t)((qd >> 16) & 0xFF);
}

ALWAYS_INLINE void DecodeDepthStencil(VGfloat* depth, uint8_t* stencil,
                                      const uint8_t* src) {
  *stencil = src[0];
  uint32_t d0 = src[1];
  uint32_t d1 = src[2];
  uint32_t d2 = src[3];
  uint32_t qd = (d0 << 0) | (d1 << 8) | (d2 << 16);
  *depth = static_cast<float>(qd) / 16777215.f;
}

}  // namespace vg
}  // namespace ho