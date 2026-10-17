#include "utils/expression_parser.h"
#include "utils/differentiation.h"
#include "utils/expression_evaluator.h"

#include <cctype>
#include <stdexcept>

ExpressionParser::ExpressionParser(const std::string& expression)
    : expression(expression), position(0) {
}

char ExpressionParser::currentChar() {
    if (position >= expression.length()) {
        return '\0';
    }

    return expression[position];
}

void ExpressionParser::skipSpaces() {
    while (std::isspace(currentChar())) {
        position++;
    }
}

ExpressionPtr ExpressionParser::parse() {
    skipSpaces();

    ExpressionPtr result = parseExpression();

    skipSpaces();

    if (position < expression.length()) {
        throw std::runtime_error("Unexpected character in expression");
    }

    return result;
}

ExpressionPtr ExpressionParser::parseExpression() {
    ExpressionPtr left = parseTerm();

    while (true) {
        skipSpaces();

        char op = currentChar();

        if (op != '+' && op != '-') {
            break;
        }

        position++;

        ExpressionPtr right = parseTerm();

        if (op == '+') {
            left = std::make_shared<ExpressionNode>(
                NodeType::Add,
                left,
                right
            );
        } else {
            left = std::make_shared<ExpressionNode>(
                NodeType::Subtract,
                left,
                right
            );
        }
    }

    return left;
}

ExpressionPtr ExpressionParser::parseTerm() {
    ExpressionPtr left = parsePower();

    while (true) {
        skipSpaces();

        char op = currentChar();

        if (op == '*' || op == '/') {
            position++;

            ExpressionPtr right = parsePower();

            if (op == '*') {
                left = std::make_shared<ExpressionNode>(
                    NodeType::Multiply,
                    left,
                    right
                );
            } else {
                left = std::make_shared<ExpressionNode>(
                    NodeType::Divide,
                    left,
                    right
                );
            }

            continue;
        }

        // Implicit multiplication:
        // 2x
        // 3(x + 1)
        if (op == 'x' || op == 'X' || op == '(') {
            ExpressionPtr right = parsePower();

            left = std::make_shared<ExpressionNode>(
                NodeType::Multiply,
                left,
                right
            );

            continue;
        }

        break;
    }

    return left;
}

ExpressionPtr ExpressionParser::parsePower() {
    ExpressionPtr left = parsePrimary();

    skipSpaces();

    if (currentChar() == '^') {
        position++;

        ExpressionPtr right = parsePower();

        left = std::make_shared<ExpressionNode>(
            NodeType::Power,
            left,
            right
        );
    }

    return left;
}

ExpressionPtr ExpressionParser::parsePrimary() {
    skipSpaces();

    char ch = currentChar();

    if (ch == '-')
    {
        position++;

        ExpressionPtr operand = parsePrimary();

        auto negativeOne =
            std::make_shared<ExpressionNode>(-1.0);

        return std::make_shared<ExpressionNode>(
            NodeType::Multiply,
            negativeOne,
            operand
        );
    }

    // Parentheses
    if (ch == '(') {
        position++;

        ExpressionPtr result = parseExpression();

        skipSpaces();

        if (currentChar() != ')') {
            throw std::runtime_error("Missing closing parenthesis");
        }

        position++;

        return result;
    }

    // Variable x
    if (ch == 'x' || ch == 'X') {
        position++;

        return std::make_shared<ExpressionNode>(
            NodeType::Variable
        );
    }

    // Number
    if (std::isdigit(ch) || ch == '.') {
        double value = parseNumber();

        return std::make_shared<ExpressionNode>(value);
    }

    throw std::runtime_error("Invalid expression");
}

double ExpressionParser::parseNumber() {
    skipSpaces();

    size_t start = position;

    bool decimalFound = false;

    while (true) {
        char ch = currentChar();

        if (std::isdigit(ch)) {
            position++;
        }
        else if (ch == '.' && !decimalFound) {
            decimalFound = true;
            position++;
        }
        else {
            break;
        }
    }

    if (start == position) {
        throw std::runtime_error("Expected number");
    }

    return std::stod(
        expression.substr(start, position - start)
    );
}

ParsedExpression parseExpression(
    const std::string& expression
) {
    ExpressionParser parser(expression);

    ExpressionPtr originalTree = parser.parse();

    ExpressionPtr derivativeTree =
        differentiate(originalTree);

    ParsedExpression result;

    result.evaluate = [originalTree](double x) {
        return evaluateExpression(originalTree, x);
    };

    result.derivative = [derivativeTree](double x) {
        return evaluateExpression(derivativeTree, x);
    };

    return result;
}