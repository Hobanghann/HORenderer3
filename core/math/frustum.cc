#include "frustum.h"

#include <vector>

#include "aabb.h"
#include "matrix4x4.h"
#include "sphere.h"
#include "vector3.h"

namespace ho {

    Frustum Frustum::FromMatrix4x4(const Matrix4x4& m) {
        Vector4 coefficients = -(m.row3 + m.row0);
        Plane left = Plane(coefficients.x, coefficients.y, coefficients.z, coefficients.w);

        coefficients = -(m.row3 - m.row0);
        Plane right = Plane(coefficients.x, coefficients.y, coefficients.z, coefficients.w);

        coefficients = -(m.row3 + m.row1);
        Plane bottom = Plane(coefficients.x, coefficients.y, coefficients.z, coefficients.w);

        coefficients = -(m.row3 - m.row1);
        Plane top = Plane(coefficients.x, coefficients.y, coefficients.z, coefficients.w);

        coefficients = -(m.row3 + m.row2);
        Plane near = Plane(coefficients.x, coefficients.y, coefficients.z, coefficients.w);

        coefficients = -(m.row3 - m.row2);
        Plane far = Plane(coefficients.x, coefficients.y, coefficients.z, coefficients.w);
        return Frustum(left, right, bottom, top, near, far);
    }
    Frustum::Frustum() {
        planes[PLANE_POS_LEFT] = Plane();
        planes[PLANE_POS_RIGHT] = Plane();
        planes[PLANE_POS_BOTTOM] = Plane();
        planes[PLANE_POS_TOP] = Plane();
        planes[PLANE_POS_NEAR] = Plane();
        planes[PLANE_POS_FAR] = Plane();
    }
    Frustum::Frustum(const Frustum& frustum) {
        planes[PLANE_POS_LEFT] = frustum.planes[PLANE_POS_LEFT];
        planes[PLANE_POS_RIGHT] = frustum.planes[PLANE_POS_RIGHT];
        planes[PLANE_POS_BOTTOM] = frustum.planes[PLANE_POS_BOTTOM];
        planes[PLANE_POS_TOP] = frustum.planes[PLANE_POS_TOP];
        planes[PLANE_POS_NEAR] = frustum.planes[PLANE_POS_NEAR];
        planes[PLANE_POS_FAR] = frustum.planes[PLANE_POS_FAR];
    }
    Frustum::Frustum(const Plane& left, const Plane& right, const Plane& bottom, const Plane& top, const Plane& near,
                     const Plane& far) {
        planes[PLANE_POS_LEFT] = left;
        planes[PLANE_POS_RIGHT] = right;
        planes[PLANE_POS_BOTTOM] = bottom;
        planes[PLANE_POS_TOP] = top;
        planes[PLANE_POS_NEAR] = near;
        planes[PLANE_POS_FAR] = far;
    }

    Frustum& Frustum::operator=(const Frustum& rhs) {
        if (this == &rhs) {
            return *this;
        }
        planes[PLANE_POS_LEFT] = rhs.planes[PLANE_POS_LEFT];
        planes[PLANE_POS_RIGHT] = rhs.planes[PLANE_POS_RIGHT];
        planes[PLANE_POS_BOTTOM] = rhs.planes[PLANE_POS_BOTTOM];
        planes[PLANE_POS_TOP] = rhs.planes[PLANE_POS_TOP];
        planes[PLANE_POS_NEAR] = rhs.planes[PLANE_POS_NEAR];
        planes[PLANE_POS_FAR] = rhs.planes[PLANE_POS_FAR];
        return *this;
    }

    bool Frustum::operator==(const Frustum& rhs) const {
        for (int i = 0; i < PLANE_POS_LAST; i++) {
            if (planes[i] != rhs.planes[i]) {
                return false;
            }
        }
        return true;
    }
    bool Frustum::operator!=(const Frustum& rhs) const { return !(*this == rhs); }

    bool Frustum::IsEqualApprox(const Frustum& rhs) const {
        for (int i = 0; i < PLANE_POS_LAST; i++) {
            if (!planes[i].IsEqualApprox(rhs.planes[i])) {
                return false;
            }
        }
        return true;
    }
    bool Frustum::IsNotEqualApprox(const Frustum& rhs) const { return !IsEqualApprox(rhs); }

    math::Side Frustum::GetSide(const Vector3& p) const {
        for (const Plane& plane : planes) {
            if (plane.GetSide(p) == math::OUTSIDE) {
                return math::OUTSIDE;
            }
        }
        return math::INSIDE;
    }
    math::Side Frustum::GetSide(const Vector3& p1, const Vector3& p2) const {
        int num_outside = 0;
        if (GetSide(p1) == math::OUTSIDE) {
            num_outside++;
        }
        if (GetSide(p2) == math::OUTSIDE) {
            num_outside++;
        }
        if (num_outside == 2) {
            return math::OUTSIDE;
        }
        if (num_outside == 0) {
            return math::INSIDE;
        }
        return math::INTERSECT;
    }
    math::Side Frustum::GetSide(const Vector3& p1, const Vector3& p2, const Vector3& p3) const {
        int num_outside = 0;
        if (GetSide(p1) == math::OUTSIDE) {
            num_outside++;
        }
        if (GetSide(p2) == math::OUTSIDE) {
            num_outside++;
        }
        if (GetSide(p3) == math::OUTSIDE) {
            num_outside++;
        }
        if (num_outside == 3) {
            return math::OUTSIDE;
        }
        if (num_outside == 0) {
            return math::INSIDE;
        }
        return math::INTERSECT;
    }
    math::Side Frustum::GetSide(const AABB& aabb) const {
        for (const Plane& plane : planes) {
            math::Side side = plane.GetSide(aabb);
            if (side == math::OUTSIDE) {
                return math::OUTSIDE;
            }
            if (side == math::INTERSECT) {
                return math::INTERSECT;
            }
        }
        return math::INSIDE;
    }
    math::Side Frustum::GetSide(const Sphere& sphere) const {
        for (const Plane& plane : planes) {
            math::Side side = plane.GetSide(sphere);
            if (side == math::OUTSIDE) {
                return math::OUTSIDE;
            }
            if (side == math::INTERSECT) {
                return math::INTERSECT;
            }
        }
        return math::INSIDE;
    }

    std::string Frustum::ToString() const {
        char buf[300];
        snprintf(buf, sizeof(buf), "Left: %s\nRight: %s\nTop: %s\nBottom: %s\nNear: %s\nFar: %s",
                 planes[PLANE_POS_LEFT].ToString().c_str(), planes[PLANE_POS_RIGHT].ToString().c_str(),
                 planes[PLANE_POS_TOP].ToString().c_str(), planes[PLANE_POS_BOTTOM].ToString().c_str(),
                 planes[PLANE_POS_NEAR].ToString().c_str(), planes[PLANE_POS_FAR].ToString().c_str());
        return buf;
    }
}  // namespace ho