#include <iostream>
#include <iomanip>
#include <vector>
#include <cmath>

#include "algorithms/bisection.h"
#include "algorithms/secant.h"
#include "algorithms/newton_raphson.h"
#include "algorithms/lagrange.h"
#include "algorithms/newton_divided_difference.h"
#include "apis/api.h"

using namespace std;

int main()
{
    cout << fixed << setprecision(6);

    cout << "========================================\n";
    cout << "             NUMERIX BACKEND\n";
    cout << "========================================\n\n";


    // ========================================
    // FUNCTION
    // f(x) = x^3 - x - 2
    // ========================================

    auto f = [](double x) {
        return x * x * x - x - 2;
    };

    auto derivative = [](double x) {
        return 3 * x * x - 1;
    };


    // ========================================
    // 1. BISECTION
    // ========================================

    cout << "1. BISECTION METHOD\n";
    cout << "----------------------------------------\n";

    BisectionResult bisectionResult =
        bisection(f, 1, 2, 0.0001, 100);

    cout << "Root       : "
         << bisectionResult.root << endl;

    cout << "Iterations : "
         << bisectionResult.iterations << endl;

    cout << "Error      : "
         << bisectionResult.error << endl;

    cout << "Converged  : "
         << (bisectionResult.converged ? "Yes" : "No")
         << "\n\n";


    // ========================================
    // 2. SECANT
    // ========================================

    cout << "2. SECANT METHOD\n";
    cout << "----------------------------------------\n";

    SecantResult secantResult =
        secant(f, 1, 2, 0.0001, 100);

    cout << "Root       : "
         << secantResult.root << endl;

    cout << "Iterations : "
         << secantResult.iterations << endl;

    cout << "Error      : "
         << secantResult.error << endl;

    cout << "Converged  : "
         << (secantResult.converged ? "Yes" : "No")
         << "\n\n";


    // ========================================
    // 3. NEWTON-RAPHSON
    // ========================================

    cout << "3. NEWTON-RAPHSON METHOD\n";
    cout << "----------------------------------------\n";

    NewtonResult newtonResult =
        newtonRaphson(
            f,
            derivative,
            1.5,
            0.0001,
            100
        );

    cout << "Root       : "
         << newtonResult.root << endl;

    cout << "Iterations : "
         << newtonResult.iterations << endl;

    cout << "Error      : "
         << newtonResult.error << endl;

    cout << "Converged  : "
         << (newtonResult.converged ? "Yes" : "No")
         << "\n\n";


    // ========================================
    // INTERPOLATION DATA
    // ========================================

    vector<double> x = {1, 2, 3, 4};
    vector<double> y = {1, 4, 9, 16};

    double targetX = 2.5;


    // ========================================
    // 4. LAGRANGE
    // ========================================

    cout << "4. LAGRANGE INTERPOLATION\n";
    cout << "----------------------------------------\n";

    LagrangeResult lagrangeResult =
        lagrangeInterpolation(
            x,
            y,
            targetX
        );

    cout << "x = "
         << targetX << endl;

    cout << "Interpolated value = "
         << lagrangeResult.value
         << "\n\n";


    // ========================================
    // 5. NEWTON DIVIDED DIFFERENCE
    // ========================================

    cout << "5. NEWTON DIVIDED DIFFERENCE\n";
    cout << "----------------------------------------\n";

    NewtonInterpolationResult newtonInterpolation =
        newtonDividedDifference(
            x,
            y,
            targetX
        );

    cout << "x = "
         << targetX << endl;

    cout << "Interpolated value = "
         << newtonInterpolation.value
         << "\n\n";


    cout << "========================================\n";
    cout << "          NUMERIX TEST COMPLETE\n";
    cout << "========================================\n";

    startServer();

    return 0;
}