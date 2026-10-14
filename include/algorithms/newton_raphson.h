#ifndef NEWTON_RAPHSON_H
#define NEWTON_RAPHSON_H

#include <functional>
#include <vector>

struct NewtonIteration {
    int iteration;
    double x;
    double fx;
    double derivative;
    double nextX;
    double error;
};

struct NewtonResult {
    double root;
    double error;
    int iterations;
    bool converged;
    std::vector<NewtonIteration> data;
};

NewtonResult newtonRaphson(
    std::function<double(double)> f,
    std::function<double(double)> derivative,
    double initialGuess,
    double tolerance,
    int maxIterations
);

#endif