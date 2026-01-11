#pragma once

#include <cmath>
#include <limits>

#include "core/macros.h"

#ifdef DOUBLE_PRECISION
typedef double real;
#else
typedef float real;
#endif

constexpr real operator""_r(long double v) { return static_cast<real>(v); }

constexpr real operator""_r(unsigned long long v) { return static_cast<real>(v); }

namespace ho {
    namespace math {
        inline constexpr real REAL_MAX = std::numeric_limits<real>::max();
        inline constexpr real REAL_MIN = std::numeric_limits<real>::min();
        inline constexpr real REAL_INF = std::numeric_limits<real>::infinity();
        inline constexpr real REAL_NaN = std::numeric_limits<real>::quiet_NaN();

        // Basic scalar/vector comparison epsilon.
        // Used for generic floating point equality checks where values are expected
        // to be of similar magnitude (not squared values).
        inline constexpr real EPSILON_CMP = 1e-5_r;

        // Squared epsilon for squared-length or squared-distance comparisons.
        // Avoids taking sqrt() when comparing vector lengths or distances.
        // This is simply EPSILON_CMP * EPSILON_CMP.
        inline constexpr real EPSILON_CMP2 = EPSILON_CMP * EPSILON_CMP;

        // Epsilon for values expected to be close to unit length or unit scale.
        // Example: checking if a normalized vector has length := 1,
        // or if a scale factor is := 1.0.
        // This value can be adjusted for stricter or looser unit checks.

        inline constexpr real EPSILON_UNIT = 1e-3_r;

        // Epsilon for determining whether a vector can be safely normalized.
        // If the vector's length is below this threshold, normalization should be
        // avoided to prevent division by very small numbers.
        inline constexpr real EPSILON_NORMALIZABLE = 1e-6_r;

        // Epsilon for checking if a point lies on a plane.
        // Used to compare the absolute signed distance from the point to the plane to
        // zero.
        inline constexpr real EPSILON_POINT_ON_PLANE = 1e-5_r;

        // Epsilon for checking if a matrix is orthogonal or orthonormal.
        inline constexpr real EPSILON_ORTHO = 1e-3_r;

        // Epsilon for rasterization comparisons.
        // Used where floating-point error accumulates during rasterization
        // (edge tests, barycentric coords, NDC/screen-space boundaries).
        // Looser tolerance to ensure stable pixel coverage near edges.
        inline constexpr real EPSILON_RASTERIZATION = 1e-4_r;

        inline constexpr real PI = 3.14159265358979323846_r;
        inline constexpr real PI2 = 1.57079632679489661923_r;
        inline constexpr real PI4 = 0.785398163397448309616_r;
        inline constexpr real INV_PI = 0.318309886183790671538_r;
        inline constexpr real DEG1_IN_RAD = PI / 180.0_r;
        inline constexpr real RAD1_IN_DEG = 1.0_r / DEG1_IN_RAD;

        enum ClockDirection : uint8_t { CLOCK_WISE, COUNTER_CLOCK_WISE };

        enum EulerOrder : uint8_t { XYZ = 0, XZY, YZX, YXZ, ZXY, ZYX };

        enum Side : uint8_t { OUTSIDE, INSIDE, ON, INTERSECT };
    }  // namespace math
}  // namespace ho