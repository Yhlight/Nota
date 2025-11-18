#pragma once

#include "ASTNode.h"

class Stmt : public ASTNode {
public:
    virtual ~Stmt() = default;
};
