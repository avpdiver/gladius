#ifndef GLADIUS_QUAT_H
#define GLADIUS_QUAT_H

#include "vec3.h"
#include "mat3.h"
#include "mat4.h"

namespace gladius
{
    namespace math
    {
        template<typename T>
        class quat
        {
        public:
            T x;
            T y;
            T z;
            T w;

        public:
            quat();
            quat(T x, T y, T z, T w);
            quat(const T v[4]);
            quat(const quat &v);
            quat(const vec3<T> &axis, T angle);
            quat(const vec3<T> &to, const vec3<T> &from);

        public:
            quat operator*(const quat &a) const;
            vec3<T> operator*(const vec3<T> &a) const;

        public:
            quat &operator*=(const quat &a);

        public:
            quat inverse() const;
            T length() const;
            quat normalize() const;

        public:
            mat3<T> toMat3() const;
            mat4<T> toMat4() const;

        public:
            static const quat ONE {0, 0, 0, 1};
        };

        typedef quat<float> quatf;
        typedef quat<double> quatd;


        template<typename T>
        inline quat<T>::quat()
        {
        }

        template<typename T>
        inline quat<T>::quat(T xi, T yi, T zi, T wi) : x(xi), y(yi), z(zi), w(wi)
        {
        }

        template<typename T>
        inline quat<T>::quat(const T v[4]) : x(v[0]), y(v[1]), z(v[2]), w(v[3])
        {
        }

        template<typename T>
        inline quat<T>::quat(const quat &v) : x(v.x), y(v.y), z(v.z), w(v.w)
        {
        }

        template<typename T>
        inline quat<T>::quat(const vec3<T> &axis, T angle)
        {
            vec3<T> axisN = axis.normalize();
            T       a     = angle * 0.5f;
            T       sina  = sin(a);
            T       cosa  = cos(a);
            x = axisN.x * sina;
            y = axisN.y * sina;
            z = axisN.z * sina;
            w = cosa;
        }

        template<typename T>
        quat<T>::quat(const vec3<T> &to, const vec3<T> &from)
        {
            vec3<T> f = from.normalize();
            vec3<T> t = to.normalize();

            T dotProdPlus1 = 1.0 + f.dot(t);

            if (dotProdPlus1 < (T) 1e-7)
            {
                w = 0;
                if (fabs(f.x) < 0.6)
                {
                    double norm = sqrt(1 - f.x * f.x);
                    x           = 0;
                    y           = f.z / norm;
                    z           = -f.y / norm;
                }
                else if (fabs(f.y) < 0.6)
                {
                    double norm = sqrt(1 - f.y * f.y);
                    x = -f.z / norm;
                    y = 0;
                    z = f.x / norm;
                }
                else
                {
                    double norm = sqrt(1 - f.z * f.z);
                    x = f.y / norm;
                    y = -f.x / norm;
                    z = 0;
                }
            }
            else
            {
                T       s   = (T) sqrt(0.5 * dotProdPlus1);
                vec3<T> tmp = (f.cross(t)) / (2.0 * s);
                x = tmp.x;
                y = tmp.y;
                z = tmp.z;
                w = s;
            }
        }

        template<typename T>
        quat<T> quat<T>::operator*(const quat &a) const
        {
            return quat(a.w * x + a.x * w + a.y * z - a.z * y, a.w * y - a.x * z + a.y * w + a.z * x, a.w * z + a.x * y - a.y * x + a.z * w, a.w * w - a.x * x - a.y * y - a.z * z);
        }

        template<typename T>
        quat<T> &quat<T>::operator*=(const quat &a)
        {
            *this = *this * a;
            return *this;
        }

        template<typename T>
        vec3<T> quat<T>::operator*(const vec3<T> &a) const
        {
            // TODO implement faster, direct multiplication
            return toMat3() * a;
        }

        template<typename T>
        quat<T> quat<T>::inverse() const
        {
            return quat(-x, -y, -z, w);
        }

        template<typename T>
        T quat<T>::length() const
        {
            T len = x * x + y * y + z * z + w * w;
            return static_cast<T>(sqrt(len));
        }

        template<typename T>
        quat<T> quat<T>::normalize() const
        {
            T invLength = 1 / length();
            return quat(x * invLength, y * invLength, z * invLength, w * invLength);
        }

        template<typename T>
        mat3<T> quat<T>::toMat3() const
        {
            T xx = x * x, xy = x * y, xz = x * z, xw = x * w, yy = y * y, yz = y * z, yw = y * w, zz = z * z, zw = z * w;
            return mat3<T>(1 - 2 * (yy + zz), 2 * (xy - zw), 2 * (xz + yw), 2 * (xy + zw), 1 - 2 * (xx + zz), 2 * (yz - xw), 2 * (xz - yw), 2 * (yz + xw), 1 - 2 * (xx + yy));
        }

        template<typename T>
        mat4<T> quat<T>::toMat4() const
        {
            return mat4<T>(toMat3());
        }

        template<typename T>
        quat<T> slerp(const quat<T> &from, const quat<T> &to, T t)
        {
            if (t <= 0)
            {
                return quat<T>(from);
            }
            else if (t >= 1)
            {
                return quat<T>(to);
            }
            else
            {
                T cosom     = from.x * to.x + from.y * to.y + from.z * to.z + from.w * to.w;
                T absCosom  = fabs(cosom);

                T       scale0;
                T       scale1;

                if ((1 - absCosom) > 1e-6f)
                {
                    T omega = safe_acos(absCosom);
                    T sinom = 1 / sin(omega);
                    scale0 = sin((1 - t) * omega) * sinom;
                    scale1 = sin(t * omega) * sinom;
                }
                else
                {
                    scale0 = 1 - t;
                    scale1 = t;
                }
                quat<T> res = quat<T>(scale0 * from.x + scale1 * to.x, scale0 * from.y + scale1 * to.y, scale0 * from.z + scale1 * to.z, scale0 * from.w + scale1 * to.w);
                return res.normalize();
            }
        }

    }
}

#endif 
