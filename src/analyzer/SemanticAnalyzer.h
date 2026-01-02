#pragma once

#include "ast/AST.h"
#include "analyzer/SymbolTable.h"
#include <vector>
#include <string>
#include <memory>
#include <unordered_set>

class SemanticAnalyzer {
public:
    SemanticAnalyzer();

    bool analyze(const RootNode& ast);

    const std::vector<std::string>& errors() const { return errors_; }

private:
    void visit(const ItemNode& node, std::shared_ptr<SymbolTable> table);
    void visit(const ComponentNode& node, std::shared_ptr<SymbolTable> table);
    void visit(const PropertyNode& node, std::shared_ptr<SymbolTable> table);
    void visit(const AssignmentNode& node, std::shared_ptr<SymbolTable> table);
    void visit(const Expression& node, std::shared_ptr<SymbolTable> table);

    std::vector<std::string> errors_;
    std::unordered_set<std::string> built_in_types_;
};
