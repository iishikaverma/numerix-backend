#include "utils/differentiation.h"

#include <stdexcept>

ExpressionPtr differentiate(const ExpressionPtr& node) {

    if (!node) {
        throw std::runtime_error("Cannot differentiate empty expression");
    }

    // d/dx (constant) = 0
    if (node->type == NodeType::Number) {
        return std::make_shared<ExpressionNode>(0.0);
    }

    // d/dx (x) = 1
    if (node->type == NodeType::Variable) {
        return std::make_shared<ExpressionNode>(1.0);
    }

    // Addition
    if (node->type == NodeType::Add) {
        return std::make_shared<ExpressionNode>(
            NodeType::Add,
            differentiate(node->left),
            differentiate(node->right)
        );
    }

    // Subtraction
    if (node->type == NodeType::Subtract) {
        return std::make_shared<ExpressionNode>(
            NodeType::Subtract,
            differentiate(node->left),
            differentiate(node->right)
        );
    }

    // Product rule:
    // d(u*v) = u'v + uv'
    if (node->type == NodeType::Multiply) {

        auto leftDerivative = differentiate(node->left);
        auto rightDerivative = differentiate(node->right);

        auto firstTerm = std::make_shared<ExpressionNode>(
            NodeType::Multiply,
            leftDerivative,
            node->right
        );

        auto secondTerm = std::make_shared<ExpressionNode>(
            NodeType::Multiply,
            node->left,
            rightDerivative
        );

        return std::make_shared<ExpressionNode>(
            NodeType::Add,
            firstTerm,
            secondTerm
        );
    }

    // Quotient rule:
    // d(u/v) = (u'v - uv') / v²
    if (node->type == NodeType::Divide) {

        auto uPrime = differentiate(node->left);
        auto vPrime = differentiate(node->right);

        auto firstTerm = std::make_shared<ExpressionNode>(
            NodeType::Multiply,
            uPrime,
            node->right
        );

        auto secondTerm = std::make_shared<ExpressionNode>(
            NodeType::Multiply,
            node->left,
            vPrime
        );

        auto numerator = std::make_shared<ExpressionNode>(
            NodeType::Subtract,
            firstTerm,
            secondTerm
        );

        auto denominator = std::make_shared<ExpressionNode>(
            NodeType::Power,
            node->right,
            std::make_shared<ExpressionNode>(2.0)
        );

        return std::make_shared<ExpressionNode>(
            NodeType::Divide,
            numerator,
            denominator
        );
    }

    // Power rule for x^n
    if (node->type == NodeType::Power) {

        // Currently support:
        // x^constant

        if (node->right->type == NodeType::Number) {

            double exponent = node->right->value;

            // n * x^(n-1)
            auto coefficient =
                std::make_shared<ExpressionNode>(exponent);

            auto newExponent =
                std::make_shared<ExpressionNode>(exponent - 1);

            auto newPower =
                std::make_shared<ExpressionNode>(
                    NodeType::Power,
                    node->left,
                    newExponent
                );

            return std::make_shared<ExpressionNode>(
                NodeType::Multiply,
                coefficient,
                newPower
            );
        }

        throw std::runtime_error(
            "Power rule currently supports only constant exponents"
        );
    }

    throw std::runtime_error(
        "Unsupported expression node"
    );
}