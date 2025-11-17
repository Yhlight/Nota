#ifndef NOTA_AST_EXPR_H
#define NOTA_AST_EXPR_H

#include "../Token.h"
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

    template<typename R>
    class ExprVisitor {
    public:
        virtual R visit(class LiteralExpr& expr) = 0;
        virtual R visit(class UnaryExpr& expr) = 0;
        virtual R visit(class BinaryExpr& expr) = 0;
        virtual R visit(class VariableExpr& expr) = 0;
        virtual R visit(class AssignExpr& expr) = 0;
        virtual R visit(class PostfixExpr& expr) = 0;
        virtual R visit(class CallExpr& expr) = 0;
        virtual R visit(class LambdaExpr& expr) = 0;
    };

    class Expr {
    public:
        virtual ~Expr() = default;
        virtual std::string accept(ExprVisitor<std::string>& visitor) = 0;
    };

    class LiteralExpr : public Expr {
    public:
        LiteralExpr(Token value) : value(value) {}

        std::string accept(ExprVisitor<std::string>& visitor) override {
            return visitor.visit(*this);
        }

        Token value;
    };

    class UnaryExpr : public Expr {
    public:
        UnaryExpr(Token op, std::unique_ptr<Expr> right)
            : op(op), right(std::move(right)) {}

        std::string accept(ExprVisitor<std::string>& visitor) override {
            return visitor.visit(*this);
        }

        Token op;
        std::unique_ptr<Expr> right;
    };

    class BinaryExpr : public Expr {
    public:
        BinaryExpr(std::unique_ptr<Expr> left, Token op, std::unique_ptr<Expr> right)
            : left(std::move(left)), op(op), right(std::move(right)) {}

        std::string accept(ExprVisitor<std::string>& visitor) override {
            return visitor.visit(*this);
        }

        std::unique_ptr<Expr> left;
        Token op;
        std::unique_ptr<Expr> right;
    };

    class VariableExpr : public Expr {
    public:
        VariableExpr(Token name) : name(name) {}

        std::string accept(ExprVisitor<std::string>& visitor) override {
            return visitor.visit(*this);
        }

        Token name;
    };

    class AssignExpr : public Expr {
    public:
        AssignExpr(Token name, std::unique_ptr<Expr> value)
            : name(name), value(std::move(value)) {}

        std::string accept(ExprVisitor<std::string>& visitor) override {
            return visitor.visit(*this);
        }

        Token name;
        std::unique_ptr<Expr> value;
    };

    class PostfixExpr : public Expr {
    public:
        PostfixExpr(std::unique_ptr<Expr> left, Token op)
            : left(std::move(left)), op(op) {}

        std::string accept(ExprVisitor<std::string>& visitor) override {
            return visitor.visit(*this);
        }

        std::unique_ptr<Expr> left;
        Token op;
    };

    class CallExpr : public Expr {
    public:
        CallExpr(std::unique_ptr<Expr> callee, std::vector<std::unique_ptr<Expr>> arguments)
            : callee(std::move(callee)), arguments(std::move(arguments)) {}

        std::string accept(ExprVisitor<std::string>& visitor) override {
            return visitor.visit(*this);
        }

        std::unique_ptr<Expr> callee;
        std::vector<std::unique_ptr<Expr>> arguments;
    };

    class LambdaExpr : public Expr {
    public:
        LambdaExpr(std::vector<Param> params, std::variant<std::unique_ptr<Expr>, std::unique_ptr<Stmt>> body)
            : params(std::move(params)), body(std::move(body)) {}

        std::string accept(ExprVisitor<std::string>& visitor) override {
            return visitor.visit(*this);
        }

        std::vector<Param> params;
        std::variant<std::unique_ptr<Expr>, std::unique_ptr<Stmt>> body;
    };

} // namespace ast
} // namespace nota

#endif // NOTA_AST_EXPR_H
