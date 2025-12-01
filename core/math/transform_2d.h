#pragma once

#include <string>

#include "basis_2d.h"
#include "matrix3x3.h"
#include "vector2.h"

namespace ho {
    struct Transform2D {
        static Transform2D FromAngle(real angle);

        constexpr Transform2D();
        constexpr Transform2D(const Basis2D& p_basis, const Vector2& p_origin);
        constexpr Transform2D(const Matrix3x3& m);
        constexpr Transform2D(const Transform2D&) = default;
        constexpr Transform2D& operator=(const Transform2D& rhs);
        ~Transform2D() = default;

        ALWAYS_INLINE real scale_x() const;
        ALWAYS_INLINE real scale_y() const;
        constexpr Vector2 basis_x() const;
        constexpr Vector2 basis_y() const;
        constexpr void set_basis_x(const Vector2& basis_x);
        constexpr void set_basis_y(const Vector2& basis_y);

        constexpr bool operator==(const Transform2D& rhs) const;
        constexpr bool operator!=(const Transform2D& rhs) const;

        constexpr bool IsEqualApprox(const Transform2D& rhs) const;
        constexpr bool IsNotEqualApprox(const Transform2D& rhs) const;

        constexpr bool IsOrthogonal() const;
        ALWAYS_INLINE Transform2D& Orthogonalize();
        ALWAYS_INLINE Transform2D Orthogonalized() const;

        constexpr bool IsOrthonormal() const;
        ALWAYS_INLINE Transform2D& Orthonormalize();
        ALWAYS_INLINE Transform2D Orthonormalized() const;

        constexpr Transform2D& Scale(const Vector2& scale);
        constexpr Transform2D& ScaleUniform(real scale);
        ALWAYS_INLINE Transform2D& Rotate(real angle);
        constexpr Transform2D& Translate(const Vector2& translation_v);

        constexpr Transform2D Scaled(const Vector2& scale) const;
        constexpr Transform2D ScaledUniform(real scale) const;
        ALWAYS_INLINE Transform2D Rotated(real angle) const;
        constexpr Transform2D Translated(const Vector2& translation_v) const;

        constexpr Transform2D& ScaleLocal(const Vector2& scale);
        constexpr Transform2D& ScaleUniformLocal(real scale);
        ALWAYS_INLINE Transform2D& RotateLocal(real angle);
        constexpr Transform2D& TranslateLocal(const Vector2& translation_v);

        constexpr Transform2D ScaledLocal(const Vector2& scale) const;
        constexpr Transform2D ScaledUniformLocal(real scale) const;
        ALWAYS_INLINE Transform2D RotatedLocal(real angle) const;
        constexpr Transform2D TranslatedLocal(const Vector2& translation_v) const;

        constexpr Transform2D& Invert();
        constexpr Transform2D Inverse() const;

        constexpr Transform2D& InvertFast();
        constexpr Transform2D InverseFast() const;

        ALWAYS_INLINE Transform2D& LookAt(const Vector2& at);
        ALWAYS_INLINE Transform2D LookedAt(const Vector2& at) const;

        constexpr Transform2D& operator*=(const Transform2D& rhs);
        constexpr Transform2D operator*(const Transform2D& rhs) const;

        constexpr Vector2 Transform(const Vector2& v) const;
        constexpr Vector2 InvTransform(const Vector2& v) const;
        constexpr Vector2 InvTransformFast(const Vector2& v) const;

        constexpr Matrix3x3 ToMatrix() const;

        std::string ToString() const;

        Basis2D basis;
        Vector2 origin;
    };

    constexpr Transform2D::Transform2D() : basis(Basis2D()), origin(Vector2::ZERO) {}
    constexpr Transform2D::Transform2D(const Basis2D& p_basis, const Vector2& p_origin)
        : basis(p_basis), origin(p_origin) {}
    constexpr Transform2D::Transform2D(const Matrix3x3& m)
        : basis(Basis2D(Vector2(m.data[0][0], m.data[1][0]), Vector2(m.data[0][1], m.data[1][1]))),
          origin(Vector2(m.data[0][2], m.data[1][2])) {}

    constexpr Transform2D& Transform2D::operator=(const Transform2D& rhs) {
        if (this == &rhs) {
            return *this;
        }
        basis = rhs.basis;
        origin = rhs.origin;
        return *this;
    }

    real Transform2D::scale_x() const { return basis.scale_x(); }
    real Transform2D::scale_y() const { return basis.scale_y(); }
    constexpr Vector2 Transform2D::basis_x() const { return basis.basis_x(); }
    constexpr Vector2 Transform2D::basis_y() const { return basis.basis_y(); }
    constexpr void Transform2D::set_basis_x(const Vector2& basis_x) { basis.set_basis_x(basis_x); }
    constexpr void Transform2D::set_basis_y(const Vector2& basis_y) { basis.set_basis_y(basis_y); }

    constexpr bool Transform2D::operator==(const Transform2D& rhs) const {
        return basis == rhs.basis && origin == rhs.origin;
    }
    constexpr bool Transform2D::operator!=(const Transform2D& rhs) const { return !(*this == rhs); }

    constexpr bool Transform2D::IsEqualApprox(const Transform2D& rhs) const {
        return basis.IsEqualApprox(rhs.basis) && origin.IsEqualApprox(rhs.origin);
    }
    constexpr bool Transform2D::IsNotEqualApprox(const Transform2D& rhs) const { return !(IsEqualApprox(rhs)); }

    constexpr bool Transform2D::IsOrthogonal() const { return basis.IsOrthogonal(); }
    Transform2D& Transform2D::Orthogonalize() {
        basis.Orthogonalize();
        return *this;
    }
    Transform2D Transform2D::Orthogonalized() const {
        Transform2D copy = *this;
        return copy.Orthogonalize();
    }
    constexpr bool Transform2D::IsOrthonormal() const { return basis.IsOrthonormal(); }
    Transform2D& Transform2D::Orthonormalize() {
        basis.Orthonormalize();
        return *this;
    }
    Transform2D Transform2D::Orthonormalized() const {
        Transform2D copy = *this;
        return copy.Orthonormalize();
    }

    constexpr Transform2D& Transform2D::Scale(const Vector2& scale) {
        basis.Scale(scale);
        origin *= scale;
        return *this;
    }
    constexpr Transform2D& Transform2D::ScaleUniform(real scale) {
        basis.ScaleUniform(scale);
        origin *= scale;
        return *this;
    }
    Transform2D& Transform2D::Rotate(real angle) {
        Basis2D r = Basis2D::FromAngle(angle);
        basis = r * basis;
        origin = r.Transform(origin);
        return *this;
    }
    constexpr Transform2D& Transform2D::Translate(const Vector2& translation_v) {
        origin += translation_v;
        return *this;
    }

    constexpr Transform2D Transform2D::Scaled(const Vector2& scale) const {
        Transform2D copy = *this;
        return copy.Scale(scale);
    }
    constexpr Transform2D Transform2D::ScaledUniform(real scale) const {
        Transform2D copy = *this;
        return copy.ScaleUniform(scale);
    }
    Transform2D Transform2D::Rotated(real angle) const {
        Transform2D copy = *this;
        return copy.Rotate(angle);
    }
    constexpr Transform2D Transform2D::Translated(const Vector2& translation_v) const {
        Transform2D copy = *this;
        return copy.Translate(translation_v);
    }

    constexpr Transform2D& Transform2D::ScaleLocal(const Vector2& scale) {
        basis.ScaleLocal(scale);
        return *this;
    }
    constexpr Transform2D& Transform2D::ScaleUniformLocal(real scale) {
        basis.ScaleUniformLocal(scale);
        return *this;
    }
    Transform2D& Transform2D::RotateLocal(real angle) {
        Basis2D r = Basis2D::FromAngle(angle);
        basis *= r;
        return *this;
    }
    constexpr Transform2D& Transform2D::TranslateLocal(const Vector2& translation_v) {
        origin += basis_x() * translation_v.x + basis_y() * translation_v.y;
        return *this;
    }

    constexpr Transform2D Transform2D::ScaledLocal(const Vector2& scale) const {
        Transform2D copy = *this;
        return copy.ScaleLocal(scale);
    }
    constexpr Transform2D Transform2D::ScaledUniformLocal(real scale) const {
        Transform2D copy = *this;
        return copy.ScaleUniformLocal(scale);
    }
    Transform2D Transform2D::RotatedLocal(real angle) const {
        Transform2D copy = *this;
        return copy.RotateLocal(angle);
    }
    constexpr Transform2D Transform2D::TranslatedLocal(const Vector2& translation_v) const {
        Transform2D copy = *this;
        return copy.TranslateLocal(translation_v);
    }

    constexpr Transform2D& Transform2D::Invert() {
        Matrix3x3 m = ToMatrix();
        Matrix3x3 inv_m = m.Inverse();
        *this = Transform2D(inv_m);
        return *this;
    }

    constexpr Transform2D Transform2D::Inverse() const {
        Transform2D copy = *this;
        return copy.Invert();
    }

    constexpr Transform2D& Transform2D::InvertFast() {
        basis.InvertFast();
        origin = basis.Transform(-origin);
        return *this;
    }
    constexpr Transform2D Transform2D::InverseFast() const {
        Transform2D copy = *this;
        return copy.InvertFast();
    }

    Transform2D& Transform2D::LookAt(const Vector2& at) {
        Vector2 adj_at = at - origin;
        if (adj_at.IsEqualApprox(Vector2::ZERO)) return *this;
        basis.LookAt(adj_at);
        return *this;
    }
    Transform2D Transform2D::LookedAt(const Vector2& at) const {
        Transform2D copy = *this;
        return copy.LookAt(at);
    }

    constexpr Transform2D& Transform2D::operator*=(const Transform2D& rhs) {
        origin += basis.Transform(rhs.origin);
        basis *= rhs.basis;

        return *this;
    }

    constexpr Transform2D Transform2D::operator*(const Transform2D& rhs) const {
        Transform2D copy = *this;
        return copy *= rhs;
    }

    constexpr Vector2 Transform2D::Transform(const Vector2& v) const { return basis.Transform(v) + origin; }
    constexpr Vector2 Transform2D::InvTransform(const Vector2& v) const {
        const Vector2 w = v - origin;
        return basis.InvTransform(w);
    }
    constexpr Vector2 Transform2D::InvTransformFast(const Vector2& v) const {
        const Vector2 w = v - origin;
        return basis.InvTransformFast(w);
    }

    constexpr Matrix3x3 Transform2D::ToMatrix() const {
        return Matrix3x3(Vector3(basis.row0.x, basis.row0.y, origin.x), Vector3(basis.row1.x, basis.row1.y, origin.y),
                         Vector3(0.0_r, 0.0_r, 1.0_r));
    }
}  // namespace ho