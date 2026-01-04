#ifndef NOTA_EXPR_H
#define NOTA_EXPR_H

#include "../Token.h"
#include <memory>
#include <vector>
#include <any>

namespace nota::ast {

    // Forward declarations
    class LiteralExpr;

    class ExprVisitor {
    public:
        virtual ~ExprVisitor() = default;
        virtual std::any visitLiteralExpr(LiteralExpr& expr) = 0;
    };

    class Expr {
    public:
        virtual ~Expr() = default;
        virtual std::any accept(ExprVisitor& visitor) = 0;
    };

    class LiteralExpr : public Expr {
    public:
        const Token value;

        explicit LiteralExpr(Token value) : value(std::move(value)) {}

        std::any accept(ExprVisitor& visitor) override {
            return visitor.visitLiteralExpr(*this);
        }
    };

} // namespace nota::ast

#endif //NOTA_EXPR_H
