#include "utils/expression_evaluator.h"

#include <cmath>
#include <stdexcept>

double evaluateExpression(
    const ExpressionPtr& node,
    double x
) {
    if (!node) {
        throw std::runtime_error(
            "Cannot evaluate empty expression"
        );
    }

    switch (node->type) {

        case NodeType::Number:
            return node->value;

        case NodeType::Variable:
            return x;

        case NodeType::Add:
            return evaluateExpression(node->left, x)
                 + evaluateExpression(node->right, x);

        case NodeType::Subtract:
            return evaluateExpression(node->left, x)
                 - evaluateExpression(node->right, x);

        case NodeType::Multiply:
            return evaluateExpression(node->left, x)
                 * evaluateExpression(node->right, x);

        case NodeType::Divide: {
            double denominator =
                evaluateExpression(node->right, x);

            if (std::abs(denominator) < 1e-12) {
                throw std::runtime_error(
                    "Division by zero"
                );
            }

            return evaluateExpression(node->left, x)
                 / denominator;
        }

        case NodeType::Power:
            return std::pow(
                evaluateExpression(node->left, x),
                evaluateExpression(node->right, x)
            );
    }

    throw std::runtime_error(
        "Unknown expression node"
    );
}