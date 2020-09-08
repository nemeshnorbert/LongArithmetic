#include "long_arithmetic.h"

int Run()
{
    std::ostream& out = std::cout;
    std::istream& in = std::cin;
    out << "Straightforward computation of powers of 2.\n";

    using Integer = LongArithmetic::Integer;

    Integer currentProduct(1);
    Integer two(2);
    Integer resultingProduct(1);

    out << "Enter degree: ";
    int degree = 0;
    in >> degree;

    for (int power = degree; power > 0; --power)
    {
        Integer::Multiply(currentProduct, two, resultingProduct);
        currentProduct = resultingProduct;
    }

    out << "2 ** " << degree << " = " << resultingProduct << '\n';
    return 0;
}

int main()
{
    Run();
    return 0;
}
