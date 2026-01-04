#pragma once

#include "AST/Expr.h"
#include "AST/Stmt.h"
#include <string>
#include <vector>
#include <memory>
#include <sstream>
#include <map>

// Forward declare ItemStmt to avoid circular dependency
struct ItemStmt;

struct CompilationResult {
    std::string html;
    std::string css;
};

class CodeGenerator : public StmtVisitor {
public:
    explicit CodeGenerator(std::map<std::string, const ItemStmt*> custom_types,
                           const std::map<const PropertyStmt*, std::any>& results);
    CompilationResult generate(const std::vector<std::unique_ptr<Stmt>>& statements);

private:
    const std::map<std::string, const ItemStmt*>& custom_types;
    const std::map<const PropertyStmt*, std::any>& results;
    std::any visit(const ComponentStmt& stmt) override;
    std::any visit(const PropertyStmt& stmt) override;
    std::any visit(const ItemStmt& stmt) override;
    std::any visit(const PackageStmt& stmt) override;
    std::any visit(const ImportStmt& stmt) override;
    std::any visit(const ExportStmt& stmt) override;

    std::stringstream html_out;
    std::stringstream css_out;
    int class_counter = 0;
};
