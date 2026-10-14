#include "crow.h"
#include "algorithms/bisection.h"
#include "algorithms/secant.h"
#include "algorithms/newton_raphson.h"
#include "algorithms/lagrange.h"
#include "algorithms/newton_divided_difference.h"

void startServer()
{
    crow::SimpleApp app;

    // Health check
    CROW_ROUTE(app, "/")
    ([]() {
        return "NUMERIX API is running!";
    });

    // Bisection API
    CROW_ROUTE(app, "/api/solve/bisection")
    .methods(crow::HTTPMethod::POST)
    ([](const crow::request& req) {

        auto body = crow::json::load(req.body);

        if (!body)
        {
            return crow::response(400, "Invalid JSON");
        }

        double a = body["a"].d();
        double b = body["b"].d();
        double tolerance = body["tolerance"].d();

        auto f = [](double x) {
            return x * x * x - x - 2;
        };

        BisectionResult result =
            bisection(f, a, b, tolerance, 100);

        crow::json::wvalue response;

        response["root"] = result.root;
        response["iterations"] = result.iterations;
        response["error"] = result.error;
        response["converged"] = result.converged;

        crow::json::wvalue steps;

        for (size_t i = 0; i < result.data.size(); i++)
        {
            steps[i]["iteration"] = result.data[i].iteration;
            steps[i]["a"] = result.data[i].a;
            steps[i]["b"] = result.data[i].b;
            steps[i]["c"] = result.data[i].c;
            steps[i]["fc"] = result.data[i].fc;
            steps[i]["error"] = result.data[i].error;
        }

        response["steps"] = std::move(steps);

        return crow::response(response);
    });

    // Secant API
    CROW_ROUTE(app, "/api/solve/secant")
    .methods(crow::HTTPMethod::POST)
    ([](const crow::request& req) {

        auto body = crow::json::load(req.body);

        if (!body)
        {
            return crow::response(400, "Invalid JSON");
        }

        double x0 = body["x0"].d();
        double x1 = body["x1"].d();
        double tolerance = body["tolerance"].d();

        auto f = [](double x) {
            return x * x * x - x - 2;
        };

        SecantResult result =
            secant(f, x0, x1, tolerance, 100);

        crow::json::wvalue response;

        response["root"] = result.root;
        response["iterations"] = result.iterations;
        response["error"] = result.error;
        response["converged"] = result.converged;

        crow::json::wvalue steps;

        for (size_t i = 0; i < result.data.size(); i++)
        {
            steps[i]["iteration"] = result.data[i].iteration;
            steps[i]["x0"] = result.data[i].x0;
            steps[i]["x1"] = result.data[i].x1;
            steps[i]["x2"] = result.data[i].x2;
            steps[i]["fx2"] = result.data[i].fx2;
            steps[i]["error"] = result.data[i].error;
        }

        response["steps"] = std::move(steps);

        return crow::response(response);
    });

    // Newton-Raphson API
    CROW_ROUTE(app, "/api/solve/newton")
    .methods(crow::HTTPMethod::POST)
    ([](const crow::request& req) {

        auto body = crow::json::load(req.body);

        if (!body)
        {
            return crow::response(400, "Invalid JSON");
        }

        double initialGuess = body["initialGuess"].d();
        double tolerance = body["tolerance"].d();

        auto f = [](double x) {
            return x * x * x - x - 2;
        };

        auto derivative = [](double x) {
            return 3 * x * x - 1;
        };

        NewtonResult result =
            newtonRaphson(
                f,
                derivative,
                initialGuess,
                tolerance,
                100
            );

        crow::json::wvalue response;

        response["root"] = result.root;
        response["iterations"] = result.iterations;
        response["error"] = result.error;
        response["converged"] = result.converged;

        crow::json::wvalue steps;

        for (size_t i = 0; i < result.data.size(); i++)
        {
            steps[i]["iteration"] = result.data[i].iteration;
            steps[i]["x"] = result.data[i].x;
            steps[i]["fx"] = result.data[i].fx;
            steps[i]["derivative"] = result.data[i].derivative;
            steps[i]["nextX"] = result.data[i].nextX;
            steps[i]["error"] = result.data[i].error;
        }

        response["steps"] = std::move(steps);

        return crow::response(response);
    });

    // Lagrange Interpolation API
    CROW_ROUTE(app, "/api/interpolate/lagrange")
    .methods(crow::HTTPMethod::POST)
    ([](const crow::request& req) {

        auto body = crow::json::load(req.body);

        if (!body)
        {
            return crow::response(400, "Invalid JSON");
        }

        std::vector<double> x;
        std::vector<double> y;

        for (const auto& value : body["x"])
        {
            x.push_back(value.d());
        }

        for (const auto& value : body["y"])
        {
            y.push_back(value.d());
        }

        double targetX = body["targetX"].d();

        LagrangeResult result =
            lagrangeInterpolation(
                x,
                y,
                targetX
            );

        crow::json::wvalue response;

        response["value"] = result.value;

        crow::json::wvalue steps;

        for (size_t i = 0; i < result.data.size(); i++)
        {
            steps[i]["index"] = result.data[i].index;
            steps[i]["basis"] = result.data[i].basis;
            steps[i]["contribution"] = result.data[i].contribution;
        }

        response["steps"] = std::move(steps);

        return crow::response(response);
    });

    // Newton Divided Difference API
    CROW_ROUTE(app, "/api/interpolate/divided-difference")
    .methods(crow::HTTPMethod::POST)
    ([](const crow::request& req) {

        auto body = crow::json::load(req.body);

        if (!body)
        {
            return crow::response(400, "Invalid JSON");
        }

        std::vector<double> x;
        std::vector<double> y;

        for (const auto& value : body["x"])
        {
            x.push_back(value.d());
        }

        for (const auto& value : body["y"])
        {
            y.push_back(value.d());
        }

        double targetX = body["targetX"].d();

        NewtonInterpolationResult result =
            newtonDividedDifference(
                x,
                y,
                targetX
            );

        crow::json::wvalue response;

        response["value"] = result.value;

        // Divided difference table
        crow::json::wvalue table;

        for (size_t i = 0; i < result.table.size(); i++)
        {
            for (size_t j = 0; j < result.table[i].size(); j++)
            {
                table[i][j] = result.table[i][j];
            }
        }

        response["table"] = std::move(table);

        return crow::response(response);
    });

    app.port(8080).multithreaded().run();
}