#include "transform_2d.h"

namespace ho {
Transform2D Transform2D::FromAngle(real angle) {
  return Transform2D(Basis2D::FromAngle(angle), Vector2::ZERO);
}

std::string Transform2D::ToString() const {
  char buf[200];
  std::snprintf(buf, sizeof(buf), "Transform2D Origin: %s, Basis : %s",
                origin.ToString().c_str(), basis.ToString().c_str());
  return std::string(buf);
}

}  // namespace ho