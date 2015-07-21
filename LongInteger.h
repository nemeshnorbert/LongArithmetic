#ifndef LONG_ARITHMETIC_LONG_INTEGER_H_
#define LONG_ARITHMETIC_LONG_INTEGER_H_

#include "LongNumber.h"

namespace LongArithmetic
{
    // Integers numbers of arbitrary size
    template <typename Traits, typename Traits::BaseType Base>
    class LongInteger
    {
    public:
        using NumberType = LongNumber<Traits, Base>;
        using SignType = signed char;
        using IntType = typename NumberType::IntType;
        using SizeType = typename NumberType::SizeType;
        using ValueType = typename NumberType::ValueType;
        using DigitType = typename NumberType::DigitType;

        LongInteger()
            : value_()
            , sign_(1)
        {}

        explicit LongInteger(ValueType integer)
            : value_(integer >= 0 ? integer : -integer)
            , sign_(integer >= 0 ? 1 : -1)
        {}

        LongInteger(ValueType integer, SizeType integerSize)
            : value_(integer >= 0 ? integer : -integer, integerSize)
            , sign_(integer >= 0 ? 1 : -1)
        {}

        LongInteger(const LongInteger& number)
            : value_(number.value_)
            , sign_(number.sign_)
        {}

        LongInteger<Traits, Base>& operator = (const LongInteger<Traits, Base>& number)
        {
            if (this != std::addressof(number))
            {
                value_ = number.value_;
                sign_ = number.sign_;
            }
            return *this;
        }

        static int Compare(const LongInteger<Traits, Base>& lhs,
            const LongInteger<Traits, Base>& rhs)
        {
            if (lhs.sign_ > 0 && rhs.sign_ > 0)
            {
                return NumberType::Compare(lhs.value_, rhs.value_);
            }
            if (lhs.sign_ < 0 && rhs.sign_ < 0)
            {
                return -NumberType::Compare(lhs.value_, rhs.value_);
            }
            if (lhs.sign_ > 0 && rhs.sign_ < 0)
            {
                return 1;
            }
            return -1;
        }


        static void Add(const LongInteger<Traits, Base>& number,
            const LongInteger<Traits, Base>& summand,
            LongInteger<Traits, Base>& result)
        {
            result.value_.Reserve(number.value_.Length() > summand.value_.Length() ?
                number.value_.Length() + 1 :
                summand.value_.Length() + 1);
            if (number.sign_ > 0 && summand.sign_ > 0)
            {
                NumberType::Add(number.value_, summand.value_, result.value_);
                result.sign_ = 1;

            }
            else if (number.sign_ > 0 && summand.sign_ < 0)
            {
                if (NumberType::Compare(number.value_, summand.value_) != -1)
                {
                    NumberType::Subtract(number.value_, summand.value_, result.value_);
                    result.sign_ = 1;
                }
                else
                {
                    NumberType::Subtract(summand.value_, number.value_, result.value_);
                    result.sign_ = -1;
                }
            }
            else if (number.sign_ < 0 && summand.sign_ > 0)
            {
                if (NumberType::Compare(number.value_, summand.value_) != -1)
                {
                    NumberType::Subtract(number.value_, summand.value_, result.value_);
                    result.sign_ = -1;
                }
                else
                {
                    NumberType::Subtract(summand.value_, number.value_, result.value_);
                    result.sign_ = 1;
                }
            }
            else
            {
                NumberType::Add(number.value_, summand.value_, result.value_);
                result.sign_ = -1;
            }
            result.TestZeroSign();
        }

        static void Subtract(const LongInteger<Traits, Base>& number,
            const LongInteger<Traits, Base>& subtrahend,
            LongInteger<Traits, Base>& result)
        {
            result.value_.Reserve(number.value_.Length() > subtrahend.value_.Length() ?
                number.value_.Length() + 1 :
                subtrahend.value_.Length() + 1);
            if (number.sign_ > 0 && subtrahend.sign_ > 0)
            {
                if (NumberType::Compare(number.value_, subtrahend.value_) != -1)
                {
                    NumberType::Subtract(number.value_, subtrahend.value_, result.value_);
                    result.sign_ = 1;
                }
                else
                {
                    NumberType::Subtract(subtrahend.value_, number.value_, result.value_);
                    result.sign_ = -1;
                }
            }
            else if (number.sign_ > 0 && subtrahend.sign_ < 0)
            {
                NumberType::Add(number.value_, subtrahend.value_, result.value_);
                result.sign_ = 1;
            }
            else if (number.sign_ < 0 && subtrahend.sign_ > 0)
            {
                NumberType::Add(number.value_, subtrahend.value_, result.value_);
                result.sign_ = -1;
            }
            else if (number.sign_ < 0 && subtrahend.sign_ < 0)
            {
                if (NumberType::Compare(number.value_, subtrahend.value_) != -1)
                {
                    NumberType::Subtract(number.value_, subtrahend.value_, result.value_);
                    result.sign_ = -1;
                }
                else
                {
                    NumberType::Subtract(subtrahend.value_, number.value_, result.value_);
                    result.sign_ = 1;
                }
            }
            result.TestZeroSign();
        }

        static void Multiply(const LongInteger<Traits, Base>& number,
            const LongInteger<Traits, Base>& multiplier,
            LongInteger<Traits, Base>& result)
        {
            if (std::addressof(result) != std::addressof(number) &&
                std::addressof(result) != std::addressof(multiplier))
            {
                result.value_.Reserve(
                    number.value_.Length() + multiplier.value_.Length() + 1);
                NumberType::Multiply(number.value_, multiplier.value_, result.value_);
                result.sign_ = number.sign_ * multiplier.sign_;
                result.TestZeroSign();
            }
            else
            {
                LongInteger<Traits, Base> resultCopy(0,
                    number.value_.Length() + multiplier.value_.Length() + 1);
                NumberType::Multiply(number.value_, multiplier.value_, resultCopy.value_);
                resultCopy.sign_ = number.sign_ * multiplier.sign_;
                resultCopy.TestZeroSign();
                result = resultCopy;
            }
        }

        static void Divide(const LongInteger<Traits, Base>& number,
            const LongInteger<Traits, Base>& divisor,
            LongInteger<Traits, Base>& result)
        {
            if (NumberType::Compare(number.value_, divisor.value_) != -1)
            {
                result.value_.Reserve(
                    number.value_.Length() - divisor.value_.Length() + 1);
                LongInteger<Traits, Base> surplus(0, divisor.value_.Length());
                NumberType::Divide(number.value_, divisor.value_,
                    result.value_, surplus.value_);
                result.sign_ = number.sign_ * divisor.sign_;
                result.TestZeroSign();
            }
            else
            {
                result = LongInteger(0);
            }
        }

        static void Mod(const LongInteger<Traits, Base>& number,
            const LongInteger<Traits, Base>& divisor,
            LongInteger<Traits, Base>& result)
        {
            if (NumberType::Compare(number.value_, divisor.value_) != -1)
            {
                result.value_.Reserve(
                    number.value_.Length() - divisor.value_.Length() + 1);
                LongInteger<Traits, Base> quotient(0, number.value_.Length() -
                    divisor.value_.Length() + 1);
                NumberType::Divide(number.value_, divisor.value_,
                    quotient.value_, result.value_);
                result.sign_ = number.sign_;
                result.TestZeroSign();
            }
            else
            {
                result = number;
            }
        }

        friend std::istream& operator >> (std::istream& in,
            LongInteger<Traits, Base>& integer)
        {
            if (in.peek() == '-')
            {
                in.get();
                integer.sign_ = -1;
            }
            else
            {
                integer.sign_ = 1;
            }

            return in >> integer.value_;
        }

        friend std::ostream& operator << (std::ostream& out,
            const LongInteger<Traits, Base>& integer)
        {
            return out << (integer.sign_ < 0 ? "-" : "")
                << integer.value_;
        }

        SignType& Sign()
        {
            return sign_;
        }

        SignType Sign() const
        {
            return sign_;
        }

        static LongInteger<Traits, Base> AbsoluteValue(
            const LongInteger<Traits, Base>& number)
        {
            LongInteger<Traits, Base> modulus(number);
            modulus.sign_ = 1;
            return modulus;
        }

        ~LongInteger()
        {
            sign_ = 1;
        }

    private:
        void TestZeroSign()
        {
            if (value_.Length() == 1 && value_.GetDigit(0) == 0)
            {
                sign_ = 1;
            }
        }

    private:
        SignType sign_;
        NumberType value_;
    };
};

#endif