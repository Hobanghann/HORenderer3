#include "projection.h"

#include "matrix4x4.h"
#include "transform_3d.h"

namespace ho {
    Vector4 Projection::Project(const Vector4& v) const {
        return Vector4(matrix.data[0][0] * v.x, matrix.data[1][1] * v.y,
                       matrix.data[2][2] * v.z + matrix.data[2][3] * v.w, -v.z);
    }

    std::string Projection::ToString() const {
        char buf[200];
        std::snprintf(buf, sizeof(buf),
                      "Projection(fov/height: %.6f rad, aspect: %.6f, "
                      "near: %.6f, far: %.6f)",
                      static_cast<float>(fov), static_cast<float>(get_aspect_ratio()),
                      static_cast<float>(get_near_dist()), static_cast<float>(get_far_dist()));
        return std::string(buf);
    }
}  // namespace ho