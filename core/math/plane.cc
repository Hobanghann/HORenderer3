#include "plane.h"

#include "aabb.h"
#include "math_funcs.h"
#include "sphere.h"

namespace ho {
    math::Side Plane::GetSide(const AABB& aabb) const {
        Vector3 nearest = aabb.min_edges;
        Vector3 farthest = aabb.max_edges;
        if (normal.x < 0.0_r) {
            nearest.x = aabb.max_edges.x;
            farthest.x = aabb.min_edges.x;
        }
        if (normal.y < 0.0_r) {
            nearest.y = aabb.max_edges.y;
            farthest.y = aabb.min_edges.y;
        }
        if (normal.z < 0.0_r) {
            nearest.z = aabb.max_edges.z;
            farthest.z = aabb.min_edges.z;
        }
        if (normal.Dot(nearest) + d > math::EPSILON_POINT_ON_PLANE) {
            return math::OUTSIDE;
        } else if (normal.Dot(farthest) + d > math::EPSILON_POINT_ON_PLANE) {
            return math::INTERSECT;
        } else {
            return math::INSIDE;
        }
    }

    math::Side Plane::GetSide(const Sphere& sphere) const {
        real res = normal.Dot(sphere.center) + d;
        if (res > math::EPSILON_POINT_ON_PLANE) {
            if (res >= sphere.radius) {
                return math::OUTSIDE;
            } else {
                return math::INTERSECT;
            }
        } else {
            if (res >= -sphere.radius) {
                return math::INTERSECT;
            } else {
                return math::INSIDE;
            }
        }
    }

    std::string Plane::ToString() const {
        char buf[100];
        snprintf(buf, sizeof(buf), "%.3f*x + %.3f*y + %.3f*z + %.3f", static_cast<float>(normal.x),
                 static_cast<float>(normal.y), static_cast<float>(normal.z), static_cast<float>(d));
        return buf;
    }
}  // namespace ho