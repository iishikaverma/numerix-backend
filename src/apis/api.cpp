#include "crow.h"
#include "algorithms/bisection.h"
#include "algorithms/secant.h"
#include "algorithms/newton_raphson.h"
#include "algorithms/lagrange.h"
#include "algorithms/newton_divided_difference.h"
#include "utils/expression_parser.h"

#include <cmath>
#include <string>
#include <vector>
#include <stdexcept>

namespace
{
    bool isNumber(const crow::json::rvalue& value)
    {
        return value.t() == crow::json::type::Number;
    }

    bool isString(const crow::json::rvalue& value)
    {
        return value.t() == crow::json::type::String;
    }

    bool isArray(const crow::json::rvalue& value)
    {
        return value.t() == crow::json::type::List;
    }

    bool isFinite(double value)
    {
        return std::isfinite(value);
    }

    bool hasRequiredFields(
        const crow::json::rvalue& body,
        const std::vector<std::string>& fields
    )
    {
        for (const auto& field : fields)
        {
            if (!body.has(field))
            {
                return false;
            }
        }

        return true;
    }

    bool hasDuplicateValues(const std::vector<double>& values)
    {
        for (size_t i = 0; i < values.size(); i++)
        {
            for (size_t j = i + 1; j < values.size(); j++)
            {
                if (values[i] == values[j])
                {
                    return true;
                }
            }
        }

        return false;
    }
}

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

        if (!hasRequiredFields(
                body,
                {"expression", "a", "b", "tolerance"}))
        {
            return crow::response(
                400,
                "Missing required fields: expression, a, b, tolerance"
            );
        }

        if (!isString(body["expression"]) ||
            !isNumber(body["a"]) ||
            !isNumber(body["b"]) ||
            !isNumber(body["tolerance"]))
        {
            return crow::response(
                400,
                "Invalid field types"
            );
        }

        try
        {
            std::string expression =
                body["expression"].s();

            if (expression.empty())
            {
                return crow::response(
                    400,
                    "Expression cannot be empty"
                );
            }

            double a = body["a"].d();
            double b = body["b"].d();
            double tolerance = body["tolerance"].d();

            if (!isFinite(a) ||
                !isFinite(b) ||
                !isFinite(tolerance))
            {
                return crow::response(
                    400,
                    "Values must be finite numbers"
                );
            }

            if (a >= b)
            {
                return crow::response(
                    400,
                    "Invalid interval: a must be less than b"
                );
            }

            if (tolerance <= 0)
            {
                return crow::response(
                    400,
                    "Tolerance must be greater than 0"
                );
            }

            auto parsed = parseExpression(expression);

            auto f = parsed.evaluate;

            double fa = f(a);
            double fb = f(b);

            if (!isFinite(fa) || !isFinite(fb))
            {
                return crow::response(
                    400,
                    "Function is not finite at the interval endpoints"
                );
            }

            if (fa * fb > 0)
            {
                return crow::response(
                    400,
                    "Invalid interval: function values must have opposite signs"
                );
            }

            BisectionResult result =
                bisection(
                    f,
                    a,
                    b,
                    tolerance,
                    100
                );

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

                steps[steps.size()] =
                    std::move(item);
            }

            response["steps"] =
                std::move(steps);

            return crow::response(response);
        }
        catch (const std::exception& e)
        {
            return crow::response(
                400,
                e.what()
            );
        }
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

        if (!hasRequiredFields(
                body,
                {"expression", "x0", "x1", "tolerance"}))
        {
            return crow::response(
                400,
                "Missing required fields: expression, x0, x1, tolerance"
            );
        }

        if (!isString(body["expression"]) ||
            !isNumber(body["x0"]) ||
            !isNumber(body["x1"]) ||
            !isNumber(body["tolerance"]))
        {
            return crow::response(
                400,
                "Invalid field types"
            );
        }

        try
        {
            std::string expression =
                body["expression"].s();

            if (expression.empty())
            {
                return crow::response(
                    400,
                    "Expression cannot be empty"
                );
            }

            double x0 = body["x0"].d();
            double x1 = body["x1"].d();
            double tolerance = body["tolerance"].d();

            if (!isFinite(x0) ||
                !isFinite(x1) ||
                !isFinite(tolerance))
            {
                return crow::response(
                    400,
                    "Values must be finite numbers"
                );
            }

            if (x0 == x1)
            {
                return crow::response(
                    400,
                    "x0 and x1 must be different"
                );
            }

            if (tolerance <= 0)
            {
                return crow::response(
                    400,
                    "Tolerance must be greater than 0"
                );
            }

            auto parsed =
                parseExpression(expression);

            auto f = parsed.evaluate;

            double fx0 = f(x0);
            double fx1 = f(x1);

            if (!isFinite(fx0) ||
                !isFinite(fx1))
            {
                return crow::response(
                    400,
                    "Function is not finite at the initial points"
                );
            }

            SecantResult result =
                secant(
                    f,
                    x0,
                    x1,
                    tolerance,
                    100
                );

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
                item["x0"] = step.x0;
                item["x1"] = step.x1;
                item["x2"] = step.x2;
                item["fx2"] = step.fx2;
                item["error"] = step.error;

                steps[steps.size()] =
                    std::move(item);
            }

            response["steps"] =
                std::move(steps);

            return crow::response(response);
        }
        catch (const std::exception& e)
        {
            return crow::response(
                400,
                e.what()
            );
        }
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

        if (!hasRequiredFields(
                body,
                {"expression", "initialGuess", "tolerance"}))
        {
            return crow::response(
                400,
                "Missing required fields: expression, initialGuess, tolerance"
            );
        }

        if (!isString(body["expression"]) ||
            !isNumber(body["initialGuess"]) ||
            !isNumber(body["tolerance"]))
        {
            return crow::response(
                400,
                "Invalid field types"
            );
        }

        try
        {
            std::string expression =
                body["expression"].s();

            if (expression.empty())
            {
                return crow::response(
                    400,
                    "Expression cannot be empty"
                );
            }

            double initialGuess =
                body["initialGuess"].d();

            double tolerance =
                body["tolerance"].d();

            if (!isFinite(initialGuess) ||
                !isFinite(tolerance))
            {
                return crow::response(
                    400,
                    "Values must be finite numbers"
                );
            }

            if (tolerance <= 0)
            {
                return crow::response(
                    400,
                    "Tolerance must be greater than 0"
                );
            }

            auto parsed =
                parseExpression(expression);

            auto f = parsed.evaluate;
            auto derivative = parsed.derivative;

            if (!isFinite(f(initialGuess)) ||
                !isFinite(derivative(initialGuess)))
            {
                return crow::response(
                    400,
                    "Function or derivative is not finite at the initial guess"
                );
            }

            auto result =
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

            crow::json::wvalue steps{
                crow::json::wvalue::list()
            };

            for (const auto& step : result.data)
            {
                crow::json::wvalue item;

                item["iteration"] = step.iteration;
                item["x"] = step.x;
                item["fx"] = step.fx;
                item["derivative"] = step.derivative;
                item["nextX"] = step.nextX;
                item["error"] = step.error;

                steps[steps.size()] =
                    std::move(item);
            }

            response["steps"] =
                std::move(steps);

            return crow::response(response);
        }
        catch (const std::exception& e)
        {
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

        if (!hasRequiredFields(
                body,
                {"x", "y", "targetX"}))
        {
            return crow::response(
                400,
                "Missing required fields: x, y, targetX"
            );
        }

        if (!isArray(body["x"]) ||
            !isArray(body["y"]) ||
            !isNumber(body["targetX"]))
        {
            return crow::response(
                400,
                "x and y must be arrays and targetX must be a number"
            );
        }

        try
        {
            std::vector<double> x;
            std::vector<double> y;

            for (const auto& value : body["x"])
            {
                if (!isNumber(value))
                {
                    return crow::response(
                        400,
                        "All x values must be numbers"
                    );
                }

                double number = value.d();

                if (!isFinite(number))
                {
                    return crow::response(
                        400,
                        "All x values must be finite"
                    );
                }

                x.push_back(number);
            }

            for (const auto& value : body["y"])
            {
                if (!isNumber(value))
                {
                    return crow::response(
                        400,
                        "All y values must be numbers"
                    );
                }

                double number = value.d();

                if (!isFinite(number))
                {
                    return crow::response(
                        400,
                        "All y values must be finite"
                    );
                }

                y.push_back(number);
            }

            double targetX =
                body["targetX"].d();

            if (!isFinite(targetX))
            {
                return crow::response(
                    400,
                    "targetX must be a finite number"
                );
            }

            if (x.empty() || y.empty())
            {
                return crow::response(
                    400,
                    "x and y arrays cannot be empty"
                );
            }

            if (x.size() != y.size())
            {
                return crow::response(
                    400,
                    "x and y arrays must have the same size"
                );
            }

            if (x.size() < 2)
            {
                return crow::response(
                    400,
                    "At least two data points are required"
                );
            }

            if (hasDuplicateValues(x))
            {
                return crow::response(
                    400,
                    "x values must be unique"
                );
            }

            LagrangeResult result =
                lagrangeInterpolation(
                    x,
                    y,
                    targetX
                );

            crow::json::wvalue response;

            response["value"] =
                result.value;

            crow::json::wvalue steps{
                crow::json::wvalue::list()
            };

            for (const auto& step : result.data)
            {
                crow::json::wvalue item;

                item["index"] = step.index;
                item["basis"] = step.basis;
                item["contribution"] =
                    step.contribution;

                steps[steps.size()] =
                    std::move(item);
            }

            response["steps"] =
                std::move(steps);

            return crow::response(response);
        }
        catch (const std::exception& e)
        {
            return crow::response(
                400,
                e.what()
            );
        }
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

        if (!hasRequiredFields(
                body,
                {"x", "y", "targetX"}))
        {
            return crow::response(
                400,
                "Missing required fields: x, y, targetX"
            );
        }

        if (!isArray(body["x"]) ||
            !isArray(body["y"]) ||
            !isNumber(body["targetX"]))
        {
            return crow::response(
                400,
                "x and y must be arrays and targetX must be a number"
            );
        }

        try
        {
            std::vector<double> x;
            std::vector<double> y;

            for (const auto& value : body["x"])
            {
                if (!isNumber(value))
                {
                    return crow::response(
                        400,
                        "All x values must be numbers"
                    );
                }

                double number = value.d();

                if (!isFinite(number))
                {
                    return crow::response(
                        400,
                        "All x values must be finite"
                    );
                }

                x.push_back(number);
            }

            for (const auto& value : body["y"])
            {
                if (!isNumber(value))
                {
                    return crow::response(
                        400,
                        "All y values must be numbers"
                    );
                }

                double number = value.d();

                if (!isFinite(number))
                {
                    return crow::response(
                        400,
                        "All y values must be finite"
                    );
                }

                y.push_back(number);
            }

            double targetX =
                body["targetX"].d();

            if (!isFinite(targetX))
            {
                return crow::response(
                    400,
                    "targetX must be a finite number"
                );
            }

            if (x.empty() || y.empty())
            {
                return crow::response(
                    400,
                    "x and y arrays cannot be empty"
                );
            }

            if (x.size() != y.size())
            {
                return crow::response(
                    400,
                    "x and y arrays must have the same size"
                );
            }

            if (x.size() < 2)
            {
                return crow::response(
                    400,
                    "At least two data points are required"
                );
            }

            if (hasDuplicateValues(x))
            {
                return crow::response(
                    400,
                    "x values must be unique"
                );
            }

            NewtonInterpolationResult result =
                newtonDividedDifference(
                    x,
                    y,
                    targetX
                );

            crow::json::wvalue response;

            response["value"] =
                result.value;

            crow::json::wvalue table{
                crow::json::wvalue::list()
            };

            for (size_t i = 0;
                i < result.table.size();
                i++)
            {
                for (size_t j = 0;
                    j < result.table[i].size();
                    j++)
                {
                    table[i][j] =
                        result.table[i][j];
                }
            }

            response["table"] =
                std::move(table);

            return crow::response(response);
        }
        catch (const std::exception& e)
        {
            return crow::response(
                400,
                e.what()
            );
        }
    });

    app.port(8080).multithreaded().run();
}