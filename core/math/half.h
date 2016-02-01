#ifndef GLADIUS_HALF_H
#define GLADIUS_HALF_H

namespace gladius
{
    namespace math
    {
        unsigned short floatToHalf(float x);
        float halfToFloat(unsigned short h);

        struct half
        {
        public:
            half()
            {
            }

            half(float x);

        public:
            inline float toFloat() const;
            inline operator float() const;

        public:
            inline half operator==(const half v) const;
            inline bool operator!=(const half v) const;

        public:
            inline half operator+(const half v) const;
            inline half operator-(const half v) const;
            inline half operator*(const half v) const;
            inline half operator/(const half v) const;

        public:
            inline bool isNaN() const;
            inline bool isFinite() const;
            inline bool isInf() const;
            inline bool isNormal() const;

        private:
            inline int exponent() const;
            inline int mantissa() const;

        private:
            unsigned short value;
        };

        inline half::half(float x) : value(floatToHalf(x))
        {
        }

        inline float half::toFloat() const
        {
            return halfToFloat(value);
        }

        inline half::operator float() const
        {
            return toFloat();
        }

        inline half half::operator==(const half v) const
        {
            return value == v.value;
        }

        bool half::operator!=(const half v) const
        {
            return value != v.value;
        }

        inline half half::operator+(const half v) const
        {
            return half(toFloat() + v.toFloat());
        }

        inline half half::operator-(const half v) const
        {
            return half(toFloat() - v.toFloat());
        }

        inline half half::operator*(const half v) const
        {
            return half(toFloat() * v.toFloat());
        }

        inline half half::operator/(const half v) const
        {
            return half(toFloat() / v.toFloat());
        }

        inline bool half::isNaN() const
        {
            return (exponent() == 31) && (mantissa() != 0);
        }

        inline bool half::isFinite() const
        {
            return (exponent() != 31);
        }

        inline bool half::isInf() const
        {
            return (exponent() == 31) && (mantissa() == 0);
        }

        inline bool half::isNormal() const
        {
            return isFinite() && ((exponent() != 0) || (mantissa() == 0));
        }

        inline int half::exponent() const
        {
            return (value & 0x7C00) >> 10;
        }

        inline int half::mantissa() const
        {
            return value & 0x03ff;
        }

        inline bool isNaN(half x)
        {
            return x.isNaN();
        }
    }
}

#endif