#ifndef BISECTION_H
#define BISECTION_H

#include <functional>
#include <vector>

struct BisectionIteration {
    int iteration;
    double a;
    double b;
    double c;
    double fc;
    double error;
};

struct BisectionResult {
    double root;
    double error;
    int iterations;
    bool converged;
    std::vector<BisectionIteration> data;
};

BisectionResult bisection(
    std::function<double(double)> f,
    double a,
    double b,
    double tolerance,
    int maxIterations
);

#endif