#pragma once

#include "ast/AST.h"
#include "analyzer/SymbolTable.h"
#include "core/Error.h"
#include "core/Type.h"
#include <vector>
#include <string>
#include <memory>
#include <unordered_set>
#include <unordered_map>

class SemanticAnalyzer {
public:
    SemanticAnalyzer();

    bool analyze(const RootNode& ast);

    const std::vector<CompilerError>& errors() const { return errors_; }

private:
    void visit(const ItemNode& node, std::shared_ptr<SymbolTable> table);
    void visit(const ComponentNode& node, std::shared_ptr<SymbolTable> table, const std::string& parent_type);
    void visit(const PropertyNode& node, std::shared_ptr<SymbolTable> table, const std::string& component_type);
    void visit(const EventHandlerNode& node, std::shared_ptr<SymbolTable> table, const std::string& component_type);
    void visit(const AssignmentNode& node, std::shared_ptr<SymbolTable> table);
    void visit(const Expression& node, std::shared_ptr<SymbolTable> table);
    ValueType get_type(const LiteralNode& node);


    std::vector<CompilerError> errors_;
    std::unordered_set<std::string> built_in_types_;
    std::unordered_map<std::string, std::unordered_set<std::string>> valid_properties_;
    std::unordered_map<std::string, std::vector<ValueType>> property_types_;
};
