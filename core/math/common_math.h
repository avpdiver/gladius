//
// Created by alexa on 15.11.2015.
//

#ifndef GLADIUS_COMMON_MATH_H
#define GLADIUS_COMMON_MATH_H

#undef max
#undef min

#include <limits>
#include <cmath>

#define LARGE_SCALAR std::numeric_limits<T>::max()
#define INFINITY_SCALAR std::numeric_limits<T>::infinity()
#define EPSILON_SCALAR std::numeric_limits<T>::epsilon()

#define SQRT12          0.7071067811865475244008443621048490
#define PI              3.1415926535897932384626433832795029
#define TWO_PI          2.0 * PI
#define HALF_PI         PI * 0.5

#define RADIANS(grad)   PI * grad / 180.0

namespace gladius
{
    namespace math
    {
        template <typename T>
        static inline T safe_acos(T x)
        {
            if (std::abs(x) <= 1.0)
            {
                return std::acos(x);
            }
            return ((x > 0) ? 0 : PI);
        }
    }
}

#endif //GLADIUS_COMMON_MATH_H
