#pragma once

#include "AST/Expr.h"
#include "AST/Stmt.h"
#include <vector>
#include <memory>
#include <string>

#include <stack>
#include <map>
#include "Token.h"

class Resolver : public ExprVisitor, public StmtVisitor {
public:
    void resolve(const std::vector<std::unique_ptr<Stmt>>& statements);

private:
    std::any visit(const ComponentStmt& stmt) override;
    std::any visit(const PropertyStmt& stmt) override;

    std::any visit(const LiteralExpr& expr) override;
    std::any visit(const IdentifierExpr& expr) override;

    void begin_scope();
    void end_scope();
    void declare(const Token& name);

    std::stack<std::map<std::string, bool>> scopes;
};
