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
    const std::map<std::string, const ItemStmt*>& get_custom_types() const;

private:
    void resolve(const Stmt& stmt);
    void resolve(const Expr& expr);

    std::any visit(const ComponentStmt& stmt) override;
    std::any visit(const PropertyStmt& stmt) override;
    std::any visit(const ItemStmt& stmt) override;

    std::any visit(const LiteralExpr& expr) override;
    std::any visit(const IdentifierExpr& expr) override;
    std::any visit(const BinaryExpr& expr) override;
    std::any visit(const GetExpr& expr) override;

    void begin_scope();
    void end_scope();
    void declare(const Token& name);
    void define(const Token& name);

    std::stack<std::map<std::string, bool>> scopes;
    std::map<std::string, const ItemStmt*> custom_types;
};
