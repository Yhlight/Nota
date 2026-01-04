#pragma once

#include "AST/Expr.h"
#include "AST/Stmt.h"
#include <map>
#include <string>
#include <any>
#include <vector>
#include <memory>

class Evaluator : public ExprVisitor, public StmtVisitor {
public:
    void evaluate(const std::vector<std::unique_ptr<Stmt>>& statements);
    const std::map<const PropertyStmt*, std::any>& get_results() const;

private:
    std::any visit(const ComponentStmt& stmt) override;
    std::any visit(const PropertyStmt& stmt) override;
    std::any visit(const ItemStmt& stmt) override;
    std::any visit(const PackageStmt& stmt) override;
    std::any visit(const ImportStmt& stmt) override;
    std::any visit(const ExportStmt& stmt) override;

    std::any visit(const LiteralExpr& expr) override;
    std::any visit(const IdentifierExpr& expr) override;
    std::any visit(const BinaryExpr& expr) override;
    std::any visit(const GetExpr& expr) override;

    std::string current_component_id;
    std::map<std::string, std::map<std::string, std::any>> component_props;
    std::map<const PropertyStmt*, std::any> results;
};
