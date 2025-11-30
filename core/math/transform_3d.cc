#include "transform_3d.h"

namespace ho {
    std::string Transform3D::ToString() const {
        char buf[256];
        std::snprintf(buf, sizeof(buf), "Transform3D Origin: %s, Basis: %s", origin.ToString().c_str(),
                      basis.ToString().c_str());
        return std::string(buf);
    }

}  // namespace ho
