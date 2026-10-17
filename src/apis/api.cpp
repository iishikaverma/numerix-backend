#include "crow.h"
#include "algorithms/bisection.h"
#include "algorithms/secant.h"
#include "algorithms/newton_raphson.h"
#include "algorithms/lagrange.h"
#include "algorithms/newton_divided_difference.h"
#include "utils/expression_parser.h"

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

        std::string expression = body["expression"].s();

        auto parsed = parseExpression(expression);

        auto f = parsed.evaluate;

        BisectionResult result =
            bisection(f, a, b, tolerance, 100);

        crow::json::wvalue response;

        response["root"] = result.root;
        response["iterations"] = result.iterations;
        response["error"] = result.error;
        response["converged"] = result.converged;

        crow::json::wvalue steps{
            crow::json::wvalue::list()
        };

        for (const auto& step : result.data)
        {
            crow::json::wvalue item;

            item["iteration"] = step.iteration;
            item["a"] = step.a;
            item["b"] = step.b;
            item["c"] = step.c;
            item["fc"] = step.fc;
            item["error"] = step.error;

            steps[steps.size()] = std::move(item);
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

        std::string expression = body["expression"].s();

        auto parsed = parseExpression(expression);

        auto f = parsed.evaluate;

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

        try 
        {

            std::string expression = body["expression"].s();

            double initialGuess =
                body["initialGuess"].d();

            double tolerance =
                body["tolerance"].d();

            int maxIterations = 100;

            auto parsed = parseExpression(expression);

            auto f = parsed.evaluate;
            auto derivative = parsed.derivative;

            auto result = newtonRaphson(
                f,
                derivative,
                initialGuess,
                tolerance,
                maxIterations
            );

            crow::json::wvalue response;

            response["root"] = result.root;
            response["iterations"] = result.iterations;
            response["error"] = result.error;
            response["converged"] = result.converged;

            crow::json::wvalue steps{
                crow::json::wvalue::list()
            };

            for (const auto& step : result.data) {

                crow::json::wvalue item;

                item["iteration"] = step.iteration;
                item["x"] = step.x;
                item["fx"] = step.fx;
                item["derivative"] = step.derivative;
                item["nextX"] = step.nextX;
                item["error"] = step.error;

                steps[steps.size()] = std::move(item);
            }

            response["steps"] = std::move(steps);

            return crow::response(response);

        }
        catch (const std::exception& e) {

            return crow::response(
                400,
                e.what()
            );
        }
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

        crow::json::wvalue steps{
            crow::json::wvalue::list()
        };

        for (const auto& step : result.data)
        {
            crow::json::wvalue item;

            item["index"] = step.index;
            item["basis"] = step.basis;
            item["contribution"] = step.contribution;

            steps[steps.size()] = std::move(item);
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
        crow::json::wvalue table{
            crow::json::wvalue::list()
        };

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