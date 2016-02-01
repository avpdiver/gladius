#ifndef GLADIUS_VEC3_H
#define GLADIUS_VEC3_H

#include "vec2.h"

namespace gladius
{
    namespace math
    {
        template<typename T>
        class vec3
        {
        public:
            T x;
            T y;
            T z;

        public:
            vec3();
            vec3(T xi, T yi, T zi);
            vec3(const T v[3]);
            vec3(const vec3 &v);

        public:
            T operator[](const int i) const;
            T &operator[](const int i);

        public:
            void operator=(const vec3 &v);
            bool operator==(const vec3 &v) const;
            bool operator!=(const vec3 &v) const;
            bool operator<(const vec3 &v) const;

        public:
            vec3 operator+(const vec3 &v) const;
            vec3 operator-(const vec3 &v) const;
            vec3 operator*(const vec3 &v) const;
            vec3 operator*(const T scalar) const;
            vec3 operator/(const vec3 &v) const;
            vec3 operator/(const T scalar) const;
            vec3 operator-() const;

        public:
            vec3 &operator+=(const vec3 &v);
            vec3 &operator-=(const vec3 &v);
            vec3 &operator*=(const T scalar);
            vec3 &operator/=(const T scalar);

        public:
            T length() const;
            T squaredLength() const;
            T dot(const vec3 &v) const;
            vec3 cross(const vec3 &v) const;
            vec3 normalize() const;
            vec3 normalize(T l) const;
            vec3 normalize(T *previousLength) const;
            vec3 normalize(T l, T *previousLength);

        public:
            vec2<T> xy() const;

            template<class t>
            vec3<t> cast() const
            {
                return vec3<t>((t) x, (t) y, (t) z);
            }

        public:
            static const vec3 ZERO;
            static const vec3 UNIT_X;
            static const vec3 UNIT_Y;
            static const vec3 UNIT_Z;
        };

        template <typename T> const vec3<T> vec3<T>::ZERO {0 ,0, 0};
        template <typename T> const vec3<T> vec3<T>::UNIT_X {1 ,0, 0};
        template <typename T> const vec3<T> vec3<T>::UNIT_Y {0 ,1, 0};
        template <typename T> const vec3<T> vec3<T>::UNIT_Z {0 ,0, 1};

        typedef vec3<half> vec3h;
        typedef vec3<float> vec3f;
        typedef vec3<double> vec3d;
        typedef vec3<int> vec3i;

        template<typename T>
        inline vec3<T>::vec3()
        {
        }

        template<typename T>
        inline vec3<T>::vec3(T xi, T yi, T zi) : x(xi), y(yi), z(zi)
        {
        }

        template<typename T>
        inline vec3<T>::vec3(const T v[3]) : x(v[0]), y(v[1]), z(v[2])
        {
        }

        template<typename T>
        inline vec3<T>::vec3(const vec3 &v) : x(v.x), y(v.y), z(v.z)
        {
        }

        template<typename T>
        inline T vec3<T>::operator[](const int i) const
        {
            return *(&x + i);
        }

        template<typename T>
        inline T &vec3<T>::operator[](const int i)
        {
            return *(&x + i);
        }

        template<typename T>
        inline void vec3<T>::operator=(const vec3<T> &v)
        {
            x = v.x;
            y = v.y;
            z = v.z;
        }

        template<typename T>
        inline bool vec3<T>::operator==(const vec3<T> &v) const
        {
            return (x == v.x && y == v.y && z == v.z);
        }

        template<typename T>
        inline bool vec3<T>::operator!=(const vec3<T> &v) const
        {
            return (x != v.x || y != v.y || z != v.z);
        }

        template<typename T>
        inline vec3<T> vec3<T>::operator+(const vec3<T> &v) const
        {
            return vec3(x + v.x, y + v.y, z + v.z);
        }

        template<typename T>
        inline vec3<T> vec3<T>::operator-(const vec3<T> &v) const
        {
            return vec3(x - v.x, y - v.y, z - v.z);
        }

        template<typename T>
        inline vec3<T> vec3<T>::operator*(const vec3<T> &v) const
        {
            return vec3(x * v.x, y * v.y, z * v.z);
        }

        template<typename T>
        inline vec3<T> vec3<T>::operator*(const T scalar) const
        {
            return vec3(x * scalar, y * scalar, z * scalar);
        }

        template<typename T>
        inline vec3<T> vec3<T>::operator/(const vec3<T> &v) const
        {
            return vec3(x / v.x, y / v.y, z / v.z);
        }

        template<typename T>
        inline vec3<T> vec3<T>::operator/(const T scalar) const
        {
            T inv = 1 / scalar;
            return vec3(x * inv, y * inv, z * inv);
        }

        template<typename T>
        inline vec3<T> vec3<T>::operator-() const
        {
            return vec3(-x, -y, -z);
        }

        template<typename T>
        inline vec3<T> &vec3<T>::operator+=(const vec3<T> &v)
        {
            x += v.x;
            y += v.y;
            z += v.z;
            return *this;
        }

        template<typename T>
        inline vec3<T> &vec3<T>::operator-=(const vec3<T> &v)
        {
            x -= v.x;
            y -= v.y;
            z -= v.z;
            return *this;
        }

        template<typename T>
        inline vec3<T> &vec3<T>::operator*=(const T scalar)
        {
            x *= scalar;
            y *= scalar;
            z *= scalar;
            return *this;
        }

        template<typename T>
        inline vec3<T> &vec3<T>::operator/=(const T scalar)
        {
            T inv = 1 / scalar;
            x *= inv;
            y *= inv;
            z *= inv;
            return *this;
        }

        template<typename T>
        inline bool vec3<T>::operator<(const vec3<T> &v) const
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
            return z < v.z;
        }

        template<typename T>
        inline T vec3<T>::length() const
        {
            return sqrt(x * x + y * y + z * z);
        }

        template<typename T>
        inline T vec3<T>::squaredLength() const
        {
            return (x * x + y * y + z * z);
        }

        template<typename T>
        inline T vec3<T>::dot(const vec3<T> &v) const
        {
            return (x * v.x + y * v.y + z * v.z);
        }

        template<typename T>
        inline vec3<T> vec3<T>::normalize() const
        {
            T length    = sqrt(x * x + y * y + z * z);
            T invLength = (T) 1.0 / length;
            return vec3(x * invLength, y * invLength, z * invLength);
        }

        template<typename T>
        inline vec3<T> vec3<T>::normalize(T l) const
        {
            T length    = sqrt(x * x + y * y + z * z);
            T invLength = l / length;
            return vec3(x * invLength, y * invLength, z * invLength);
        }

        template<typename T>
        inline vec3<T> vec3<T>::normalize(T *previousLength) const
        {
            *previousLength = sqrt(x * x + y * y + z * z);
            T invLength = 1.0 / *previousLength;
            return vec3(x * invLength, y * invLength, z * invLength);
        }

        template<typename T>
        inline vec3<T> vec3<T>::normalize(T l, T *previousLength)
        {
            *previousLength = sqrt(x * x + y * y + z * z);
            T invLength = l / *previousLength;
            return vec3(x * invLength, y * invLength, z * invLength);
        }

        template<typename T>
        inline vec3<T> vec3<T>::cross(const vec3<T> &v) const
        {
            return vec3(y * v.z - z * v.y, z * v.x - x * v.z, x * v.y - y * v.x);
        }

        template<typename T>
        inline vec2<T> vec3<T>::xy() const
        {
            return vec2<T>(x, y);
        }

    }
}

#endif