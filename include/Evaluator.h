#pragma once

#include "AST/Expr.h"
#include "AST/Stmt.h"
#include <map>
#include <string>
#include <any>
#include <vector>
#include <memory>
#include <stack>

class Evaluator : public ExprVisitor, public StmtVisitor {
public:
    void evaluate(const std::vector<std::unique_ptr<Stmt>>& statements);
    const std::map<const PropertyStmt*, std::any>& get_results() const;

private:
    std::any visit(const ComponentStmt& stmt) override;
    std::any visit(const PropertyStmt& stmt) override;
    std::any visit(const ItemStmt& stmt) override;

    std::any visit(const LiteralExpr& expr) override;
    std::any visit(const IdentifierExpr& expr) override;
    std::any visit(const BinaryExpr& expr) override;
    std::any visit(const GetExpr& expr) override;

    std::string current_component_id;
    std::stack<std::string> component_stack;
    std::map<std::string, std::map<std::string, std::any>> component_props;
    std::map<const PropertyStmt*, std::any> results;
};
