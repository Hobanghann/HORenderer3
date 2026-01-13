#include "color128.h"

#include "color32.h"
#include "vector3.h"

namespace ho {
    Color128 Color128::FromVector3(const Vector3& v) {
        return Color128((static_cast<float>(v.x) + 1.f) * 0.5f, (static_cast<float>(v.y) + 1.f) * 0.5f,
                        (static_cast<float>(v.z) + 1.f) * 0.5f);
    }

    Color128::Color128(const Color32& c)
        : r(static_cast<float>(c.r)),
          g(static_cast<float>(c.g)),
          b(static_cast<float>(c.b)),
          a(static_cast<float>(c.a)) {
        float inv_max = 1.f / 255.f;
        *this *= inv_max;
    }

    // Approximation using Gamma 2.0 (squaring) for performance.
    // This is significantly faster than the standard IEC 61966-2-1 formula (pow 2.4).
    Color128 Color128::sRGBToLinear() const { return Color128(r * r, g * g, b * b, a); }

    // Approximation using Gamma 2.0 (square root) for performance.
    // 'sqrt' is much cheaper than the standard 'pow(x, 1.0/2.2)' operation.
    Color128 Color128::LinearTosRGB() const { return Color128(math::Sqrt(r), math::Sqrt(g), math::Sqrt(b), a); }

    Vector3 Color128::ToVector3() const {
        return Vector3(static_cast<real>(r) * 2.0_r - 1.0_r, static_cast<real>(g) * 2.0_r - 1.0_r,
                       static_cast<real>(b) * 2.0_r - 1.0_r);
    }

    std::string Color128::ToString() const {
        char buf[100];
        snprintf(buf, sizeof(buf), "(R: %.3f, G: %.3f, B: %.3f, A: %.3f)", r, g, b, a);
        return buf;
    }
}  // namespace ho