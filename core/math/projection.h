#pragma once

#include <string>

#include "matrix4x4.h"
#include "transform_3d.h"

namespace ho {
    struct Projection {
        ALWAYS_INLINE Projection();
        ALWAYS_INLINE Projection(real p_fov, real p_aspect_ratio, real p_near, real p_far);
        ALWAYS_INLINE Projection(real p_fov, real p_width, real p_height, real p_near, real p_far);
        ALWAYS_INLINE Projection(const Matrix4x4& m);
        ALWAYS_INLINE Projection(const Projection&) = default;
        ALWAYS_INLINE Projection& operator=(const Projection& rhs);
        ~Projection() = default;

        ALWAYS_INLINE real get_fov() const;
        ALWAYS_INLINE real get_aspect_ratio() const;
        ALWAYS_INLINE real get_near_dist() const;
        ALWAYS_INLINE real get_far_dist() const;

        ALWAYS_INLINE void set_fov(real p_fov);
        ALWAYS_INLINE void set_aspect_ratio(real p_ratio);
        ALWAYS_INLINE void set_aspect_ratio(real p_width, real p_height);
        ALWAYS_INLINE void set_near_dist(real p_near);
        ALWAYS_INLINE void set_far_dist(real p_far);

        constexpr bool operator==(const Projection& rhs) const;
        constexpr bool operator!=(const Projection& rhs) const;

        constexpr bool IsEqualApprox(const Projection& rhs) const;
        constexpr bool IsNotEqualApprox(const Projection& rhs) const;

        Vector4 Project(const Vector4& v) const;

        std::string ToString() const;

        union {
            struct {
                Vector4 row0;
                Vector4 row1;
                Vector4 row2;
                Vector4 row3;
            };
            Matrix4x4 matrix;
        };

       private:
        // Store projection parameters (fov, aspect_ratio, near, far) directly to
        // avoid catastrophic cancellation when reconstructing them from the
        // projection matrix. This ensures numerical stability and eliminates
        // precision loss in inverse computations.
        real fov;
        real aspect_ratio;
        real near;
        real far;
    };

    Projection::Projection() : matrix(Matrix4x4()), fov(0.0_r), aspect_ratio(0.0_r), near(0.0_r), far(0.0_r) {}
    Projection::Projection(real p_fov, real p_aspect_ratio, real p_near, real p_far)
        : fov(p_fov), aspect_ratio(p_aspect_ratio), near(p_near), far(p_far) {
        const real fov_factor = 1.0_r / math::Tan(fov * 0.5_r);
        real near_minus_far = (near - far);
        if (math::IsEqualApprox(near_minus_far, 0.0_r)) {
            near_minus_far = math::REAL_MIN;
        }
        real inv_near_minus_far = 1.0_r / near_minus_far;
        matrix =
            Matrix4x4({fov_factor / aspect_ratio, 0.0_r, 0.0_r, 0.0_r}, {0.0_r, fov_factor, 0.0_r, 0.0_r},
                      {0.0_r, 0.0_r, -(near + far) * inv_near_minus_far, -(2.0_r * near * far) * inv_near_minus_far},
                      {0.0_r, 0.0_r, -1.0_r, 0.0_r});
    }
    Projection::Projection(real p_fov, real p_width, real p_height, real p_near, real p_far)
        : fov(p_fov), aspect_ratio(p_width / p_height), near(p_near), far(p_far) {
        const real fov_factor = 1.0_r / math::Tan(fov * 0.5_r);
        real near_minus_far = (near - far);
        if (math::IsEqualApprox(near_minus_far, 0.0_r)) {
            near_minus_far = math::REAL_MIN;
        }
        real inv_near_minus_far = 1.0_r / near_minus_far;
        matrix =
            Matrix4x4({fov_factor / aspect_ratio, 0.0_r, 0.0_r, 0.0_r}, {0.0_r, fov_factor, 0.0_r, 0.0_r},
                      {0.0_r, 0.0_r, -(near + far) * inv_near_minus_far, -(2.0_r * near * far) * inv_near_minus_far},
                      {0.0_r, 0.0_r, -1.0_r, 0.0_r});
    }
    Projection::Projection(const Matrix4x4& m) : matrix(m) {
        // Extract projection parameters from the matrix.
        // Use double precision internally to minimize cancellation and rounding
        // errors.

        const double m00 = static_cast<double>(m.data[0][0]);
        const double m11 = static_cast<double>(m.data[1][1]);
        const double m22 = static_cast<double>(m.data[2][2]);
        const double m23 = static_cast<double>(m.data[2][3]);

        // If matrix is invalid or uninitialized, fallback to zero projection.
        if (math::IsZeroApprox(static_cast<real>(m00))) {
            fov = 0.0_r;
            aspect_ratio = 0.0_r;
            near = 0.0_r;
            far = 0.0_r;
            return;
        }

        const double fov_factor = m11;
        const double fov_rad = 2.0 * std::atan(1.0 / fov_factor);
        fov = static_cast<real>(fov_rad);

        aspect_ratio = static_cast<real>(fov_factor / m00);

        const double near_d = m23 / (m22 + 1.0);
        const double far_d = m23 / (m22 - 1.0);

        near = static_cast<real>(near_d);
        far = static_cast<real>(far_d);
    }

    Projection& Projection::operator=(const Projection& rhs) {
        if (this == &rhs) return *this;
        matrix = rhs.matrix;
        fov = rhs.fov;
        aspect_ratio = rhs.aspect_ratio;
        near = rhs.near;
        far = rhs.far;
        return *this;
    }

    real Projection::get_fov() const { return fov; }
    real Projection::get_aspect_ratio() const { return aspect_ratio; }
    real Projection::get_far_dist() const { return far; }

    real Projection::get_near_dist() const { return near; }

    void Projection::set_fov(real p_fov) {
        fov = p_fov;
        const real fov_factor = 1.0_r / math::Tan(fov * 0.5_r);
        const real aspect = get_aspect_ratio();
        matrix.data[0][0] = fov_factor / aspect;
        matrix.data[1][1] = fov_factor;
    }
    void Projection::set_aspect_ratio(real p_ratio) {
        const real prev_aspect = aspect_ratio;
        aspect_ratio = p_ratio;
        matrix.data[0][0] = (matrix.data[0][0] * prev_aspect) / aspect_ratio;
    }
    void Projection::set_aspect_ratio(real p_width, real p_height) {
        const real prev_aspect = get_aspect_ratio();
        aspect_ratio = p_width / p_height;
        matrix.data[0][0] = (matrix.data[0][0] * prev_aspect) / aspect_ratio;
    }
    void Projection::set_near_dist(real p_near) {
        near = p_near;
        const real f = get_far_dist();
        // Compute using double precision to minimize accumulated rounding error.
        const double nd = static_cast<double>(near);
        const double fd = static_cast<double>(f);
        const double inv_nf = 1.0 / (nd - fd);

        matrix.data[2][2] = static_cast<real>(-(nd + fd) * inv_nf);
        matrix.data[2][3] = static_cast<real>(-(2.0 * nd * fd) * inv_nf);
    }

    void Projection::set_far_dist(real p_far) {
        far = p_far;
        const real n = get_near_dist();
        // Use double precision for consistent accuracy with set_near_dist().
        const double nd = static_cast<double>(n);
        const double fd = static_cast<double>(far);
        const double inv_nf = 1.0 / (nd - fd);

        matrix.data[2][2] = static_cast<real>(-(nd + fd) * inv_nf);
        matrix.data[2][3] = static_cast<real>(-(2.0 * nd * fd) * inv_nf);
    }

    constexpr bool Projection::operator==(const Projection& rhs) const {
        return matrix == rhs.matrix && fov == rhs.fov && aspect_ratio == rhs.aspect_ratio && near == rhs.near &&
               far == rhs.far;
    }
    constexpr bool Projection::operator!=(const Projection& rhs) const { return !(*this == rhs); }

    constexpr bool Projection::IsEqualApprox(const Projection& rhs) const {
        return matrix.IsEqualApprox(rhs.matrix) && math::IsEqualApprox(fov, rhs.fov) &&
               math::IsEqualApprox(aspect_ratio, rhs.aspect_ratio) && math::IsEqualApprox(near, rhs.near) &&
               math::IsEqualApprox(far, rhs.far);
    }
    constexpr bool Projection::IsNotEqualApprox(const Projection& rhs) const { return !IsEqualApprox(rhs); }

    Vector4 Projection::Project(const Vector4& v) const {
        return Vector4(matrix.data[0][0] * v.x, matrix.data[1][1] * v.y,
                       matrix.data[2][2] * v.z + matrix.data[2][3] * v.w, -v.z);
    }

    ALWAYS_INLINE constexpr Matrix4x4 operator*(const Projection& p, const Transform3D& t) {
        return p.matrix * t.ToMatrix();
    }

}  // namespace ho