#ifndef LONG_ARITHMETIC_LONG_ARITHMETIC_H_
#define LONG_ARITHMETIC_LONG_ARITHMETIC_H_

#include "LongNumber.h"
#include "LongInteger.h"
#include "LongRational.h"

namespace LongArithmetic
{
    using Number = LongNumber<LongArithmeticTraits, DEFAULT_NUMBER_BASE>;
    using Integer = LongInteger<LongArithmeticTraits, DEFAULT_NUMBER_BASE>;
    using Rational = LongRational<LongArithmeticTraits, DEFAULT_NUMBER_BASE>;
};

#endif