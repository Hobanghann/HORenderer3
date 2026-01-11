#pragma once

#include <string>

#include "core/macros.h"
#include "math_defs.h"
#include "math_funcs.h"

namespace ho {
    struct Vector2;
    struct Vector4;

    struct Vector3 {
        static const Vector3 UNIT_X;
        static const Vector3 UNIT_Y;
        static const Vector3 UNIT_Z;
        static const Vector3 ZERO;

        constexpr Vector3();
        constexpr Vector3(real p_x, real p_y, real p_z);
        constexpr Vector3(const Vector3&) = default;
        explicit Vector3(const Vector2& v);
        explicit Vector3(const Vector4& v);
        constexpr Vector3& operator=(const Vector3& rhs);
        ~Vector3() = default;

        constexpr Vector3 operator+(const Vector3& rhs) const;
        constexpr Vector3& operator+=(const Vector3& rhs);

        constexpr Vector3 operator-(const Vector3& rhs) const;
        constexpr Vector3& operator-=(const Vector3& rhs);

        constexpr Vector3 operator*(const Vector3& rhs) const;
        constexpr Vector3& operator*=(const Vector3& rhs);

        constexpr Vector3 operator/(const Vector3& rhs) const;
        constexpr Vector3& operator/=(const Vector3& rhs);

        constexpr Vector3 operator*(real scalar) const;
        constexpr Vector3& operator*=(real scalar);

        constexpr Vector3 operator/(real scalar) const;
        constexpr Vector3& operator/=(real scalar);

        constexpr Vector3 operator-() const;

        constexpr bool operator==(const Vector3& rhs) const;
        constexpr bool operator!=(const Vector3& rhs) const;

        constexpr bool operator<(const Vector3& rhs) const;
        constexpr bool operator<=(const Vector3& rhs) const;
        constexpr bool operator>(const Vector3& rhs) const;
        constexpr bool operator>=(const Vector3& rhs) const;

        constexpr bool IsEqualApprox(const Vector3& rhs) const;
        constexpr bool IsNotEqualApprox(const Vector3& rhs) const;

        constexpr real Dot(const Vector3& rhs) const;

        constexpr Vector3 Cross(const Vector3& rhs) const;

        real Magnitude() const;
        constexpr real SqrdMagnitude() const;

        void Normalize();
        Vector3 Normalized() const;

        constexpr Vector3& Project(const Vector3& onto_v);
        constexpr Vector3 Projected(const Vector3& onto_v) const;

        constexpr bool IsUnit() const;
        constexpr bool IsUnitApprox() const;

        bool IsFinite() const;

        Vector2 ToCartesian() const;

        Vector4 ToHomogeneous() const;

        std::string ToString() const;

        union {
            struct {
                real x;
                real y;
                real z;
            };
            real data[3];
        };
    };

    constexpr Vector3::Vector3() : x(0.0_r), y(0.0_r), z(0.0_r) {}
    constexpr Vector3::Vector3(real p_x, real p_y, real p_z) : x(p_x), y(p_y), z(p_z) {}

    constexpr Vector3& Vector3::operator=(const Vector3& rhs) {
        if (this == &rhs) return *this;
        x = rhs.x;
        y = rhs.y;
        z = rhs.z;
        return *this;
    }

    INLINE constexpr const Vector3 Vector3::UNIT_X = Vector3(1.0_r, 0.0_r, 0.0_r);
    INLINE constexpr const Vector3 Vector3::UNIT_Y = Vector3(0.0_r, 1.0_r, 0.0_r);
    INLINE constexpr const Vector3 Vector3::UNIT_Z = Vector3(0.0_r, 0.0_r, 1.0_r);
    INLINE constexpr const Vector3 Vector3::ZERO = Vector3(0.0_r, 0.0_r, 0.0_r);

    constexpr Vector3 Vector3::operator+(const Vector3& rhs) const { return Vector3(x + rhs.x, y + rhs.y, z + rhs.z); }
    constexpr Vector3& Vector3::operator+=(const Vector3& rhs) {
        x += rhs.x;
        y += rhs.y;
        z += rhs.z;
        return *this;
    }

    constexpr Vector3 Vector3::operator-(const Vector3& rhs) const { return Vector3(x - rhs.x, y - rhs.y, z - rhs.z); }
    constexpr Vector3& Vector3::operator-=(const Vector3& rhs) {
        x -= rhs.x;
        y -= rhs.y;
        z -= rhs.z;
        return *this;
    }

    constexpr Vector3 Vector3::operator*(const Vector3& rhs) const { return Vector3(x * rhs.x, y * rhs.y, z * rhs.z); }
    constexpr Vector3& Vector3::operator*=(const Vector3& rhs) {
        x *= rhs.x;
        y *= rhs.y;
        z *= rhs.z;
        return *this;
    }

    constexpr Vector3 Vector3::operator/(const Vector3& rhs) const { return Vector3(x / rhs.x, y / rhs.y, z / rhs.z); }
    constexpr Vector3& Vector3::operator/=(const Vector3& rhs) {
        x /= rhs.x;
        y /= rhs.y;
        z /= rhs.z;
        return *this;
    }

    constexpr Vector3 Vector3::operator*(real scalar) const { return Vector3(scalar * x, scalar * y, scalar * z); }
    constexpr Vector3& Vector3::operator*=(real scalar) {
        x *= scalar;
        y *= scalar;
        z *= scalar;
        return *this;
    }

    constexpr Vector3 Vector3::operator/(real scalar) const {
        const real inv = 1.0_r / scalar;
        return *this * inv;
    }
    constexpr Vector3& Vector3::operator/=(real scalar) {
        real inv = 1.0_r / scalar;
        *this *= inv;
        return *this;
    }

    constexpr Vector3 Vector3::operator-() const { return Vector3(-x, -y, -z); }

    constexpr Vector3 operator*(real scalar, const Vector3& v) { return v * scalar; }
    constexpr Vector3 operator/(real scalar, const Vector3& vector) {
        return Vector3(scalar / vector.x, scalar / vector.y, scalar / vector.z);
    }

    constexpr bool Vector3::operator==(const Vector3& rhs) const { return x == rhs.x && y == rhs.y && z == rhs.z; }
    constexpr bool Vector3::operator!=(const Vector3& rhs) const { return !(*this == rhs); }

    constexpr bool Vector3::operator<(const Vector3& rhs) const {
        if (x == rhs.x) {
            if (y == rhs.y) {
                return z < rhs.z;
            }
            return y < rhs.y;
        }
        return x < rhs.x;
    }
    constexpr bool Vector3::operator<=(const Vector3& rhs) const {
        if (x == rhs.x) {
            if (y == rhs.y) {
                return z <= rhs.z;
            }
            return y < rhs.y;
        }
        return x < rhs.x;
    }
    constexpr bool Vector3::operator>(const Vector3& rhs) const { return !(*this <= rhs); }
    constexpr bool Vector3::operator>=(const Vector3& rhs) const { return !(*this < rhs); }

    constexpr bool Vector3::IsEqualApprox(const Vector3& rhs) const {
        return math::IsEqualApprox(x, rhs.x) && math::IsEqualApprox(y, rhs.y) && math::IsEqualApprox(z, rhs.z);
    }
    constexpr bool Vector3::IsNotEqualApprox(const Vector3& rhs) const { return !IsEqualApprox(rhs); }

    constexpr real Vector3::Dot(const Vector3& rhs) const { return x * rhs.x + y * rhs.y + z * rhs.z; }

    constexpr Vector3 Vector3::Cross(const Vector3& rhs) const {
        Vector3 ret((y * rhs.z) - (z * rhs.y), (z * rhs.x) - (x * rhs.z), (x * rhs.y) - (y * rhs.x));

        return ret;
    }

    constexpr real Vector3::SqrdMagnitude() const { return this->Dot(*this); }

    constexpr Vector3& Vector3::Project(const Vector3& onto_v) {
        const real sq_mag = onto_v.SqrdMagnitude();

        if (math::IsZeroApprox(sq_mag)) {
            return *this;
        }

        const real dot = this->Dot(onto_v);
        *this = onto_v * (dot / sq_mag);
        return *this;
    }
    constexpr Vector3 Vector3::Projected(const Vector3& onto_v) const {
        Vector3 v = *this;
        return v.Project(onto_v);
    }

    constexpr bool Vector3::IsUnit() const { return SqrdMagnitude() == 1.0_r; }
    constexpr bool Vector3::IsUnitApprox() const {
        return math::IsEqualApprox(SqrdMagnitude(), 1.0_r, math::EPSILON_UNIT);
    }
}  // namespace ho
