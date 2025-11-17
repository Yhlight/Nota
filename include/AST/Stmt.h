#ifndef NOTA_AST_STMT_H
#define NOTA_AST_STMT_H

#include "../Token.h"
#include "Expr.h"
#include "Type.h"
#include <any>
#include <memory>
#include <optional>
#include <vector>

namespace nota {
namespace ast {

    class StmtVisitor {
    public:
        virtual std::any visit(class VarDeclStmt &stmt) = 0;
        virtual std::any visit(class BlockStmt &stmt) = 0;
        virtual std::any visit(class IfStmt &stmt) = 0;
        virtual std::any visit(class WhileStmt &stmt) = 0;
        virtual std::any visit(class DoWhileStmt &stmt) = 0;
        virtual std::any visit(class ExpressionStmt &stmt) = 0;
        virtual std::any visit(class ForEachStmt &stmt) = 0;
        virtual std::any visit(class ForStmt &stmt) = 0;
        virtual std::any visit(class MatchStmt &stmt) = 0;
        virtual std::any visit(class FuncDeclStmt &stmt) = 0;
        virtual std::any visit(class ReturnStmt &stmt) = 0;
        virtual std::any visit(class ClassDeclStmt &stmt) = 0;
        virtual std::any visit(class ImportStmt &stmt) = 0;
    };

    class Stmt {
    public:
        virtual ~Stmt() = default;
        virtual std::any accept(StmtVisitor &visitor) = 0;
    };

    class VarDeclStmt : public Stmt {
    public:
        VarDeclStmt(Token name, std::unique_ptr<Type> type,
                    std::unique_ptr<Expr> initializer)
            : name(name), type(std::move(type)), initializer(std::move(initializer)) {}

        std::any accept(StmtVisitor &visitor) override {
            return visitor.visit(*this);
        }

        Token name;
        std::unique_ptr<Type> type;
        std::unique_ptr<Expr> initializer;
    };

    class BlockStmt : public Stmt {
    public:
        BlockStmt(std::vector<std::unique_ptr<Stmt>> statements)
            : statements(std::move(statements)) {}

        std::any accept(StmtVisitor &visitor) override {
            return visitor.visit(*this);
        }

        std::vector<std::unique_ptr<Stmt>> statements;
    };

    class IfStmt : public Stmt {
    public:
        IfStmt(std::unique_ptr<Expr> condition, std::unique_ptr<Stmt> then_branch,
               std::unique_ptr<Stmt> else_branch)
            : condition(std::move(condition)),
              then_branch(std::move(then_branch)),
              else_branch(std::move(else_branch)) {}

        std::any accept(StmtVisitor &visitor) override {
            return visitor.visit(*this);
        }

        std::unique_ptr<Expr> condition;
        std::unique_ptr<Stmt> then_branch;
        std::unique_ptr<Stmt> else_branch;
    };

    class WhileStmt : public Stmt {
    public:
        WhileStmt(std::unique_ptr<Expr> condition, std::unique_ptr<Stmt> body)
            : condition(std::move(condition)), body(std::move(body)) {}

        std::any accept(StmtVisitor &visitor) override {
            return visitor.visit(*this);
        }

        std::unique_ptr<Expr> condition;
        std::unique_ptr<Stmt> body;
    };

    class DoWhileStmt : public Stmt {
    public:
        DoWhileStmt(std::unique_ptr<Stmt> body, std::unique_ptr<Expr> condition)
            : body(std::move(body)), condition(std::move(condition)) {}

        std::any accept(StmtVisitor &visitor) override {
            return visitor.visit(*this);
        }

        std::unique_ptr<Stmt> body;
        std::unique_ptr<Expr> condition;
    };

    class ExpressionStmt : public Stmt {
    public:
        ExpressionStmt(std::unique_ptr<Expr> expression)
            : expression(std::move(expression)) {}

        std::any accept(StmtVisitor &visitor) override {
            return visitor.visit(*this);
        }

        std::unique_ptr<Expr> expression;
    };

    class ForEachStmt : public Stmt {
    public:
        ForEachStmt(Token variable, std::unique_ptr<Expr> container,
                    std::unique_ptr<Stmt> body)
            : variable(variable), container(std::move(container)),
              body(std::move(body)) {}

        std::any accept(StmtVisitor &visitor) override {
            return visitor.visit(*this);
        }

        Token variable;
        std::unique_ptr<Expr> container;
        std::unique_ptr<Stmt> body;
    };

    class ForStmt : public Stmt {
    public:
        ForStmt(std::unique_ptr<Stmt> initializer,
                std::unique_ptr<Expr> condition, std::unique_ptr<Expr> increment,
                std::unique_ptr<Stmt> body)
            : initializer(std::move(initializer)),
              condition(std::move(condition)), increment(std::move(increment)),
              body(std::move(body)) {}

        std::any accept(StmtVisitor &visitor) override {
            return visitor.visit(*this);
        }

        std::unique_ptr<Stmt> initializer;
        std::unique_ptr<Expr> condition;
        std::unique_ptr<Expr> increment;
        std::unique_ptr<Stmt> body;
    };

    class MatchCase {
    public:
        MatchCase(std::vector<std::unique_ptr<Expr>> values,
                  std::unique_ptr<Stmt> body)
            : values(std::move(values)), body(std::move(body)) {}

        std::vector<std::unique_ptr<Expr>> values;
        std::unique_ptr<Stmt> body;
    };

    class MatchStmt : public Stmt {
    public:
        MatchStmt(std::unique_ptr<Expr> expression, std::vector<MatchCase> cases,
                  std::unique_ptr<Stmt> else_branch)
            : expression(std::move(expression)), cases(std::move(cases)),
              else_branch(std::move(else_branch)) {}

        std::any accept(StmtVisitor &visitor) override {
            return visitor.visit(*this);
        }

        std::unique_ptr<Expr> expression;
        std::vector<MatchCase> cases;
        std::unique_ptr<Stmt> else_branch;
    };

    class FuncDeclStmt : public Stmt {
    public:
        FuncDeclStmt(Token name, std::vector<Param> params,
                     std::unique_ptr<Stmt> body, std::unique_ptr<Type> return_type)
            : name(name), params(std::move(params)), body(std::move(body)),
              return_type(std::move(return_type)) {}

        std::any accept(StmtVisitor &visitor) override {
            return visitor.visit(*this);
        }

        Token name;
        std::vector<Param> params;
        std::unique_ptr<Stmt> body;
        std::unique_ptr<Type> return_type;
    };

    class ReturnStmt : public Stmt {
    public:
        ReturnStmt(std::unique_ptr<Expr> value) : value(std::move(value)) {}

        std::any accept(StmtVisitor &visitor) override {
            return visitor.visit(*this);
        }

        std::unique_ptr<Expr> value;
    };

    class ClassDeclStmt : public Stmt {
    public:
        ClassDeclStmt(Token name, std::vector<std::unique_ptr<FuncDeclStmt>> methods)
            : name(name), methods(std::move(methods)) {}

        std::any accept(StmtVisitor &visitor) override {
            return visitor.visit(*this);
        }

        Token name;
        std::vector<std::unique_ptr<FuncDeclStmt>> methods;
    };

    class ImportStmt : public Stmt {
    public:
        ImportStmt(Token path, std::optional<Token> alias)
            : path(path), alias(alias) {}

        std::any accept(StmtVisitor &visitor) override {
            return visitor.visit(*this);
        }

        Token path;
        std::optional<Token> alias;
    };

} // namespace ast
} // namespace nota

#endif // NOTA_AST_STMT_H
