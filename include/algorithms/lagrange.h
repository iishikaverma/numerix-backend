#ifndef LAGRANGE_H
#define LAGRANGE_H

#include <vector>

struct LagrangeTerm {
    int index;
    double basis;
    double contribution;
};

struct LagrangeResult {
    double value;
    std::vector<LagrangeTerm> data;
};

LagrangeResult lagrangeInterpolation(
    const std::vector<double>& x,
    const std::vector<double>& y,
    double targetX
);

#endif