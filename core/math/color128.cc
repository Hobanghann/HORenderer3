#include "color128.h"

#include "color32.h"
#include "vector3.h"

namespace ho {
Color128 Color128::FromVector3(const Vector3& v) {
  return Color128((static_cast<float>(v.x) + 1.f) * 0.5f,
                  (static_cast<float>(v.y) + 1.f) * 0.5f,
                  (static_cast<float>(v.z) + 1.f) * 0.5f);
}

Color128::Color128(const Color32& c)
    : r(static_cast<float>(c.r)),
      g(static_cast<float>(c.g)),
      b(static_cast<float>(c.b)),
      a(static_cast<float>(c.a)) {
  float inv_max = 1.f / 255.f;
  *this *= inv_max;
}

// Conversion linear to srgb quaternion as listed in
// https://en.wikipedia.org/wiki/SRGB#The_sRGbtransferfunction
Color128 Color128::sRGBToLinear() const {
  return Color128(
      r < 0.04045f ? r * (1.0f / 12.92f)
                   : math::Pow(static_cast<float>((r + 0.055f) *
                                                  (1.0f / (1.f + 0.055f))),
                               2.4f),
      g < 0.04045f ? g * (1.0f / 12.92f)
                   : math::Pow(static_cast<float>((g + 0.055f) *
                                                  (1.0f / (1.f + 0.055f))),
                               2.4f),
      b < 0.04045f ? b * (1.0f / 12.92f)
                   : math::Pow(static_cast<float>((b + 0.055f) *
                                                  (1.0f / (1.f + 0.055f))),
                               2.4f),
      a);
}
// Conversion srgb to linear quaternion as listed in
// https://en.wikipedia.org/wiki/SRGB#The_sRGbtransferfunction
Color128 Color128::LinearTosRGB() const {
  return Color128(
      r < 0.0031308f ? 12.92f * r
                     : (1.0f + 0.055f) * math::Pow(r, 1.0f / 2.4f) - 0.055f,
      g < 0.0031308f ? 12.92f * g
                     : (1.0f + 0.055f) * math::Pow(g, 1.0f / 2.4f) - 0.055f,
      b < 0.0031308f ? 12.92f * b
                     : (1.0f + 0.055f) * math::Pow(b, 1.0f / 2.4f) - 0.055f,
      a);
}

Vector3 Color128::ToVector3() const {
  return Vector3(static_cast<real>(r) * 2.0_r - 1.0_r,
                 static_cast<real>(g) * 2.0_r - 1.0_r,
                 static_cast<real>(b) * 2.0_r - 1.0_r);
}

std::string Color128::ToString() const {
  char buf[100];
  snprintf(buf, sizeof(buf), "(R: %.3f, G: %.3f, B: %.3f, A: %.3f)", r, g, b,
           a);
  return buf;
}
}  // namespace ho