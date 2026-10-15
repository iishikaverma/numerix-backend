#ifndef EXPRESSION_NODE_H
#define EXPRESSION_NODE_H

#include <memory>
#include <string>

enum class NodeType {
    Number,
    Variable,
    Add,
    Subtract,
    Multiply,
    Divide,
    Power
};

struct ExpressionNode {
    NodeType type;

    // Used when the node is a number
    double value = 0.0;

    // Used for operators
    std::shared_ptr<ExpressionNode> left;
    std::shared_ptr<ExpressionNode> right;

    ExpressionNode(NodeType type)
        : type(type) {}

    ExpressionNode(double value)
        : type(NodeType::Number), value(value) {}

    ExpressionNode(
        NodeType type,
        std::shared_ptr<ExpressionNode> left,
        std::shared_ptr<ExpressionNode> right
    )
        : type(type), left(left), right(right) {}
};

using ExpressionPtr = std::shared_ptr<ExpressionNode>;

#endif