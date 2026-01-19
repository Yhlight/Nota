#pragma once
#include "AST.h"
#include <string>
#include <map>

// Context for variable resolution
using Context = std::map<std::string, std::string>;

class CodeGen {
public:
    std::string generateHTML(const Node& root, const Context& parentContext = {}); // Pass context down
    std::string generateCSS(const Node& root);
};

class Evaluator : public ExprVisitor {
public:
    explicit Evaluator(const Context& ctx = {}) : context(ctx) {}

    std::string evaluate(Expr& expr);
    std::string visitBinaryExpr(BinaryExpr& expr) override;
    std::string visitLiteralExpr(LiteralExpr& expr) override;
    std::string visitVariableExpr(VariableExpr& expr) override;

private:
    const Context& context;
};
