#pragma once

#include <string>
#include <vector>

#include "quaternion.h"
#include "vector3.h"

namespace ho {
    struct Quaternion;

    struct Matrix3x3 {
        static const Matrix3x3 IDENTITY;

        static Matrix3x3 FromAxisAngle(const Vector3& axis, real angle);
        static Matrix3x3 FromEuler(real x, real y, real z, math::EulerOrder order = math::YXZ);
        constexpr static Matrix3x3 FromQuaternion(const Quaternion& q);

        constexpr Matrix3x3();
        constexpr Matrix3x3(const Vector3& p_row0, const Vector3& p_row1, const Vector3& p_row2);
        constexpr Matrix3x3(const Matrix3x3&) = default;
        constexpr Matrix3x3& operator=(const Matrix3x3& rhs);
        ~Matrix3x3() = default;

        constexpr Vector3 col0() const;
        constexpr Vector3 col1() const;
        constexpr Vector3 col2() const;
        constexpr void set_col0(const Vector3& col);
        constexpr void set_col1(const Vector3& col);
        constexpr void set_col2(const Vector3& col);

        constexpr Matrix3x3 operator+(const Matrix3x3& rhs) const;
        constexpr Matrix3x3& operator+=(const Matrix3x3& rhs);

        constexpr Matrix3x3 operator-(const Matrix3x3& rhs) const;
        constexpr Matrix3x3& operator-=(const Matrix3x3& rhs);

        constexpr Matrix3x3 operator*(const Matrix3x3& rhs) const;
        constexpr Matrix3x3& operator*=(const Matrix3x3& rhs);

        constexpr Matrix3x3 operator*(real scalar) const;
        constexpr Matrix3x3& operator*=(real scalar);

        constexpr Matrix3x3 operator/(real scalar) const;
        constexpr Matrix3x3& operator/=(real scalar);

        constexpr bool operator==(const Matrix3x3& rhs) const;
        constexpr bool operator!=(const Matrix3x3& rhs) const;

        constexpr bool IsEqualApprox(const Matrix3x3& rhs) const;
        constexpr bool IsNotEqualApprox(const Matrix3x3& rhs) const;

        constexpr bool IsOrthogonal() const;
        Matrix3x3& Orthogonalize();
        Matrix3x3 Orthogonalized() const;

        constexpr bool IsOrthonormal() const;
        Matrix3x3& Orthonormalize();
        Matrix3x3 Orthonormalized() const;

        constexpr Matrix3x3& Transpose();
        constexpr Matrix3x3 Transposed() const;

        constexpr Matrix3x3& Invert();
        constexpr Matrix3x3 Inverse() const;

        constexpr real Trace() const;
        constexpr real Determinant() const;

        std::string ToString() const;

        union {
            struct {
                Vector3 row0;
                Vector3 row1;
                Vector3 row2;
            };
            real data[3][3];
        };
    };

    constexpr Matrix3x3::Matrix3x3() : row0(Vector3()), row1(Vector3()), row2(Vector3()) {}

    constexpr Matrix3x3::Matrix3x3(const Vector3& p_row0, const Vector3& p_row1, const Vector3& p_row2)
        : row0(p_row0), row1(p_row1), row2(p_row2) {}

    INLINE constexpr Matrix3x3 Matrix3x3::IDENTITY =
        Matrix3x3(Vector3(1.0_r, 0.0_r, 0.0_r), Vector3(0.0_r, 1.0_r, 0.0_r), Vector3(0.0_r, 0.0_r, 1.0_r));

    constexpr Matrix3x3& Matrix3x3::operator=(const Matrix3x3& rhs) {
        row0 = rhs.row0;
        row1 = rhs.row1;
        row2 = rhs.row2;
        return *this;
    }

    constexpr Vector3 Matrix3x3::col0() const { return Vector3(row0.x, row1.x, row2.x); }
    constexpr Vector3 Matrix3x3::col1() const { return Vector3(row0.y, row1.y, row2.y); }
    constexpr Vector3 Matrix3x3::col2() const { return Vector3(row0.z, row1.z, row2.z); }
    constexpr void Matrix3x3::set_col0(const Vector3& col) {
        row0.x = col.x;
        row1.x = col.y;
        row2.x = col.z;
    }
    constexpr void Matrix3x3::set_col1(const Vector3& col) {
        row0.y = col.x;
        row1.y = col.y;
        row2.y = col.z;
    }
    constexpr void Matrix3x3::set_col2(const Vector3& col) {
        row0.z = col.x;
        row1.z = col.y;
        row2.z = col.z;
    }

    constexpr Matrix3x3 Matrix3x3::operator+(const Matrix3x3& rhs) const {
        return Matrix3x3(row0 + rhs.row0, row1 + rhs.row1, row2 + rhs.row2);
    }
    constexpr Matrix3x3& Matrix3x3::operator+=(const Matrix3x3& rhs) {
        row0 += rhs.row0;
        row1 += rhs.row1;
        row2 += rhs.row2;
        return *this;
    }

    constexpr Matrix3x3 Matrix3x3::operator-(const Matrix3x3& rhs) const {
        return Matrix3x3(row0 - rhs.row0, row1 - rhs.row1, row2 - rhs.row2);
    }
    constexpr Matrix3x3& Matrix3x3::operator-=(const Matrix3x3& rhs) {
        row0 -= rhs.row0;
        row1 -= rhs.row1;
        row2 -= rhs.row2;
        return *this;
    }

    constexpr Matrix3x3 Matrix3x3::operator*(const Matrix3x3& rhs) const {
        const Vector3 rhs_c1 = rhs.col0();
        const Vector3 rhs_c2 = rhs.col1();
        const Vector3 rhs_c3 = rhs.col2();

        return Matrix3x3(Vector3(row0.Dot(rhs_c1), row0.Dot(rhs_c2), row0.Dot(rhs_c3)),
                         Vector3(row1.Dot(rhs_c1), row1.Dot(rhs_c2), row1.Dot(rhs_c3)),
                         Vector3(row2.Dot(rhs_c1), row2.Dot(rhs_c2), row2.Dot(rhs_c3)));
    }
    constexpr Matrix3x3& Matrix3x3::operator*=(const Matrix3x3& rhs) {
        const Vector3 rhs_c1 = rhs.col0();
        const Vector3 rhs_c2 = rhs.col1();
        const Vector3 rhs_c3 = rhs.col2();

        const Vector3 new_r1(row0.Dot(rhs_c1), row0.Dot(rhs_c2), row0.Dot(rhs_c3));
        const Vector3 new_r2(row1.Dot(rhs_c1), row1.Dot(rhs_c2), row1.Dot(rhs_c3));
        const Vector3 new_r3(row2.Dot(rhs_c1), row2.Dot(rhs_c2), row2.Dot(rhs_c3));

        row0 = new_r1;
        row1 = new_r2;
        row2 = new_r3;
        return *this;
    }

    constexpr Matrix3x3 Matrix3x3::operator*(real scalar) const {
        return Matrix3x3(row0 * scalar, row1 * scalar, row2 * scalar);
    }
    constexpr Matrix3x3& Matrix3x3::operator*=(real scalar) {
        row0 *= scalar;
        row1 *= scalar;
        row2 *= scalar;
        return *this;
    }

    constexpr Matrix3x3 Matrix3x3::operator/(real scalar) const {
        return Matrix3x3(row0 / scalar, row1 / scalar, row2 / scalar);
    }
    constexpr Matrix3x3& Matrix3x3::operator/=(real scalar) {
        row0 /= scalar;
        row1 /= scalar;
        row2 /= scalar;
        return *this;
    }

    constexpr bool Matrix3x3::operator==(const Matrix3x3& rhs) const {
        return row0 == rhs.row0 && row1 == rhs.row1 && row2 == rhs.row2;
    }
    constexpr bool Matrix3x3::operator!=(const Matrix3x3& rhs) const { return !(*this == rhs); }

    constexpr bool Matrix3x3::IsEqualApprox(const Matrix3x3& rhs) const {
        return row0.IsEqualApprox(rhs.row0) && row1.IsEqualApprox(rhs.row1) && row2.IsEqualApprox(rhs.row2);
    }
    constexpr bool Matrix3x3::IsNotEqualApprox(const Matrix3x3& rhs) const { return !IsEqualApprox(rhs); }

    constexpr bool Matrix3x3::IsOrthogonal() const {
        const Vector3 c0 = col0();
        const Vector3 c1 = col1();
        const Vector3 c2 = col2();
        return math::IsEqualApprox(c0.Dot(c1), 0.0_r, math::EPSILON_ORTHO) &&
               math::IsEqualApprox(c0.Dot(c2), 0.0_r, math::EPSILON_ORTHO) &&
               math::IsEqualApprox(c1.Dot(c2), 0.0_r, math::EPSILON_ORTHO);
    }

    constexpr bool Matrix3x3::IsOrthonormal() const {
        return IsOrthogonal() && col0().IsUnitApprox() && col1().IsUnitApprox() && col2().IsUnitApprox();
    }

    constexpr Matrix3x3& Matrix3x3::Transpose() {
        real tmp = 0.0_r;
        tmp = data[0][1];
        data[0][1] = data[1][0];
        data[1][0] = tmp;
        tmp = data[0][2];
        data[0][2] = data[2][0];
        data[2][0] = tmp;
        tmp = data[1][2];
        data[1][2] = data[2][1];
        data[2][1] = tmp;
        return *this;
    }
    constexpr Matrix3x3 Matrix3x3::Transposed() const {
        return Matrix3x3(Vector3(row0.x, row1.x, row2.x), Vector3(row0.y, row1.y, row2.y),
                         Vector3(row0.z, row1.z, row2.z));
    }

    constexpr Matrix3x3& Matrix3x3::Invert() {
        const real det = Determinant();

        const real inv_det = 1.0_r / det;

        const Matrix3x3& m = *this;
        Matrix3x3 adj = Matrix3x3();

        adj.row0.x = m.row1.y * m.row2.z - m.row1.z * m.row2.y;
        adj.row0.y = -(m.row0.y * m.row2.z - m.row0.z * m.row2.y);
        adj.row0.z = m.row0.y * m.row1.z - m.row0.z * m.row1.y;

        adj.row1.x = -(m.row1.x * m.row2.z - m.row1.z * m.row2.x);
        adj.row1.y = m.row0.x * m.row2.z - m.row0.z * m.row2.x;
        adj.row1.z = -(m.row0.x * m.row1.z - m.row0.z * m.row1.x);

        adj.row2.x = m.row1.x * m.row2.y - m.row1.y * m.row2.x;
        adj.row2.y = -(m.row0.x * m.row2.y - m.row0.y * m.row2.x);
        adj.row2.z = m.row0.x * m.row1.y - m.row0.y * m.row1.x;

        *this = adj * inv_det;
        return *this;
    }

    constexpr Matrix3x3 Matrix3x3::Inverse() const {
        Matrix3x3 copy = *this;
        return copy.Invert();
    }

    constexpr real Matrix3x3::Trace() const { return row0.x + row1.y + row2.z; }

    constexpr real Matrix3x3::Determinant() const {
        return row0.x * (row1.y * row2.z - row1.z * row2.y) - row0.y * (row1.x * row2.z - row1.z * row2.x) +
               row0.z * (row1.x * row2.y - row1.y * row2.x);
    }

    // This function for converting a quaternion to a 3x3 matrix is adapted from
    // the set_quaternion() method in the Godot Engine's Basis class.
    // Source: https://github.com/godotengine/godot/blob/master/core/math/basis.cpp
    constexpr Matrix3x3 Matrix3x3::FromQuaternion(const Quaternion& q) {
        Matrix3x3 m;
        const real d = q.SqrdMagnitude();
        const real s = 2.0_r / d;
        const real xs = q.x * s, ys = q.y * s, zs = q.z * s;
        const real wx = q.w * xs, wy = q.w * ys, wz = q.w * zs;
        const real xx = q.x * xs, xy = q.x * ys, xz = q.x * zs;
        const real yy = q.y * ys, yz = q.y * zs, zz = q.z * zs;
        m.row0 = Vector3(1.0_r - (yy + zz), xy - wz, xz + wy);
        m.row1 = Vector3(xy + wz, 1.0_r - (xx + zz), yz - wx);
        m.row2 = Vector3(xz - wy, yz + wx, 1.0_r - (xx + yy));
        return m;
    }

    ALWAYS_INLINE constexpr Matrix3x3 operator*(real scalar, const Matrix3x3& matrix) {
        return Matrix3x3(scalar * matrix.row0, scalar * matrix.row1, scalar * matrix.row2);
    }

    ALWAYS_INLINE constexpr Vector3 operator*(const Matrix3x3& m, const Vector3& v) {
        return Vector3(m.row0.Dot(v), m.row1.Dot(v), m.row2.Dot(v));
    }

}  // namespace ho
