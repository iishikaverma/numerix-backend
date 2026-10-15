#ifndef EXPRESSION_PARSER_H
#define EXPRESSION_PARSER_H

#include "utils/expression_node.h"
#include <string>

class ExpressionParser {
private:
    std::string expression;
    size_t position;

    char currentChar();
    void skipSpaces();

    ExpressionPtr parseExpression();
    ExpressionPtr parseTerm();
    ExpressionPtr parsePower();
    ExpressionPtr parsePrimary();

    double parseNumber();

public:
    explicit ExpressionParser(const std::string& expression);

    ExpressionPtr parse();
};

#endif