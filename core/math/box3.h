#ifndef GLADIUS_BOX3_H
#define GLADIUS_BOX3_H

#include <algorithm>

#include "vec3.h"

namespace gladius
{
    namespace math
    {
        template<typename T>
        struct box3
        {
            T xmin;
            T xmax;
            T ymin;

            T ymax;
            T zmin;
            T zmax;

        public:
            box3() : xmin(INFINITY), xmax(-INFINITY), ymin(INFINITY), ymax(-INFINITY), zmin(INFINITY), zmax(-INFINITY)
            {
            }

            box3(T xmin, T xmax, T ymin, T ymax, T zmin, T zmax) : xmin(xmin), xmax(xmax), ymin(ymin), ymax(ymax), zmin(zmin), zmax(zmax)
            {
            }

            box3(const vec3<T> &p, const vec3<T> &q) : xmin(std::min(p.x, q.x)), xmax(std::max(p.x, q.x)), ymin(std::min(p.y, q.y)), ymax(std::max(p.y, q.y)), zmin(std::min(p.z, q.z)), zmax(std::max(p.z, q.z))
            {
            }

        public:
            vec3<T> center() const
            {
                return vec3<T>((xmin + xmax) / 2, (ymin + ymax) / 2, (zmin + zmax) / 2);
            }

        public:
            box3<T> enlarge(const vec3<T> &p) const
            {
                return box3<T>(std::min(xmin, p.x), std::max(xmax, p.x), std::min(ymin, p.y), std::max(ymax, p.y), std::min(zmin, p.z), std::max(zmax, p.z));
            }

            box3<T> enlarge(const box3<T> &r) const
            {
                return box3<T>(std::min(xmin, r.xmin), std::max(xmax, r.xmax), std::min(ymin, r.ymin), std::max(ymax, r.ymax), std::min(zmin, r.zmin), std::max(zmax, r.zmax));
            }

        public:
            bool contains(const vec3<T> &p) const
            {
                return p.x >= xmin && p.x <= xmax && p.y >= ymin && p.y <= ymax && p.z >= zmin && p.z <= zmax;
            }

        public:
            template<class t>
            box3<t> cast()
            {
                return box3<t>(xmin, xmax, ymin, ymax, zmin, zmax);
            }
        };

        typedef box3<int> box3i;
        typedef box3<float> box3f;
        typedef box3<double> box3d;
    }
}

#endif