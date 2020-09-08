#include <cassert>
#include "long_arithmetic.h"


using Integer = LongArithmetic::Integer;

Integer Factorial(int n)
{
    assert(n >= 0);
    Integer currentProduct(1);
    Integer two(2);
    Integer resultingProduct(1);
    for (int i = 1; i <= n; ++i)
    {
        Integer::Multiply(currentProduct, Integer(i), resultingProduct);
        currentProduct = resultingProduct;
    }
    return resultingProduct;
}

Integer Binomial(int n, int k)
{
    if (k > n)
    {
        return Integer(0);
    }
    Integer binomial;
    Integer product;
    Integer::Multiply(Factorial(k), Factorial(n-k), product);
    Integer::Divide(Factorial(n), product, binomial);
    return binomial;
}

bool RunTest(std::ostream& out, int n)
{
    // This test checks validity of equality \sum_{k=0}^n (-1)^k {n \choose k} = 0
    Integer currentSum(0);
    Integer resultingSum(0);
    for (int k = 0; k <= n; ++k)
    {
        Integer binomial = Binomial(n, k);
        if (k % 2)
        {
            Integer::Subtract(currentSum, binomial, resultingSum);
        }
        else
        {
            Integer::Add(currentSum, binomial, resultingSum);
        }
        currentSum = resultingSum;
    }
    if (Integer::Compare(resultingSum, Integer(0)) != 0)
    {
        out << "Test failed: "
            << "\\sum_{k=0}^" << n << " (-1)^k {" << n << "\\choose k} = "
            << resultingSum << " != 0\n";
        return false;
    }
    out << "Test passed\n";
    return true;

}

int main() {
    for (int attempt = 0; attempt < 10; ++attempt)
    {
        int n = (attempt + 1) * (attempt + 1);
        if(!RunTest(std::cout, n))
        {
            return 1;
        }
    }
    return 0;
}

