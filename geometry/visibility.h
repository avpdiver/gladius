//
// Created by alexa on 05.12.2015.
//

#ifndef GLADIUS_VISIBILITY_H
#define GLADIUS_VISIBILITY_H

#include "../core/math/box3.h"
#include "../core/math/vec4.h"
#include "../core/math/mat4.h"

using namespace gladius::math;

namespace gladius
{
    namespace geometry
    {
        enum class e_visibility
        {
            INVISIBLE,
            VISIBLE,
            PARTIALLY_VISIBLE
        };

        void get_frustum_planes(const mat4d &toScreen, vec4d frustumPlanes[6]);
        e_visibility get_visibility(const vec4d frustumPlanes[6], const box3d &b);
    }
}

#endif //GLADIUS_VISIBILITY_H
