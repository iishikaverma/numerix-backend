#include "algorithms/secant.h"
#include <cmath>

SecantResult secant(
    std::function<double(double)> f,
    double x0,
    double x1,
    double tolerance,
    int maxIterations
) {
    SecantResult result{0, 0, 0, false, {}};

    for (int i = 1; i <= maxIterations; i++) {

        double f0 = f(x0);
        double f1 = f(x1);

        double denominator = f1 - f0;

        if (std::abs(denominator) < 1e-12) {
            break;
        }

        double x2 =
            x1 - f1 * (x1 - x0) / denominator;

        double fx2 = f(x2);

        double error = std::abs(x2 - x1);

        result.data.push_back({
            i,
            x0,
            x1,
            x2,
            fx2,
            error
        });

        result.root = x2;
        result.error = error;
        result.iterations = i;

        if (error < tolerance || std::abs(fx2) < tolerance) {
            result.converged = true;
            break;
        }

        x0 = x1;
        x1 = x2;
    }

    return result;
}