#ifndef GLADIUS_MAT3_H
#define GLADIUS_MAT3_H

#include "vec3.h"
#include "box3.h"

namespace gladius
{
    namespace math
    {
        template<typename T>
        class mat3
        {
        protected:
            union
            {
                T m[3][3];
                T _m[9];
            };

        public:
            mat3();
            mat3(T m00, T m01, T m02, T m10, T m11, T m12, T m20, T m21, T m22);
            mat3(const T *array);
            explicit mat3(const T array[3][3]);
            mat3(const mat3 &mat);

        public:
            const T *coefficients() const;

        public:
            T *operator[](int iRow) const;

        public:
            vec3<T> getColumn(int iCol) const;
            void setColumn(int iCol, const vec3<T> &v);

        public:
            void fromAxes(const vec3<T> &xAxis, const vec3<T> &yAxis, const vec3<T> &zAxis);

        public:
            mat3 &operator=(const mat3 &mat);
            bool operator==(const mat3 &mat) const;
            bool operator!=(const mat3 &mat) const;

        public:
            mat3 operator+(const mat3 &mat) const;
            mat3 operator-(const mat3 &mat) const;
            mat3 operator*(const mat3 &mat) const;
            vec3<T> operator*(const vec3<T> &v) const;
            vec2<T> operator*(const vec2<T> &v) const;
            box3<T> operator*(const box3<T> &b) const;
            mat3 operator*(const T scalar) const;

        public:
            mat3 operator-() const;

        public:
            mat3 transpose() const;
            bool inverse(mat3 &mInv, T tolerance = 1e-06) const;
            mat3 inverse(T tolerance = 1e-06) const;

        public:
            T determinant() const;
            T trace() const;

        public:
            template<class t>
            mat3<t> cast() const
            {
                return mat3<t>((t) m[0][0], (t) m[0][1], (t) m[0][2], (t) m[1][0], (t) m[1][1], (t) m[1][2], (t) m[2][0], (t) m[2][1], (t) m[2][2]);
            }

        public:
            static const mat3 ZERO;
            static const mat3 IDENTITY;
        };

        template<typename T> const mat3<T> mat3<T>::ZERO {
                0, 0, 0,
                0, 0, 0,
                0, 0, 0
        };

        template<typename T> const mat3<T> mat3<T>::IDENTITY {
                1, 0, 0,
                0, 1, 0,
                0, 0, 1
        };

        typedef mat3<float> mat3f;
        typedef mat3<double> mat3d;

        template<typename T>
        inline mat3<T>::mat3()
        {
        }

        template<typename T>
        inline mat3<T>::mat3(T m00, T m01, T m02, T m10, T m11, T m12, T m20, T m21, T m22)
        {
            m[0][0] = m00;
            m[0][1] = m01;
            m[0][2] = m02;
            m[1][0] = m10;
            m[1][1] = m11;
            m[1][2] = m12;
            m[2][0] = m20;
            m[2][1] = m21;
            m[2][2] = m22;
        }

        template<typename T>
        inline mat3<T>::mat3(const T *array)
        {
            memcpy(m, array, 9 * sizeof(T));
        }

        template<typename T>
        inline mat3<T>::mat3(const T array[3][3])
        {
            memcpy(m, array, 9 * sizeof(T));
        }

        template<typename T>
        inline mat3<T>::mat3(const mat3 &mat)
        {
            memcpy(m, mat.m, 9 * sizeof(T));
        }

        template<typename T>
        inline const T *mat3<T>::coefficients() const
        {
            return _m;
        }

        template<typename T>
        inline T *mat3<T>::operator[](int iRow) const
        {
            return (T *) m[iRow];
        }

        template<typename T>
        inline vec3<T> mat3<T>::getColumn(int iCol) const
        {
            return vec3<T>(m[0][iCol], m[1][iCol], m[2][iCol]);
        }

        template<typename T>
        inline void mat3<T>::setColumn(int iCol, const vec3<T> &v)
        {
            m[0][iCol] = v.x;
            m[1][iCol] = v.y;
            m[2][iCol] = v.z;
        }

        template<typename T>
        inline void mat3<T>::fromAxes(const vec3<T> &xAxis, const vec3<T> &yAxis, const vec3<T> &zAxis)
        {
            setColumn(0, xAxis);
            setColumn(1, yAxis);
            setColumn(2, zAxis);
        }

        template<typename T>
        inline mat3<T> &mat3<T>::operator=(const mat3<T> &mat)
        {
            memcpy(m, mat.m, 9 * sizeof(T));
            return *this;
        }

        template<typename T>
        bool mat3<T>::operator==(const mat3<T> &mat) const
        {
            for (int iRow = 0; iRow < 3; iRow++)
            {
                for (int iCol = 0; iCol < 3; iCol++)
                {
                    if (m[iRow][iCol] != mat.m[iRow][iCol])
                    {
                        return false;
                    }
                }
            }
            return true;
        }

        template<typename T>
        inline bool mat3<T>::operator!=(const mat3<T> &mat) const
        {
            return !operator==(mat);
        }

        template<typename T>
        mat3<T> mat3<T>::operator+(const mat3<T> &mat) const
        {
            mat3<T> kSum;
            for (int   iRow = 0; iRow < 3; iRow++)
            {
                for (int iCol = 0; iCol < 3; iCol++)
                {
                    kSum.m[iRow][iCol] = m[iRow][iCol] + mat.m[iRow][iCol];
                }
            }
            return kSum;
        }

        template<typename T>
        mat3<T> mat3<T>::operator-(const mat3<T> &mat) const
        {
            mat3<T> kDiff;
            for (int   iRow = 0; iRow < 3; iRow++)
            {
                for (int iCol = 0; iCol < 3; iCol++)
                {
                    kDiff.m[iRow][iCol] = m[iRow][iCol] - mat.m[iRow][iCol];
                }
            }
            return kDiff;
        }

        template<typename T>
        mat3<T> mat3<T>::operator*(const mat3<T> &mat) const
        {
            mat3<T> kProd;
            for (int   iRow = 0; iRow < 3; iRow++)
            {
                for (int iCol = 0; iCol < 3; iCol++)
                {
                    kProd.m[iRow][iCol] = m[iRow][0] * mat.m[0][iCol] + m[iRow][1] * mat.m[1][iCol] + m[iRow][2] * mat.m[2][iCol];
                }
            }
            return kProd;
        }

        template<typename T>
        vec3<T> mat3<T>::operator*(const vec3<T> &v) const
        {
            vec3<T> kProd;
            for (int   iRow = 0; iRow < 3; iRow++)
            {
                kProd[iRow] = m[iRow][0] * v[0] + m[iRow][1] * v[1] + m[iRow][2] * v[2];
            }
            return kProd;
        }

        template<typename T>
        vec2<T> mat3<T>::operator*(const vec2<T> &v) const
        {
            vec3<T> prod = operator* (vec3<T>(v.x, v.y, 1));
            return (prod.xy() / prod.z);
        }

        template<typename T>
        box3<T> mat3<T>::operator*(const box3<T> &v) const
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
        mat3<T> mat3<T>::operator*(T fScalar) const
        {
            mat3<T> kProd;
            for (int   iRow = 0; iRow < 3; iRow++)
            {
                for (int iCol = 0; iCol < 3; iCol++)
                {
                    kProd[iRow][iCol] = fScalar * m[iRow][iCol];
                }
            }
            return kProd;
        }

        template<typename T>
        mat3<T> mat3<T>::operator-() const
        {
            mat3<T> kNeg;
            for (int   iRow = 0; iRow < 3; iRow++)
            {
                for (int iCol = 0; iCol < 3; iCol++)
                {
                    kNeg[iRow][iCol] = -m[iRow][iCol];
                }
            }
            return kNeg;
        }

        template<typename T>
        mat3<T> mat3<T>::transpose() const
        {
            mat3<T> kTranspose;
            for (int   iRow = 0; iRow < 3; iRow++)
            {
                for (int iCol = 0; iCol < 3; iCol++)
                {
                    kTranspose[iRow][iCol] = m[iCol][iRow];
                }
            }
            return kTranspose;
        }

        template<typename T>
        bool mat3<T>::inverse(mat3<T> &mInv, T tolerance) const
        {
            // Invert a 3x3 using cofactors.  This is about 8 times faster than
            // the Numerical Recipes code which uses Gaussian elimination.
            mInv[0][0] = m[1][1] * m[2][2] - m[1][2] * m[2][1];
            mInv[0][1] = m[0][2] * m[2][1] - m[0][1] * m[2][2];
            mInv[0][2] = m[0][1] * m[1][2] - m[0][2] * m[1][1];
            mInv[1][0] = m[1][2] * m[2][0] - m[1][0] * m[2][2];
            mInv[1][1] = m[0][0] * m[2][2] - m[0][2] * m[2][0];
            mInv[1][2] = m[0][2] * m[1][0] - m[0][0] * m[1][2];
            mInv[2][0] = m[1][0] * m[2][1] - m[1][1] * m[2][0];
            mInv[2][1] = m[0][1] * m[2][0] - m[0][0] * m[2][1];
            mInv[2][2] = m[0][0] * m[1][1] - m[0][1] * m[1][0];
            T fDet = m[0][0] * mInv[0][0] + m[0][1] * mInv[1][0] + m[0][2] * mInv[2][0];
            if (abs(fDet) <= tolerance)
            {
                return false;
            }
            T     fInvDet = 1.0 / fDet;
            for (int iRow    = 0; iRow < 3; iRow++)
            {
                for (int iCol = 0; iCol < 3; iCol++)
                    mInv[iRow][iCol] *= fInvDet;
            }
            return true;
        }

        template<typename T>
        mat3<T> mat3<T>::inverse(T tolerance) const
        {
            mat3<T> kInverse;
            inverse(kInverse, tolerance);
            return kInverse;
        }

        template<typename T>
        T mat3<T>::determinant() const
        {
            T fCofactor00 = m[1][1] * m[2][2] - m[1][2] * m[2][1];
            T fCofactor10 = m[1][2] * m[2][0] - m[1][0] * m[2][2];
            T fCofactor20 = m[1][0] * m[2][1] - m[1][1] * m[2][0];
            T fDet        = m[0][0] * fCofactor00 + m[0][1] * fCofactor10 + m[0][2] * fCofactor20;
            return fDet;
        }

        template<typename T>
        T mat3<T>::trace() const
        {
            return m[0][0] + m[1][1] + m[2][2];
        }

    }
}

#endif