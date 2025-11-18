#pragma once

#include "ASTNode.h"

class Expr : public ASTNode {
public:
    virtual ~Expr() = default;
};

#include "ExprNodes.h"
