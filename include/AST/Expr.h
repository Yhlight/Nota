#ifndef NOTA_AST_EXPR_H
#define NOTA_AST_EXPR_H

#include "../Token.h"
#include <any>
#include <memory>
#include <optional>
#include <variant>
#include <vector>

namespace nota {
namespace ast {

    class Stmt;

    struct Param {
        Token name;
        std::optional<Token> type;
    };

    class ExprVisitor {
    public:
        virtual std::any visit(class LiteralExpr &expr) = 0;
        virtual std::any visit(class UnaryExpr &expr) = 0;
        virtual std::any visit(class BinaryExpr &expr) = 0;
        virtual std::any visit(class VariableExpr &expr) = 0;
        virtual std::any visit(class AssignExpr &expr) = 0;
        virtual std::any visit(class PostfixExpr &expr) = 0;
        virtual std::any visit(class CallExpr &expr) = 0;
        virtual std::any visit(class LambdaExpr &expr) = 0;
        virtual std::any visit(class ArrayLiteralExpr &expr) = 0;
        virtual std::any visit(class SubscriptExpr &expr) = 0;
    };

    class Expr {
    public:
        virtual ~Expr() = default;
        virtual std::any accept(ExprVisitor &visitor) = 0;
    };

    class LiteralExpr : public Expr {
    public:
        LiteralExpr(Token value) : value(value) {}

        std::any accept(ExprVisitor &visitor) override {
            return visitor.visit(*this);
        }

        Token value;
    };

    class UnaryExpr : public Expr {
    public:
        UnaryExpr(Token op, std::unique_ptr<Expr> right)
            : op(op), right(std::move(right)) {}

        std::any accept(ExprVisitor &visitor) override {
            return visitor.visit(*this);
        }

        Token op;
        std::unique_ptr<Expr> right;
    };

    class BinaryExpr : public Expr {
    public:
        BinaryExpr(std::unique_ptr<Expr> left, Token op,
                   std::unique_ptr<Expr> right)
            : left(std::move(left)), op(op), right(std::move(right)) {}

        std::any accept(ExprVisitor &visitor) override {
            return visitor.visit(*this);
        }

        std::unique_ptr<Expr> left;
        Token op;
        std::unique_ptr<Expr> right;
    };

    class VariableExpr : public Expr {
    public:
        VariableExpr(Token name) : name(name) {}

        std::any accept(ExprVisitor &visitor) override {
            return visitor.visit(*this);
        }

        Token name;
    };

    class AssignExpr : public Expr {
    public:
        AssignExpr(std::unique_ptr<Expr> name, std::unique_ptr<Expr> value)
            : name(std::move(name)), value(std::move(value)) {}

        std::any accept(ExprVisitor &visitor) override {
            return visitor.visit(*this);
        }

        std::unique_ptr<Expr> name;
        std::unique_ptr<Expr> value;
    };

    class PostfixExpr : public Expr {
    public:
        PostfixExpr(std::unique_ptr<Expr> left, Token op)
            : left(std::move(left)), op(op) {}

        std::any accept(ExprVisitor &visitor) override {
            return visitor.visit(*this);
        }

        std::unique_ptr<Expr> left;
        Token op;
    };

    class CallExpr : public Expr {
    public:
        CallExpr(std::unique_ptr<Expr> callee,
                 std::vector<std::unique_ptr<Expr>> arguments)
            : callee(std::move(callee)), arguments(std::move(arguments)) {}

        std::any accept(ExprVisitor &visitor) override {
            return visitor.visit(*this);
        }

        std::unique_ptr<Expr> callee;
        std::vector<std::unique_ptr<Expr>> arguments;
    };

    class LambdaExpr : public Expr {
    public:
        LambdaExpr(
            std::vector<Param> params,
            std::variant<std::unique_ptr<Expr>, std::unique_ptr<Stmt>> body)
            : params(std::move(params)), body(std::move(body)) {}

        std::any accept(ExprVisitor &visitor) override {
            return visitor.visit(*this);
        }

        std::vector<Param> params;
        std::variant<std::unique_ptr<Expr>, std::unique_ptr<Stmt>> body;
    };

    class ArrayLiteralExpr : public Expr {
    public:
        ArrayLiteralExpr(std::vector<std::unique_ptr<Expr>> elements)
            : elements(std::move(elements)) {}

        std::any accept(ExprVisitor &visitor) override {
            return visitor.visit(*this);
        }

        std::vector<std::unique_ptr<Expr>> elements;
    };

    class SubscriptExpr : public Expr {
    public:
        SubscriptExpr(std::unique_ptr<Expr> callee, std::unique_ptr<Expr> index)
            : callee(std::move(callee)), index(std::move(index)) {}

        std::any accept(ExprVisitor &visitor) override {
            return visitor.visit(*this);
        }

        std::unique_ptr<Expr> callee;
        std::unique_ptr<Expr> index;
    };

} // namespace ast
} // namespace nota

#endif // NOTA_AST_EXPR_H
