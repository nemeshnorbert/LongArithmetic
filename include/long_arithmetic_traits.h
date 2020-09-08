#ifndef LONG_ARITHMETIC_LONG_ARITHMETIC_TRAITS_H_
#define LONG_ARITHMETIC_LONG_ARITHMETIC_TRAITS_H_

namespace LongArithmetic
{
    const unsigned int DECIMAL_BASE = 10;
    const unsigned int DEFAULT_NUMBER_BASE = 100000000;

    class LongArithmeticTraits
    {
    public:
        using DigitType = unsigned long long;
        using ValueType = long long;
        using UnsignedValueType = unsigned long long;
        using SizeType = unsigned int;
        using IntType = long long;
        using BaseType = unsigned int;
    };
};

#endif