#ifndef NOTA_STMT_H
#define NOTA_STMT_H

#include "Expr.h"
#include <memory>
#include <vector>

namespace nota {

class Stmt;
struct ExpressionStmt;
struct VarDeclStmt;
struct BlockStmt;
struct IfStmt;
struct WhileStmt;
struct ForStmt;

class StmtVisitor {
public:
    virtual ~StmtVisitor() = default;
    virtual void visitExpressionStmt(const ExpressionStmt& stmt) = 0;
    virtual void visitVarDeclStmt(const VarDeclStmt& stmt) = 0;
    virtual void visitBlockStmt(const BlockStmt& stmt) = 0;
    virtual void visitIfStmt(const IfStmt& stmt) = 0;
    virtual void visitWhileStmt(const WhileStmt& stmt) = 0;
    virtual void visitForStmt(const ForStmt& stmt) = 0;
};

class Stmt {
public:
    virtual ~Stmt() = default;
    virtual void accept(StmtVisitor& visitor) const = 0;
};

struct ExpressionStmt : public Stmt {
    ExpressionStmt(std::unique_ptr<Expr> expression)
        : expression(std::move(expression)) {}

    void accept(StmtVisitor& visitor) const override {
        visitor.visitExpressionStmt(*this);
    }

    const std::unique_ptr<Expr> expression;
};

struct VarDeclStmt : public Stmt {
    VarDeclStmt(Token name, std::unique_ptr<Expr> initializer)
        : name(std::move(name)), initializer(std::move(initializer)) {}

    void accept(StmtVisitor& visitor) const override {
        visitor.visitVarDeclStmt(*this);
    }

    const Token name;
    const std::unique_ptr<Expr> initializer;
};

struct BlockStmt : public Stmt {
    BlockStmt(std::vector<std::unique_ptr<Stmt>> statements)
        : statements(std::move(statements)) {}

    void accept(StmtVisitor& visitor) const override {
        visitor.visitBlockStmt(*this);
    }

    const std::vector<std::unique_ptr<Stmt>> statements;
};

struct IfStmt : public Stmt {
    IfStmt(std::unique_ptr<Expr> condition,
           std::unique_ptr<Stmt> thenBranch,
           std::unique_ptr<Stmt> elseBranch)
        : condition(std::move(condition)),
          thenBranch(std::move(thenBranch)),
          elseBranch(std::move(elseBranch)) {}

    void accept(StmtVisitor& visitor) const override {
        visitor.visitIfStmt(*this);
    }

    const std::unique_ptr<Expr> condition;
    const std::unique_ptr<Stmt> thenBranch;
    const std::unique_ptr<Stmt> elseBranch;
};

struct WhileStmt : public Stmt {
    WhileStmt(std::unique_ptr<Expr> condition, std::unique_ptr<Stmt> body)
        : condition(std::move(condition)), body(std::move(body)) {}

    void accept(StmtVisitor& visitor) const override {
        visitor.visitWhileStmt(*this);
    }

    const std::unique_ptr<Expr> condition;
    const std::unique_ptr<Stmt> body;
};

struct ForStmt : public Stmt {
    ForStmt(std::unique_ptr<Stmt> initializer,
            std::unique_ptr<Expr> condition,
            std::unique_ptr<Expr> increment,
            std::unique_ptr<Stmt> body)
        : initializer(std::move(initializer)),
          condition(std::move(condition)),
          increment(std::move(increment)),
          body(std::move(body)) {}

    void accept(StmtVisitor& visitor) const override {
        visitor.visitForStmt(*this);
    }

    const std::unique_ptr<Stmt> initializer;
    const std::unique_ptr<Expr> condition;
    const std::unique_ptr<Expr> increment;
    const std::unique_ptr<Stmt> body;
};

} // namespace nota

#endif // NOTA_STMT_H
