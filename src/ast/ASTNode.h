#pragma once

#include <memory>
#include <any>

// Forward declarations for the Visitor pattern
class Visitor;

class ASTNode {
public:
    virtual ~ASTNode() = default;
    virtual std::any accept(Visitor& visitor) = 0;
};
