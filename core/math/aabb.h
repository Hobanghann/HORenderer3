#pragma once

#include <vector>

#include "vector3.h"

namespace ho {
    // axis aligned box bounding volume
    struct AABB {
        static AABB ConstructFromPositions(const std::vector<Vector3>& positions);

        constexpr static AABB Merge(const AABB& lhs, const AABB& rhs);

        constexpr AABB() = default;
        constexpr AABB(const AABB& aabb);
        constexpr AABB(const Vector3& min, const Vector3& max);
        constexpr AABB& operator=(const AABB& rhs);
        ~AABB() = default;

        constexpr bool operator==(const AABB& rhs) const;
        constexpr bool operator!=(const AABB& rhs) const;

        constexpr bool IsEqualApprox(const AABB& rhs) const;
        constexpr bool IsNotEqualApprox(const AABB& rhs) const;

        std::string ToString() const;

        Vector3 min_edges;
        Vector3 max_edges;
        Vector3 center;
    };

    constexpr AABB::AABB(const AABB& aabb) = default;
    constexpr AABB::AABB(const Vector3& min, const Vector3& max)
        : min_edges(min), max_edges(max), center(0.5_r * (max + min)) {}
    constexpr AABB& AABB::operator=(const AABB& rhs) = default;

    constexpr bool AABB::operator==(const AABB& rhs) const {
        return (min_edges == rhs.min_edges && max_edges == rhs.max_edges && center == rhs.center);
    }
    constexpr bool AABB::operator!=(const AABB& rhs) const { return !(*this == rhs); }

    constexpr bool AABB::IsEqualApprox(const AABB& rhs) const {
        return (min_edges.IsEqualApprox(rhs.min_edges) && max_edges.IsEqualApprox(rhs.max_edges) &&
                center.IsEqualApprox(rhs.center));
    }

    constexpr bool AABB::IsNotEqualApprox(const AABB& rhs) const { return !IsEqualApprox(rhs); }

    constexpr AABB AABB::Merge(const AABB& lhs, const AABB& rhs) {
        real min_x = math::Min(lhs.min_edges.x, rhs.min_edges.x);
        real min_y = math::Min(lhs.min_edges.y, rhs.min_edges.y);
        real min_z = math::Min(lhs.min_edges.z, rhs.min_edges.z);

        real max_x = math::Max(lhs.max_edges.x, rhs.max_edges.x);
        real max_y = math::Max(lhs.max_edges.y, rhs.max_edges.y);
        real max_z = math::Max(lhs.max_edges.z, rhs.max_edges.z);

        return AABB(Vector3(min_x, min_y, min_z), Vector3(max_x, max_y, max_z));
    }

}  // namespace ho
