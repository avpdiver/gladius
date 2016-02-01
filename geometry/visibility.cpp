//
// Created by alexa on 05.12.2015.
//

#include "visibility.h"
#include "../core/math/vec4.h"
#include "../core/math/box3.h"
#include "../core/math/mat4.h"

namespace gladius
{
    namespace geometry
    {

        e_visibility get_visibility(const vec4d &clip, const box3d &b)
        {
            double x0 = b.xmin * clip.x;
            double x1 = b.xmax * clip.x;
            double y0 = b.ymin * clip.y;
            double y1 = b.ymax * clip.y;
            double z0 = b.zmin * clip.z + clip.w;
            double z1 = b.zmax * clip.z + clip.w;
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

        e_visibility get_visibility(const vec4d frustumPlanes[6], const box3d &b)
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

        void get_frustum_planes(const mat4d &toScreen, vec4d frustumPlanes[6])
        {
            const double *m = toScreen.coefficients();

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
    }
}