#ifndef LONG_ARITHMETIC_LONG_ARITHMETIC_H_
#define LONG_ARITHMETIC_LONG_ARITHMETIC_H_

#include "long_number.h"
#include "long_integer.h"
#include "long_rational.h"

namespace LongArithmetic
{
    using Number = LongNumber<LongArithmeticTraits, DEFAULT_NUMBER_BASE>;
    using Integer = LongInteger<LongArithmeticTraits, DEFAULT_NUMBER_BASE>;
    using Rational = LongRational<LongArithmeticTraits, DEFAULT_NUMBER_BASE>;
};

#endif
