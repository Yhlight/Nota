#pragma once

#include "token.hpp"
#include <memory>
#include <vector>

// Forward declarations for the visitor pattern
struct BinaryExpr;
struct UnaryExpr;
struct LiteralExpr;
struct GroupingExpr;
struct VariableExpr;
struct AssignExpr;
struct CallExpr;
struct TypeExpr;
struct ArrayTypeExpr;
struct ArrayLiteralExpr;
struct SubscriptExpr;
struct LambdaExpr;

// Forward declarations for Stmt nodes
struct ExpressionStmt;
struct LetStmt;
struct MutStmt;
struct BlockStmt;
struct IfStmt;
struct WhileStmt;
struct ForStmt;
struct ForEachStmt;
struct DoWhileStmt;
struct FunctionStmt;
struct ReturnStmt;

struct Param {
    Token name;
    std::unique_ptr<TypeExpr> type;
};

// Visitor interface for expressions
class ExprVisitor {
public:
    virtual ~ExprVisitor() = default;
    virtual void visit(const BinaryExpr& expr) = 0;
    virtual void visit(const UnaryExpr& expr) = 0;
    virtual void visit(const LiteralExpr& expr) = 0;
    virtual void visit(const GroupingExpr& expr) = 0;
    virtual void visit(const VariableExpr& expr) = 0;
    virtual void visit(const AssignExpr& expr) = 0;
    virtual void visit(const CallExpr& expr) = 0;
    virtual void visit(const TypeExpr& expr) = 0;
    virtual void visit(const ArrayTypeExpr& expr) = 0;
    virtual void visit(const ArrayLiteralExpr& expr) = 0;
    virtual void visit(const SubscriptExpr& expr) = 0;
    virtual void visit(const LambdaExpr& expr) = 0;
};

// Base class for all expression nodes
class Expr {
public:
    virtual ~Expr() = default;
    virtual void accept(ExprVisitor& visitor) const = 0;
};

struct BinaryExpr : public Expr {
    BinaryExpr(std::unique_ptr<Expr> left, Token op, std::unique_ptr<Expr> right)
        : left(std::move(left)), op(op), right(std::move(right)) {}

    void accept(ExprVisitor& visitor) const override {
        visitor.visit(*this);
    }

    const std::unique_ptr<Expr> left;
    const Token op;
    const std::unique_ptr<Expr> right;
};

struct UnaryExpr : public Expr {
    UnaryExpr(Token op, std::unique_ptr<Expr> right)
        : op(op), right(std::move(right)) {}

    void accept(ExprVisitor& visitor) const override {
        visitor.visit(*this);
    }

    const Token op;
    const std::unique_ptr<Expr> right;
};

struct LiteralExpr : public Expr {
    LiteralExpr(Token value) : value(value) {}

    void accept(ExprVisitor& visitor) const override {
        visitor.visit(*this);
    }

    const Token value;
};

struct GroupingExpr : public Expr {
    GroupingExpr(std::unique_ptr<Expr> expression)
        : expression(std::move(expression)) {}

    void accept(ExprVisitor& visitor) const override {
        visitor.visit(*this);
    }

    const std::unique_ptr<Expr> expression;
};

struct VariableExpr : public Expr {
    VariableExpr(Token name) : name(name) {}

    void accept(ExprVisitor& visitor) const override {
        visitor.visit(*this);
    }

    const Token name;
};

struct AssignExpr : public Expr {
    AssignExpr(Token name, std::unique_ptr<Expr> value)
        : name(name), value(std::move(value)) {}

    void accept(ExprVisitor& visitor) const override {
        visitor.visit(*this);
    }

    const Token name;
    const std::unique_ptr<Expr> value;
};

struct CallExpr : public Expr {
    CallExpr(std::unique_ptr<Expr> callee, Token paren, std::vector<std::unique_ptr<Expr>> arguments)
        : callee(std::move(callee)), paren(paren), arguments(std::move(arguments)) {}

    void accept(ExprVisitor& visitor) const override {
        visitor.visit(*this);
    }

    const std::unique_ptr<Expr> callee;
    const Token paren; // For error reporting
    const std::vector<std::unique_ptr<Expr>> arguments;
};

struct TypeExpr : public Expr {
    TypeExpr(Token name) : name(name) {}

    void accept(ExprVisitor& visitor) const override {
        visitor.visit(*this);
    }

    const Token name;
};

struct ArrayTypeExpr : public TypeExpr {
    ArrayTypeExpr(std::unique_ptr<TypeExpr> element_type, std::unique_ptr<Expr> size)
        : TypeExpr(element_type->name), element_type(std::move(element_type)), size(std::move(size)) {}

    void accept(ExprVisitor& visitor) const override {
        visitor.visit(*this);
    }

    const std::unique_ptr<TypeExpr> element_type;
    const std::unique_ptr<Expr> size; // nullptr for dynamic arrays
};

struct ArrayLiteralExpr : public Expr {
    ArrayLiteralExpr(Token bracket, std::vector<std::unique_ptr<Expr>> elements)
        : bracket(bracket), elements(std::move(elements)) {}

    void accept(ExprVisitor& visitor) const override {
        visitor.visit(*this);
    }

    const Token bracket;
    const std::vector<std::unique_ptr<Expr>> elements;
};

struct SubscriptExpr : public Expr {
    SubscriptExpr(std::unique_ptr<Expr> callee, Token bracket, std::unique_ptr<Expr> index)
        : callee(std::move(callee)), bracket(bracket), index(std::move(index)) {}

    void accept(ExprVisitor& visitor) const override {
        visitor.visit(*this);
    }

    const std::unique_ptr<Expr> callee;
    const Token bracket;
    const std::unique_ptr<Expr> index;
};

struct LambdaExpr : public Expr {
    LambdaExpr(std::vector<Param> params, std::unique_ptr<Expr> body)
        : params(std::move(params)), body(std::move(body)) {}

    void accept(ExprVisitor& visitor) const override {
        visitor.visit(*this);
    }

    const std::vector<Param> params;
    const std::unique_ptr<Expr> body;
};

// Visitor interface for statements
class StmtVisitor {
public:
    virtual ~StmtVisitor() = default;
    virtual void visit(const ExpressionStmt& stmt) = 0;
    virtual void visit(const LetStmt& stmt) = 0;
    virtual void visit(const MutStmt& stmt) = 0;
    virtual void visit(const BlockStmt& stmt) = 0;
    virtual void visit(const IfStmt& stmt) = 0;
    virtual void visit(const WhileStmt& stmt) = 0;
    virtual void visit(const ForStmt& stmt) = 0;
    virtual void visit(const ForEachStmt& stmt) = 0;
    virtual void visit(const DoWhileStmt& stmt) = 0;
    virtual void visit(const FunctionStmt& stmt) = 0;
    virtual void visit(const ReturnStmt& stmt) = 0;
};

// Base class for all statement nodes
class Stmt {
public:
    virtual ~Stmt() = default;
    virtual void accept(StmtVisitor& visitor) const = 0;
};

struct ExpressionStmt : public Stmt {
    ExpressionStmt(std::unique_ptr<Expr> expression)
        : expression(std::move(expression)) {}

    void accept(StmtVisitor& visitor) const override {
        visitor.visit(*this);
    }

    const std::unique_ptr<Expr> expression;
};

struct LetStmt : public Stmt {
    LetStmt(Token name, std::unique_ptr<TypeExpr> type, std::unique_ptr<Expr> initializer)
        : name(name), type(std::move(type)), initializer(std::move(initializer)) {}

    void accept(StmtVisitor& visitor) const override {
        visitor.visit(*this);
    }

    const Token name;
    const std::unique_ptr<TypeExpr> type;
    const std::unique_ptr<Expr> initializer;
};

struct BlockStmt : public Stmt {
    BlockStmt(std::vector<std::unique_ptr<Stmt>> statements)
        : statements(std::move(statements)) {}

    void accept(StmtVisitor& visitor) const override {
        visitor.visit(*this);
    }

    const std::vector<std::unique_ptr<Stmt>> statements;
};

struct IfStmt : public Stmt {
    IfStmt(std::unique_ptr<Expr> condition, std::unique_ptr<Stmt> then_branch, std::unique_ptr<Stmt> else_branch)
        : condition(std::move(condition)), then_branch(std::move(then_branch)), else_branch(std::move(else_branch)) {}

    void accept(StmtVisitor& visitor) const override {
        visitor.visit(*this);
    }

    const std::unique_ptr<Expr> condition;
    const std::unique_ptr<Stmt> then_branch;
    const std::unique_ptr<Stmt> else_branch;
};

struct WhileStmt : public Stmt {
    WhileStmt(std::unique_ptr<Expr> condition, std::unique_ptr<Stmt> body)
        : condition(std::move(condition)), body(std::move(body)) {}

    void accept(StmtVisitor& visitor) const override {
        visitor.visit(*this);
    }

    const std::unique_ptr<Expr> condition;
    const std::unique_ptr<Stmt> body;
};

struct ForStmt : public Stmt {
    ForStmt(std::unique_ptr<Stmt> initializer, std::unique_ptr<Expr> condition, std::unique_ptr<Expr> increment, std::unique_ptr<Stmt> body)
        : initializer(std::move(initializer)), condition(std::move(condition)), increment(std::move(increment)), body(std::move(body)) {}

    void accept(StmtVisitor& visitor) const override {
        visitor.visit(*this);
    }

    const std::unique_ptr<Stmt> initializer;
    const std::unique_ptr<Expr> condition;
    const std::unique_ptr<Expr> increment;
    const std::unique_ptr<Stmt> body;
};

struct ForEachStmt : public Stmt {
    ForEachStmt(Token variable, std::unique_ptr<Expr> container, std::unique_ptr<Stmt> body)
        : variable(variable), container(std::move(container)), body(std::move(body)) {}

    void accept(StmtVisitor& visitor) const override {
        visitor.visit(*this);
    }

    const Token variable;
    const std::unique_ptr<Expr> container;
    const std::unique_ptr<Stmt> body;
};

struct DoWhileStmt : public Stmt {
    DoWhileStmt(std::unique_ptr<Stmt> body, std::unique_ptr<Expr> condition)
        : body(std::move(body)), condition(std::move(condition)) {}

    void accept(StmtVisitor& visitor) const override {
        visitor.visit(*this);
    }

    const std::unique_ptr<Stmt> body;
    const std::unique_ptr<Expr> condition;
};

struct FunctionStmt : public Stmt {
    FunctionStmt(Token name, std::vector<Param> params, std::unique_ptr<TypeExpr> return_type, std::vector<std::unique_ptr<Stmt>> body)
        : name(name), params(std::move(params)), return_type(std::move(return_type)), body(std::move(body)) {}

    void accept(StmtVisitor& visitor) const override {
        visitor.visit(*this);
    }

    const Token name;
    const std::vector<Param> params;
    const std::unique_ptr<TypeExpr> return_type;
    const std::vector<std::unique_ptr<Stmt>> body;
};

struct ReturnStmt : public Stmt {
    ReturnStmt(Token keyword, std::unique_ptr<Expr> value)
        : keyword(keyword), value(std::move(value)) {}

    void accept(StmtVisitor& visitor) const override {
        visitor.visit(*this);
    }

    const Token keyword;
    const std::unique_ptr<Expr> value;
};

struct MutStmt : public Stmt {
    MutStmt(Token name, std::unique_ptr<TypeExpr> type, std::unique_ptr<Expr> initializer)
        : name(name), type(std::move(type)), initializer(std::move(initializer)) {}

    void accept(StmtVisitor& visitor) const override {
        visitor.visit(*this);
    }

    const Token name;
    const std::unique_ptr<TypeExpr> type;
    const std::unique_ptr<Expr> initializer;
};
