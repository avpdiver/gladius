//
// Created by alexa on 05.12.2015.
//

#ifndef GLADIUS_VISIBILITY_H
#define GLADIUS_VISIBILITY_H

#include "glm/vec4.hpp"
#include "glm/mat4x4.hpp"
#include "../core/math/aabb.h"

using namespace glm;

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

        template <typename T> void get_frustum_planes(const tmat4x4<T> &m, tvec4<T> frustumPlanes[6])
        {
            // Extract the LEFT plane
            frustumPlanes[0].x = m[12] + m[0];
            frustumPlanes[0].y = m[13] + m[1];
            frustumPlanes[0].z = m[14] + m[2];
            frustumPlanes[0].w = m[15] + m[3];

            // Extract the RIGHT plane
            frustumPlanes[1].x = m[12] - m[0];
            frustumPlanes[1].y = m[13] - m[1];
            frustumPlanes[1].z = m[14] - m[2];
            frustumPlanes[1].w = m[15] - m[3];

            // Extract the BOTTOM plane
            frustumPlanes[2].x = m[12] + m[4];
            frustumPlanes[2].y = m[13] + m[5];
            frustumPlanes[2].z = m[14] + m[6];
            frustumPlanes[2].w = m[15] + m[7];

            // Extract the TOP plane
            frustumPlanes[3].x = m[12] - m[4];
            frustumPlanes[3].y = m[13] - m[5];
            frustumPlanes[3].z = m[14] - m[6];
            frustumPlanes[3].w = m[15] - m[7];

            // Extract the NEAR plane
            frustumPlanes[4].x = m[12] + m[8];
            frustumPlanes[4].y = m[13] + m[9];
            frustumPlanes[4].z = m[14] + m[10];
            frustumPlanes[4].w = m[15] + m[11];

            // Extract the FAR plane
            frustumPlanes[5].x = m[12] - m[8];
            frustumPlanes[5].y = m[13] - m[9];
            frustumPlanes[5].z = m[14] - m[10];
            frustumPlanes[5].w = m[15] - m[11];
        }


        template <typename T> e_visibility get_visibility(const tvec4<T> &clip, const math::aabb<T> &b)
        {
            double x0 = b.m_min.x * clip.x;
            double x1 = b.m_max.x * clip.x;
            double y0 = b.m_min.y * clip.y;
            double y1 = b.m_max.y * clip.y;
            double z0 = b.m_min.z * clip.z + clip.w;
            double z1 = b.m_max.z * clip.z + clip.w;
            double p1 = x0 + y0 + z0;
            double p2 = x1 + y0 + z0;
            double p3 = x1 + y1 + z0;
            double p4 = x0 + y1 + z0;
            double p5 = x0 + y0 + z1;
            double p6 = x1 + y0 + z1;
            double p7 = x1 + y1 + z1;
            double p8 = x0 + y1 + z1;

            if (p1 <= 0 && p2 <= 0 && p3 <= 0 && p4 <= 0 && p5 <= 0 && p6 <= 0 && p7 <= 0 && p8 <= 0) {
                return e_visibility::INVISIBLE;
            }

            if (p1 > 0 && p2 > 0 && p3 > 0 && p4 > 0 && p5 > 0 && p6 > 0 && p7 > 0 && p8 > 0) {
                return e_visibility::VISIBLE;
            }

            return e_visibility::PARTIALLY_VISIBLE;
        }

        template <typename T> e_visibility get_visibility(const tvec4<T> frustumPlanes[6], const math::aabb<T> &b)
        {
            e_visibility v0 = get_visibility(frustumPlanes[0], b);
            if (v0 == e_visibility::INVISIBLE) {
                return e_visibility::INVISIBLE;
            }

            e_visibility v1 = get_visibility(frustumPlanes[1], b);
            if (v1 == e_visibility::INVISIBLE) {
                return e_visibility::INVISIBLE;
            }

            e_visibility v2 = get_visibility(frustumPlanes[2], b);
            if (v2 == e_visibility::INVISIBLE) {
                return e_visibility::INVISIBLE;
            }

            e_visibility v3 = get_visibility(frustumPlanes[3], b);
            if (v3 == e_visibility::INVISIBLE) {
                return e_visibility::INVISIBLE;
            }

            e_visibility v4 = get_visibility(frustumPlanes[4], b);
            if (v4 == e_visibility::INVISIBLE) {
                return e_visibility::INVISIBLE;
            }

            if (v0 == e_visibility::VISIBLE && v1 == e_visibility::VISIBLE &&
                    v2 == e_visibility::VISIBLE && v3 == e_visibility::VISIBLE &&
                    v4 == e_visibility::VISIBLE)
            {
                return e_visibility::VISIBLE;
            }

            return e_visibility::PARTIALLY_VISIBLE;
        }
    }
}

#endif //GLADIUS_VISIBILITY_H
