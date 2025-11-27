#pragma once

#include <cstdint>
#include <string>

#include "math_defs.h"
#include "math_funcs.h"
#include "src/macros.h"

namespace ho {
struct Color32;
struct Vector3;

struct Color128 {
  static Color128 FromVector3(const Vector3& v);

  constexpr Color128();
  constexpr Color128(const Color128&) = default;
  constexpr Color128(float red, float green, float blue, float alpha = 1.f);
  explicit Color128(const Color32& c);
  constexpr Color128& operator=(const Color128& rhs);
  ~Color128() = default;

  constexpr Color128 operator+(const Color128& rhs) const;
  constexpr Color128& operator+=(const Color128& rhs);

  constexpr Color128 operator-(const Color128& rhs) const;
  constexpr Color128& operator-=(const Color128& rhs);

  constexpr Color128 operator*(const Color128& rhs) const;
  constexpr Color128& operator*=(const Color128& rhs);

  constexpr Color128 operator*(float scalar) const;
  constexpr Color128& operator*=(float scalar);

  constexpr Color128 operator/(const Color128& rhs) const;
  constexpr Color128& operator/=(const Color128& rhs);

  constexpr Color128 operator/(float scalar) const;
  constexpr Color128& operator/=(float scalar);

  constexpr bool operator==(const Color128& rhs) const;
  constexpr bool operator!=(const Color128& rhs) const;

  constexpr bool IsEqualApprox(const Color128& rhs) const;
  constexpr bool IsNotEqualApprox(const Color128& rhs) const;

  constexpr real Luminance() const;
  constexpr real Luminance(real rweight, real gweight, real bweight) const;

  constexpr Color128 Over(const Color128& background) const;
  constexpr Color128 Under(const Color128& background) const;

  Color128 sRGBToLinear() const;
  Color128 LinearTosRGB() const;

  Vector3 ToVector3() const;

  std::string ToString() const;

  float r;
  float g;
  float b;
  float a;
};

constexpr Color128::Color128() : r(0.f), g(0.f), b(0.f), a(1.f) {}
constexpr Color128::Color128(float red, float green, float blue, float alpha)
    : r(red), g(green), b(blue), a(alpha) {}

constexpr Color128& Color128::operator=(const Color128& rhs) {
  if (this == &rhs) {
    return *this;
  }
  r = rhs.r;
  g = rhs.g;
  b = rhs.b;
  a = rhs.a;
  return *this;
}

constexpr bool Color128::operator==(const Color128& rhs) const {
  return r == rhs.r && g == rhs.g && b == rhs.b && a == rhs.a;
}

constexpr bool Color128::operator!=(const Color128& rhs) const {
  return !(*this == rhs);
}

constexpr bool Color128::IsEqualApprox(const Color128& rhs) const {
  return math::IsEqualApprox(r, rhs.r) && math::IsEqualApprox(g, rhs.g) &&
         math::IsEqualApprox(b, rhs.b) && math::IsEqualApprox(a, rhs.a);
}
constexpr bool Color128::IsNotEqualApprox(const Color128& rhs) const {
  return !IsEqualApprox(rhs);
}

constexpr Color128 Color128::operator+(const Color128& rhs) const {
  return Color128(r + rhs.r, g + rhs.g, b + rhs.b, a + rhs.a);
}
constexpr Color128& Color128::operator+=(const Color128& rhs) {
  r += rhs.r;
  g += rhs.g;
  b += rhs.b;
  a += rhs.a;
  return *this;
}

constexpr Color128 Color128::operator-(const Color128& rhs) const {
  return Color128(r - rhs.r, g - rhs.g, b - rhs.b, a - rhs.a);
}
constexpr Color128& Color128::operator-=(const Color128& rhs) {
  r -= rhs.r;
  g -= rhs.g;
  b -= rhs.b;
  a -= rhs.a;
  return *this;
}

constexpr Color128 Color128::operator*(const Color128& rhs) const {
  return Color128(r * rhs.r, g * rhs.g, b * rhs.b, a * rhs.a);
}

constexpr Color128& Color128::operator*=(const Color128& rhs) {
  r *= rhs.r;
  g *= rhs.g;
  b *= rhs.b;
  a *= rhs.a;
  return *this;
}

constexpr Color128 Color128::operator*(float scalar) const {
  return Color128(r * scalar, g * scalar, b * scalar, a * scalar);
}

constexpr Color128& Color128::operator*=(float scalar) {
  r *= scalar;
  g *= scalar;
  b *= scalar;
  a *= scalar;
  return *this;
}

constexpr Color128 Color128::operator/(const Color128& rhs) const {
  return Color128(r / rhs.r, g / rhs.g, b / rhs.b, a / rhs.a);
}

constexpr Color128& Color128::operator/=(const Color128& rhs) {
  r /= rhs.r;
  g /= rhs.g;
  b /= rhs.b;
  a /= rhs.a;
  return *this;
}

constexpr Color128 Color128::operator/(float scalar) const {
  return Color128(r / scalar, g / scalar, b / scalar, a / scalar);
}

constexpr Color128& Color128::operator/=(float scalar) {
  r /= scalar;
  g /= scalar;
  b /= scalar;
  a /= scalar;
  return *this;
}

_ALWAYS_INLINE_ constexpr Color128 operator*(float scalar, const Color128& c) {
  return c * scalar;
}

// weights as listed in
// https://en.wikipedia.org/wiki/Relative_luminance
constexpr real Color128::Luminance() const {
  return 0.2126_r * (real)r + 0.7152_r * (real)g + 0.0722_r * (real)b;
}
constexpr real Color128::Luminance(real rweight, real gweight,
                                   real bweight) const {
  return static_cast<real>(r) * rweight + static_cast<real>(g) * gweight +
         static_cast<real>(b) * bweight;
}

// Implement over operation(*this over background) in Porter-Duff Blending
// Model https://keithp.com/~keithp/porterduff/p253-porter.pdf
constexpr Color128 Color128::Over(const Color128& background) const {
  Color128 res;
  float transparency = 1.0f - a;  // (1 - α_A)
  res.a = a + background.a * transparency;

  if (res.a == 0) {
    return Color128(0.f, 0.f, 0.f, 0.f);
  } else {
    float inv_a = 1.f / res.a;
    res.r = (r * a + background.r * background.a * transparency) * inv_a;
    res.g = (g * a + background.g * background.a * transparency) * inv_a;
    res.b = (b * a + background.b * background.a * transparency) * inv_a;
  }
  return res;
}
constexpr Color128 Color128::Under(const Color128& foreground) const {
  {
    Color128 res;
    float transparency = 1.0f - foreground.a;
    res.a = a * transparency + foreground.a;
    if (res.a == 0) {
      return Color128(0.f, 0.f, 0.f, 0.f);
    } else {
      float inv_a = 1.f / res.a;
      res.r = (r * a * transparency + foreground.r * foreground.a) * inv_a;
      res.g = (g * a * transparency + foreground.g * foreground.a) * inv_a;
      res.b = (b * a * transparency + foreground.b * foreground.a) * inv_a;
    }
    return res;
  }
}
}  // namespace ho
