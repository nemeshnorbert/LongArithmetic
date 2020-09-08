#ifndef LONG_ARITHMETIC_LONG_NUMBER_H_
#define LONG_ARITHMETIC_LONG_NUMBER_H_

#include <iostream>
#include <limits>
#include <stdexcept>
#include <sstream>

#include "long_arithmetic_traits.h"

namespace LongArithmetic
{
    // Natural numbers of arbitrary size
    template <typename Traits, typename Traits::BaseType Base>
    class LongNumber
    {
    public:
        using DigitType = typename Traits::DigitType;
        using ValueType = typename Traits::ValueType;
        using UnsignedValueType = typename Traits::UnsignedValueType;
        using SizeType = typename Traits::SizeType;
        using IntType = typename Traits::IntType;
        using BaseType = typename Traits::BaseType;

        static_assert(std::numeric_limits<ValueType>::is_signed
            &&  std::numeric_limits<ValueType>::is_integer
            &&  std::numeric_limits<IntType>::is_signed
            &&  std::numeric_limits<IntType>::is_integer
            && !std::numeric_limits<DigitType>::is_signed
            &&  std::numeric_limits<DigitType>::is_integer
            && !std::numeric_limits<UnsignedValueType>::is_signed
            &&  std::numeric_limits<UnsignedValueType>::is_integer
            && !std::numeric_limits<SizeType>::is_signed
            &&  std::numeric_limits<SizeType>::is_integer
            && !std::numeric_limits<BaseType>::is_signed
            &&  std::numeric_limits<BaseType>::is_integer
            && (std::numeric_limits<IntType>::digits >
            std::numeric_limits<SizeType>::digits)
            && (std::numeric_limits<DigitType>::digits >=
            std::numeric_limits<SizeType>::digits)
            && (std::numeric_limits<DigitType>::digits >=
            2 * std::numeric_limits<BaseType>::digits),
            "Wrong configuration for long arithmetics");

        LongNumber()
            : coefficients_(0)
            , length_(1)
            , base_(Base)
        {
            SizeType defaultSize = 8;
            Allocate(defaultSize);
            coefficients_[0] = 0;
        }

        explicit LongNumber(UnsignedValueType integer)
            : coefficients_(0)
            , base_(Base)
        {
            CreateNumber(integer);
        }

        LongNumber(UnsignedValueType integer, SizeType integerSize)
            : coefficients_(0)
            , base_(Base)
        {
            CreateNumber(integer, integerSize);
        }

        LongNumber(const LongNumber<Traits, Base>& number)
            : coefficients_(0)
            , length_(number.length_)
            , base_(number.base_)
        {
            Allocate(number.size_);
            const DigitType* ptrNumber = number.coefficients_;
            for (IntType i = 0; i < length_; ++i)
            {
                coefficients_[i] = ptrNumber[i];
            }
        }

        LongNumber<Traits, Base>& operator = (const LongNumber<Traits, Base>& number)
        {
            if (this != std::addressof(number))
            {
                if (size_ < number.length_)
                {
                    Deallocate();
                    Allocate(number.length_);
                }
                length_ = number.length_;
                const DigitType *ptrNumber = number.coefficients_;
                for (SizeType i = 0; i < length_; ++i)
                {
                    coefficients_[i] = ptrNumber[i];
                }
                base_ = number.base_;
            }
            return *this;
        }

        static int Compare(const LongNumber<Traits, Base>& lhs,
            const LongNumber<Traits, Base>& rhs)
        {
            if (lhs.length_ > rhs.length_)
            {
                return 1;
            }
            if (lhs.length_ < rhs.length_)
            {
                return -1;
            }

            const DigitType* pRight = rhs.coefficients_;
            const DigitType* pLeft = lhs.coefficients_;
            for (IntType i = lhs.length_ - 1; i >= 0; --i)
            {
                if (pLeft[i] > pRight[i])
                {
                    return 1;
                }
                if (pLeft[i] < pRight[i])
                {
                    return -1;
                }
            }
            return 0;
        }

        SizeType& Length()
        {
            return length_;
        }

        SizeType Length() const
        {
            return length_;
        }

        SizeType Size() const
        {
            return size_;
        }

        DigitType& GetDigit(IntType i)
        {
            return coefficients_[i];
        }

        // Function writes result of adding number and summand into result
        // Note that memory allocated for result must suffice for calculation
        static void Add(const LongNumber<Traits, Base>& number,
            const LongNumber<Traits, Base>& summand,
            LongNumber<Traits, Base>& result)
        {
            IntType i;
            IntType temp, carry = 0;
            const DigitType* pNumber = number.coefficients_;
            const DigitType* pSummand = summand.coefficients_;
            if (number.length_ < summand.length_)
            {
                Add(summand, number, result);
                return;
            }
            DigitType* pResult = result.coefficients_;
            for (i = 0; i < summand.length_; ++i)
            {
                temp = pNumber[i] + pSummand[i] + carry;
                if (temp >= Base)
                {
                    pResult[i] = temp - Base;
                    carry = 1;
                }
                else
                {
                    pResult[i] = temp;
                    carry = 0;
                }
            }
            for (; i < number.length_; ++i)
            {
                temp = pNumber[i] + carry;
                if (temp >= Base)
                {
                    pResult[i] = temp - Base;
                    carry = 1;
                }
                else
                {
                    pResult[i] = temp;
                    carry = 0;
                }
            }
            if (carry)
            {
                pResult[i] = carry;
                result.length_ = number.length_ + 1;
            }
            else
            {
                result.length_ = number.length_;
            }
        }

        // Function writes result of subtraction subtrahend from number into result
        // Note that memory allocated for result must suffice for calculation
        static void Subtract(const LongNumber<Traits, Base>& number,
            const LongNumber<Traits, Base>& subtrahend,
            LongNumber<Traits, Base>& result)
        {
            const DigitType* pNumber = number.coefficients_;
            const DigitType* pSubtrahend = subtrahend.coefficients_;
            DigitType* pResult = result.coefficients_;
            IntType i;
            IntType temp, carry = 0;
            for (i = 0; i < subtrahend.length_; ++i)
            {
                temp = carry + pNumber[i] - pSubtrahend[i];
                if (temp < 0)
                {
                    pResult[i] = temp + Base;
                    carry = -1;
                }
                else
                {
                    pResult[i] = temp;
                    carry = 0;
                }
            }
            for (; i < number.length_; ++i)
            {
                temp = pNumber[i] + carry;
                if (temp < 0)
                {
                    pResult[i] = temp + Base;
                    carry = -1;
                }
                else
                {
                    pResult[i] = temp;
                    carry = 0;
                }
            }
            i = number.length_ - 1;
            while (i > 0 && pResult[i] == 0)
            {
                --i;
            }

            result.length_ = static_cast<SizeType>(i + 1);
        }

        // Function writes product of number and multiplier into result
        // Note that
        // Memory allocated for result must suffice for calculation
        // Call Multiply(A,B,A) doesn't work
        static void Multiply(const LongNumber<Traits, Base>& number,
            const LongNumber<Traits, Base>& multiplier,
            LongNumber<Traits, Base>& result)
        {
            IntType i, j;
            IntType carry, temp;
            const DigitType* pNumber = number.coefficients_;
            const DigitType* pMultiplier = multiplier.coefficients_;
            DigitType* pResult = result.coefficients_;
            result.MakeZero();
            for (i = 0; i < number.length_; ++i)
            {
                carry = 0;
                for (j = 0; j < multiplier.length_; ++j)
                {
                    temp = pNumber[i] * pMultiplier[j] + pResult[i + j] + carry;
                    carry = temp / Base;
                    pResult[i + j] = temp - Base * carry;
                }
                pResult[i + j] = carry;
            }
            i = number.length_ + multiplier.length_ - 1;
            while (i > 0 && pResult[i] == 0)
            {
                --i;
            }
            result.length_ = static_cast<SizeType>(i + 1);
        }

        // Function writes result of division of number over divisor into result.
        // Note that memory allocated for result must suffice for calculation.
        static void Divide(const LongNumber<Traits, Base>& number,
            const LongNumber<Traits, Base>& divisor,
            LongNumber<Traits, Base>& result,
            LongNumber<Traits, Base>& surplus)
        {
            if (number.length_ < divisor.length_)
            {
                result.MakeZero();
                surplus = number;
                return;
            }
            if (divisor.length_ == 1)
            {
                SmallDivide(number, divisor.coefficients_[0],
                    result, surplus.coefficients_[0]);
                surplus.length_ = 1;
                return;
            }

            IntType i;
            LongNumber<Traits, Base> numberCopy(0, number.length_ + 1);
            for (i = 0; i < number.length_; ++i)
                numberCopy.coefficients_[i] = number.coefficients_[i];
            numberCopy.coefficients_[number.length_] = 0;
            numberCopy.length_ = number.length_;

            LongNumber<Traits, Base>* ptrDivisor = (LongNumber<Traits, Base>*)(&divisor);

            const DigitType* pDivisor = divisor.coefficients_;
            DigitType* pNumberCopy = numberCopy.coefficients_;
            DigitType* pResult = result.coefficients_;

            IntType diffLength = number.length_ - divisor.length_;

            IntType numberCopyCounter, resultShift;
            DigitType* numberCopyShift;
            IntType tempValue, storageValue, temp;
            IntType factorGuess, helpValue;
            IntType borrow, carry;

            DigitType scale = Base / (pDivisor[divisor.length_ - 1] + 1);
            if (scale > 1)
            {
                SmallMultiply(numberCopy, scale, numberCopy);
                SmallMultiply(*ptrDivisor, scale, *ptrDivisor);
            }

            for (resultShift = diffLength, numberCopyCounter = divisor.length_ + resultShift;
                resultShift >= 0;
                --resultShift, --numberCopyCounter)
            {
                factorGuess = (pNumberCopy[numberCopyCounter] * Base +
                    pNumberCopy[numberCopyCounter - 1]) / pDivisor[divisor.length_ - 1];
                helpValue = (pNumberCopy[numberCopyCounter] * Base +
                    pNumberCopy[numberCopyCounter - 1]) % pDivisor[divisor.length_ - 1];
                while (helpValue < Base)
                {
                    storageValue = pDivisor[divisor.length_ - 2] * factorGuess;
                    tempValue = helpValue * Base + pNumberCopy[numberCopyCounter - 2];
                    if ((storageValue > tempValue) || (factorGuess == Base))
                    {
                        --factorGuess;
                        helpValue += pDivisor[divisor.length_ - 1];
                    }
                    else
                    {
                        break;
                    }
                }
                carry = 0;
                borrow = 0;
                numberCopyShift = pNumberCopy + resultShift;
                for (i = 0; i < divisor.length_; ++i)
                {
                    tempValue = pDivisor[i] * factorGuess + carry;
                    carry = tempValue / Base;
                    tempValue -= carry * Base;
                    storageValue = numberCopyShift[i] - tempValue + borrow;
                    if (storageValue < 0)
                    {
                        numberCopyShift[i] = storageValue + Base;
                        borrow = -1;
                    }
                    else
                    {
                        numberCopyShift[i] = storageValue;
                        borrow = 0;
                    }
                }
                storageValue = numberCopyShift[i] - carry + borrow;
                if (storageValue < 0)
                {
                    numberCopyShift[i] = storageValue + Base;
                    borrow = -1;
                }
                else
                {
                    numberCopyShift[i] = storageValue;
                    borrow = 0;
                }
                if (borrow == 0)
                {
                    pResult[resultShift] = factorGuess;
                }
                else
                {
                    pResult[resultShift] = factorGuess - 1;
                    carry = 0;
                    for (i = 0; i < divisor.length_; ++i)
                    {
                        temp = numberCopyShift[i] + pDivisor[i] + carry;
                        if (temp >= Base)
                        {
                            numberCopyShift[i] = temp - Base;
                            carry = 1;
                        }
                        else
                        {
                            numberCopyShift[i] = temp;
                            carry = 0;
                        }
                    }
                    numberCopyShift[i] = numberCopyShift[i] + carry - Base;
                }
                i = numberCopy.length_ - 1;
                while (i > 0 && pNumberCopy[i] == 0)
                {
                    --i;
                }
                numberCopy.length_ = static_cast<SizeType>(i + 1);
            }
            while (diffLength > 0 && pResult[diffLength] == 0)
            {
                diffLength--;
            }

            result.length_ = static_cast<SizeType>(diffLength + 1);
            if (scale > 1)
            {
                DigitType junk = 0;
                SmallDivide(*ptrDivisor, scale, *ptrDivisor, junk);
                SmallDivide(numberCopy, scale, surplus, junk);
            }
            else
            {
                surplus = numberCopy;
            }
        }

        // Function writes product of number and numeral into result
        // Note that memory allocated for result must suffice for calculation
        static void SmallMultiply(const LongNumber<Traits, Base>& number,
            const DigitType numeral,
            LongNumber<Traits, Base>& result)
        {
            IntType i;
            IntType temp, carry = 0;
            const DigitType* pNumber = number.coefficients_;
            DigitType *pResult = result.coefficients_;
            for (i = 0; i < number.length_; ++i)
            {
                temp = pNumber[i] * numeral + carry;
                carry = temp / Base;
                pResult[i] = temp - carry * Base;
            }
            if (carry)
            {
                pResult[i] = carry;
                result.length_ = number.length_ + 1;
            }
            else
                result.length_ = number.length_;
        }

        // Function writes result of division of number over numeral into result
        // Note that memory allocated for result must suffice for calculation
        static void SmallDivide(const LongNumber<Traits, Base>& number,
            const DigitType numeral,
            LongNumber<Traits, Base>& result,
            DigitType& surplus)
        {
            IntType r = 0, temp;
            const DigitType* pNumber = number.coefficients_;
            DigitType* pResult = result.coefficients_;
            IntType i;
            for (i = number.length_ - 1; i >= 0; --i)
            {
                temp = r * Base + pNumber[i];
                pResult[i] = temp / numeral;
                r = temp - pResult[i] * numeral;
            }
            surplus = r;
            i = number.length_ - 1;
            while (i > 0 && pResult[i] == 0)
            {
                --i;
            }
            result.length_ = static_cast<SizeType>(i + 1);
        }

        // Reads number from stream istr, validate it and converts to inner format
        friend std::istream& operator >> (std::istream& istr,
            LongNumber<Traits, Base>& number)
        {
            LongNumber<Traits, DECIMAL_BASE> decimalNumber;
            bool valid = true;
            decimalNumber.length() = 0;

            std::string numberString;
            istr >> numberString;

            decimalNumber.allocate(numberString.size());
            for (IntType i = 0; i < numberString.size(); ++i)
            {
                char symbol = numberString[i];
                if ('0' <= symbol && symbol <= '9')
                {
                    decimalNumber.GetDigit(i) = symbol - '0';
                }
                else
                {
                    valid = false;
                    break;
                }
            }
            if (decimalNumber.length() != 1)
            {
                //all numbers must start from non-zero digit
                valid &= (decimalNumber.GetDigit(0) != 0);
            }

            if (valid)
            {
                for (IntType i = 0; i < decimalNumber.length() / 2; ++i)
                {
                    //in inner format digits placed in reversed order
                    std::swap(decimalNumber.GetDigit(i),
                        decimalNumber.GetDigit(decimalNumber.length() - i - 1));
                }
                Convert(decimalNumber, number);
            }
            return istr;
        }

        // Converts number to decimal format and writes to stream ostr
        friend std::ostream& operator << (std::ostream& ostr,
            const LongNumber<Traits, Base>& number)
        {
            LongNumber<Traits, DECIMAL_BASE> decimalNumber;
            Convert(number, decimalNumber);
            for (IntType i = decimalNumber.Length() - 1; i >= 0; --i)
            {
                ostr << decimalNumber.GetDigit(i);
            }
            return ostr;
        }

        // Returns a BaseNumber with digits in newBase scale of notation
        template <BaseType oldBase, BaseType newBase>
        static void Convert(const LongNumber<Traits, oldBase> oldNumber,
            LongNumber<Traits, newBase>& newNumber)
        {
            //Prepare enough space
            SizeType newSize;
            if (newBase > oldBase)
            {
                newSize = 1 + oldNumber.Length() / (IntegerLength(newBase, oldBase) - 1);
            }
            else
            {
                newSize = 1 + oldNumber.Length() * IntegerLength(oldBase, newBase);
            }
            if (newNumber.Size() < newSize)
            {
                newNumber.Deallocate();
                newNumber.Allocate(newSize);
            }
            newNumber.Length() = 0;

            LongNumber<Traits, oldBase> newBaseCopy(newBase);
            LongNumber<Traits, oldBase> number(oldNumber);
            LongNumber<Traits, oldBase> quotient(0, number.length_);
            LongNumber<Traits, oldBase> surplus(0, newBaseCopy.length_);

            LongNumber<Traits, oldBase>* pDividend = &number;
            LongNumber<Traits, oldBase>* pQuotient = &quotient;

            do
            {
                Divide(*pDividend, newBaseCopy, *pQuotient, surplus);
                newNumber.GetDigit(newNumber.Length()) = 0;
                for (IntType k = 0, power = 1; k < surplus.length_; ++k, power *= Base)
                {
                    newNumber.GetDigit(newNumber.Length()) +=
                        surplus.coefficients_[k] * power;
                }
                newNumber.Length()++;
                std::swap(pDividend, pQuotient);
                // while(*p_dividend != 0)
            } while (pDividend->length_ != 1 || pDividend->coefficients_[0] != 0);
        }

        // Evaluate length of integer in Base scale of notation
        static SizeType IntegerLength(UnsignedValueType integer,
            BaseType scale)
        {
            if (integer != 0)
            {
                if (scale > 0)
                {
                    SizeType integerLength = 0;
                    while (integer >= 1)
                    {
                        integer /= scale;
                        ++integerLength;
                    }
                    return integerLength;
                }
                else
                {
                    throw std::invalid_argument("Negative parameter");
                }
            }
            else
            {
                return 1;
            }
        }

        /* Creates number in Base scale of notation by value integer */
        void CreateNumber(UnsignedValueType integer)
        {
            Deallocate();
            Allocate(IntegerLength(integer, Base));
            length_ = size_;
            for (IntType i = 0; i < length_; ++i)
            {
                coefficients_[i] = DigitType(integer % Base);
                integer = (integer - coefficients_[i]) / Base;
            }
        }

        // Creates number in Base scale of notation by value integer
        // and allocates integerSize elements for coefficients_
        void CreateNumber(UnsignedValueType integer, SizeType integerSize)
        {
            SizeType integerLength = IntegerLength(integer, Base);
            if (integerLength <= SizeType(integerSize))
            {
                length_ = integerLength;
                Deallocate();
                Allocate(integerSize);
                for (IntType i = 0; i < length_; ++i)
                {
                    coefficients_[i] = DigitType(integer % Base);
                    integer = (integer - coefficients_[i]) / Base;
                }
            }
            else
                throw std::out_of_range("Parameter out of range");
        }

        // Sets all allocated digits to zero
        void MakeZero()
        {
            for (IntType i = 0; i < size_; ++i)
            {
                coefficients_[i] ^= coefficients_[i];
            }
            length_ = 1;
        }

        // Allocates new data and set new value for size_, without freeing old data
        void Allocate(SizeType newSize)
        {
            coefficients_ = new DigitType[newSize];
            if (coefficients_ == 0)
            {
                throw std::runtime_error("Can\'t allocate memory");
            }
            size_ = newSize;
        }

        // Changes size of coefficients_ array
        void Resize(SizeType newSize)
        {
            DigitType* storage = new DigitType[newSize];
            if (storage == 0)
                throw std::runtime_error("Can\'t reallocate memory");
            SizeType rest = std::min(length_, newSize);
            if (coefficients_ != 0)
            {
                for (IntType i = 0; i < rest; ++i)
                    storage[i] = coefficients_[i];
                delete[] coefficients_;
            }
            coefficients_ = storage;
            size_ = newSize;
        }

        // Ensures that newSize of elements there are in coefficients_
        void Reserve(SizeType newSize)
        {
            if (newSize > size_)
            {
                DigitType* storage = new DigitType[newSize];
                if (storage == 0)
                    throw std::runtime_error("Can\'t reallocate memory");
                if (coefficients_ != 0)
                {
                    for (IntType i = 0; i < length_; ++i)
                        storage[i] = coefficients_[i];
                    delete[] coefficients_;
                }
                coefficients_ = storage;
                size_ = newSize;
            }
        }

        // Destroys data with check for null pointer and set size_ to zero
        void Deallocate()
        {
            if (coefficients_ != 0)
                delete[] coefficients_;
            size_ = 0;
        }

        // Destroys data allocated and set to zero length_ and size_
        ~LongNumber()
        {
            Deallocate();
            length_ = 0;
        }

    private:
        DigitType *coefficients_;
        SizeType length_;
        SizeType size_;
        BaseType base_;
    };
};

#endif
