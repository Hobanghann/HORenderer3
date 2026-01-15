#include "vector3.h"

#include <string>

#include "vector2.h"
#include "vector4.h"

namespace ho {
    Vector3::Vector3(const Vector2& v) : x(v.x), y(v.y), z(0.0_r) {}
    Vector3::Vector3(const Vector4& v) : x(v.x), y(v.y), z(v.z) {}

    real Vector3::Magnitude() const { return math::Sqrt(SqrdMagnitude()); }

    void Vector3::Normalize() {
        const real inv_mag = math::InvSqrtFast(SqrdMagnitude());
        *this = inv_mag * *this;
    }
    Vector3 Vector3::Normalized() const {
        const real inv_mag = math::InvSqrtFast(SqrdMagnitude());
        return inv_mag * *this;
    }

    bool Vector3::IsFinite() const { return math::IsFinite(x) && math::IsFinite(y) && math::IsFinite(z); }

    Vector2 Vector3::ToCartesian() const {
        const real inv_z = 1.0_r / z;
        return Vector2(x * inv_z, y * inv_z);
    }

    Vector4 Vector3::ToHomogeneous() const { return Vector4(x, y, z, 1.0_r); }

    std::string Vector3::ToString() const {
        char buf[100];
        snprintf(buf, sizeof(buf), "(%.3f, %.3f, %.3f)", static_cast<float>(x), static_cast<float>(y),
                 static_cast<float>(z));
        return buf;
    }
}  // namespace ho