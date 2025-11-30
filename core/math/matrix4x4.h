#pragma once

#include <string>
#include <vector>

#include "math_funcs.h"
#include "vector4.h"

namespace ho {
    struct Matrix3x3;
    struct Vector3;

    struct Matrix4x4 {
        static const Matrix4x4 IDENTITY;

        constexpr Matrix4x4();
        constexpr Matrix4x4(const Vector4& p_row0, const Vector4& p_row1, const Vector4& p_row2, const Vector4& p_row3);
        constexpr Matrix4x4(const Matrix4x4&) = default;
        constexpr Matrix4x4& operator=(const Matrix4x4& rhs);
        ~Matrix4x4() = default;

        constexpr Vector4 col0() const;
        constexpr Vector4 col1() const;
        constexpr Vector4 col2() const;
        constexpr Vector4 col3() const;
        constexpr void set_col0(const Vector4& col);
        constexpr void set_col1(const Vector4& col);
        constexpr void set_col2(const Vector4& col);
        constexpr void set_col3(const Vector4& col);

        constexpr Matrix4x4 operator+(const Matrix4x4& rhs) const;
        constexpr Matrix4x4& operator+=(const Matrix4x4& rhs);

        constexpr Matrix4x4 operator-(const Matrix4x4& rhs) const;
        constexpr Matrix4x4& operator-=(const Matrix4x4& rhs);

        constexpr Matrix4x4 operator*(const Matrix4x4& rhs) const;
        constexpr Matrix4x4& operator*=(const Matrix4x4& rhs);

        constexpr Matrix4x4 operator*(real scalar) const;
        constexpr Matrix4x4& operator*=(real scalar);

        constexpr Matrix4x4 operator/(real scalar) const;
        constexpr Matrix4x4& operator/=(real scalar);

        constexpr bool operator==(const Matrix4x4& rhs) const;
        constexpr bool operator!=(const Matrix4x4& rhs) const;

        constexpr bool IsEqualApprox(const Matrix4x4& rhs) const;
        constexpr bool IsNotEqualApprox(const Matrix4x4& rhs) const;

        constexpr bool IsOrthogonal() const;
        Matrix4x4& Orthogonalize();
        Matrix4x4 Orthogonalized() const;

        constexpr bool IsOrthonormal() const;
        Matrix4x4& Orthonormalize();
        Matrix4x4 Orthonormalized() const;

        constexpr Matrix4x4& Transpose();
        constexpr Matrix4x4 Transposed() const;

        constexpr Matrix4x4& Invert();
        constexpr Matrix4x4 Inverse() const;

        constexpr real Trace() const;
        constexpr real Determinant() const;

        Matrix3x3 ToMatrix3x3() const;
        std::string ToString() const;

        union {
            struct {
                Vector4 row0;
                Vector4 row1;
                Vector4 row2;
                Vector4 row3;
            };
            real data[4][4];
        };
    };

    constexpr Matrix4x4::Matrix4x4() : row0(Vector4()), row1(Vector4()), row2(Vector4()), row3(Vector4()) {}

    constexpr Matrix4x4::Matrix4x4(const Vector4& p_row0, const Vector4& p_row1, const Vector4& p_row2,
                                   const Vector4& p_row3)
        : row0(p_row0), row1(p_row1), row2(p_row2), row3(p_row3) {}

    _INLINE_ constexpr Matrix4x4 Matrix4x4::IDENTITY =
        Matrix4x4(Vector4(1.0_r, 0.0_r, 0.0_r, 0.0_r), Vector4(0.0_r, 1.0_r, 0.0_r, 0.0_r),
                  Vector4(0.0_r, 0.0_r, 1.0_r, 0.0_r), Vector4(0.0_r, 0.0_r, 0.0_r, 1.0_r));

    constexpr Matrix4x4& Matrix4x4::operator=(const Matrix4x4& rhs) {
        row0 = rhs.row0;
        row1 = rhs.row1;
        row2 = rhs.row2;
        row3 = rhs.row3;
        return *this;
    }

    constexpr Vector4 Matrix4x4::col0() const { return Vector4(row0.x, row1.x, row2.x, row3.x); }
    constexpr Vector4 Matrix4x4::col1() const { return Vector4(row0.y, row1.y, row2.y, row3.y); }
    constexpr Vector4 Matrix4x4::col2() const { return Vector4(row0.z, row1.z, row2.z, row3.z); }
    constexpr Vector4 Matrix4x4::col3() const { return Vector4(row0.w, row1.w, row2.w, row3.w); }

    constexpr void Matrix4x4::set_col0(const Vector4& c) {
        row0.x = c.x;
        row1.x = c.y;
        row2.x = c.z;
        row3.x = c.w;
    }
    constexpr void Matrix4x4::set_col1(const Vector4& c) {
        row0.y = c.x;
        row1.y = c.y;
        row2.y = c.z;
        row3.y = c.w;
    }
    constexpr void Matrix4x4::set_col2(const Vector4& c) {
        row0.z = c.x;
        row1.z = c.y;
        row2.z = c.z;
        row3.z = c.w;
    }
    constexpr void Matrix4x4::set_col3(const Vector4& c) {
        row0.w = c.x;
        row1.w = c.y;
        row2.w = c.z;
        row3.w = c.w;
    }

    constexpr Matrix4x4 Matrix4x4::operator+(const Matrix4x4& rhs) const {
        return Matrix4x4(row0 + rhs.row0, row1 + rhs.row1, row2 + rhs.row2, row3 + rhs.row3);
    }
    constexpr Matrix4x4& Matrix4x4::operator+=(const Matrix4x4& rhs) {
        row0 += rhs.row0;
        row1 += rhs.row1;
        row2 += rhs.row2;
        row3 += rhs.row3;
        return *this;
    }

    constexpr Matrix4x4 Matrix4x4::operator-(const Matrix4x4& rhs) const {
        return Matrix4x4(row0 - rhs.row0, row1 - rhs.row1, row2 - rhs.row2, row3 - rhs.row3);
    }
    constexpr Matrix4x4& Matrix4x4::operator-=(const Matrix4x4& rhs) {
        row0 -= rhs.row0;
        row1 -= rhs.row1;
        row2 -= rhs.row2;
        row3 -= rhs.row3;
        return *this;
    }

    constexpr Matrix4x4 Matrix4x4::operator*(const Matrix4x4& rhs) const {
        const Vector4 c1 = rhs.col0(), c2 = rhs.col1(), c3 = rhs.col2(), c4 = rhs.col3();
        return Matrix4x4(Vector4(row0.Dot(c1), row0.Dot(c2), row0.Dot(c3), row0.Dot(c4)),
                         Vector4(row1.Dot(c1), row1.Dot(c2), row1.Dot(c3), row1.Dot(c4)),
                         Vector4(row2.Dot(c1), row2.Dot(c2), row2.Dot(c3), row2.Dot(c4)),
                         Vector4(row3.Dot(c1), row3.Dot(c2), row3.Dot(c3), row3.Dot(c4)));
    }

    constexpr Matrix4x4& Matrix4x4::operator*=(const Matrix4x4& rhs) {
        *this = *this * rhs;
        return *this;
    }

    constexpr Matrix4x4 Matrix4x4::operator*(real scalar) const {
        return Matrix4x4(row0 * scalar, row1 * scalar, row2 * scalar, row3 * scalar);
    }
    constexpr Matrix4x4& Matrix4x4::operator*=(real scalar) {
        row0 *= scalar;
        row1 *= scalar;
        row2 *= scalar;
        row3 *= scalar;
        return *this;
    }

    constexpr Matrix4x4 Matrix4x4::operator/(real scalar) const {
        return Matrix4x4(row0 / scalar, row1 / scalar, row2 / scalar, row3 / scalar);
    }
    constexpr Matrix4x4& Matrix4x4::operator/=(real scalar) {
        row0 /= scalar;
        row1 /= scalar;
        row2 /= scalar;
        row3 /= scalar;
        return *this;
    }

    constexpr bool Matrix4x4::operator==(const Matrix4x4& r) const {
        return row0 == r.row0 && row1 == r.row1 && row2 == r.row2 && row3 == r.row3;
    }
    constexpr bool Matrix4x4::operator!=(const Matrix4x4& r) const { return !(*this == r); }

    constexpr bool Matrix4x4::IsEqualApprox(const Matrix4x4& r) const {
        return row0.IsEqualApprox(r.row0) && row1.IsEqualApprox(r.row1) && row2.IsEqualApprox(r.row2) &&
               row3.IsEqualApprox(r.row3);
    }
    constexpr bool Matrix4x4::IsNotEqualApprox(const Matrix4x4& r) const { return !IsEqualApprox(r); }

    constexpr bool Matrix4x4::IsOrthogonal() const {
        const Vector4 c0 = col0();
        const Vector4 c1 = col1();
        const Vector4 c2 = col2();
        const Vector4 c3 = col3();
        return math::IsEqualApprox(c0.Dot(c1), 0.0_r, math::EPSILON_ORTHO) &&
               math::IsEqualApprox(c0.Dot(c2), 0.0_r, math::EPSILON_ORTHO) &&
               math::IsEqualApprox(c0.Dot(c3), 0.0_r, math::EPSILON_ORTHO) &&
               math::IsEqualApprox(c1.Dot(c2), 0.0_r, math::EPSILON_ORTHO) &&
               math::IsEqualApprox(c1.Dot(c3), 0.0_r, math::EPSILON_ORTHO) &&
               math::IsEqualApprox(c2.Dot(c3), 0.0_r, math::EPSILON_ORTHO);
    }

    constexpr bool Matrix4x4::IsOrthonormal() const {
        return IsOrthogonal() && col0().IsUnitApprox() && col1().IsUnitApprox() && col2().IsUnitApprox() &&
               col3().IsUnitApprox();
    }

    constexpr Matrix4x4& Matrix4x4::Transpose() {
        *this = Matrix4x4(Vector4(row0.x, row1.x, row2.x, row3.x), Vector4(row0.y, row1.y, row2.y, row3.y),
                          Vector4(row0.z, row1.z, row2.z, row3.z), Vector4(row0.w, row1.w, row2.w, row3.w));
        return *this;
    }

    constexpr Matrix4x4 Matrix4x4::Transposed() const {
        Matrix4x4 copy = *this;
        return copy.Transpose();
    }

    constexpr Matrix4x4& Matrix4x4::Invert() {
        const real* m = &data[0][0];
        real inv[16] = {0.0_r};

        inv[0] = m[5] * m[10] * m[15] - m[5] * m[11] * m[14] - m[9] * m[6] * m[15] + m[9] * m[7] * m[14] +
                 m[13] * m[6] * m[11] - m[13] * m[7] * m[10];

        inv[4] = -m[4] * m[10] * m[15] + m[4] * m[11] * m[14] + m[8] * m[6] * m[15] - m[8] * m[7] * m[14] -
                 m[12] * m[6] * m[11] + m[12] * m[7] * m[10];

        inv[8] = m[4] * m[9] * m[15] - m[4] * m[11] * m[13] - m[8] * m[5] * m[15] + m[8] * m[7] * m[13] +
                 m[12] * m[5] * m[11] - m[12] * m[7] * m[9];

        inv[12] = -m[4] * m[9] * m[14] + m[4] * m[10] * m[13] + m[8] * m[5] * m[14] - m[8] * m[6] * m[13] -
                  m[12] * m[5] * m[10] + m[12] * m[6] * m[9];

        inv[1] = -m[1] * m[10] * m[15] + m[1] * m[11] * m[14] + m[9] * m[2] * m[15] - m[9] * m[3] * m[14] -
                 m[13] * m[2] * m[11] + m[13] * m[3] * m[10];

        inv[5] = m[0] * m[10] * m[15] - m[0] * m[11] * m[14] - m[8] * m[2] * m[15] + m[8] * m[3] * m[14] +
                 m[12] * m[2] * m[11] - m[12] * m[3] * m[10];

        inv[9] = -m[0] * m[9] * m[15] + m[0] * m[11] * m[13] + m[8] * m[1] * m[15] - m[8] * m[3] * m[13] -
                 m[12] * m[1] * m[11] + m[12] * m[3] * m[9];

        inv[13] = m[0] * m[9] * m[14] - m[0] * m[10] * m[13] - m[8] * m[1] * m[14] + m[8] * m[2] * m[13] +
                  m[12] * m[1] * m[10] - m[12] * m[2] * m[9];

        inv[2] = m[1] * m[6] * m[15] - m[1] * m[7] * m[14] - m[5] * m[2] * m[15] + m[5] * m[3] * m[14] +
                 m[13] * m[2] * m[7] - m[13] * m[3] * m[6];

        inv[6] = -m[0] * m[6] * m[15] + m[0] * m[7] * m[14] + m[4] * m[2] * m[15] - m[4] * m[3] * m[14] -
                 m[12] * m[2] * m[7] + m[12] * m[3] * m[6];

        inv[10] = m[0] * m[5] * m[15] - m[0] * m[7] * m[13] - m[4] * m[1] * m[15] + m[4] * m[3] * m[13] +
                  m[12] * m[1] * m[7] - m[12] * m[3] * m[5];

        inv[14] = -m[0] * m[5] * m[14] + m[0] * m[6] * m[13] + m[4] * m[1] * m[14] - m[4] * m[2] * m[13] -
                  m[12] * m[1] * m[6] + m[12] * m[2] * m[5];

        inv[3] = -m[1] * m[6] * m[11] + m[1] * m[7] * m[10] + m[5] * m[2] * m[11] - m[5] * m[3] * m[10] -
                 m[9] * m[2] * m[7] + m[9] * m[3] * m[6];

        inv[7] = m[0] * m[6] * m[11] - m[0] * m[7] * m[10] - m[4] * m[2] * m[11] + m[4] * m[3] * m[10] +
                 m[8] * m[2] * m[7] - m[8] * m[3] * m[6];

        inv[11] = -m[0] * m[5] * m[11] + m[0] * m[7] * m[9] + m[4] * m[1] * m[11] - m[4] * m[3] * m[9] -
                  m[8] * m[1] * m[7] + m[8] * m[3] * m[5];

        inv[15] = m[0] * m[5] * m[10] - m[0] * m[6] * m[9] - m[4] * m[1] * m[10] + m[4] * m[2] * m[9] +
                  m[8] * m[1] * m[6] - m[8] * m[2] * m[5];

        real det = m[0] * inv[0] + m[1] * inv[4] + m[2] * inv[8] + m[3] * inv[12];

        if (math::IsZeroApprox(det)) {
            const Vector4 nanv(math::REAL_NaN, math::REAL_NaN, math::REAL_NaN, math::REAL_NaN);
            row0 = row1 = row2 = row3 = nanv;
            return *this;
        }

        const real inv_det = 1.0_r / det;
        for (int i = 0; i < 16; ++i) inv[i] *= inv_det;

        for (int r = 0; r < 4; ++r)
            for (int c = 0; c < 4; ++c) data[r][c] = inv[r * 4 + c];

        return *this;
    }

    constexpr Matrix4x4 Matrix4x4::Inverse() const {
        Matrix4x4 copy = *this;
        return copy.Invert();
    }

    constexpr real Matrix4x4::Trace() const { return row0.x + row1.y + row2.z + row3.w; }

    constexpr real Matrix4x4::Determinant() const {
        const real subfactor00 = row2.z * row3.w - row3.z * row2.w;
        const real subfactor01 = row2.y * row3.w - row3.y * row2.w;
        const real subfactor02 = row2.y * row3.z - row3.y * row2.z;
        const real subfactor03 = row2.x * row3.w - row3.x * row2.w;
        const real subfactor04 = row2.x * row3.z - row3.x * row2.z;
        const real subfactor05 = row2.x * row3.y - row3.x * row2.y;

        return row0.x * (row1.y * subfactor00 - row1.z * subfactor01 + row1.w * subfactor02) -
               row0.y * (row1.x * subfactor00 - row1.z * subfactor03 + row1.w * subfactor04) +
               row0.z * (row1.x * subfactor01 - row1.y * subfactor03 + row1.w * subfactor05) -
               row0.w * (row1.x * subfactor02 - row1.y * subfactor04 + row1.z * subfactor05);
    }

    _ALWAYS_INLINE_ constexpr Matrix4x4 operator*(real s, const Matrix4x4& m) {
        return Matrix4x4(s * m.row0, s * m.row1, s * m.row2, s * m.row3);
    }

    _ALWAYS_INLINE_ constexpr Vector4 operator*(const Matrix4x4& m, const Vector4& v) {
        return Vector4(m.row0.Dot(v), m.row1.Dot(v), m.row2.Dot(v), m.row3.Dot(v));
    }
}  // namespace ho
