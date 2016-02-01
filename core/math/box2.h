#ifndef GLADIUS_BOX2_H
#define GLADIUS_BOX2_H

#include <algorithm>

#include "vec2.h"


namespace gladius
{
    namespace math
    {
        template<typename T>
        struct box2
        {
        public:
            T xmin;
            T xmax;
            T ymin;
            T ymax;

        public:
            box2() : xmin(INFINITY), xmax(-INFINITY), ymin(INFINITY), ymax(-INFINITY)
            {
            }

            box2(T xmin, T xmax, T ymin, T ymax) : xmin(xmin), xmax(xmax), ymin(ymin), ymax(ymax)
            {
            }

            box2(const vec2<T> &p, const vec2<T> &q) : xmin(std::min(p.x, q.x)), xmax(std::max(p.x, q.x)), ymin(std::min(p.y, q.y)), ymax(std::max(p.y, q.y))
            {
            }

        public:
            vec2<T> center() const
            {
                return vec2<T>((xmin + xmax) / 2, (ymin + ymax) / 2);
            }

            T width() const
            {
                return xmax - xmin;
            }

            T height() const
            {
                return ymax - ymin;
            }

            T area() const
            {
                return (xmax - xmin) * (ymax - ymin);
            }

        public:
            box2 enlarge(T w) const
            {
                return box2(xmin - w, xmax + w, ymin - w, ymax + w);
            }

            box2 enlarge(const vec2<T> &p) const
            {
                return box2(std::min(xmin, p.x), std::max(xmax, p.x), std::min(ymin, p.y), std::max(ymax, p.y));
            }

            box2 enlarge(const box2 &r) const
            {
                return box2(std::min(xmin, r.xmin), std::max(xmax, r.xmax), std::min(ymin, r.ymin), std::max(ymax, r.ymax));
            }

        public:
            bool contains(const vec2<T> &p) const
            {
                return p.x >= xmin && p.x <= xmax && p.y >= ymin && p.y <= ymax;
            }

            bool contains(const box2<T> &bb) const
            {
                return bb.xmin >= xmin && bb.xmax <= xmax && bb.ymin >= ymin && bb.ymax <= ymax;
            }

            bool intersects(const box2 &a) const
            {
                return a.xmax >= xmin && a.xmin <= xmax && a.ymax >= ymin && a.ymin <= ymax;
            }

        public:
            vec2<T> nearestInnerPoint(vec2<T> a) const
            {
                vec2<T> nearest = a;
                if (a.x < xmin)
                {
                    nearest.x = xmin;
                }
                else if (a.x > xmax)
                {
                    nearest.x = xmax;
                }

                if (a.y < ymin)
                {
                    nearest.y = ymin;
                }
                else if (a.y > ymax)
                {
                    nearest.y = ymax;
                }
                return nearest;
            }

            float distanceTo(vec2<T> a) const
            {
                return nearestInnerPoint(a).distanceTo(a);
            }

            float squaredDistanceTo(vec2<T> a) const
            {
                return (nearestInnerPoint(a)).squaredDistanceTo(a);
            }

        public:
            bool operator==(const box2 &b) const
            {
                return xmin == b.xmin && xmax == b.xmax && ymin == b.ymin && ymax == b.max;
            }

            bool operator!=(const box2 &b) const
            {
                return xmin != b.xmin || xmax != b.xmax || ymin != b.ymin || ymax != b.ymax;
            }
        };

        typedef box2<int> box2i;
        typedef box2<float> box2f;
        typedef box2<double> box2d;
    }
}

#endif