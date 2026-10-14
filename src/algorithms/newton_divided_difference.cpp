#include "algorithms/newton_divided_difference.h"

NewtonInterpolationResult newtonDividedDifference(
    const std::vector<double>& x,
    const std::vector<double>& y,
    double targetX
) {
    int n = x.size();

    std::vector<std::vector<double>> table(
        n,
        std::vector<double>(n, 0.0)
    );

    // First column = y values
    for (int i = 0; i < n; i++) {
        table[i][0] = y[i];
    }

    // Divided difference table
    for (int j = 1; j < n; j++) {

        for (int i = 0; i < n - j; i++) {

            table[i][j] =
                (table[i + 1][j - 1] -
                 table[i][j - 1])
                /
                (x[i + j] - x[i]);
        }
    }

    // Newton interpolation formula
    double result = table[0][0];

    double product = 1.0;

    for (int j = 1; j < n; j++) {

        product *= (targetX - x[j - 1]);

        result += table[0][j] * product;
    }

    return {
        result,
        table
    };
}