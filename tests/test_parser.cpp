#include "utils/expression_parser.h"

#include <iostream>

int main() {

    std::string expression = "2x + 3";

    try {

        auto parsed = parseExpression(expression);

        double x = 4.0;

        double functionValue = parsed.evaluate(x);
        double derivativeValue = parsed.derivative(x);

        std::cout << "Expression: "
                  << expression << std::endl;

        std::cout << "x = "
                  << x << std::endl;

        std::cout << "f(x) = "
                  << functionValue << std::endl;

        std::cout << "f'(x) = "
                  << derivativeValue << std::endl;

    }
    catch (const std::exception& e) {

        std::cerr << "Error: "
                  << e.what() << std::endl;

        return 1;
    }

    return 0;
}