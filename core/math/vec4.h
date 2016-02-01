#ifndef GLADIUS_VEC4_H
#define GLADIUS_VEC4_H

#include "vec3.h"


namespace gladius
{
    namespace math
    {
        template<typename T>
        class vec4
        {
        public:
            T x;
            T y;
            T z;
            T w;

        public:
            vec4();
            vec4(T xi, T yi, T zi, T wi);
            vec4(const T v[4]);
            vec4(const vec4 &v);
            vec4(const vec3<T> &v);
            vec4(const vec3<T> &v, const T wi);

        public:
            T operator[](const int i) const;
            T &operator[](const int i);

        public:
            vec4 operator=(const vec4 &v);
            bool operator==(const vec4 &v) const;
            bool operator!=(const vec4 &v) const;
            bool operator<(const vec4 &v) const;

        public:
            vec4 operator+(const vec4 &v) const;
            vec4 operator-(const vec4 &v) const;
            vec4 operator*(const vec4 &v) const;
            vec4 operator*(const T scalar) const;
            vec4 operator/(const vec4 &v) const;
            vec4 operator/(const T scalar) const;
            vec4 operator-() const;

        public:
            vec4 &operator+=(const vec4 &v);
            vec4 &operator-=(const vec4 &v);
            vec4 &operator*=(const T scalar);
            vec4 &operator/=(const T scalar);

        public:
            T dot(const vec3<T> &v) const;
            T dot(const vec4 &v) const;

        public:
            vec3<T> xyz() const;
            vec2<T> xy() const;

            template<class t>
            vec4<t> cast() const
            {
                return vec4<t>((t) x, (t) y, (t) z, (t) w);
            }

        public:
            static const vec4 ZERO;
            static const vec4 UNIT_X;
            static const vec4 UNIT_Y;
            static const vec4 UNIT_Z;
            static const vec4 UNIT_W;
        };

        template<typename T> const vec4<T> vec4<T>::ZERO {0 ,0, 0, 0};
        template<typename T> const vec4<T> vec4<T>::UNIT_X {1 ,0, 0, 0};
        template<typename T> const vec4<T> vec4<T>::UNIT_Y {0 ,1, 0, 0};
        template<typename T> const vec4<T> vec4<T>::UNIT_Z {0 ,0, 1, 0};
        template<typename T> const vec4<T> vec4<T>::UNIT_W {0 ,0, 0, 1};

        typedef vec4<half> vec4h;
        typedef vec4<float> vec4f;
        typedef vec4<double> vec4d;
        typedef vec4<int> vec4i;

        template<typename T>
        inline vec4<T>::vec4()
        {
        }

        template<typename T>
        inline vec4<T>::vec4(T xi, T yi, T zi, T wi) : x(xi), y(yi), z(zi), w(wi)
        {
        }

        template<typename T>
        inline vec4<T>::vec4(const T v[4]) : x(v[0]), y(v[1]), z(v[2]), w(v[3])
        {
        }

        template<typename T>
        inline vec4<T>::vec4(const vec4 &v) : x(v.x), y(v.y), z(v.z), w(v.w)
        {
        }

        template<typename T>
        inline vec4<T>::vec4(const vec3<T> &v) : x(v.x), y(v.y), z(v.z), w(1)
        {
        }

        template<typename T>
        inline vec4<T>::vec4(const vec3<T> &v, const T wi) : x(v.x), y(v.y), z(v.z), w(wi)
        {
        }

        template<typename T>
        inline T vec4<T>::operator[](const int i) const
        {
            return *(&x + i);
        }

        template<typename T>
        inline T &vec4<T>::operator[](const int i)
        {
            return *(&x + i);
        }

        template<typename T>
        inline vec4<T> vec4<T>::operator=(const vec4<T> &v)
        {
            x = v.x;
            y = v.y;
            z = v.z;
            w = v.w;
            return *this;
        }

        template<typename T>
        inline bool vec4<T>::operator==(const vec4<T> &v) const
        {
            return (x == v.x && y == v.y && z == v.z && w == v.w);
        }

        template<typename T>
        inline bool vec4<T>::operator!=(const vec4<T> &v) const
        {
            return (x != v.x || y != v.y || z != v.z || w != v.w);
        }

        template<typename T>
        inline vec4<T> vec4<T>::operator+(const vec4<T> &v) const
        {
            return vec4(x + v.x, y + v.y, z + v.z, w + v.w);
        }

        template<typename T>
        inline vec4<T> vec4<T>::operator-(const vec4<T> &v) const
        {
            return vec4(x - v.x, y - v.y, z - v.z, w - v.w);
        }

        template<typename T>
        inline vec4<T> vec4<T>::operator*(const vec4<T> &v) const
        {
            return vec4(x * v.x, y * v.y, z * v.z, w * v.w);
        }

        template<typename T>
        inline vec4<T> vec4<T>::operator*(const T scalar) const
        {
            return vec4(x * scalar, y * scalar, z * scalar, w * scalar);
        }

        template<typename T>
        inline vec4<T> vec4<T>::operator/(const vec4<T> &v) const
        {
            return vec4(x / v.x, y / v.y, z / v.z, w / v.w);
        }

        template<typename T>
        inline vec4<T> vec4<T>::operator/(const T scalar) const
        {
            T inv = 1 / scalar;
            return vec4(x * inv, y * inv, z * inv, w * inv);
        }

        template<typename T>
        inline vec4<T> vec4<T>::operator-() const
        {
            return vec4(-x, -y, -z, -w);
        }

        template<typename T>
        inline vec4<T> &vec4<T>::operator+=(const vec4<T> &v)
        {
            x += v.x;
            y += v.y;
            z += v.z;
            w += v.w;
            return *this;
        }

        template<typename T>
        inline vec4<T> &vec4<T>::operator-=(const vec4<T> &v)
        {
            x -= v.x;
            y -= v.y;
            z -= v.z;
            w -= v.w;
            return *this;
        }

        template<typename T>
        inline vec4<T> &vec4<T>::operator*=(const T scalar)
        {
            x *= scalar;
            y *= scalar;
            z *= scalar;
            w *= scalar;
            return *this;
        }

        template<typename T>
        inline vec4<T> &vec4<T>::operator/=(const T scalar)
        {
            T inv = 1 / scalar;
            x *= inv;
            y *= inv;
            z *= inv;
            w *= inv;
            return *this;
        }

        template<typename T>
        inline bool vec4<T>::operator<(const vec4<T> &v) const
        {
            if (x < v.x)
            {
                return true;
            }
            if (x > v.x)
            {
                return false;
            }
            if (y < v.y)
            {
                return true;
            }
            if (y > v.y)
            {
                return false;
            }
            if (z < v.z)
            {
                return true;
            }
            if (z > v.z)
            {
                return false;
            }
            return w < v.w;
        }

        template<typename T>
        inline T vec4<T>::dot(const vec3<T> &v) const
        {
            return (x * v.x + y * v.y + z * v.z + w);
        }

        template<typename T>
        inline T vec4<T>::dot(const vec4<T> &v) const
        {
            return (x * v.x + y * v.y + z * v.z + w * v.w);
        }

        template<typename T>
        inline vec3<T> vec4<T>::xyz() const
        {
            return vec3<T>(x, y, z);
        }

        template<typename T>
        inline vec2<T> vec4<T>::xy() const
        {
            return vec2<T>(x, y);
        }

    }
}

#endif