#include "sphere.h"

#include <cstdint>

#include "math_funcs.h"

namespace ho {

    Sphere Sphere::FromPositionsCentroid(const std::vector<Vector3>& positions) {
        if (positions.empty()) {
            return Sphere();
        }
        if (positions.size() == 1) {
            return Sphere(positions[0], 0.0_r);
        }
        Vector3 s_center;
        real radius = 0;

        int n = 0;
        for (size_t i = 0; i < positions.size(); i++) {
            // construct sphere center
            n += 1;
            const Vector3& x = Vector3(positions[i]);
            s_center += (x - s_center) * (1.0_r / static_cast<real>(n));
        }

        // construct sphere radius
        real max_squared_distance = 0;
        for (size_t i = 0; i < positions.size(); i++) {
            real squared_distance = math::SqrdDistance(s_center, positions[i]);
            max_squared_distance = math::Max(max_squared_distance, squared_distance);
        }
        radius = math::Sqrt(max_squared_distance);

        return Sphere(s_center, radius);
    }

    // @brief Constructs a bounding sphere for the mesh using Ritter's Algorithm.
    //
    // This method provides a fast, linear-time (O(N)) approximation of the
    // minimum bounding sphere. While not perfectly optimal, it produces a
    // sphere that is generally much tighter than one based on the simple
    // average of vertex positions.
    //
    // The algorithm proceeds in two main steps:
    //
    // 1.  Find Initial Sphere:
    // - An initial guess is made by finding the two most distant points in the
    // vertex set to form the sphere's initial diameter.
    // - (Optimization: To avoid an O(N^2) search, we find the min/max points
    // along each axis (X, Y, Z) and use the most distant pair among them).
    //
    // 2.  Expand the Sphere:
    // - The algorithm iterates through all vertices one more time.
    // - If a vertex is found outside the current sphere, the sphere is expanded
    // just enough to include both the old sphere and this new point. The new
    // sphere will pass through the new point and the point on the old
    // sphere's surface farthest from the new point.
    //
    // This process is repeated until all vertices are contained within the sphere.

    Sphere Sphere::FromPositionsRitter(const std::vector<Vector3>& positions) {
        if (positions.empty()) {
            return Sphere();
        }

        if (positions.size() == 1) {
            return Sphere(positions[0], 0.f);
        }

        // Seed the initial sphere using the most separated pair among axis extremes
        size_t x_min_idx = 0, x_max_idx = 0;
        size_t y_min_idx = 0, y_max_idx = 0;
        size_t z_min_idx = 0, z_max_idx = 0;

        real x_min = math::REAL_MAX, y_min = math::REAL_MAX, z_min = math::REAL_MAX;
        real x_max = -math::REAL_MAX, y_max = -math::REAL_MAX, z_max = -math::REAL_MAX;

        for (size_t i = 0; i < positions.size(); i++) {
            const Vector3& p = positions[i];
            if (p.x < x_min) {
                x_min = p.x;
                x_min_idx = i;
            }
            if (p.x > x_max) {
                x_max = p.x;
                x_max_idx = i;
            }
            if (p.y < y_min) {
                y_min = p.y;
                y_min_idx = i;
            }
            if (p.y > y_max) {
                y_max = p.y;
                y_max_idx = i;
            }
            if (p.z < z_min) {
                z_min = p.z;
                z_min_idx = i;
            }
            if (p.z > z_max) {
                z_max = p.z;
                z_max_idx = i;
            }
        }

        const Vector3& x_min_v = positions[x_min_idx];
        const Vector3& x_max_v = positions[x_max_idx];
        const Vector3& y_min_v = positions[y_min_idx];
        const Vector3& y_max_v = positions[y_max_idx];
        const Vector3& z_min_v = positions[z_min_idx];
        const Vector3& z_max_v = positions[z_max_idx];

        const real x_d2 = math::SqrdDistance(x_min_v, x_max_v);
        const real y_d2 = math::SqrdDistance(y_min_v, y_max_v);
        const real z_d2 = math::SqrdDistance(z_min_v, z_max_v);

        Vector3 a, b;
        if (x_d2 > y_d2 && x_d2 > z_d2) {
            a = x_min_v;
            b = x_max_v;
        } else if (y_d2 > z_d2) {
            a = y_min_v;
            b = y_max_v;
        } else {
            a = z_min_v;
            b = z_max_v;
        }

        // Initial sphere: diameter AB
        Vector3 center = (a + b) * 0.5_r;
        real radius = math::Sqrt(math::SqrdDistance(a, b)) * 0.5_r;

        // Ritter expansion: adjust only when a point lies outside the current sphere
        for (size_t i = 0; i < positions.size(); i++) {
            const Vector3& p = positions[i];
            const real d2 = math::SqrdDistance(p, center);

            // If the point is outside the sphere (with a small tolerance)
            if (d2 > radius * radius * (1.0_r + math::EPSILON_CMP2)) {
                const real d = math::Sqrt(d2);
                if (d > 0.0_r) {
                    const real new_r = (radius + d) * 0.5_r;
                    // Move center toward p by (new_r - radius) along the direction to p
                    const Vector3 dir = (p - center) / d;
                    center += dir * (new_r - radius);
                    radius = new_r;
                }
            }
        }

        return Sphere(center, radius);
    }

    Sphere Sphere::Merge(const Sphere& lhs, const Sphere& rhs) {
        const Vector3 c1 = lhs.center;
        const Vector3 c2 = rhs.center;
        const real r1 = lhs.radius;
        const real r2 = rhs.radius;

        const Vector3 diff = c2 - c1;
        const real dist = diff.Magnitude();

        if (r1 >= dist + r2) {
            return lhs;
        }
        if (r2 >= dist + r1) {
            return rhs;
        }

        real new_radius = (dist + r1 + r2) * 0.5_r;

        Vector3 dir = diff / dist;
        Vector3 new_center = c1 + dir * (new_radius - r1);

        return Sphere{new_center, new_radius};
    }

    std::string Sphere::ToString() const {
        char buf[100];
        snprintf(buf, sizeof(buf), "Center: (%.3f, %.3f, %.3f), Radius : %.3f", static_cast<float>(center.x),
                 static_cast<float>(center.y), static_cast<float>(center.z), static_cast<float>(radius));
        return buf;
    }
}  // namespace ho
