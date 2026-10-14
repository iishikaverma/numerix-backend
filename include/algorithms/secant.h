#ifndef SECANT_H
#define SECANT_H

#include <functional>
#include <vector>

struct SecantIteration {
    int iteration;
    double x0;
    double x1;
    double x2;
    double fx2;
    double error;
};

struct SecantResult {
    double root;
    double error;
    int iterations;
    bool converged;
    std::vector<SecantIteration> data;
};

SecantResult secant(
    std::function<double(double)> f,
    double x0,
    double x1,
    double tolerance,
    int maxIterations
);

#endif