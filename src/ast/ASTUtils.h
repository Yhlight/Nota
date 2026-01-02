#pragma once

#include "ast/AST.h"
#include <memory>

std::unique_ptr<ComponentNode> deep_copy(const ComponentNode& node);
std::unique_ptr<Expression> deep_copy_expression(const Expression& expr);
ASTValue deep_copy_ast_value(const ASTValue& value);
