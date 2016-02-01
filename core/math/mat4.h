#ifndef GLADIUS_MAT4_H
#define GLADIUS_MAT4_H

#include "vec2.h"
#include "vec3.h"
#include "vec4.h"
#include "box3.h"
#include "mat3.h"
#include "common_math.h"

namespace gladius
{
    namespace math
    {
        template<typename T>
        class mat4
        {
        protected:
            union
            {
                T m[4][4];
                T _m[16];
            };

        public:
            mat4();

            mat4(T m00, T m01, T m02, T m03, T m10, T m11, T m12, T m13, T m20, T m21, T m22, T m23, T m30, T m31, T m32, T m33);

            mat4(const T *array);

            mat4(const T array[4][4]);

            mat4(const mat3<T> &m3x3);

        public:
            const T *coefficients() const;

        public:
            const T *operator[](int iRow) const;

        public:
            void operator=(const mat3<T> &m3);

            bool operator==(const mat4 &m2) const;

            bool operator!=(const mat4 &m2) const;

        public:
            mat4 operator+(const mat4 &m2) const;

            mat4 operator-(const mat4 &m2) const;

            mat4    operator*(const mat4 &m2) const;

            vec4<T> operator*(const vec4<T> &v) const;

            vec3<T> operator*(const vec3<T> &v) const;

            box3<T> operator*(const box3<T> &b) const;

            mat4    operator*(T f) const;

        public:
            mat4 transpose(void) const;

            mat4 adjoint() const;

            mat4 inverse() const;

        public:
            mat3<T> mat3x3() const;

        public:
            T determinant() const;

        public:
            static mat4 translate(const vec3<T> &v);

            static mat4 rotatex(T angle);

            static mat4 rotatey(T angle);

            static mat4 rotatez(T angle);

        public:
            static mat4 perspectiveProjection(T fovy, T aspect, T zNear, T zFar);

            static mat4 orthoProjection(T xRight, T xLeft, T yTop, T yBottom, T zNear, T zFar);

        public:
            template<class t>
            mat4<t> cast() const
            {
                return mat4<t>((t) m[0][0], (t) m[0][1], (t) m[0][2], (t) m[0][3], (t) m[1][0], (t) m[1][1], (t) m[1][2], (t) m[1][3], (t) m[2][0], (t) m[2][1], (t) m[2][2], (t) m[2][3], (t) m[3][0], (t) m[3][1], (t) m[3][2], (t) m[3][3]);
            }

        public:
            static const mat4 ZERO;
            static const mat4 IDENTITY;
        };

        template<typename T> const mat4<T> mat4<T>::ZERO {
                0, 0, 0, 0,
                0, 0, 0, 0,
                0, 0, 0, 0,
                0, 0, 0, 0
        };

        template<typename T> const mat4<T> mat4<T>::IDENTITY {
                1, 0, 0, 0,
                0, 1, 0, 0,
                0, 0, 1, 0,
                0, 0, 0, 1
        };

        typedef mat4<float>  mat4f;
        typedef mat4<double> mat4d;

        template<typename T>
        inline mat4<T>::mat4()
        {
        }

        template<typename T>
        inline mat4<T>::mat4(T m00, T m01, T m02, T m03, T m10, T m11, T m12, T m13, T m20, T m21, T m22, T m23, T m30, T m31, T m32, T m33)
        {
            m[0][0] = m00;
            m[0][1] = m01;
            m[0][2] = m02;
            m[0][3] = m03;
            m[1][0] = m10;
            m[1][1] = m11;
            m[1][2] = m12;
            m[1][3] = m13;
            m[2][0] = m20;
            m[2][1] = m21;
            m[2][2] = m22;
            m[2][3] = m23;
            m[3][0] = m30;
            m[3][1] = m31;
            m[3][2] = m32;
            m[3][3] = m33;
        }

        template<typename T>
        inline mat4<T>::mat4(const T *array)
        {
            memcpy(m, array, 16 * sizeof(T));
        }

        template<typename T>
        inline mat4<T>::mat4(const T array[4][4])
        {
            memcpy(m, array, 16 * sizeof(T));
        }

        template<typename T>
        inline mat4<T>::mat4(const mat3<T> &m3x3)
        {
            operator=(IDENTITY);
            operator=(m3x3);
        }

        template<typename T>
        inline const T *mat4<T>::coefficients() const
        {
            return _m;
        }

        template<typename T>
        inline const T *mat4<T>::operator[](int iRow) const
        {
            //assert(iRow < 4);
            return m[iRow];
        }


        template<typename T>
        inline void mat4<T>::operator=(const mat3<T> &m3)
        {
            const T *c = m3.coefficients();
            m[0][0] = c[0 * 3 + 0];
            m[0][1] = c[0 * 3 + 1];
            m[0][2] = c[0 * 3 + 2];
            m[1][0] = c[1 * 3 + 0];
            m[1][1] = c[1 * 3 + 1];
            m[1][2] = c[1 * 3 + 2];
            m[2][0] = c[2 * 3 + 0];
            m[2][1] = c[2 * 3 + 1];
            m[2][2] = c[2 * 3 + 2];
        }

        template<typename T>
        inline bool mat4<T>::operator==(const mat4<T> &m2) const
        {
            if (m[0][0] != m2.m[0][0] || m[0][1] != m2.m[0][1] || m[0][2] != m2.m[0][2] || m[0][3] != m2.m[0][3] || m[1][0] != m2.m[1][0] || m[1][1] != m2.m[1][1] || m[1][2] != m2.m[1][2] || m[1][3] != m2.m[1][3] || m[2][0] != m2.m[2][0] || m[2][1] != m2.m[2][1] || m[2][2] != m2.m[2][2] || m[2][3] != m2.m[2][3] || m[3][0] != m2.m[3][0] || m[3][1] != m2.m[3][1] || m[3][2] != m2.m[3][2] || m[3][3] != m2.m[3][3])
                return false;

            return true;
        }

        template<typename T>
        inline bool mat4<T>::operator!=(const mat4<T> &m2) const
        {
            if (m[0][0] != m2.m[0][0] || m[0][1] != m2.m[0][1] || m[0][2] != m2.m[0][2] || m[0][3] != m2.m[0][3] || m[1][0] != m2.m[1][0] || m[1][1] != m2.m[1][1] || m[1][2] != m2.m[1][2] || m[1][3] != m2.m[1][3] || m[2][0] != m2.m[2][0] || m[2][1] != m2.m[2][1] || m[2][2] != m2.m[2][2] || m[2][3] != m2.m[2][3] || m[3][0] != m2.m[3][0] || m[3][1] != m2.m[3][1] || m[3][2] != m2.m[3][2] || m[3][3] != m2.m[3][3])
                return true;

            return false;
        }

        template<typename T>
        inline mat4<T> mat4<T>::operator+(const mat4<T> &m2) const
        {
            mat4<T> r;

            r.m[0][0] = m[0][0] + m2.m[0][0];
            r.m[0][1] = m[0][1] + m2.m[0][1];
            r.m[0][2] = m[0][2] + m2.m[0][2];
            r.m[0][3] = m[0][3] + m2.m[0][3];

            r.m[1][0] = m[1][0] + m2.m[1][0];
            r.m[1][1] = m[1][1] + m2.m[1][1];
            r.m[1][2] = m[1][2] + m2.m[1][2];
            r.m[1][3] = m[1][3] + m2.m[1][3];

            r.m[2][0] = m[2][0] + m2.m[2][0];
            r.m[2][1] = m[2][1] + m2.m[2][1];
            r.m[2][2] = m[2][2] + m2.m[2][2];
            r.m[2][3] = m[2][3] + m2.m[2][3];

            r.m[3][0] = m[3][0] + m2.m[3][0];
            r.m[3][1] = m[3][1] + m2.m[3][1];
            r.m[3][2] = m[3][2] + m2.m[3][2];
            r.m[3][3] = m[3][3] + m2.m[3][3];

            return r;
        }

        template<typename T>
        inline mat4<T> mat4<T>::operator-(const mat4<T> &m2) const
        {
            mat4 r;
            r.m[0][0] = m[0][0] - m2.m[0][0];
            r.m[0][1] = m[0][1] - m2.m[0][1];
            r.m[0][2] = m[0][2] - m2.m[0][2];
            r.m[0][3] = m[0][3] - m2.m[0][3];

            r.m[1][0] = m[1][0] - m2.m[1][0];
            r.m[1][1] = m[1][1] - m2.m[1][1];
            r.m[1][2] = m[1][2] - m2.m[1][2];
            r.m[1][3] = m[1][3] - m2.m[1][3];

            r.m[2][0] = m[2][0] - m2.m[2][0];
            r.m[2][1] = m[2][1] - m2.m[2][1];
            r.m[2][2] = m[2][2] - m2.m[2][2];
            r.m[2][3] = m[2][3] - m2.m[2][3];

            r.m[3][0] = m[3][0] - m2.m[3][0];
            r.m[3][1] = m[3][1] - m2.m[3][1];
            r.m[3][2] = m[3][2] - m2.m[3][2];
            r.m[3][3] = m[3][3] - m2.m[3][3];


            return r;
        }

        template<typename T>
        inline mat4<T> mat4<T>::operator*(const mat4<T> &m2) const
        {
            mat4 r;
            r.m[0][0] = m[0][0] * m2.m[0][0] + m[0][1] * m2.m[1][0] + m[0][2] * m2.m[2][0] + m[0][3] * m2.m[3][0];
            r.m[0][1] = m[0][0] * m2.m[0][1] + m[0][1] * m2.m[1][1] + m[0][2] * m2.m[2][1] + m[0][3] * m2.m[3][1];
            r.m[0][2] = m[0][0] * m2.m[0][2] + m[0][1] * m2.m[1][2] + m[0][2] * m2.m[2][2] + m[0][3] * m2.m[3][2];
            r.m[0][3] = m[0][0] * m2.m[0][3] + m[0][1] * m2.m[1][3] + m[0][2] * m2.m[2][3] + m[0][3] * m2.m[3][3];

            r.m[1][0] = m[1][0] * m2.m[0][0] + m[1][1] * m2.m[1][0] + m[1][2] * m2.m[2][0] + m[1][3] * m2.m[3][0];
            r.m[1][1] = m[1][0] * m2.m[0][1] + m[1][1] * m2.m[1][1] + m[1][2] * m2.m[2][1] + m[1][3] * m2.m[3][1];
            r.m[1][2] = m[1][0] * m2.m[0][2] + m[1][1] * m2.m[1][2] + m[1][2] * m2.m[2][2] + m[1][3] * m2.m[3][2];
            r.m[1][3] = m[1][0] * m2.m[0][3] + m[1][1] * m2.m[1][3] + m[1][2] * m2.m[2][3] + m[1][3] * m2.m[3][3];

            r.m[2][0] = m[2][0] * m2.m[0][0] + m[2][1] * m2.m[1][0] + m[2][2] * m2.m[2][0] + m[2][3] * m2.m[3][0];
            r.m[2][1] = m[2][0] * m2.m[0][1] + m[2][1] * m2.m[1][1] + m[2][2] * m2.m[2][1] + m[2][3] * m2.m[3][1];
            r.m[2][2] = m[2][0] * m2.m[0][2] + m[2][1] * m2.m[1][2] + m[2][2] * m2.m[2][2] + m[2][3] * m2.m[3][2];
            r.m[2][3] = m[2][0] * m2.m[0][3] + m[2][1] * m2.m[1][3] + m[2][2] * m2.m[2][3] + m[2][3] * m2.m[3][3];

            r.m[3][0] = m[3][0] * m2.m[0][0] + m[3][1] * m2.m[1][0] + m[3][2] * m2.m[2][0] + m[3][3] * m2.m[3][0];
            r.m[3][1] = m[3][0] * m2.m[0][1] + m[3][1] * m2.m[1][1] + m[3][2] * m2.m[2][1] + m[3][3] * m2.m[3][1];
            r.m[3][2] = m[3][0] * m2.m[0][2] + m[3][1] * m2.m[1][2] + m[3][2] * m2.m[2][2] + m[3][3] * m2.m[3][2];
            r.m[3][3] = m[3][0] * m2.m[0][3] + m[3][1] * m2.m[1][3] + m[3][2] * m2.m[2][3] + m[3][3] * m2.m[3][3];

            return r;
        }

        template<typename T>
        inline vec4<T> mat4<T>::operator*(const vec4<T> &v) const
        {
            return vec4<T>(m[0][0] * v.x + m[0][1] * v.y + m[0][2] * v.z + m[0][3] * v.w, m[1][0] * v.x + m[1][1] * v.y + m[1][2] * v.z + m[1][3] * v.w, m[2][0] * v.x + m[2][1] * v.y + m[2][2] * v.z + m[2][3] * v.w, m[3][0] * v.x + m[3][1] * v.y + m[3][2] * v.z + m[3][3] * v.w);
        }

        template<typename T>
        inline vec3<T> mat4<T>::operator*(const vec3<T> &v) const
        {
            vec3<T> r;

            T fInvW = T(1.0) / (m[3][0] * v.x + m[3][1] * v.y + m[3][2] * v.z + m[3][3]);

            r.x = (m[0][0] * v.x + m[0][1] * v.y + m[0][2] * v.z + m[0][3]) * fInvW;
            r.y = (m[1][0] * v.x + m[1][1] * v.y + m[1][2] * v.z + m[1][3]) * fInvW;
            r.z = (m[2][0] * v.x + m[2][1] * v.y + m[2][2] * v.z + m[2][3]) * fInvW;

            return r;
        }

        template<typename T>
        box3<T> mat4<T>::operator*(const box3<T> &v) const
        {
            box3<T> b;
            b = b.enlarge(operator*(vec3<T>(v.xmin, v.ymin, v.zmin)));
            b = b.enlarge(operator*(vec3<T>(v.xmax, v.ymin, v.zmin)));
            b = b.enlarge(operator*(vec3<T>(v.xmin, v.ymax, v.zmin)));
            b = b.enlarge(operator*(vec3<T>(v.xmax, v.ymax, v.zmin)));
            b = b.enlarge(operator*(vec3<T>(v.xmin, v.ymin, v.zmax)));
            b = b.enlarge(operator*(vec3<T>(v.xmax, v.ymin, v.zmax)));
            b = b.enlarge(operator*(vec3<T>(v.xmin, v.ymax, v.zmax)));
            b = b.enlarge(operator*(vec3<T>(v.xmax, v.ymax, v.zmax)));
            return b;
        }

        template<typename T>
        inline mat4<T> mat4<T>::operator*(T f) const
        {
            mat4<T> r;

            r.m[0][0] = m[0][0] * f;
            r.m[0][1] = m[0][1] * f;
            r.m[0][2] = m[0][2] * f;
            r.m[0][3] = m[0][3] * f;

            r.m[1][0] = m[1][0] * f;
            r.m[1][1] = m[1][1] * f;
            r.m[1][2] = m[1][2] * f;
            r.m[1][3] = m[1][3] * f;

            r.m[2][0] = m[2][0] * f;
            r.m[2][1] = m[2][1] * f;
            r.m[2][2] = m[2][2] * f;
            r.m[2][3] = m[2][3] * f;

            r.m[3][0] = m[3][0] * f;
            r.m[3][1] = m[3][1] * f;
            r.m[3][2] = m[3][2] * f;
            r.m[3][3] = m[3][3] * f;

            return r;
        }

        template<typename T>
        inline mat4<T> mat4<T>::transpose(void) const
        {
            return mat4(m[0][0], m[1][0], m[2][0], m[3][0], m[0][1], m[1][1], m[2][1], m[3][1], m[0][2], m[1][2], m[2][2], m[3][2], m[0][3], m[1][3], m[2][3], m[3][3]);
        }

        template<typename T>
        inline static T MINOR(const mat4<T> &m, int r0, int r1, int r2, int c0, int c1, int c2)
        {
            return T(m[r0][c0] * (m[r1][c1] * m[r2][c2] - m[r2][c1] * m[r1][c2]) - m[r0][c1] * (m[r1][c0] * m[r2][c2] - m[r2][c0] * m[r1][c2]) + m[r0][c2] * (m[r1][c0] * m[r2][c1] - m[r2][c0] * m[r1][c1]));
        }

        template<typename T>
        mat4<T> mat4<T>::adjoint() const
        {
            return mat4(MINOR(*this, 1, 2, 3, 1, 2, 3), -MINOR(*this, 0, 2, 3, 1, 2, 3), MINOR(*this, 0, 1, 3, 1, 2, 3), -MINOR(*this, 0, 1, 2, 1, 2, 3),

                        -MINOR(*this, 1, 2, 3, 0, 2, 3), MINOR(*this, 0, 2, 3, 0, 2, 3), -MINOR(*this, 0, 1, 3, 0, 2, 3), MINOR(*this, 0, 1, 2, 0, 2, 3),

                        MINOR(*this, 1, 2, 3, 0, 1, 3), -MINOR(*this, 0, 2, 3, 0, 1, 3), MINOR(*this, 0, 1, 3, 0, 1, 3), -MINOR(*this, 0, 1, 2, 0, 1, 3),

                        -MINOR(*this, 1, 2, 3, 0, 1, 2), MINOR(*this, 0, 2, 3, 0, 1, 2), -MINOR(*this, 0, 1, 3, 0, 1, 2), MINOR(*this, 0, 1, 2, 0, 1, 2));
        }

        template<typename T>
        mat4<T> mat4<T>::inverse() const
        {
            return adjoint() * (1.0f / determinant());
        }

        template<typename T>
        mat3<T> mat4<T>::mat3x3() const
        {
            return mat3<T>(m[0][0], m[0][1], m[0][2], m[1][0], m[1][1], m[1][2], m[2][0], m[2][1], m[2][2]);
        }

        template<typename T>
        T mat4<T>::determinant() const
        {
            return (T) (m[0][0] * MINOR(*this, 1, 2, 3, 1, 2, 3) - m[0][1] * MINOR(*this, 1, 2, 3, 0, 2, 3) + m[0][2] * MINOR(*this, 1, 2, 3, 0, 1, 3) - m[0][3] * MINOR(*this, 1, 2, 3, 0, 1, 2));
        }

        template<typename T>
        inline mat4<T> mat4<T>::translate(const vec3<T> &v)
        {
            return mat4<T>(1, 0, 0, v.x, 0, 1, 0, v.y, 0, 0, 1, v.z, 0, 0, 0, 1);
        }

        template<typename T>
        inline mat4<T> mat4<T>::rotatex(T angle)
        {
            T ca = (T) cos(angle * PI / 180.0);
            T sa = (T) sin(angle * PI / 180.0);
            return mat4<T>(1, 0, 0, 0, 0, ca, -sa, 0, 0, sa, ca, 0, 0, 0, 0, 1);
        }

        template<typename T>
        inline mat4<T> mat4<T>::rotatey(T angle)
        {
            T ca = (T) cos(angle * PI / 180.0);
            T sa = (T) sin(angle * PI / 180.0);
            return mat4<T>(ca, 0, sa, 0, 0, 1, 0, 0, -sa, 0, ca, 0, 0, 0, 0, 1);
        }

        template<typename T>
        inline mat4<T> mat4<T>::rotatez(T angle)
        {
            T ca = (T) cos(angle * PI / 180.0);
            T sa = (T) sin(angle * PI / 180.0);
            return mat4<T>(ca, -sa, 0, 0, sa, ca, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1);
        }

        template<typename T>
        inline mat4<T> mat4<T>::perspectiveProjection(T fovy, T aspect, T zNear, T zFar)
        {
            T f = (T) 1 / tan(RADIANS(fovy) / 2);
            return mat4<T>(f / aspect, 0, 0, 0, 0, f, 0, 0, 0, 0, (zFar + zNear) / (zNear - zFar), (2 * zFar * zNear) / (zNear - zFar), 0, 0, -1, 0);
        }

        template<typename T>
        inline mat4<T> mat4<T>::orthoProjection(T xRight, T xLeft, T yTop, T yBottom, T zNear, T zFar)
        {
            T tx, ty, tz;
            tx = -(xRight + xLeft) / (xRight - xLeft);
            ty = -(yTop + yBottom) / (yTop - yBottom);
            tz = -(zFar + zNear) / (zFar - zNear);
            return mat4<T>(2 / (xRight - xLeft), 0, 0, tx, 0, 2 / (yTop - yBottom), 0, ty, 0, 0, -2 / (zFar - zNear), tz, 0, 0, 0, 1);
        }
    }
}

#endif

