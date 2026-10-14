#ifndef NEWTON_DIVIDED_DIFFERENCE_H
#define NEWTON_DIVIDED_DIFFERENCE_H

#include <vector>

struct NewtonInterpolationResult {
    double value;
    std::vector<std::vector<double>> table;
};

NewtonInterpolationResult newtonDividedDifference(
    const std::vector<double>& x,
    const std::vector<double>& y,
    double targetX
);

#endif