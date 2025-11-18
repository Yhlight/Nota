#pragma once

#include <any>

// Forward declarations for concrete AST node types
class BinaryExpr;
class UnaryExpr;
class LiteralExpr;

class Visitor {
public:
    virtual ~Visitor() = default;

    // Expression visitors
    virtual std::any visit(BinaryExpr& expr) = 0;
    virtual std::any visit(UnaryExpr& expr) = 0;
    virtual std::any visit(LiteralExpr& expr) = 0;
};
