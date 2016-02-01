#ifndef GLADIUS_VEC2_H
#define GLADIUS_VEC2_H

#include <cmath>

#include "half.h"

namespace gladius
{
    namespace math
    {
        template<typename T>
        class vec2
        {
        public:
            T x;
            T y;

        public:
            vec2();
            vec2(T xi, T yi);
            vec2(const T v[2]);
            vec2(const vec2 &v);

        public:
            T operator[](const int i) const;
            T &operator[](const int i);

        public:
            void operator=(const vec2 &v);
            bool operator==(const vec2 &v) const;
            bool operator!=(const vec2 &v) const;
            bool operator<(const vec2 &v) const;

        public:
            vec2 operator+(const vec2 &v) const;
            vec2 operator-(const vec2 &v) const;
            vec2 operator*(const vec2 &v) const;
            vec2 operator*(const T scalar) const;
            vec2 operator/(const vec2 &v) const;
            vec2 operator/(const T scalar) const;
            vec2 operator-() const;

        public:
            vec2 &operator+=(const vec2 &v);
            vec2 &operator-=(const vec2 &v);
            vec2 &operator*=(const T scalar);
            vec2 &operator/=(const T scalar);

        public:
            T length() const;
            T squaredLength() const;
            T dot(const vec2 &v) const;
            vec2 normalize() const;
            vec2 normalize(T l) const;
            vec2 normalize(T *previousLength) const;
            vec2 normalize(T l, T *previousLength);

        public:
            template<class t>
            vec2<t> cast()
            {
                return vec2<t>((t) x, (t) y);
            }

        public:
            static const vec2 ZERO;
            static const vec2 UNIT_X;
            static const vec2 UNIT_Y;
        };

        template<typename T> const vec2<T> vec2<T>::ZERO {0, 0};
        template<typename T> const vec2<T> vec2<T>::UNIT_X {1, 0};
        template<typename T> const vec2<T> vec2<T>::UNIT_Y {0, 1};

        typedef vec2<half> vec2h;
        typedef vec2<float> vec2f;
        typedef vec2<double> vec2d;
        typedef vec2<int> vec2i;

        template<typename T>
        inline vec2<T>::vec2()
        {
        }

        template<typename T>
        inline vec2<T>::vec2(T xi, T yi) : x(xi), y(yi)
        {
        }

        template<typename T>
        inline vec2<T>::vec2(const T v[2]) : x(v[0]), y(v[1])
        {
        }

        template<typename T>
        inline vec2<T>::vec2(const vec2 &v) : x(v.x), y(v.y)
        {
        }

        template<typename T>
        inline T vec2<T>::operator[](const int i) const
        {
            return *(&x + i);
        }

        template<typename T>
        inline T &vec2<T>::operator[](const int i)
        {
            return *(&x + i);
        }

        template<typename T>
        inline void vec2<T>::operator=(const vec2<T> &v)
        {
            x = v.x;
            y = v.y;
        }

        template<typename T>
        inline bool vec2<T>::operator==(const vec2<T> &v) const
        {
            return (x == v.x && y == v.y);
        }

        template<typename T>
        inline bool vec2<T>::operator!=(const vec2<T> &v) const
        {
            return (x != v.x || y != v.y);
        }

        template<typename T>
        inline vec2<T> vec2<T>::operator+(const vec2<T> &v) const
        {
            return vec2(x + v.x, y + v.y);
        }

        template<typename T>
        inline vec2<T> vec2<T>::operator-(const vec2<T> &v) const
        {
            return vec2(x - v.x, y - v.y);
        }

        template<typename T>
        inline vec2<T> vec2<T>::operator*(const vec2<T> &v) const
        {
            return vec2(x * v.x, y * v.y);
        }

        template<typename T>
        inline vec2<T> vec2<T>::operator*(const T scalar) const
        {
            return vec2(x * scalar, y * scalar);
        }

        template<typename T>
        inline vec2<T> vec2<T>::operator/(const vec2<T> &v) const
        {
            return vec2(x / v.x, y / v.y);
        }

        template<typename T>
        inline vec2<T> vec2<T>::operator/(const T scalar) const
        {
            T inv = 1 / scalar;
            return vec2(x * inv, y * inv);
        }

        template<typename T>
        inline vec2<T> vec2<T>::operator-() const
        {
            return vec2(-x, -y);
        }

        template<typename T>
        inline vec2<T> &vec2<T>::operator+=(const vec2<T> &v)
        {
            x += v.x;
            y += v.y;
            return *this;
        }

        template<typename T>
        inline vec2<T> &vec2<T>::operator-=(const vec2<T> &v)
        {
            x -= v.x;
            y -= v.y;
            return *this;
        }

        template<typename T>
        inline vec2<T> &vec2<T>::operator*=(const T scalar)
        {
            x *= scalar;
            y *= scalar;
            return *this;
        }

        template<typename T>
        inline vec2<T> &vec2<T>::operator/=(const T scalar)
        {
            T inv = 1 / scalar;
            x *= inv;
            y *= inv;
            return *this;
        }

        template<typename T>
        inline bool vec2<T>::operator<(const vec2<T> &v) const
        {
            if (x < v.x)
            {
                return true;
            }
            if (x > v.x)
            {
                return false;
            }
            return y < v.y;
        }

        template<typename T>
        inline T vec2<T>::length() const
        {
            return (T) std::sqrt(float(x * x + y * y));
        }

        template<typename T>
        inline T vec2<T>::squaredLength() const
        {
            return x * x + y * y;
        }

        template<typename T>
        inline T vec2<T>::dot(const vec2<T> &v) const
        {
            return (x * v.x + y * v.y);
        }

        template<typename T>
        inline vec2<T> vec2<T>::normalize() const
        {
            T length    = sqrt(x * x + y * y);
            T invLength = 1.0 / length;
            return vec2(x * invLength, y * invLength);
        }

        template<typename T>
        inline vec2<T> vec2<T>::normalize(T l) const
        {
            T length    = sqrt(x * x + y * y);
            T invLength = l / length;
            return vec2(x * invLength, y * invLength);
        }

        template<typename T>
        inline vec2<T> vec2<T>::normalize(T *previousLength) const
        {
            *previousLength = sqrt(x * x + y * y);
            T invLength = 1.0 / *previousLength;
            return vec2(x * invLength, y * invLength);
        }

        template<typename T>
        inline vec2<T> vec2<T>::normalize(T l, T *previousLength)
        {
            *previousLength = sqrt(x * x + y * y);
            T invLength = l / *previousLength;
            return vec2(x * invLength, y * invLength);
        }

    }
}

#endif