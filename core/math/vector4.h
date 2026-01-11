#pragma once

#include <string>

#include "core/macros.h"
#include "math_defs.h"
#include "math_funcs.h"

namespace ho {
    struct Vector2;
    struct Vector3;
    struct Quaternion;

    struct Vector4 {
        static const Vector4 UNIT_X;
        static const Vector4 UNIT_Y;
        static const Vector4 UNIT_Z;
        static const Vector4 UNIT_W;
        static const Vector4 ZERO;

        constexpr Vector4();
        constexpr Vector4(real p_x, real p_y, real p_z, real p_w);
        explicit Vector4(const Vector2& v);
        explicit Vector4(const Vector3& v);
        explicit Vector4(const Quaternion& q);
        constexpr Vector4(const Vector4&) = default;
        constexpr Vector4& operator=(const Vector4& rhs);
        ~Vector4() = default;

        constexpr Vector4 operator+(const Vector4& rhs) const;
        constexpr Vector4& operator+=(const Vector4& rhs);

        constexpr Vector4 operator-(const Vector4& rhs) const;
        constexpr Vector4& operator-=(const Vector4& rhs);

        constexpr Vector4 operator*(const Vector4& rhs) const;
        constexpr Vector4& operator*=(const Vector4& rhs);

        constexpr Vector4 operator/(const Vector4& rhs) const;
        constexpr Vector4& operator/=(const Vector4& rhs);

        constexpr Vector4 operator*(real scalar) const;
        constexpr Vector4& operator*=(real scalar);

        constexpr Vector4 operator/(real scalar) const;
        constexpr Vector4& operator/=(real scalar);

        constexpr Vector4 operator-() const;

        constexpr bool operator==(const Vector4& rhs) const;
        constexpr bool operator!=(const Vector4& rhs) const;

        constexpr bool operator<(const Vector4& rhs) const;
        constexpr bool operator<=(const Vector4& rhs) const;
        constexpr bool operator>(const Vector4& rhs) const;
        constexpr bool operator>=(const Vector4& rhs) const;

        constexpr bool IsEqualApprox(const Vector4& rhs) const;
        constexpr bool IsNotEqualApprox(const Vector4& rhs) const;

        constexpr real Dot(const Vector4& rhs) const;

        real Magnitude() const;
        constexpr real SqrdMagnitude() const;

        void Normalize();
        Vector4 Normalized() const;

        constexpr Vector4& Project(const Vector4& onto_v);
        constexpr Vector4 Projected(const Vector4& onto_v) const;

        constexpr bool IsUnit() const;
        constexpr bool IsUnitApprox() const;

        bool IsFinite() const;

        Vector3 ToCartesian() const;

        std::string ToString() const;

        union {
            struct {
                real x;
                real y;
                real z;
                real w;
            };
            real data[4];
        };
    };

    constexpr Vector4::Vector4() : x(0.0_r), y(0.0_r), z(0.0_r), w(0.0_r) {}
    constexpr Vector4::Vector4(real p_x, real p_y, real p_z, real p_w) : x(p_x), y(p_y), z(p_z), w(p_w) {}

    INLINE constexpr Vector4 Vector4::UNIT_X = Vector4(1.0_r, 0.0_r, 0.0_r, 0.0_r);
    INLINE constexpr Vector4 Vector4::UNIT_Y = Vector4(0.0_r, 1.0_r, 0.0_r, 0.0_r);
    INLINE constexpr Vector4 Vector4::UNIT_Z = Vector4(0.0_r, 0.0_r, 1.0_r, 0.0_r);
    INLINE constexpr Vector4 Vector4::UNIT_W = Vector4(0.0_r, 0.0_r, 0.0_r, 1.0_r);
    INLINE constexpr Vector4 Vector4::ZERO = Vector4(0.0_r, 0.0_r, 0.0_r, 0.0_r);

    constexpr Vector4& Vector4::operator=(const Vector4& rhs) {
        if (this == &rhs) return *this;
        x = rhs.x;
        y = rhs.y;
        z = rhs.z;
        w = rhs.w;
        return *this;
    }

    constexpr Vector4 Vector4::operator+(const Vector4& rhs) const {
        return Vector4(x + rhs.x, y + rhs.y, z + rhs.z, w + rhs.w);
    }
    constexpr Vector4& Vector4::operator+=(const Vector4& rhs) {
        x += rhs.x;
        y += rhs.y;
        z += rhs.z;
        w += rhs.w;
        return *this;
    }

    constexpr Vector4 Vector4::operator-(const Vector4& rhs) const {
        return Vector4(x - rhs.x, y - rhs.y, z - rhs.z, w - rhs.w);
    }
    constexpr Vector4& Vector4::operator-=(const Vector4& rhs) {
        x -= rhs.x;
        y -= rhs.y;
        z -= rhs.z;
        w -= rhs.w;
        return *this;
    }

    constexpr Vector4 Vector4::operator*(const Vector4& rhs) const {
        return Vector4(x * rhs.x, y * rhs.y, z * rhs.z, w * rhs.w);
    }
    constexpr Vector4& Vector4::operator*=(const Vector4& rhs) {
        x *= rhs.x;
        y *= rhs.y;
        z *= rhs.z;
        w *= rhs.w;
        return *this;
    }

    constexpr Vector4 Vector4::operator/(const Vector4& rhs) const {
        return Vector4(x / rhs.x, y / rhs.y, z / rhs.z, w / rhs.w);
    }
    constexpr Vector4& Vector4::operator/=(const Vector4& rhs) {
        x /= rhs.x;
        y /= rhs.y;
        z /= rhs.z;
        w /= rhs.w;
        return *this;
    }

    constexpr Vector4 Vector4::operator*(real scalar) const {
        return Vector4(x * scalar, y * scalar, z * scalar, w * scalar);
    }
    constexpr Vector4& Vector4::operator*=(real scalar) {
        x *= scalar;
        y *= scalar;
        z *= scalar;
        w *= scalar;
        return *this;
    }

    constexpr Vector4 Vector4::operator/(real scalar) const {
        const real inv = 1.0_r / scalar;
        return *this * inv;
    }
    constexpr Vector4& Vector4::operator/=(real scalar) {
        const real inv = 1.0_r / scalar;
        *this *= inv;
        return *this;
    }

    constexpr Vector4 Vector4::operator-() const { return Vector4(-x, -y, -z, -w); }

    ALWAYS_INLINE constexpr Vector4 operator*(real scalar, const Vector4& vector) { return vector * scalar; }
    constexpr Vector4 operator/(real scalar, const Vector4& vector) {
        return Vector4(scalar / vector.x, scalar / vector.y, scalar / vector.z, scalar / vector.w);
    }

    constexpr bool Vector4::operator==(const Vector4& rhs) const {
        return x == rhs.x && y == rhs.y && z == rhs.z && w == rhs.w;
    }
    constexpr bool Vector4::operator!=(const Vector4& rhs) const { return !(*this == rhs); }

    constexpr bool Vector4::operator<(const Vector4& rhs) const {
        if (x == rhs.x) {
            if (y == rhs.y) {
                if (z == rhs.z) {
                    return w < rhs.w;
                }
                return z < rhs.z;
            }
            return y < rhs.y;
        }
        return x < rhs.x;
    }
    constexpr bool Vector4::operator<=(const Vector4& rhs) const {
        if (x == rhs.x) {
            if (y == rhs.y) {
                if (z == rhs.z) {
                    return w <= rhs.w;
                }
                return z < rhs.z;
            }
            return y < rhs.y;
        }
        return x < rhs.x;
    }
    constexpr bool Vector4::operator>(const Vector4& rhs) const { return !(*this <= rhs); }
    constexpr bool Vector4::operator>=(const Vector4& rhs) const { return !(*this < rhs); }

    constexpr bool Vector4::IsEqualApprox(const Vector4& rhs) const {
        return math::IsEqualApprox(x, rhs.x) && math::IsEqualApprox(y, rhs.y) && math::IsEqualApprox(z, rhs.z) &&
               math::IsEqualApprox(w, rhs.w);
    }
    constexpr bool Vector4::IsNotEqualApprox(const Vector4& rhs) const { return !IsEqualApprox(rhs); }

    constexpr real ho::Vector4::Dot(const Vector4& rhs) const { return x * rhs.x + y * rhs.y + z * rhs.z + w * rhs.w; }

    constexpr real Vector4::SqrdMagnitude() const { return this->Dot(*this); }

    constexpr bool Vector4::IsUnit() const { return SqrdMagnitude() == 1.0_r; }
    constexpr bool Vector4::IsUnitApprox() const {
        return math::IsEqualApprox(SqrdMagnitude(), 1.0_r, math::EPSILON_UNIT);
    }

    constexpr Vector4& Vector4::Project(const Vector4& onto_v) {
        const real sq_mag = onto_v.SqrdMagnitude();

        if (math::IsZeroApprox(sq_mag)) {
            return *this;
        }

        const real dot = this->Dot(onto_v);
        *this = onto_v * (dot / sq_mag);
        return *this;
    }
    constexpr Vector4 Vector4::Projected(const Vector4& onto_v) const {
        Vector4 v = *this;
        return v.Project(onto_v);
    }

}  // namespace ho
