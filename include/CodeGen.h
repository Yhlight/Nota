#pragma once
#include "AST.h"
#include <string>

class CodeGen {
public:
    std::string generateHTML(const ComponentNode& root);
    std::string generateCSS(const ComponentNode& root);
};

// Expose Evaluator for testing if needed, or make it part of CodeGen public interface
// For now, let's just make it a standalone class in header or export it.
// The tests need it to verify property values.

class Evaluator : public ExprVisitor {
public:
    std::string evaluate(Expr& expr);
    std::string visitBinaryExpr(BinaryExpr& expr) override;
    std::string visitLiteralExpr(LiteralExpr& expr) override;
    std::string visitVariableExpr(VariableExpr& expr) override;
};
