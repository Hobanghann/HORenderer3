#pragma once

#include <string>

#include "vector3.h"

namespace ho {
    struct Matrix3x3;
    struct Quaternion {
       public:
        static Quaternion FromAxisAngle(const Vector3& axis, real angle);
        static Quaternion FromEuler(real x, real y, real z, math::EulerOrder order = math::YXZ);
        static Quaternion FromMatrix(const Matrix3x3& m);

        constexpr Quaternion();
        constexpr Quaternion(const Quaternion& q) = default;
        constexpr Quaternion(real p_x, real p_y, real p_z, real p_w);
        constexpr Quaternion(const Vector3& vector_part, real scalar_part = 0.0_r);
        constexpr Quaternion& operator=(const Quaternion& rhs);
        ~Quaternion() = default;

        Vector3 axis() const;
        real angle() const;

        constexpr Quaternion operator+(const Quaternion& rhs) const;
        constexpr Quaternion& operator+=(const Quaternion& rhs);

        constexpr Quaternion operator-(const Quaternion& rhs) const;
        constexpr Quaternion& operator-=(const Quaternion& rhs);

        constexpr Quaternion operator*(const Quaternion& rhs) const;
        constexpr Quaternion& operator*=(const Quaternion& rhs);

        constexpr Quaternion operator*(real p_scalar) const;
        constexpr Quaternion& operator*=(real p_scalar);

        constexpr Quaternion operator/(real p_scalar) const;
        constexpr Quaternion& operator/=(real p_scalar);

        constexpr Quaternion operator-() const;

        constexpr bool operator==(const Quaternion& rhs) const;
        constexpr bool operator!=(const Quaternion& rhs) const;

        constexpr bool IsEqualApprox(const Quaternion& rhs) const;
        constexpr bool IsNotEqualApprox(const Quaternion& rhs) const;

        constexpr real Dot(const Quaternion& rhs) const;

        real Magnitude() const;
        constexpr real SqrdMagnitude() const;

        Quaternion& Normalize();
        Quaternion Normalized() const;

        constexpr bool IsUnit() const;
        constexpr bool IsUnitApprox() const;

        constexpr Quaternion& Conjugate();
        constexpr Quaternion Conjugated() const;

        constexpr Quaternion& Invert();
        constexpr Quaternion Inverse() const;

        constexpr bool IsPurelyImaginary() const;

        constexpr Vector3 Transform(const Vector3& v) const;
        constexpr Vector3 InvTransform(const Vector3& v) const;

        std::string ToString() const;

        union {
            struct {
                real x;
                real y;
                real z;
                real w;
            };

            struct {
                Vector3 vector;
                real scalar;
            };

            real data[4];
        };
    };

    constexpr Quaternion::Quaternion() : x(0.0_r), y(0.0_r), z(0.0_r), w(1.0_r) {}
    constexpr Quaternion::Quaternion(real p_x, real p_y, real p_z, real p_w) : x(p_x), y(p_y), z(p_z), w(p_w) {}
    constexpr Quaternion::Quaternion(const Vector3& vector_part, real scalar_part)
        : vector(vector_part), scalar(scalar_part) {}
    constexpr Quaternion& Quaternion::operator=(const Quaternion& rhs) {
        x = rhs.x;
        y = rhs.y;
        z = rhs.z;
        w = rhs.w;
        return *this;
    }

    constexpr Quaternion Quaternion::operator+(const Quaternion& rhs) const {
        return Quaternion(x + rhs.x, y + rhs.y, z + rhs.z, w + rhs.w);
    }
    constexpr Quaternion& Quaternion::operator+=(const Quaternion& rhs) {
        x += rhs.x;
        y += rhs.y;
        z += rhs.z;
        w += rhs.w;
        return *this;
    }

    constexpr Quaternion Quaternion::operator-(const Quaternion& rhs) const {
        return Quaternion(x - rhs.x, y - rhs.y, z - rhs.z, w - rhs.w);
    }
    constexpr Quaternion& Quaternion::operator-=(const Quaternion& rhs) {
        x -= rhs.x;
        y -= rhs.y;
        z -= rhs.z;
        w -= rhs.w;
        return *this;
    }

    constexpr Quaternion Quaternion::operator*(const Quaternion& rhs) const {
        const real l_x = w * rhs.x + x * rhs.w + y * rhs.z - z * rhs.y;
        const real l_y = w * rhs.y + y * rhs.w + z * rhs.x - x * rhs.z;
        const real l_z = w * rhs.z + z * rhs.w + x * rhs.y - y * rhs.x;
        const real l_w = w * rhs.w - x * rhs.x - y * rhs.y - z * rhs.z;
        return Quaternion(l_x, l_y, l_z, l_w);
    }
    constexpr Quaternion& Quaternion::operator*=(const Quaternion& rhs) {
        *this = *this * rhs;
        return *this;
    }

    constexpr Quaternion Quaternion::operator*(real p_scalar) const {
        return Quaternion(p_scalar * x, p_scalar * y, p_scalar * z, p_scalar * w);
    }
    constexpr Quaternion& Quaternion::operator*=(real p_scalar) {
        x *= p_scalar;
        y *= p_scalar;
        z *= p_scalar;
        w *= p_scalar;
        return *this;
    }

    constexpr Quaternion Quaternion::operator/(real p_scalar) const {
        const real inv_s = 1.0_r / p_scalar;
        return *this * inv_s;
    }
    constexpr Quaternion& Quaternion::operator/=(real p_scalar) {
        const real inv_s = 1.0_r / p_scalar;
        x *= inv_s;
        y *= inv_s;
        z *= inv_s;
        w *= inv_s;
        return *this;
    }

    constexpr Quaternion Quaternion::operator-() const { return Quaternion(-x, -y, -z, -w); }

    constexpr Quaternion operator*(real p_scalar, const Quaternion& q) { return q * p_scalar; }

    constexpr bool Quaternion::operator==(const Quaternion& rhs) const {
        return x == rhs.x && y == rhs.y && z == rhs.z && w == rhs.w;
    }
    constexpr bool Quaternion::operator!=(const Quaternion& rhs) const { return !(*this == rhs); }

    constexpr bool Quaternion::IsEqualApprox(const Quaternion& rhs) const {
        return math::IsEqualApprox(x, rhs.x) && math::IsEqualApprox(y, rhs.y) && math::IsEqualApprox(z, rhs.z) &&
               math::IsEqualApprox(w, rhs.w);
    }
    constexpr bool Quaternion::IsNotEqualApprox(const Quaternion& rhs) const { return !IsEqualApprox(rhs); }

    constexpr real Quaternion::Dot(const Quaternion& rhs) const {
        return x * rhs.x + y * rhs.y + z * rhs.z + w * rhs.w;
    }

    constexpr real Quaternion::SqrdMagnitude() const { return x * x + y * y + z * z + w * w; }

    constexpr Quaternion& Quaternion::Conjugate() {
        x = -x;
        y = -y;
        z = -z;
        return *this;
    }
    constexpr Quaternion Quaternion::Conjugated() const { return Quaternion(-x, -y, -z, w); }

    constexpr bool Quaternion::IsUnit() const { return SqrdMagnitude() == 1.0_r; }
    constexpr bool Quaternion::IsUnitApprox() const { return math::IsEqualApprox(SqrdMagnitude(), 1.0_r); }
    constexpr Quaternion& Quaternion::Invert() {
        Conjugate();
        const real inv_mag = 1.0_r / SqrdMagnitude();
        x *= inv_mag;
        y *= inv_mag;
        z *= inv_mag;
        w *= inv_mag;
        return *this;
    }
    constexpr Quaternion Quaternion::Inverse() const {
        Quaternion copy = *this;
        return copy.Invert();
    }

    constexpr bool Quaternion::IsPurelyImaginary() const { return math::IsZeroApprox(w); }

    constexpr Vector3 Quaternion::Transform(const Vector3& v) const {
        const Vector3 u(x, y, z);
        const Vector3 uv = u.Cross(v);
        return v + ((uv * w) + u.Cross(uv)) * 2.0_r;
    }
    constexpr Vector3 Quaternion::InvTransform(const Vector3& v) const { return Conjugated().Transform(v); }
}  // namespace ho
