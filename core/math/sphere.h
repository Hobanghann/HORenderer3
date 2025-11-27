#pragma once

#include <string>
#include <vector>

#include "vector3.h"

namespace ho {
struct Mesh;
struct Sphere {
  static Sphere ConstructCentroidFromPositions(
      const std::vector<Vector3>& positions);
  static Sphere ConstructRitterFromPositions(
      const std::vector<Vector3>& positions);

  static Sphere Merge(const Sphere& lhs, const Sphere& rhs);

  constexpr Sphere();
  constexpr Sphere(const Sphere& s) = default;
  constexpr Sphere(const Vector3& p_center, real p_radius);
  constexpr Sphere& operator=(const Sphere& c);
  ~Sphere() = default;

  constexpr bool operator==(const Sphere& rhs) const;
  constexpr bool operator!=(const Sphere& rhs) const;

  constexpr bool IsEqualApprox(const Sphere& rhs) const;
  constexpr bool IsNotEqualApprox(const Sphere& rhs) const;

  std::string ToString() const;

  Vector3 center;
  real radius;
};

constexpr Sphere::Sphere() : center(Vector3::ZERO), radius(0.0_r) {}
constexpr Sphere::Sphere(const Vector3& p_center, real p_radius)
    : center(p_center), radius(p_radius) {}

constexpr Sphere& Sphere::operator=(const Sphere& bounding_volume) = default;

constexpr bool Sphere::operator==(const Sphere& rhs) const {
  return center == rhs.center && radius == rhs.radius;
}
constexpr bool Sphere::operator!=(const Sphere& rhs) const {
  return !(*this == rhs);
}

constexpr bool Sphere::IsEqualApprox(const Sphere& rhs) const {
  return center.IsEqualApprox(rhs.center) &&
         math::IsEqualApprox(radius, rhs.radius);
}
constexpr bool Sphere::IsNotEqualApprox(const Sphere& rhs) const {
  return !IsEqualApprox(rhs);
}
}  // namespace ho
