#include "stdafx.h"

// Low level implementation of long arithmetic
#include "LongArithmetic.h"

int main()
{
    // Straightforward computation of 2^{10000}

    using Integer = LongArithmetic::Integer;

    Integer currentProduct(1);
    Integer two(2);
    Integer resultingProduct(1);

    for (int i = 0; i < 10000; ++i)
    {
        Integer::Multiply(currentProduct, two, resultingProduct);
        currentProduct = resultingProduct;
    }

    std::cout << resultingProduct;
    return 0;
}