#include "color32.h"

#include <cstdint>

#include "color128.h"

namespace ho {

Color32::Color32(const Color128& c)
    : r(static_cast<uint8_t>(math::Clamp(c.r, 0.0_r, 1.0_r) * 255.0_r)),
      g(static_cast<uint8_t>(math::Clamp(c.g, 0.0_r, 1.0_r) * 255.0_r)),
      b(static_cast<uint8_t>(math::Clamp(c.b, 0.0_r, 1.0_r) * 255.0_r)),
      a(static_cast<uint8_t>(math::Clamp(c.a, 0.0_r, 1.0_r) * 255.0_r)) {}

std::string Color32::ToString() const {
  char buf[100];
  snprintf(buf, sizeof(buf), "(R: %d, G: %d, B: %d, A: %d)", r, g, b, a);
  return buf;
}

}  // namespace ho
