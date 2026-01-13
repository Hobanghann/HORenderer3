#pragma once

#include <string>

#include "matrix4x4.h"
#include "transform_3d.h"

namespace ho {
    struct Projection {
        ALWAYS_INLINE static Projection CreateOrthographic(real p_width, real p_height, real p_near, real p_far);
        ALWAYS_INLINE static Projection CreateOrthographic(const Matrix4x4& m);
        ALWAYS_INLINE static Projection CreatePerspective(real p_fov, real p_width, real p_height, real p_near,
                                                          real p_far);
        ALWAYS_INLINE static Projection CreatePerspective(const Matrix4x4& m);

        ALWAYS_INLINE Projection();
        ALWAYS_INLINE Projection(const Projection&) = default;
        ALWAYS_INLINE Projection& operator=(const Projection& rhs);
        ~Projection() = default;

        ALWAYS_INLINE real get_fov() const;
        ALWAYS_INLINE real get_aspect_ratio() const;
        ALWAYS_INLINE real get_near_dist() const;
        ALWAYS_INLINE real get_far_dist() const;

        ALWAYS_INLINE void set_fov(real p_fov);
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
        union {
            real fov;
            real height;
        };
        real aspect_ratio;
        real near;
        real far;

        bool is_orthographic;
    };

    Projection Projection::CreateOrthographic(real p_width, real p_height, real p_near, real p_far) {
        Projection proj;
        proj.is_orthographic = true;
        proj.height = p_height;
        proj.aspect_ratio = p_width / p_height;
        proj.near = p_near;
        proj.far = p_far;

        real range = p_far - p_near;

        if (math::IsEqualApprox(range, 0.0_r)) {
            range = math::REAL_MIN;
        }
        real inv_range = 1.0_r / range;

        proj.matrix =
            Matrix4x4({2.0_r / p_width, 0.0_r, 0.0_r, 0.0_r}, {0.0_r, 2.0_r / p_height, 0.0_r, 0.0_r},
                      {0.0_r, 0.0_r, -2.0_r * inv_range, -(p_far + p_near) * inv_range}, {0.0_r, 0.0_r, 0.0_r, 1.0_r});

        return proj;
    }
    Projection Projection::CreateOrthographic(const Matrix4x4& m) {
        Projection proj;
        proj.is_orthographic = true;
        proj.matrix = m;

        // Extract projection parameters from the matrix.
        // Use double precision internally to minimize cancellation and rounding
        // errors.
        const double m00 = static_cast<double>(m.data[0][0]);
        const double m11 = static_cast<double>(m.data[1][1]);
        const double m22 = static_cast<double>(m.data[2][2]);
        const double m23 = static_cast<double>(m.data[2][3]);

        // If matrix is invalid or uninitialized, fallback to zero projection.
        if (math::IsZeroApprox(static_cast<real>(m11)) || math::IsZeroApprox(static_cast<real>(m00))) {
            proj.height = 0.0_r;
            proj.aspect_ratio = 0.0_r;
            proj.near = 0.0_r;
            proj.far = 0.0_r;
            return proj;
        }

        proj.height = static_cast<real>(2.0 / m11);
        double width = 2.0 / m00;
        proj.aspect_ratio = static_cast<real>(width / proj.height);

        if (math::IsZeroApprox(static_cast<real>(m22))) {
            proj.near = 0.0_r;
            proj.far = 0.0_r;
        } else {
            proj.near = static_cast<real>((m23 + 1.0) / m22);
            proj.far = static_cast<real>((m23 - 1.0) / m22);
        }

        return proj;
    }
    Projection Projection::CreatePerspective(real p_fov, real p_width, real p_height, real p_near, real p_far) {
        Projection proj;
        proj.is_orthographic = false;
        proj.fov = p_fov;
        proj.aspect_ratio = p_width / p_height;
        proj.near = p_near;
        proj.far = p_far;
        const real fov_factor = 1.0_r / math::Tan(proj.fov * 0.5_r);
        real near_minus_far = (proj.near - proj.far);
        if (math::IsEqualApprox(near_minus_far, 0.0_r)) {
            near_minus_far = math::REAL_MIN;
        }
        real inv_near_minus_far = 1.0_r / near_minus_far;
        proj.matrix =
            Matrix4x4({fov_factor / proj.aspect_ratio, 0.0_r, 0.0_r, 0.0_r}, {0.0_r, fov_factor, 0.0_r, 0.0_r},
                      {0.0_r, 0.0_r, (proj.near + proj.far) * inv_near_minus_far,
                       (2.0_r * proj.near * proj.far) * inv_near_minus_far},
                      {0.0_r, 0.0_r, -1.0_r, 0.0_r});
        return proj;
    }
    Projection Projection::CreatePerspective(const Matrix4x4& m) {
        Projection proj;
        proj.is_orthographic = false;
        proj.matrix = m;

        // Extract projection parameters from the matrix.
        // Use double precision internally to minimize cancellation and rounding
        // errors.
        const double m00 = static_cast<double>(m.data[0][0]);
        const double m11 = static_cast<double>(m.data[1][1]);
        const double m22 = static_cast<double>(m.data[2][2]);
        const double m23 = static_cast<double>(m.data[2][3]);

        // If matrix is invalid or uninitialized, fallback to zero projection.
        if (math::IsZeroApprox(static_cast<real>(m00))) {
            proj.fov = 0.0_r;
            proj.aspect_ratio = 0.0_r;
            proj.near = 0.0_r;
            proj.far = 0.0_r;
            return proj;
        }

        const double fov_factor = m11;
        const double fov_rad = 2.0 * std::atan(1.0 / fov_factor);
        proj.fov = static_cast<real>(fov_rad);

        proj.aspect_ratio = static_cast<real>(fov_factor / m00);

        const double near_d = m23 / (m22 - 1.0);
        const double far_d = m23 / (m22 + 1.0);

        proj.near = static_cast<real>(near_d);
        proj.far = static_cast<real>(far_d);
        return proj;
    }

    Projection::Projection()
        : matrix(Matrix4x4()), fov(0.0_r), aspect_ratio(0.0_r), near(0.0_r), far(0.0_r), is_orthographic(false) {}

    Projection& Projection::operator=(const Projection& rhs) {
        if (this == &rhs) {
            return *this;
        }
        matrix = rhs.matrix;
        fov = rhs.fov;
        aspect_ratio = rhs.aspect_ratio;
        near = rhs.near;
        far = rhs.far;
        is_orthographic = rhs.is_orthographic;
        return *this;
    }

    real Projection::get_fov() const {
        if (is_orthographic) {
            return 0.0_r;
        }
        return fov;
    }
    real Projection::get_aspect_ratio() const { return aspect_ratio; }
    real Projection::get_far_dist() const { return far; }
    real Projection::get_near_dist() const { return near; }

    void Projection::set_fov(real p_fov) {
        if (!is_orthographic) {
            fov = p_fov;
            const real fov_factor = 1.0_r / math::Tan(fov * 0.5_r);
            const real aspect = get_aspect_ratio();
            matrix.data[0][0] = fov_factor / aspect;
            matrix.data[1][1] = fov_factor;
        }
    }
    void Projection::set_aspect_ratio(real p_width, real p_height) {
        aspect_ratio = p_width / p_height;

        if (is_orthographic) {
            real new_width = height * aspect_ratio;

            matrix.data[0][0] = 2.0_r / new_width;
            matrix.data[1][1] = 2.0_r / height;
        } else {
            matrix.data[0][0] = matrix.data[1][1] / aspect_ratio;
        }
    }
    void Projection::set_near_dist(real p_near) {
        near = p_near;
        const real f = get_far_dist();

        const double nd = static_cast<double>(near);
        const double fd = static_cast<double>(f);

        if (is_orthographic) {
            double range = fd - nd;
            if (range == 0.0) {
                range = 1e-10;
            }
            const double inv_range = 1.0 / range;

            matrix.data[2][2] = static_cast<real>(-2.0 * inv_range);
            matrix.data[2][3] = static_cast<real>(-(fd + nd) * inv_range);
        } else {
            const double inv_nf = 1.0 / (nd - fd);

            matrix.data[2][2] = static_cast<real>((nd + fd) * inv_nf);
            matrix.data[2][3] = static_cast<real>((2.0 * nd * fd) * inv_nf);
        }
    }

    void Projection::set_far_dist(real p_far) {
        far = p_far;
        const real n = get_near_dist();

        const double nd = static_cast<double>(n);
        const double fd = static_cast<double>(far);

        if (is_orthographic) {
            double range = fd - nd;
            if (range == 0.0) {
                range = 1e-10;
            }
            const double inv_range = 1.0 / range;

            matrix.data[2][2] = static_cast<real>(-2.0 * inv_range);
            matrix.data[2][3] = static_cast<real>(-(fd + nd) * inv_range);
        } else {
            const double inv_nf = 1.0 / (nd - fd);

            matrix.data[2][2] = static_cast<real>((nd + fd) * inv_nf);
            matrix.data[2][3] = static_cast<real>((2.0 * nd * fd) * inv_nf);
        }
    }

    constexpr bool Projection::operator==(const Projection& rhs) const {
        if (is_orthographic != rhs.is_orthographic) {
            return false;
        }
        return matrix == rhs.matrix && fov == rhs.fov && aspect_ratio == rhs.aspect_ratio && near == rhs.near &&
               far == rhs.far;
    }
    constexpr bool Projection::operator!=(const Projection& rhs) const { return !(*this == rhs); }

    constexpr bool Projection::IsEqualApprox(const Projection& rhs) const {
        if (is_orthographic != rhs.is_orthographic) {
            return false;
        }
        return matrix.IsEqualApprox(rhs.matrix) && math::IsEqualApprox(fov, rhs.fov) &&
               math::IsEqualApprox(aspect_ratio, rhs.aspect_ratio) && math::IsEqualApprox(near, rhs.near) &&
               math::IsEqualApprox(far, rhs.far);
    }
    constexpr bool Projection::IsNotEqualApprox(const Projection& rhs) const { return !IsEqualApprox(rhs); }

    ALWAYS_INLINE constexpr Matrix4x4 operator*(const Projection& p, const Transform3D& t) {
        return p.matrix * t.ToMatrix();
    }

}  // namespace ho