#include "vector2.h"

#include <string>

#include "math_funcs.h"
#include "vector3.h"
#include "vector4.h"

namespace ho {
    Vector2::Vector2(const Vector3& v) : x(v.x), y(v.y) {}
    Vector2::Vector2(const Vector4& v) : x(v.x), y(v.y) {}

    real Vector2::Magnitude() const { return math::Sqrt(SqrdMagnitude()); }

    Vector2& Vector2::Normalize() {
        const real inv_mag = math::InvSqrtFast(SqrdMagnitude());
        *this = inv_mag * *this;
        return *this;
    }

    Vector2 Vector2::Normalized() const {
        const real inv_mag = math::InvSqrtFast(SqrdMagnitude());
        return inv_mag * *this;
    }

    bool Vector2::IsFinite() const { return math::IsFinite(x) && math::IsFinite(y); }

    Vector3 Vector2::ToHomogeneous() const { return Vector3(x, y, 1.0_r); }

    std::string Vector2::ToString() const {
        char buf[100];
        snprintf(buf, sizeof(buf), "(%.3f, %.3f)", (float)x, (float)y);
        return buf;
    }
}  // namespace ho