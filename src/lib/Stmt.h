#ifndef NOTA_STMT_H
#define NOTA_STMT_H

#include "Expr.h"
#include <memory>
#include <vector>

namespace nota {

class Stmt;
struct ExpressionStmt;
struct VarDeclStmt;

class StmtVisitor {
public:
    virtual ~StmtVisitor() = default;
    virtual void visitExpressionStmt(const ExpressionStmt& stmt) = 0;
    virtual void visitVarDeclStmt(const VarDeclStmt& stmt) = 0;
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

} // namespace nota

#endif // NOTA_STMT_H