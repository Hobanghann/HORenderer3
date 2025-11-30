#include "basis_3d.h"

#include <cassert>
#include <cstdio>

namespace ho {
    Basis3D Basis3D::FromAxisAngle(const Vector3& axis, real angle) {
        return Basis3D(Quaternion::FromAxisAngle(axis, angle));
    }

    Basis3D Basis3D::FromEuler(real x, real y, real z, math::EulerOrder order) {
        return Basis3D(Matrix3x3::FromEuler(x, y, z, order));
    }

    Basis3D& Basis3D::RotateEuler(real x_angle, real y_angle, real z_angle, math::EulerOrder order) {
        const Matrix3x3 rm = Matrix3x3::FromEuler(x_angle, y_angle, z_angle, order);
        matrix = rm * matrix;
        return *this;
    }
    Basis3D& Basis3D::RotateAxisAngle(const Vector3& axis, real angle) {
        const Quaternion rq = Quaternion::FromAxisAngle(axis, angle);
        RotateQuaternion(rq);
        return *this;
    }

    Basis3D Basis3D::RotatedEuler(real x_angle, real y_angle, real z_angle, math::EulerOrder order) const {
        Basis3D b = *this;
        return b.RotateEuler(x_angle, y_angle, z_angle, order);
    }
    Basis3D Basis3D::RotatedAxisAngle(const Vector3& axis, real angle) const {
        Basis3D b = *this;
        return b.RotateAxisAngle(axis, angle);
    }

    Basis3D& Basis3D::RotateEulerLocal(real x_angle, real y_angle, real z_angle, math::EulerOrder order) {
        const Matrix3x3 rm = Matrix3x3::FromEuler(x_angle, y_angle, z_angle, order);
        matrix = matrix * rm;
        return *this;
    }

    Basis3D& Basis3D::RotateAxisAngleLocal(const Vector3& axis, real angle) {
        const Quaternion rq = Quaternion::FromAxisAngle(axis, angle);
        RotateQuaternionLocal(rq);
        return *this;
    }

    Basis3D Basis3D::RotatedEulerLocal(real x_angle, real y_angle, real z_angle, math::EulerOrder order) const {
        Basis3D b = *this;
        return b.RotateEulerLocal(x_angle, y_angle, z_angle, order);
    }
    Basis3D Basis3D::RotatedAxisAngleLocal(const Vector3& axis, real angle) const {
        Basis3D b = *this;
        return b.RotateAxisAngleLocal(axis, angle);
    }
    Basis3D& Basis3D::LookAt(const Vector3& at, const Vector3& up, bool is_facing_target) {
        if (at.IsEqualApprox(Vector3::ZERO)) {
            return *this;
        }

        const real lx = basis_x().Magnitude();
        const real ly = basis_y().Magnitude();
        const real lz = basis_z().Magnitude();

        Vector3 forward = at.Normalized();
        if (!is_facing_target) {
            forward = -forward;
        }

        Vector3 right = up.Cross(forward);
        if (right.IsEqualApprox(Vector3::ZERO)) {
            if (math::Abs(up.x) <= math::Abs(up.y) && math::Abs(up.x) <= math::Abs(up.z)) {
                right = up.Cross(Vector3(1.0_r, 0.0_r, 0.0_r));
            } else {
                right = up.Cross(Vector3(0.0_r, 1.0_r, 0.0_r));
            }
        }
        right.Normalize();

        Vector3 new_up = forward.Cross(right);

        set_basis_x(right * lx);
        set_basis_y(new_up * ly);
        set_basis_z(forward * lz);
        return *this;
    }

    Basis3D Basis3D::LookedAt(const Vector3& target, const Vector3& up, bool is_facing_target) const {
        Basis3D b = *this;
        return b.LookAt(target, up, is_facing_target);
    }

    std::string Basis3D::ToString() const {
        char buf[200];
        snprintf(buf, sizeof(buf), "Basis3D X: %s, Y: %s, Z: %s", basis_x().ToString().c_str(),
                 basis_y().ToString().c_str(), basis_z().ToString().c_str());
        return buf;
    }
}  // namespace ho