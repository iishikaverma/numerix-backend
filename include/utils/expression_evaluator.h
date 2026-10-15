#ifndef EXPRESSION_EVALUATOR_H
#define EXPRESSION_EVALUATOR_H

#include "utils/expression_node.h"

double evaluateExpression(
    const ExpressionPtr& node,
    double x
);

#endif