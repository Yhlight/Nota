#pragma once

#include "../ast.hpp"
#include "chunk.hpp"

class Compiler : public StmtVisitor, public ExprVisitor {
public:
    Compiler();
    Chunk compile(const std::vector<std::unique_ptr<Stmt>>& statements);

private:
    void visit(const ExpressionStmt& stmt) override;
    void visit(const LetStmt& stmt) override;
    void visit(const MutStmt& stmt) override;
    void visit(const BlockStmt& stmt) override;
    void visit(const IfStmt& stmt) override;
    void visit(const WhileStmt& stmt) override;
    void visit(const ForStmt& stmt) override;
    void visit(const ForEachStmt& stmt) override;
    void visit(const FunctionStmt& stmt) override;
    void visit(const ReturnStmt& stmt) override;

    void visit(const BinaryExpr& expr) override;
    void visit(const UnaryExpr& expr) override;
    void visit(const LiteralExpr& expr) override;
    void visit(const GroupingExpr& expr) override;
    void visit(const VariableExpr& expr) override;
    void visit(const AssignExpr& expr) override;
    void visit(const CallExpr& expr) override;
    void visit(const TypeExpr& expr) override;
    void visit(const ArrayTypeExpr& expr) override;
    void visit(const ArrayLiteralExpr& expr) override;
    void visit(const SubscriptExpr& expr) override;

    Chunk chunk;
};
