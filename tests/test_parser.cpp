// Standalone correctness check for the expression parser/evaluator.
// Build (from numerix-backend/):
//   g++ -std=c++17 -I include tests/test_parser.cpp \
//       src/utils/expression_parser.cpp src/utils/differentiation.cpp \
//       src/utils/expression_evaluator.cpp -o test_parser && ./test_parser
// No test framework — assert-based.

#include "utils/expression_parser.h"

#include <cassert>
#include <cmath>
#include <stdexcept>
#include <iostream>

static bool approx(double a, double b) {
    return std::abs(a - b) < 1e-9;
}

int main() {
    // Fix #1: unary minus binds looser than '^', so -x^2 == -(x^2).
    assert(approx(parseExpression("-x^2").evaluate(3), -9.0));      // was +9 before fix
    assert(approx(parseExpression("-x^2 + 1").evaluate(2), -3.0));
    assert(approx(parseExpression("-2*3").evaluate(0), -6.0));
    assert(approx(parseExpression("-(x^2)").evaluate(3), -9.0));    // explicit form, same result

    // Derivative of -x^2 is -2x -> at x=3, -6 (confirms corrected tree is differentiated).
    assert(approx(parseExpression("-x^2").derivative(3), -6.0));

    // Fix #2: non-finite power result is a clear error, not a silent NaN/inf.
    bool threw = false;
    try {
        parseExpression("(-8)^0.5").evaluate(0);  // sqrt of negative -> NaN
    } catch (const std::runtime_error&) {
        threw = true;
    }
    assert(threw);

    std::cout << "All parser correctness checks passed.\n";
    return 0;
}
