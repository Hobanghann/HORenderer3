#include "vector4.h"

#include "math_funcs.h"
#include "quaternion.h"
#include "vector2.h"
#include "vector3.h"

namespace ho {
    Vector4::Vector4(const Vector2& v) : x(v.x), y(v.y), z(0.0_r), w(0.0_r) {}
    Vector4::Vector4(const Vector3& v) : x(v.x), y(v.y), z(v.z), w(0.0_r) {}
    Vector4::Vector4(const Quaternion& q) : x(q.x), y(q.y), z(q.z), w(q.w) {}

    real Vector4::Magnitude() const { return math::Sqrt(SqrdMagnitude()); }

    void Vector4::Normalize() {
        const real inv_mag = math::InvSqrtFast(SqrdMagnitude());
        *this = inv_mag * *this;
    }
    Vector4 Vector4::Normalized() const {
        const real inv_mag = math::InvSqrtFast(SqrdMagnitude());
        return inv_mag * *this;
    }

    bool Vector4::IsFinite() const {
        return math::IsFinite(x) && math::IsFinite(y) && math::IsFinite(z) && math::IsFinite(w);
    }

    Vector3 Vector4::ToCartesian() const {
        const real inv_w = 1.f / w;
        return Vector3(inv_w * x, inv_w * y, inv_w * z);
    }

    std::string Vector4::ToString() const {
        char buf[100];
        snprintf(buf, sizeof(buf), "(%.3f, %.3f, %.3f, %.3f)", static_cast<float>(x), static_cast<float>(y),
                 static_cast<float>(z), static_cast<float>(w));
        return buf;
    }
}  // namespace ho
