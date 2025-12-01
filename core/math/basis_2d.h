#pragma once

#include <cassert>
#include <string>

#include "math_defs.h"
#include "matrix2x2.h"

namespace ho {
    struct Basis2D {
        static Basis2D FromAngle(real angle);

        constexpr Basis2D();
        constexpr Basis2D(const Basis2D&) = default;
        constexpr Basis2D(const Vector2& basis_x, const Vector2& basis_y);
        constexpr Basis2D(real scale_x, real scale_y, const Vector2& basis_x, const Vector2& basis_y);
        constexpr explicit Basis2D(const Matrix2x2& m);
        constexpr Basis2D& operator=(const Basis2D& rhs);
        ~Basis2D() = default;

        ALWAYS_INLINE real scale_x() const;
        ALWAYS_INLINE real scale_y() const;
        constexpr Vector2 basis_x() const;
        constexpr Vector2 basis_y() const;

        constexpr void set_basis_x(const Vector2& basis_x);
        constexpr void set_basis_y(const Vector2& basis_y);

        constexpr bool operator==(const Basis2D& rhs) const;
        constexpr bool operator!=(const Basis2D& rhs) const;

        constexpr bool IsEqualApprox(const Basis2D& rhs) const;
        constexpr bool IsNotEqualApprox(const Basis2D& rhs) const;

        ALWAYS_INLINE constexpr bool IsOrthogonal() const;
        ALWAYS_INLINE Basis2D& Orthogonalize();
        ALWAYS_INLINE Basis2D Orthogonalized() const;

        ALWAYS_INLINE constexpr bool IsOrthonormal() const;
        ALWAYS_INLINE Basis2D& Orthonormalize();
        ALWAYS_INLINE Basis2D Orthonormalized() const;

        constexpr Basis2D& Scale(const Vector2& scale);
        constexpr Basis2D Scaled(const Vector2& scale) const;

        constexpr Basis2D& ScaleLocal(const Vector2& scale);
        constexpr Basis2D ScaledLocal(const Vector2& scale) const;

        constexpr Basis2D& ScaleUniform(real scale);
        constexpr Basis2D ScaledUniform(real scale) const;

        constexpr Basis2D& ScaleUniformLocal(real scale);
        constexpr Basis2D ScaledUniformLocal(real scale) const;

        Basis2D& Rotate(real angle);
        Basis2D Rotated(real angle) const;

        Basis2D& RotateLocal(real angle);
        Basis2D RotatedLocal(real angle) const;

        constexpr Basis2D& Invert();
        constexpr Basis2D Inverse() const;

        constexpr Basis2D& InvertFast();
        constexpr Basis2D InverseFast() const;

        Basis2D& LookAt(const Vector2& at);
        Basis2D LookedAt(const Vector2& at) const;

        constexpr Basis2D& operator*=(const Basis2D& rhs);
        constexpr Basis2D operator*(const Basis2D& rhs) const;

        constexpr Vector2 Transform(const Vector2& v) const;
        constexpr Vector2 InvTransform(const Vector2& v) const;
        constexpr Vector2 InvTransformFast(const Vector2& v) const;

        std::string ToString() const;

        union {
            struct {
                Vector2 row0;
                Vector2 row1;
            };
            Matrix2x2 matrix;
        };
    };

    constexpr Basis2D::Basis2D() : row0(Vector2::UNIT_X), row1(Vector2::UNIT_Y) {}
    constexpr Basis2D::Basis2D(const Vector2& basis_x, const Vector2& basis_y)
        : row0(basis_x.x, basis_y.x), row1(basis_x.y, basis_y.y) {}
    constexpr Basis2D::Basis2D(real scale_x, real scale_y, const Vector2& basis_x, const Vector2& basis_y)
        : row0(scale_x * basis_x.x, scale_y * basis_y.x), row1(scale_x * basis_x.y, scale_y * basis_y.y) {}
    constexpr Basis2D::Basis2D(const Matrix2x2& m) : matrix(m) {}

    constexpr Basis2D& Basis2D::operator=(const Basis2D& rhs) {
        if (this == &rhs) {
            return *this;
        }
        matrix = rhs.matrix;
        return *this;
    }

    real Basis2D::scale_x() const { return basis_x().Magnitude(); }
    real Basis2D::scale_y() const { return basis_y().Magnitude(); }
    constexpr Vector2 Basis2D::basis_x() const { return Vector2(row0.x, row1.x); }
    constexpr Vector2 Basis2D::basis_y() const { return Vector2(row0.y, row1.y); }

    constexpr void Basis2D::set_basis_x(const Vector2& basis_x) {
        row0.x = basis_x.x;
        row1.x = basis_x.y;
    }
    constexpr void Basis2D::set_basis_y(const Vector2& basis_y) {
        row0.y = basis_y.x;
        row1.y = basis_y.y;
    }

    constexpr bool Basis2D::operator==(const Basis2D& rhs) const { return matrix == rhs.matrix; }
    constexpr bool Basis2D::operator!=(const Basis2D& rhs) const { return !(*this == rhs); }

    constexpr bool Basis2D::IsEqualApprox(const Basis2D& rhs) const { return matrix.IsEqualApprox(rhs.matrix); }

    constexpr bool Basis2D::IsNotEqualApprox(const Basis2D& rhs) const { return !IsEqualApprox(rhs); }

    constexpr bool Basis2D::IsOrthogonal() const { return matrix.IsOrthogonal(); }
    Basis2D& Basis2D::Orthogonalize() {
        matrix.Orthogonalize();
        return *this;
    }
    Basis2D Basis2D::Orthogonalized() const {
        Basis2D copy = *this;
        return copy.Orthogonalize();
    }

    constexpr bool Basis2D::IsOrthonormal() const { return matrix.IsOrthonormal(); }
    Basis2D& Basis2D::Orthonormalize() {
        matrix.Orthonormalize();
        return *this;
    }
    Basis2D Basis2D::Orthonormalized() const {
        Basis2D copy = *this;
        return copy.Orthonormalize();
    }

    constexpr Basis2D& Basis2D::Scale(const Vector2& scale) {
        row0 *= scale.x;
        row1 *= scale.y;
        return *this;
    }
    constexpr Basis2D Basis2D::Scaled(const Vector2& scale) const {
        Basis2D copy = *this;
        return copy.Scale(scale);
    }

    constexpr Basis2D& Basis2D::ScaleLocal(const Vector2& scale) {
        row0 *= scale;
        row1 *= scale;
        return *this;
    }
    constexpr Basis2D Basis2D::ScaledLocal(const Vector2& scale) const {
        Basis2D copy = *this;
        return copy.ScaleLocal(scale);
    }

    constexpr Basis2D& Basis2D::ScaleUniform(real scale) {
        row0 *= scale;
        row1 *= scale;
        return *this;
    }
    constexpr Basis2D Basis2D::ScaledUniform(real scale) const {
        Basis2D copy = *this;
        return copy.ScaleUniform(scale);
    }

    constexpr Basis2D& Basis2D::ScaleUniformLocal(real scale) {
        Vector2 s(scale, scale);
        row0 *= scale;
        row1 *= scale;
        return *this;
    }
    constexpr Basis2D Basis2D::ScaledUniformLocal(real scale) const {
        Basis2D copy = *this;
        return copy.ScaleUniformLocal(scale);
    }

    constexpr Basis2D& Basis2D::Invert() {
        matrix.Invert();
        return *this;
    }
    constexpr Basis2D Basis2D::Inverse() const {
        Basis2D b = *this;
        return b.Invert();
    }

    constexpr Basis2D& Basis2D::InvertFast() {
        matrix.Transpose();
        return *this;
    }
    constexpr Basis2D Basis2D::InverseFast() const {
        Basis2D b = *this;
        return b.InvertFast();
    }

    constexpr Basis2D& Basis2D::operator*=(const Basis2D& rhs) {
        matrix *= rhs.matrix;
        return *this;
    }

    constexpr Basis2D Basis2D::operator*(const Basis2D& rhs) const {
        Basis2D result = *this;
        result *= rhs;
        return result;
    }

    constexpr Vector2 Basis2D::Transform(const Vector2& v) const { return matrix * v; }
    constexpr Vector2 Basis2D::InvTransform(const Vector2& v) const { return Inverse().Transform(v); }
    constexpr Vector2 Basis2D::InvTransformFast(const Vector2& v) const { return InverseFast().Transform(v); }
}  // namespace ho