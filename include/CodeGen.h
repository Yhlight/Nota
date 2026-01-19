#pragma once
#include "AST.h"
#include <string>

class CodeGen {
public:
    std::string generateHTML(const Node& root);
    std::string generateCSS(const Node& root);
};

class Evaluator : public ExprVisitor {
public:
    std::string evaluate(Expr& expr);
    std::string visitBinaryExpr(BinaryExpr& expr) override;
    std::string visitLiteralExpr(LiteralExpr& expr) override;
    std::string visitVariableExpr(VariableExpr& expr) override;
};
