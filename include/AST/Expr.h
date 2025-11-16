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
        virtual R visit(class UnaryExpr& expr) = 0;
        virtual R visit(class BinaryExpr& expr) = 0;
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

    class UnaryExpr : public Expr {
    public:
        UnaryExpr(Token op, std::unique_ptr<Expr> right)
            : op(op), right(std::move(right)) {}

        void accept(ExprVisitor<void>& visitor) override {
            visitor.visit(*this);
        }

        Token op;
        std::unique_ptr<Expr> right;
    };

    class BinaryExpr : public Expr {
    public:
        BinaryExpr(std::unique_ptr<Expr> left, Token op, std::unique_ptr<Expr> right)
            : left(std::move(left)), op(op), right(std::move(right)) {}

        void accept(ExprVisitor<void>& visitor) override {
            visitor.visit(*this);
        }

        std::unique_ptr<Expr> left;
        Token op;
        std::unique_ptr<Expr> right;
    };

} // namespace ast
} // namespace nota

#endif // NOTA_AST_EXPR_H
