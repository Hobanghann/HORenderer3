#include "matrix3x3.h"

#include <string>
#include <vector>

#include "math_funcs.h"
#include "quaternion.h"

namespace ho {
    Matrix3x3 Matrix3x3::FromAxisAngle(const Vector3& axis, real angle) {
        Vector3 n = axis.Normalized();
        real x = n.x, y = n.y, z = n.z;
        real s, c;
        math::SinCos(s, c, angle);
        real t = 1.0_r - c;

        Matrix3x3 m;
        m.data[0][0] = c + t * x * x;
        m.data[0][1] = t * x * y - s * z;
        m.data[0][2] = t * x * z + s * y;

        m.data[1][0] = t * x * y + s * z;
        m.data[1][1] = c + t * y * y;
        m.data[1][2] = t * y * z - s * x;

        m.data[2][0] = t * x * z - s * y;
        m.data[2][1] = t * y * z + s * x;
        m.data[2][2] = c + t * z * z;

        return m;
    }
    Matrix3x3 Matrix3x3::FromEuler(real x, real y, real z, math::EulerOrder order) {
        real sx, cx;
        math::SinCos(sx, cx, x);
        const Matrix3x3 mx = Matrix3x3({1.0_r, 0.0_r, 0.0_r}, {0.0_r, cx, -sx}, {0.0_r, sx, cx});
        real sy, cy;
        math::SinCos(sy, cy, y);
        const Matrix3x3 my = Matrix3x3({cy, 0.0_r, sy}, {0.0_r, 1.0_r, 0.0_r}, {-sy, 0.0_r, cy});
        real sz, cz;
        math::SinCos(sz, cz, z);
        const Matrix3x3 mz = {{cz, -sz, 0.0_r}, {sz, cz, 0.0_r}, {0.0_r, 0.0_r, 1.0_r}};

        Matrix3x3 m;
        switch (order) {
            case math::XYZ:
                m = mx * my * mz;
                break;
            case math::XZY:
                m = mx * mz * my;
                break;
            case math::YXZ:
                m = my * mx * mz;
                break;
            case math::YZX:
                m = my * mz * mx;
                break;
            case math::ZXY:
                m = mz * mx * my;
                break;
            case math::ZYX:
                m = mz * my * mx;
                break;
            default:
                m = Matrix3x3();
                break;
        }
        return m;
    }

    Matrix3x3& Matrix3x3::Orthogonalize() {
        const real l0 = col0().Magnitude();
        const real l1 = col1().Magnitude();
        const real l2 = col2().Magnitude();
        Orthonormalize();
        set_col0(col0() * l0);
        set_col1(col1() * l1);
        set_col2(col2() * l2);
        return *this;
    }

    Matrix3x3 Matrix3x3::Orthogonalized() const {
        Matrix3x3 copy = *this;
        return copy.Orthogonalize();
    }

    Matrix3x3& Matrix3x3::Orthonormalize() {
        Vector3 x = col0();
        Vector3 y = col1();
        Vector3 z = col2();

        x.Normalize();
        y -= x * (x.Dot(y));
        y.Normalize();
        z -= (x * (x.Dot(z)) + y * (y.Dot(z)));
        z.Normalize();

        set_col0(x);
        set_col1(y);
        set_col2(z);
        return *this;
    }

    Matrix3x3 Matrix3x3::Orthonormalized() const {
        Matrix3x3 copy = *this;
        return copy.Orthonormalize();
    }

    std::string Matrix3x3::ToString() const {
        char res[300];
        std::snprintf(res, sizeof(res), "| %.3f , %.3f , %.3f |\n| %.3f , %.3f , %.3f |\n| %.3f , %.3f , %.3f |",
                      static_cast<float>(row0.x), static_cast<float>(row0.y), static_cast<float>(row0.z),
                      static_cast<float>(row1.x), static_cast<float>(row1.y), static_cast<float>(row1.z),
                      static_cast<float>(row2.x), static_cast<float>(row2.y), static_cast<float>(row2.z));

        return res;
    }

}  // namespace ho
