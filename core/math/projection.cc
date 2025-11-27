#include "projection.h"

#include "matrix4x4.h"
#include "transform_3d.h"

namespace ho {

std::string Projection::ToString() const {
  char buf[200];
  std::snprintf(buf, sizeof(buf),
                "Projection(fov: %.6f rad, focal: %.6f, "
                "aspect: %.6f, near: %.6f, far: %.6f)",
                (float)get_fov(), (float)get_aspect_ratio(),
                (float)get_near_dist(), (float)get_far_dist());
  return std::string(buf);
}
}  // namespace ho