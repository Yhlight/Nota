#ifndef NOTA_AST_STMT_H
#define NOTA_AST_STMT_H

#include "../Token.h"
#include "Expr.h"
#include <memory>
#include <optional>
#include <vector>

namespace nota {
namespace ast {

    template<typename R>
    class StmtVisitor {
    public:
        virtual R visit(class VarDeclStmt& stmt) = 0;
        virtual R visit(class BlockStmt& stmt) = 0;
        virtual R visit(class IfStmt& stmt) = 0;
        virtual R visit(class WhileStmt& stmt) = 0;
    };

    class Stmt {
    public:
        virtual ~Stmt() = default;
        virtual void accept(StmtVisitor<void>& visitor) = 0;
    };

    class VarDeclStmt : public Stmt {
    public:
        VarDeclStmt(Token name, std::optional<Token> type, std::unique_ptr<Expr> initializer)
            : name(name), type(type), initializer(std::move(initializer)) {}

        void accept(StmtVisitor<void>& visitor) override {
            visitor.visit(*this);
        }

        Token name;
        std::optional<Token> type;
        std::unique_ptr<Expr> initializer;
    };

    class BlockStmt : public Stmt {
    public:
        BlockStmt(std::vector<std::unique_ptr<Stmt>> statements)
            : statements(std::move(statements)) {}

        void accept(StmtVisitor<void>& visitor) override {
            visitor.visit(*this);
        }

        std::vector<std::unique_ptr<Stmt>> statements;
    };

    class IfStmt : public Stmt {
    public:
        IfStmt(std::unique_ptr<Expr> condition, std::unique_ptr<Stmt> then_branch, std::unique_ptr<Stmt> else_branch)
            : condition(std::move(condition)), then_branch(std::move(then_branch)), else_branch(std::move(else_branch)) {}

        void accept(StmtVisitor<void>& visitor) override {
            visitor.visit(*this);
        }

        std::unique_ptr<Expr> condition;
        std::unique_ptr<Stmt> then_branch;
        std::unique_ptr<Stmt> else_branch;
    };

    class WhileStmt : public Stmt {
    public:
        WhileStmt(std::unique_ptr<Expr> condition, std::unique_ptr<Stmt> body)
            : condition(std::move(condition)), body(std::move(body)) {}

        void accept(StmtVisitor<void>& visitor) override {
            visitor.visit(*this);
        }

        std::unique_ptr<Expr> condition;
        std::unique_ptr<Stmt> body;
    };

} // namespace ast
} // namespace nota

#endif // NOTA_AST_STMT_H
