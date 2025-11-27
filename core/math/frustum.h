#pragma once;

#include <array>
#include <string>

#include "math_defs.h"
#include "plane.h"
// frustum defined by 6 planes in view, world, local space
// 6 planes that define frustum defined by affine_matrix
namespace ho {
struct Vector3;
struct Matrix4x4;
struct Plane;
struct AABB;
struct Sphere;

enum PlanePos {
  PLANE_POS_LEFT = 0,
  PLANE_POS_RIGHT,
  PLANE_POS_BOTTOM,
  PLANE_POS_TOP,
  PLANE_POS_NEAR,
  PLANE_POS_FAR,
  PLANE_POS_LAST
};

struct Frustum {
  static Frustum FromMatrix4x4(const Matrix4x4& m);

  Frustum();
  Frustum(const Frustum& frustum);
  Frustum(const Plane& left, const Plane& right, const Plane& bottom,
          const Plane& top, const Plane& near, const Plane& far);
  Frustum& operator=(const Frustum& rhs);
  ~Frustum() = default;

  bool operator==(const Frustum& rhs) const;
  bool operator!=(const Frustum& rhs) const;

  bool IsEqualApprox(const Frustum& rhs) const;
  bool IsNotEqualApprox(const Frustum& rhs) const;

  math::Side GetSide(const Vector3& p) const;
  math::Side GetSide(const Vector3& p1, const Vector3& p2) const;
  math::Side GetSide(const Vector3& p1, const Vector3& p2,
                     const Vector3& p3) const;
  math::Side GetSide(const AABB& aabb) const;
  math::Side GetSide(const Sphere& sphere) const;

  std::string ToString() const;

  Plane planes[PLANE_POS_LAST] = {Plane()};
};
}  // namespace ho
