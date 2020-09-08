#ifndef LONG_ARITHMETIC_LONG_RATIONAL_H_
#define LONG_ARITHMETIC_LONG_RATIONAL_H_

#include "long_integer.h"

namespace LongArithmetic
{
    // Rational numbers of arbitrary size
    template <typename Traits, typename Traits::BaseType Base>
    class LongRational
    {
    public:
        typedef LongInteger<Traits, Base> IntegerType;
        using IntType = typename IntegerType::IntType;
        using SizeType = typename IntegerType::SizeType;
        using ValueType = typename IntegerType::ValueType;
        using DigitType = typename IntegerType::DigitType;
        using SignType = typename IntegerType::SignType;

        LongRational() : numerator_(0), denominator_(1)
        {
        }

        LongRational(const ValueType number)
            : numerator_(number),
            denominator_(1)
        {
        }

        LongRational(const ValueType numerator, const ValueType denominator)
            : numerator_(numerator),
            denominator_(denominator)
        {
            Normalize(true);
        }

        LongRational(const IntegerType& number)
            : numerator_(number),
            denominator_(1)
        {
        }

        LongRational(const IntegerType& numerator, const IntegerType& denominator)
            : numerator_(numerator),
            denominator_(denominator)
        {
            Normalize(true);
        }

        LongRational(const LongRational<Traits, Base>& rational)
            : numerator_(rational.numerator_),
            denominator_(rational.denominator_)
        {
        }

        LongRational<Traits, Base>& operator = (const LongRational<Traits, Base>& rational)
        {
            if (this != std::addressof(rational))
            {
                numerator_ = rational.numerator_;
                denominator_ = rational.denominator_;
            }
            return *this;
        }


        static int Compare(const LongRational<Traits, Base>& lhs,
            const LongRational<Traits, Base>& rhs)
        {
            IntegerType leftProduct, rightProduct;
            IntegerType::Multiply(lhs.numerator_, rhs.denominator_, leftProduct);
            IntegerType::Multiply(lhs.denominator_, rhs.numerator_, rightProduct);
            return IntegerType::Compare(leftProduct, rightProduct);
        }

        static void Add(const LongRational<Traits, Base>& rational,
            const LongRational<Traits, Base>& summand,
            LongRational<Traits, Base>& result)
        {
            IntegerType leftProduct, rightProduct;
            IntegerType::Multiply(rational.numerator_, summand.denominator_, leftProduct);
            IntegerType::Multiply(rational.denominator_, summand.numerator_, rightProduct);
            IntegerType::Add(leftProduct, rightProduct, result.numerator_);
            IntegerType::Multiply(rational.denominator_, summand.denominator_, result.denominator_);
            result.Normalize(true);
        }

        static void Subtract(const LongRational<Traits, Base>& rational,
            const LongRational<Traits, Base>& subtrahend,
            LongRational<Traits, Base>& result)
        {
            IntegerType leftProduct, rightProduct;
            IntegerType::Multiply(rational.numerator_, subtrahend.denominator_, leftProduct);
            IntegerType::Multiply(rational.denominator_, subtrahend.numerator_, rightProduct);
            IntegerType::Subtract(leftProduct, rightProduct, result.numerator_);
            IntegerType::Multiply(rational.denominator_, subtrahend.denominator_, result.denominator_);
            result.Normalize(true);
        }

        static void Multiply(const LongRational<Traits, Base>& rational,
            const LongRational<Traits, Base>& multiplier,
            LongRational<Traits, Base>& result)
        {
            IntegerType upProduct, downProduct;
            IntegerType::Multiply(rational.numerator_, multiplier.numerator_, upProduct);
            IntegerType::Multiply(rational.denominator_, multiplier.denominator_, downProduct);
            result.numerator_ = upProduct, result.denominator_ = downProduct;
            result.Normalize(true);
        }

        static void Divide(const LongRational<Traits, Base>& rational,
            const LongRational<Traits, Base>& divisor,
            LongRational<Traits, Base>& result)
        {
            IntegerType up_product, down_product;
            IntegerType::Multiply(rational.numerator_, divisor.denominator_, up_product);
            IntegerType::Multiply(rational.denominator_, divisor.numerator_, down_product);
            result.numerator_ = up_product, result.denominator_ = down_product;
            result.Normalize(false);
        }

        friend std::istream& operator >> (std::istream& in, LongRational<Traits, Base>& rational)
        {
            std::string buffer;
            in >> buffer;
            std::string::size_type divisorPosition = buffer.find('/');
            std::stringstream numeratorStream(buffer.substr(0, divisorPosition));
            numeratorStream >> rational.numerator_;
            std::stringstream denominatorStream(divisorPosition != std::string::npos ?
                buffer.substr(divisorPosition + 1) :
                "1");
            denominatorStream >> rational.denominator_;
            rational.Normalize(true);
            return in;
        }

        friend std::ostream& operator << (std::ostream& out,
            const LongRational<Traits, Base>& rational)
        {
            LongRational<Traits, Base> rational_copy(rational);
            rational_copy.Normalize(true);
            if (!IntegerType::Compare(rational_copy.numerator_, IntegerType(0)) ||
                !IntegerType::Compare(rational_copy.denominator_, IntegerType(1)))
                return out << rational_copy.numerator_;
            else
                return out << rational_copy.numerator_ << '/' << rational_copy.denominator_;
        }

        void Normalize(bool reduce)
        {
            if (!IntegerType::Compare(denominator_, IntegerType(0)))
            {
                throw std::domain_error("Division by zero");
            }
            if (!IntegerType::Compare(numerator_, IntegerType(0)))
            {
                denominator_ = IntegerType(1);
            }
            if (reduce)
            {
                IntegerType divisor = GCD(numerator_, denominator_);
                IntegerType::Divide(numerator_, divisor, numerator_);
                IntegerType::Divide(denominator_, divisor, denominator_);
            }
            numerator_.Sign() *= denominator_.Sign();
            denominator_.Sign() *= denominator_.Sign();
        }

        IntegerType Numerator() const
        {
            return numerator_;
        }

        IntegerType Denominator() const
        {
            return denominator_;
        }

        SignType Sign() const
        {
            return numerator_.Sign();
        }

        SignType& Sign()
        {
            return numerator_.Sign();
        }

    private:
        static IntegerType GCD(IntegerType lhs, IntegerType rhs)
        {
            IntegerType zero(0);
            while (true)
            {
                if (!IntegerType::Compare(lhs, zero))
                {
                    return rhs;
                }
                IntegerType::Mod(rhs, lhs, rhs);
                if (!IntegerType::Compare(rhs, zero))
                {
                    return lhs;
                }
                IntegerType::Mod(lhs, rhs, lhs);
            }
        }

    private:
        IntegerType numerator_;
        IntegerType denominator_; // Note that always denominator_ > 0
    };
};

#endif
