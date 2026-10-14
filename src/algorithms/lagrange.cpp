#include "algorithms/lagrange.h"

LagrangeResult lagrangeInterpolation(
    const std::vector<double>& x,
    const std::vector<double>& y,
    double targetX
) {
    LagrangeResult result{0.0, {}};

    int n = x.size();

    for (int i = 0; i < n; i++) {

        double basis = 1.0;

        for (int j = 0; j < n; j++) {

            if (i != j) {
                basis *=
                    (targetX - x[j]) /
                    (x[i] - x[j]);
            }
        }

        double contribution = y[i] * basis;

        result.value += contribution;

        result.data.push_back({
            i,
            basis,
            contribution
        });
    }

    return result;
}