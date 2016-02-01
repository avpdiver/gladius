#ifndef GLADIUS_MAT2_H
#define GLADIUS_MAT2_H

#include "vec2.h"
#include "box2.h"

namespace gladius
{
    namespace math
    {
        template<typename T>
        class mat2
        {
        protected:
            union
            {
                T m[2][2];
                T _m[4];
            };

        public:
            mat2();

            mat2(T m00, T m01, T m10, T m11);

            explicit mat2(const T array[2][2]);

            mat2(const mat2 &mat);

        public:
            const T *coefficients() const;

        public:
            T *operator[](int iRow) const;

        public:
            vec2<T> getColumn(int iCol) const;

            void setColumn(int iCol, const vec2<T> &v);

        public:
            mat2 &operator=(const mat2 &mat);

            bool operator==(const mat2 &mat) const;

            bool operator!=(const mat2 &mat) const;

        public:
            mat2 operator+(const mat2 &mat) const;

            mat2 operator-(const mat2 &mat) const;

            mat2 operator*(const mat2 &mat) const;

            vec2<T> operator*(const vec2<T> &v) const;

            mat2 operator*(const T scalar) const;

        public:
            mat2 operator-() const;

            mat2 transpose() const;

            bool inverse(mat2 &mInv, T tolerance = 1e-06) const;

            mat2 inverse(T tolerance = 1e-06) const;

        public:
            T determinant() const;

            T trace() const;

        public:
            template<class t>
            mat2<t> cast() const
            {
                return mat2<t>((t) m[0][0], (t) m[0][1], (t) m[1][0], (t) m[1][1]);
            }

        public:
            static const mat2 ZERO;
            static const mat2 IDENTITY;
        };

        template<typename T> const mat2<T> mat2<T>::ZERO{
                0, 0,
                0, 0
        };

        template<typename T> const mat2<T> mat2<T>::IDENTITY {
                1, 0,
                0, 1
        };

        typedef mat2<float>  mat2f;
        typedef mat2<double> mat2d;

        template<typename T>
        inline mat2<T>::mat2()
        {
        }

        template<typename T>
        inline mat2<T>::mat2(T m00, T m01, T m10, T m11)
        {
            m[0][0] = m00;
            m[0][1] = m01;
            m[1][0] = m10;
            m[1][1] = m11;
        }

        template<typename T>
        inline mat2<T>::mat2(const T array[2][2])
        {
            memcpy(m, array, 4 * sizeof(T));
        }

        template<typename T>
        inline mat2<T>::mat2(const mat2 &mat)
        {
            memcpy(m, mat.m, 4 * sizeof(T));
        }

        template<typename T>
        inline const T *mat2<T>::coefficients() const
        {
            return _m;
        }

        template<typename T>
        inline T *mat2<T>::operator[](int iRow) const
        {
            return (T *) m[iRow];
        }

        template<typename T>
        inline vec2<T> mat2<T>::getColumn(int iCol) const
        {
            return vec2<T>(m[0][iCol], m[1][iCol]);
        }

        template<typename T>
        inline void mat2<T>::setColumn(int iCol, const vec2<T> &v)
        {
            m[0][iCol] = v.x;
            m[1][iCol] = v.y;
        }

        template<typename T>
        inline mat2<T> &mat2<T>::operator=(const mat2<T> &mat)
        {
            memcpy(m, mat.m, 4 * sizeof(T));
            return *this;
        }

        template<typename T>
        bool mat2<T>::operator==(const mat2<T> &mat) const
        {
            for (int iRow = 0; iRow < 2; iRow++)
            {
                for (int iCol = 0; iCol < 2; iCol++)
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
        inline bool mat2<T>::operator!=(const mat2<T> &mat) const
        {
            return !operator==(mat);
        }

        template<typename T>
        mat2<T> mat2<T>::operator+(const mat2<T> &mat) const
        {
            mat2<T>  kSum;
            for (int iRow = 0; iRow < 2; iRow++)
            {
                for (int iCol = 0; iCol < 2; iCol++)
                {
                    kSum.m[iRow][iCol] = m[iRow][iCol] + mat.m[iRow][iCol];
                }
            }
            return kSum;
        }

        template<typename T>
        mat2<T> mat2<T>::operator-(const mat2<T> &mat) const
        {
            mat2<T>  kDiff;
            for (int iRow = 0; iRow < 2; iRow++)
            {
                for (int iCol = 0; iCol < 2; iCol++)
                {
                    kDiff.m[iRow][iCol] = m[iRow][iCol] - mat.m[iRow][iCol];
                }
            }
            return kDiff;
        }

        template<typename T>
        mat2<T> mat2<T>::operator*(const mat2<T> &mat) const
        {
            mat2<T>  kProd;
            for (int iRow = 0; iRow < 2; iRow++)
            {
                for (int iCol = 0; iCol < 2; iCol++)
                {
                    kProd.m[iRow][iCol] = m[iRow][0] * mat.m[0][iCol] + m[iRow][1] * mat.m[1][iCol];
                }
            }
            return kProd;
        }

        template<typename T>
        vec2<T> mat2<T>::operator*(const vec2<T> &v) const
        {
            vec2<T>  kProd;
            for (int iRow = 0; iRow < 2; iRow++)
            {
                kProd[iRow] = m[iRow][0] * v[0] + m[iRow][1] * v[1];
            }
            return kProd;
        }

        template<typename T>
        mat2<T> mat2<T>::operator*(T fScalar) const
        {
            mat2<T>  kProd;
            for (int iRow = 0; iRow < 2; iRow++)
            {
                for (int iCol = 0; iCol < 2; iCol++)
                {
                    kProd[iRow][iCol] = fScalar * m[iRow][iCol];
                }
            }
            return kProd;
        }

        template<typename T>
        mat2<T> mat2<T>::operator-() const
        {
            mat2<T>  kNeg;
            for (int iRow = 0; iRow < 2; iRow++)
            {
                for (int iCol = 0; iCol < 2; iCol++)
                {
                    kNeg[iRow][iCol] = -m[iRow][iCol];
                }
            }
            return kNeg;
        }

        template<typename T>
        mat2<T> mat2<T>::transpose() const
        {
            mat2<T>  kTranspose;
            for (int iRow = 0; iRow < 2; iRow++)
            {
                for (int iCol = 0; iCol < 2; iCol++)
                {
                    kTranspose[iRow][iCol] = m[iCol][iRow];
                }
            }
            return kTranspose;
        }

        template<typename T>
        bool mat2<T>::inverse(mat2<T> &mInv, T tolerance) const
        {
            T det = determinant();

            if (abs(det) <= tolerance)
            {
                return false;
            }

            T invDet = 1 / det;

            mInv[0][0] = m[1][1] * invDet;
            mInv[0][1] = -m[0][1] * invDet;
            mInv[1][0] = -m[1][0] * invDet;
            mInv[1][1] = m[0][0] * invDet;
            return true;
        }

        template<typename T>
        mat2<T> mat2<T>::inverse(T tolerance) const
        {
            mat2<T> kInverse;
            inverse(kInverse, tolerance);
            return kInverse;
        }

        template<typename T>
        T mat2<T>::determinant() const
        {
            return m[0][0] * m[1][1] - m[1][0] * m[0][1];
        }

        template<typename T>
        T mat2<T>::trace() const
        {
            return m[0][0] + m[1][1];
        }

    }
}

#endif