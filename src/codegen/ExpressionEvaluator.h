#pragma once

#include "ast/AST.h"
#include "analyzer/SymbolTable.h"
#include <unordered_map>

class ExpressionEvaluator {
public:
    ExpressionEvaluator(std::unordered_map<const ComponentNode*, ComponentNode*>& component_map, ComponentNode* root);

    ComponentNode* evaluate_target(const Expression& expression, SymbolTable& table, ComponentNode* current_component);
    ASTValue evaluate_value(const ASTValue& value, SymbolTable& table);
    ASTValue evaluate_expression(const Expression& expression, SymbolTable& table, ComponentNode* current_component);


private:
    ComponentNode* evaluate_member_access(const MemberAccessNode& node, SymbolTable& table, ComponentNode* current_component);
    ComponentNode* evaluate_index_access(const IndexAccessNode& node, SymbolTable& table, ComponentNode* current_component);

    std::unordered_map<const ComponentNode*, ComponentNode*>& component_map_;
    ComponentNode* root_component_;
};
