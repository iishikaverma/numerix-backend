#include "algorithms/bisection.h"
#include <cmath>

BisectionResult bisection(
    std::function<double(double)> f,
    double a,
    double b,
    double tolerance,
    int maxIterations
) {
    BisectionResult result{0, 0, 0, false, {}};

    double fa = f(a);
    double fb = f(b);

    // Root must be bracketed
    if (fa * fb > 0) {
        return result;
    }

    double previousC = 0.0;

    for (int i = 1; i <= maxIterations; i++) {

        double c = (a + b) / 2.0;
        double fc = f(c);

        double error;

        if (i == 1)
            error = std::abs(b - a);
        else
            error = std::abs(c - previousC);

        result.data.push_back({
            i,
            a,
            b,
            c,
            fc,
            error
        });

        result.root = c;
        result.error = error;
        result.iterations = i;

        if (std::abs(fc) < tolerance || error < tolerance) {
            result.converged = true;
            break;
        }

        if (fa * fc < 0) {
            b = c;
            fb = fc;
        } else {
            a = c;
            fa = fc;
        }

        previousC = c;
    }

    return result;
}