#pragma once

#include "Visitor.h"
#include "Chunk.h"
#include "Stmt.h"
#include <vector>
#include <memory>

class Compiler : public Visitor {
public:
    Chunk compile(const std::vector<std::unique_ptr<Stmt>>& statements);

    std::any visit(BinaryExpr& expr) override;
    std::any visit(UnaryExpr& expr) override;
    std::any visit(LiteralExpr& expr) override;
    std::any visit(VariableExpr& expr) override;

    std::any visit(VarDeclStmt& stmt) override;
    std::any visit(PrintStmt& stmt) override;
    std::any visit(ExprStmt& stmt) override;

private:
    Chunk chunk_;
};
