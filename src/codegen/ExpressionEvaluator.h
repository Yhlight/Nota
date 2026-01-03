#pragma once

#include "ast/AST.h"
#include "analyzer/SymbolTable.h"
#include "core/Error.h"
#include <unordered_map>
#include <vector>

struct BinaryOperationNode;

class ExpressionEvaluator {
public:
    ExpressionEvaluator(std::unordered_map<const ComponentNode*, ComponentNode*>& component_map, ComponentNode* root);

    void build_parent_map(ComponentNode* component, ComponentNode* parent);
    ComponentNode* evaluate_target(const Expression& expression, SymbolTable& table, ComponentNode* current_component);
    ASTValue evaluate_value(const ASTValue& value, SymbolTable& table, ComponentNode* current_component);
    ASTValue evaluate_expression(const Expression& expression, SymbolTable& table, ComponentNode* current_component);

    const std::vector<CompilerError>& errors() const { return errors_; }

private:
    ComponentNode* evaluate_member_access(const MemberAccessNode& node, SymbolTable& table, ComponentNode* current_component);
    ComponentNode* evaluate_index_access(const IndexAccessNode& node, SymbolTable& table, ComponentNode* current_component);
    ASTValue evaluate_binary_operation(const BinaryOperationNode& node, SymbolTable& table, ComponentNode* current_component);

    std::unordered_map<const ComponentNode*, ComponentNode*>& component_map_;
    std::unordered_map<ComponentNode*, ComponentNode*> parent_map_;
    ComponentNode* root_component_;
    std::vector<CompilerError> errors_;
};
