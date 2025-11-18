#pragma once

#include <any>

// Forward declarations for concrete AST node types
class BinaryExpr;
class UnaryExpr;
class LiteralExpr;
class VariableExpr;
class VarDeclStmt;
class PrintStmt;
class ExprStmt;

class Visitor {
public:
    virtual ~Visitor() = default;

    // Expression visitors
    virtual std::any visit(BinaryExpr& expr) = 0;
    virtual std::any visit(UnaryExpr& expr) = 0;
    virtual std::any visit(LiteralExpr& expr) = 0;
    virtual std::any visit(VariableExpr& expr) = 0;

    // Statement visitors
    virtual std::any visit(VarDeclStmt& stmt) = 0;
    virtual std::any visit(PrintStmt& stmt) = 0;
    virtual std::any visit(ExprStmt& stmt) = 0;
};
