#include "algorithms/newton_raphson.h"
#include <cmath>

NewtonResult newtonRaphson(
    std::function<double(double)> f,
    std::function<double(double)> derivative,
    double initialGuess,
    double tolerance,
    int maxIterations
) {
    NewtonResult result{initialGuess, 0, 0, false, {}};

    double x = initialGuess;

    for (int i = 1; i <= maxIterations; i++) {

        double fx = f(x);
        double dfx = derivative(x);

        // Avoid division by zero
        if (std::abs(dfx) < 1e-12) {
            break;
        }

        double nextX = x - (fx / dfx);

        double error = std::abs(nextX - x);

        result.data.push_back({
            i,
            x,
            fx,
            dfx,
            nextX,
            error
        });

        result.root = nextX;
        result.error = error;
        result.iterations = i;

        if (error < tolerance ||
            std::abs(f(nextX)) < tolerance) {

            result.converged = true;
            break;
        }

        x = nextX;
    }

    return result;
}