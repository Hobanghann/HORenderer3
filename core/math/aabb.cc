#include "aabb.h"

#include <cstdint>
#include <string>

#include "math_funcs.h"

namespace ho {

    AABB AABB::ConstructFromPositions(const std::vector<Vector3>& positions) {
        if (positions.size() == 0) {
            return AABB();
        }
        if (positions.size() == 1) {
            return AABB(positions[0], positions[0]);
        }
        Vector3 min_edges = Vector3(math::REAL_MAX, math::REAL_MAX, math::REAL_MAX);
        Vector3 max_edges = Vector3(-math::REAL_MAX, -math::REAL_MAX, -math::REAL_MAX);

        for (uint32_t i = 0; i < positions.size(); i++) {
            // construct aabb edges
            if (positions[i].x > max_edges.x) {
                max_edges.x = positions[i].x;
            }
            if (positions[i].x < min_edges.x) {
                min_edges.x = positions[i].x;
            }
            if (positions[i].y > max_edges.y) {
                max_edges.y = positions[i].y;
            }
            if (positions[i].y < min_edges.y) {
                min_edges.y = positions[i].y;
            }
            if (positions[i].z > max_edges.z) {
                max_edges.z = positions[i].z;
            }
            if (positions[i].z < min_edges.z) {
                min_edges.z = positions[i].z;
            }
        }
        return AABB(min_edges, max_edges);
    }

    std::string AABB::ToString() const {
        char buf[100];
        snprintf(buf, sizeof(buf), "Min: (%.3f, %.3f, %.3f), Max : (%.3f, %.3f, %.3f)", (float)min_edges.x,
                 (float)min_edges.y, (float)min_edges.z, (float)max_edges.x, (float)max_edges.y, (float)max_edges.z);
        return buf;
    }
}  // namespace ho