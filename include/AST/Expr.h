#ifndef NOTA_AST_EXPR_H
#define NOTA_AST_EXPR_H

#include "../Token.h"
#include <memory>

namespace nota {
namespace ast {

    template<typename R>
    class ExprVisitor {
    public:
        virtual R visit(class LiteralExpr& expr) = 0;
    };

    class Expr {
    public:
        virtual ~Expr() = default;
        virtual void accept(ExprVisitor<void>& visitor) = 0;
    };

    class LiteralExpr : public Expr {
    public:
        LiteralExpr(Token value) : value(value) {}

        void accept(ExprVisitor<void>& visitor) override {
            visitor.visit(*this);
        }

        Token value;
    };

} // namespace ast
} // namespace nota

#endif // NOTA_AST_EXPR_H
