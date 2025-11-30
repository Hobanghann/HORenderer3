#pragma once

#include <string>

#include "core/macros.h"
#include "math_defs.h"
#include "math_funcs.h"

namespace ho {

    struct Vector3;
    struct Vector4;

    struct Vector2 {
        static const Vector2 UNIT_X;
        static const Vector2 UNIT_Y;
        static const Vector2 ZERO;

        constexpr Vector2();
        constexpr Vector2(real x, real y);
        explicit Vector2(const Vector3& v);
        explicit Vector2(const Vector4& v);
        constexpr Vector2(const Vector2&) = default;
        constexpr Vector2& operator=(const Vector2& rhs);
        ~Vector2() = default;

        constexpr Vector2 operator+(const Vector2& rhs) const;
        constexpr Vector2& operator+=(const Vector2& rhs);

        constexpr Vector2 operator-(const Vector2& rhs) const;
        constexpr Vector2& operator-=(const Vector2& rhs);

        constexpr Vector2 operator*(const Vector2& rhs) const;
        constexpr Vector2& operator*=(const Vector2& rhs);

        constexpr Vector2 operator/(const Vector2& rhs) const;
        constexpr Vector2& operator/=(const Vector2& rhs);

        constexpr Vector2 operator*(real scalar) const;
        constexpr Vector2& operator*=(real scalar);

        constexpr Vector2 operator/(real scalar) const;
        constexpr Vector2& operator/=(real scalar);

        constexpr Vector2 operator-() const;

        constexpr bool operator==(const Vector2& rhs) const;
        constexpr bool operator!=(const Vector2& rhs) const;

        constexpr bool operator<(const Vector2& rhs) const;
        constexpr bool operator<=(const Vector2& rhs) const;
        constexpr bool operator>(const Vector2& rhs) const;
        constexpr bool operator>=(const Vector2& rhs) const;

        constexpr bool IsEqualApprox(const Vector2& rhs) const;
        constexpr bool IsNotEqualApprox(const Vector2& rhs) const;

        constexpr real Dot(const Vector2& rhs) const;

        constexpr real Cross(const Vector2& rhs) const;

        real Magnitude() const;
        constexpr real SqrdMagnitude() const;

        Vector2& Normalize();
        Vector2 Normalized() const;

        constexpr Vector2& Project(const Vector2& onto_v);
        constexpr Vector2 Projected(const Vector2& onto_v) const;

        constexpr bool IsUnit() const;
        constexpr bool IsUnitApprox() const;

        bool IsFinite() const;

        Vector3 ToHomogeneous() const;

        std::string ToString() const;

        union {
            struct {
                real x;
                real y;
            };
            real data[2];
        };
    };

    constexpr Vector2::Vector2() : x(0.0_r), y(0.0_r) {}
    constexpr Vector2::Vector2(real p_x, real p_y) : x(p_x), y(p_y) {}

    _INLINE_ constexpr Vector2 Vector2::UNIT_X = Vector2(1.0_r, 0.0_r);
    _INLINE_ constexpr Vector2 Vector2::UNIT_Y = Vector2(0.0_r, 1.0_r);
    _INLINE_ constexpr Vector2 Vector2::ZERO = Vector2(0.0_r, 0.0_r);

    constexpr Vector2& Vector2::operator=(const Vector2& rhs) {
        if (this == &rhs) return *this;
        x = rhs.x;
        y = rhs.y;
        return *this;
    }

    constexpr Vector2 Vector2::operator+(const Vector2& rhs) const { return Vector2(x + rhs.x, y + rhs.y); }
    constexpr Vector2& Vector2::operator+=(const Vector2& rhs) {
        x += rhs.x;
        y += rhs.y;
        return *this;
    }

    constexpr Vector2 Vector2::operator-(const Vector2& rhs) const { return Vector2(x - rhs.x, y - rhs.y); }
    constexpr Vector2& Vector2::operator-=(const Vector2& rhs) {
        x -= rhs.x;
        y -= rhs.y;
        return *this;
    }

    constexpr Vector2 Vector2::operator*(const Vector2& rhs) const { return Vector2(x * rhs.x, y * rhs.y); }
    constexpr Vector2& Vector2::operator*=(const Vector2& rhs) {
        x *= rhs.x;
        y *= rhs.y;
        return *this;
    }

    constexpr Vector2 Vector2::operator/(const Vector2& rhs) const { return Vector2(x / rhs.x, y / rhs.y); }
    constexpr Vector2& Vector2::operator/=(const Vector2& rhs) {
        x /= rhs.x;
        y /= rhs.y;
        return *this;
    }

    constexpr Vector2 Vector2::operator*(real scalar) const { return Vector2(scalar * x, scalar * y); }
    constexpr Vector2& Vector2::operator*=(real scalar) {
        x *= scalar;
        y *= scalar;
        return *this;
    }

    constexpr Vector2 Vector2::operator/(real scalar) const {
        const real inv = 1.0_r / scalar;
        return *this * inv;
    }
    constexpr Vector2& Vector2::operator/=(real scalar) {
        const real inv = 1.0_r / scalar;
        *this *= inv;
        return *this;
    }

    constexpr Vector2 Vector2::operator-() const { return Vector2(-x, -y); }

    constexpr bool Vector2::operator==(const Vector2& rhs) const { return x == rhs.x && y == rhs.y; }
    constexpr bool Vector2::operator!=(const Vector2& vector) const { return !(*this == vector); }

    constexpr bool Vector2::operator<(const Vector2& rhs) const { return x == rhs.x ? (y < rhs.y) : (x < rhs.x); }
    constexpr bool Vector2::operator<=(const Vector2& rhs) const { return x == rhs.x ? (y <= rhs.y) : (x < rhs.x); }
    constexpr bool Vector2::operator>(const Vector2& rhs) const { return !(*this <= rhs); }
    constexpr bool Vector2::operator>=(const Vector2& rhs) const { return !(*this < rhs); }

    constexpr bool Vector2::IsEqualApprox(const Vector2& rhs) const {
        return math::IsEqualApprox(x, rhs.x) && math::IsEqualApprox(y, rhs.y);
    }
    constexpr bool Vector2::IsNotEqualApprox(const Vector2& rhs) const { return !IsEqualApprox(rhs); }

    constexpr Vector2 operator*(real scalar, const Vector2& vector) { return vector * scalar; }

    constexpr real Vector2::Dot(const Vector2& rhs) const { return x * rhs.x + y * rhs.y; }

    constexpr real Vector2::Cross(const Vector2& rhs) const { return x * rhs.y - y * rhs.x; }

    constexpr real Vector2::SqrdMagnitude() const { return this->Dot(*this); }

    constexpr Vector2& Vector2::Project(const Vector2& onto_v) {
        const real sq_mag = onto_v.SqrdMagnitude();

        if (math::IsZeroApprox(sq_mag)) {
            return *this;
        }

        const real dot = this->Dot(onto_v);
        *this = onto_v * (dot / sq_mag);
        return *this;
    }
    constexpr Vector2 Vector2::Projected(const Vector2& onto_v) const {
        Vector2 v = *this;
        return v.Project(onto_v);
    }

    constexpr bool Vector2::IsUnit() const { return SqrdMagnitude() == 1.0_r; }
    constexpr bool Vector2::IsUnitApprox() const {
        return math::IsEqualApprox(SqrdMagnitude(), 1.0_r, math::EPSILON_UNIT);
    }

}  // namespace ho
