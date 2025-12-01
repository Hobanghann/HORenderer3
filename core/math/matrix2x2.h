#pragma once

#include <string>
#include <vector>

#include "vector2.h"

namespace ho {
    struct Matrix2x2 {
        static const Matrix2x2 IDENTITY;

        constexpr Matrix2x2();
        constexpr Matrix2x2(const Vector2& p_row0, const Vector2& p_row1);
        constexpr Matrix2x2(const Matrix2x2&) = default;
        constexpr Matrix2x2& operator=(const Matrix2x2& rhs);
        ~Matrix2x2() = default;

        constexpr Vector2 col0() const;
        constexpr Vector2 col1() const;
        constexpr void set_col0(const Vector2& col);
        constexpr void set_col1(const Vector2& col);

        constexpr Matrix2x2 operator+(const Matrix2x2& rhs) const;
        constexpr Matrix2x2& operator+=(const Matrix2x2& rhs);

        constexpr Matrix2x2 operator-(const Matrix2x2& rhs) const;
        constexpr Matrix2x2& operator-=(const Matrix2x2& rhs);

        constexpr Matrix2x2 operator*(const Matrix2x2& rhs) const;
        constexpr Matrix2x2& operator*=(const Matrix2x2& rhs);

        constexpr Matrix2x2 operator*(real scalar) const;
        constexpr Matrix2x2& operator*=(real scalar);

        constexpr Matrix2x2 operator/(real scalar) const;
        constexpr Matrix2x2& operator/=(real scalar);

        constexpr bool operator==(const Matrix2x2& rhs) const;
        constexpr bool operator!=(const Matrix2x2& rhs) const;

        constexpr bool IsEqualApprox(const Matrix2x2& rhs) const;
        constexpr bool IsNotEqualApprox(const Matrix2x2& rhs) const;

        constexpr bool IsOrthogonal() const;
        ALWAYS_INLINE Matrix2x2& Orthogonalize();
        ALWAYS_INLINE Matrix2x2 Orthogonalized() const;

        constexpr bool IsOrthonormal() const;
        ALWAYS_INLINE Matrix2x2& Orthonormalize();
        ALWAYS_INLINE Matrix2x2 Orthonormalized() const;

        constexpr Matrix2x2& Transpose();
        constexpr Matrix2x2 Transposed() const;

        constexpr Matrix2x2& Invert();
        constexpr Matrix2x2 Inverse() const;

        constexpr real Trace() const;

        constexpr real Determinant() const;

        std::string ToString() const;

        union {
            struct {
                Vector2 row0;
                Vector2 row1;
            };
            real data[2][2];
        };
    };

    constexpr Matrix2x2::Matrix2x2() : row0(Vector2()), row1(Vector2()) {}
    constexpr Matrix2x2::Matrix2x2(const Vector2& p_row0, const Vector2& p_row1) : row0(p_row0), row1(p_row1) {}

    INLINE constexpr Matrix2x2 Matrix2x2::IDENTITY = Matrix2x2(Vector2(1.0_r, 0.0_r), Vector2(0.0_r, 1.0_r));

    constexpr Matrix2x2& Matrix2x2::operator=(const Matrix2x2& rhs) {
        if (this == &rhs) {
            return *this;
        }
        row0 = rhs.row0;
        row1 = rhs.row1;
        return *this;
    }
    constexpr Vector2 Matrix2x2::col0() const { return Vector2(row0.x, row1.x); }
    constexpr Vector2 Matrix2x2::col1() const { return Vector2(row0.y, row1.y); }
    constexpr void Matrix2x2::set_col0(const Vector2& col) {
        row0.x = col.x;
        row1.x = col.y;
    }
    constexpr void Matrix2x2::set_col1(const Vector2& col) {
        row0.y = col.x;
        row1.y = col.y;
    }

    constexpr Matrix2x2 Matrix2x2::operator+(const Matrix2x2& rhs) const {
        return Matrix2x2(row0 + rhs.row0, row1 + rhs.row1);
    }
    constexpr Matrix2x2& Matrix2x2::operator+=(const Matrix2x2& rhs) {
        row0 += rhs.row0;
        row1 += rhs.row1;
        return *this;
    }

    constexpr Matrix2x2 Matrix2x2::operator-(const Matrix2x2& rhs) const {
        return Matrix2x2(row0 - rhs.row0, row1 - rhs.row1);
    }
    constexpr Matrix2x2& Matrix2x2::operator-=(const Matrix2x2& rhs) {
        row0 -= rhs.row0;
        row1 -= rhs.row1;
        return *this;
    }

    constexpr Matrix2x2 Matrix2x2::operator*(const Matrix2x2& rhs) const {
        const Vector2 rhs_c1 = rhs.col0();
        const Vector2 rhs_c2 = rhs.col1();

        return Matrix2x2(Vector2(row0.Dot(rhs_c1), row0.Dot(rhs_c2)), Vector2(row1.Dot(rhs_c1), row1.Dot(rhs_c2)));
    }
    constexpr Matrix2x2& Matrix2x2::operator*=(const Matrix2x2& rhs) {
        const Vector2 rhs_c1 = rhs.col0();
        const Vector2 rhs_c2 = rhs.col1();

        const Vector2 new_r1(row0.Dot(rhs_c1), row0.Dot(rhs_c2));
        const Vector2 new_r2(row1.Dot(rhs_c1), row1.Dot(rhs_c2));

        row0 = new_r1;
        row1 = new_r2;
        return *this;
    }

    constexpr Matrix2x2 Matrix2x2::operator*(real scalar) const { return Matrix2x2(scalar * row0, scalar * row1); }
    constexpr Matrix2x2& Matrix2x2::operator*=(real scalar) {
        row0 *= scalar;
        row1 *= scalar;
        return *this;
    }

    constexpr Matrix2x2 Matrix2x2::operator/(real scalar) const { return Matrix2x2(row0 / scalar, row1 / scalar); }
    constexpr Matrix2x2& Matrix2x2::operator/=(real scalar) {
        row0 /= scalar;
        row1 /= scalar;
        return *this;
    }

    constexpr bool Matrix2x2::operator==(const Matrix2x2& rhs) const { return row0 == rhs.row0 && row1 == rhs.row1; }
    constexpr bool Matrix2x2::operator!=(const Matrix2x2& rhs) const { return !(*this == rhs); }

    constexpr bool Matrix2x2::IsEqualApprox(const Matrix2x2& rhs) const {
        return row0.IsEqualApprox(rhs.row0) && row1.IsEqualApprox(rhs.row1);
    }
    constexpr bool Matrix2x2::IsNotEqualApprox(const Matrix2x2& rhs) const { return !IsEqualApprox(rhs); }

    constexpr bool Matrix2x2::IsOrthogonal() const {
        return math::IsEqualApprox(col0().Dot(col1()), 0.0_r, math::EPSILON_ORTHO);
    }

    Matrix2x2& Matrix2x2::Orthogonalize() {
        const real l0 = col0().Magnitude();
        const real l1 = col1().Magnitude();
        Orthonormalize();
        set_col0(col0() * l0);
        set_col1(col1() * l1);
        return *this;
    }

    Matrix2x2 Matrix2x2::Orthogonalized() const {
        Matrix2x2 copy = *this;
        return copy.Orthogonalize();
    }

    constexpr bool Matrix2x2::IsOrthonormal() const {
        return IsOrthogonal() && col0().IsUnitApprox() && col1().IsUnitApprox();
    }

    Matrix2x2& Matrix2x2::Orthonormalize() {
        Vector2 x = col0();
        Vector2 y = col1();

        x.Normalize();
        y -= x * (x.Dot(y));
        y.Normalize();

        set_col0(x);
        set_col1(y);
        return *this;
    }
    Matrix2x2 Matrix2x2::Orthonormalized() const {
        Matrix2x2 copy = *this;
        return copy.Orthonormalize();
    }

    constexpr Matrix2x2& Matrix2x2::Transpose() {
        const real tmp = data[0][1];
        data[0][1] = data[1][0];
        data[1][0] = tmp;
        return *this;
    }
    constexpr Matrix2x2 Matrix2x2::Transposed() const {
        return Matrix2x2(Vector2(row0.x, row1.x), Vector2(row0.y, row1.y));
    }

    constexpr Matrix2x2& Matrix2x2::Invert() {
        const real det = Determinant();

        const real inv_det = 1.0_r / det;

        const real temp_x = row0.x;
        row0.x = row1.y * inv_det;
        row1.x = -row1.x * inv_det;
        row0.y = -row0.y * inv_det;
        row1.y = temp_x * inv_det;

        return *this;
    }
    constexpr Matrix2x2 Matrix2x2::Inverse() const {
        Matrix2x2 copy = *this;
        return copy.Invert();
    }

    constexpr real Matrix2x2::Trace() const { return row0.x + row1.y; }

    constexpr real Matrix2x2::Determinant() const { return row0.x * row1.y - row0.y * row1.x; }

    ALWAYS_INLINE constexpr Matrix2x2 operator*(float scalar, const Matrix2x2& matrix) {
        return Matrix2x2(scalar * matrix.row0, scalar * matrix.row1);
    }

    ALWAYS_INLINE constexpr Vector2 operator*(const Matrix2x2& m, const Vector2& v) {
        return Vector2(m.row0.Dot(v), m.row1.Dot(v));
    }
}  // namespace ho
