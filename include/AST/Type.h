#ifndef NOTA_AST_TYPE_H
#define NOTA_AST_TYPE_H

#include "../Token.h"
#include <any>
#include <memory>

namespace nota {
namespace ast {

class Expr;

class TypeVisitor {
  public:
    virtual std::any visit(class BaseType &type) = 0;
    virtual std::any visit(class ArrayType &type) = 0;
};

class Type {
  public:
    virtual ~Type() = default;
    virtual std::any accept(TypeVisitor &visitor) = 0;
};

class BaseType : public Type {
  public:
    BaseType(Token name) : name(name) {}

    std::any accept(TypeVisitor &visitor) override {
        return visitor.visit(*this);
    }

    Token name;
};

class ArrayType : public Type {
  public:
    ArrayType(std::unique_ptr<Type> base, std::unique_ptr<Expr> size)
        : base(std::move(base)), size(std::move(size)) {}

    std::any accept(TypeVisitor &visitor) override {
        return visitor.visit(*this);
    }

    std::unique_ptr<Type> base;
    std::unique_ptr<Expr> size;
};

} // namespace ast
} // namespace nota

#endif // NOTA_AST_TYPE_H
