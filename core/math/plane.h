#pragma once

#include <string>

#include "vector3.h"

// plane defined by normal vector and 1 point
// when plane defined by 4-dimensional vector, x, y, z component is normal
// vector component, w component is dot product normal vector and point
namespace ho {
    struct AABB;
    struct Sphere;
    struct Plane {
        ALWAYS_INLINE Plane();
        ALWAYS_INLINE Plane(real p_a, real p_b, real p_c, real p_d);
        ALWAYS_INLINE Plane(const Vector3& p_normal, const Vector3& p_point);
        ALWAYS_INLINE Plane(const Vector3& v1, const Vector3& v2, const Vector3& v3,
                            math::ClockDirection = math::COUNTER_CLOCK_WISE);
        ALWAYS_INLINE Plane(const Plane&) = default;
        ALWAYS_INLINE Plane& operator=(const Plane& rhs);
        ~Plane() = default;

        ALWAYS_INLINE bool operator==(const Plane& rhs) const;
        ALWAYS_INLINE bool operator!=(const Plane& rhs) const;

        ALWAYS_INLINE bool IsEqualApprox(const Plane& rhs) const;
        ALWAYS_INLINE bool IsNotEqualApprox(const Plane& rhs) const;

        ALWAYS_INLINE real DistToPoint(const Vector3& point) const;

        ALWAYS_INLINE Vector3 Project(const Vector3& point) const;

        ALWAYS_INLINE math::Side GetSide(const Vector3& p) const;
        ALWAYS_INLINE math::Side GetSide(const Vector3& p1, const Vector3& p2) const;
        ALWAYS_INLINE math::Side GetSide(const Vector3& p1, const Vector3& p2, const Vector3& p3) const;
        math::Side GetSide(const AABB& aabb) const;
        math::Side GetSide(const Sphere& sphere) const;

        std::string ToString() const;

        Vector3 normal;
        real d;

       private:
        template <size_t N>
        ALWAYS_INLINE math::Side GetSide(const std::array<Vector3, N>& points) const;
    };

    Plane::Plane() : normal(Vector3::ZERO), d(0.0_r) {}
    Plane::Plane(real p_a, real p_b, real p_c, real p_d) {
        normal = Vector3(p_a, p_b, p_c);
        const real l = normal.Magnitude();
        normal /= l;
        d = p_d / l;
    }
    Plane::Plane(const Vector3& p_normal, const Vector3& p_point)
        : normal(p_normal.Normalized()), d(-p_normal.Dot(p_point)) {}
    Plane::Plane(const Vector3& v1, const Vector3& v2, const Vector3& v3, math::ClockDirection dir) {
        switch (dir) {
            case math::CLOCK_WISE:
                normal = (v3 - v1).Cross(v2 - v1).Normalized();
                break;
            case math::COUNTER_CLOCK_WISE:
                normal = (v2 - v1).Cross(v3 - v1).Normalized();
                break;
        }
        d = -normal.Dot(v1);
    }

    Plane& Plane::operator=(const Plane& rhs) {
        normal = rhs.normal;
        d = rhs.d;
        return *this;
    }

    bool Plane::operator==(const Plane& rhs) const { return normal == rhs.normal && d == rhs.d; }
    bool Plane::operator!=(const Plane& rhs) const { return !(*this == rhs); }

    bool Plane::IsEqualApprox(const Plane& rhs) const {
        return normal.IsEqualApprox(rhs.normal) && math::IsEqualApprox(d, rhs.d);
    }
    bool Plane::IsNotEqualApprox(const Plane& rhs) const { return !IsEqualApprox(rhs); }
    real Plane::DistToPoint(const Vector3& point) const { return normal.Dot(point) + d; }

    Vector3 Plane::Project(const Vector3& point) const {
        real dist = DistToPoint(point);
        return point - dist * normal;
    }

    template <size_t N>
    math::Side Plane::GetSide(const std::array<Vector3, N>& points) const {
        int num_outside = 0;
        for (const auto& p : points) {
            if (GetSide(p) == math::OUTSIDE) ++num_outside;
        }

        if (num_outside == N) return math::OUTSIDE;
        if (num_outside == 0) return math::INSIDE;
        return math::INTERSECT;
    }

    math::Side Plane::GetSide(const Vector3& p) const {
        real res = normal.Dot(p) + d;
        if (res > math::EPSILON_POINT_ON_PLANE) {
            return math::OUTSIDE;
        } else if (res < -math::EPSILON_POINT_ON_PLANE) {
            return math::INSIDE;
        } else {
            return math::ON;
        }
    }

    math::Side Plane::GetSide(const Vector3& p1, const Vector3& p2) const {
        int num_outside = 0;
        if (GetSide(p1) == math::OUTSIDE) {
            num_outside++;
        }
        if (GetSide(p2) == math::OUTSIDE) {
            num_outside++;
        }
        if (num_outside == 2) {
            return math::OUTSIDE;
        }
        if (num_outside == 0) {
            return math::INSIDE;
        }
        return math::INTERSECT;
    }

    math::Side Plane::GetSide(const Vector3& p1, const Vector3& p2, const Vector3& p3) const {
        int num_outside = 0;
        if (GetSide(p1) == math::OUTSIDE) {
            num_outside++;
        }
        if (GetSide(p2) == math::OUTSIDE) {
            num_outside++;
        }
        if (GetSide(p3) == math::OUTSIDE) {
            num_outside++;
        }
        if (num_outside == 3) {
            return math::OUTSIDE;
        }
        if (num_outside == 0) {
            return math::INSIDE;
        }
        return math::INTERSECT;
    }

}  // namespace ho
