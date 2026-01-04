#ifndef NOTA_EXPR_H
#define NOTA_EXPR_H

#include "../Token.h"
#include <memory>
#include <vector>
#include <any>

namespace nota::ast {

    // Forward declarations
    class LiteralExpr;
    class BinaryExpr;

    class ExprVisitor {
    public:
        virtual ~ExprVisitor() = default;
        virtual std::any visitLiteralExpr(LiteralExpr& expr) = 0;
        virtual std::any visitBinaryExpr(BinaryExpr& expr) = 0;
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

    class BinaryExpr : public Expr {
    public:
        const std::unique_ptr<Expr> left;
        const Token op;
        const std::unique_ptr<Expr> right;

        BinaryExpr(std::unique_ptr<Expr> left, Token op, std::unique_ptr<Expr> right)
            : left(std::move(left)), op(std::move(op)), right(std::move(right)) {}

        std::any accept(ExprVisitor& visitor) override {
            return visitor.visitBinaryExpr(*this);
        }
    };

} // namespace nota::ast

#endif //NOTA_EXPR_H
