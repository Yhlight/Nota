#pragma once

#include "Token.h"
#include "NotaObjects.h"
#include <memory>
#include <vector>
#include <variant>

namespace nota {

class Interpreter; // Forward declaration

// Forward declarations
struct Binary;
struct Grouping;
struct Literal;
struct Unary;
struct Variable;
struct Assign;
struct Postfix;
struct CallExpr;
struct GetExpr;
struct SetExpr;
struct ThisExpr;

class ExprVisitor {
public:
    virtual ~ExprVisitor() = default;
    virtual void visit(const std::shared_ptr<Binary>& expr) = 0;
    virtual void visit(const std::shared_ptr<Grouping>& expr) = 0;
    virtual void visit(const std::shared_ptr<Literal>& expr) = 0;
    virtual void visit(const std::shared_ptr<Unary>& expr) = 0;
    virtual void visit(const std::shared_ptr<Variable>& expr) = 0;
    virtual void visit(const std::shared_ptr<Assign>& expr) = 0;
    virtual void visit(const std::shared_ptr<Postfix>& expr) = 0;
    virtual void visit(const std::shared_ptr<CallExpr>& expr) = 0;
    virtual void visit(const std::shared_ptr<GetExpr>& expr) = 0;
    virtual void visit(const std::shared_ptr<SetExpr>& expr) = 0;
    virtual void visit(const std::shared_ptr<ThisExpr>& expr) = 0;
};

class Expr {
public:
    virtual ~Expr() = default;
    virtual void accept(ExprVisitor& visitor) = 0;
};

struct Binary : Expr, public std::enable_shared_from_this<Binary> {
    Binary(std::shared_ptr<Expr> left, Token op, std::shared_ptr<Expr> right)
        : left(left), op(op), right(right) {}

    void accept(ExprVisitor& visitor) override {
        visitor.visit(shared_from_this());
    }

    std::shared_ptr<Expr> left;
    Token op;
    std::shared_ptr<Expr> right;
};

struct Grouping : Expr, public std::enable_shared_from_this<Grouping> {
    Grouping(std::shared_ptr<Expr> expression)
        : expression(expression) {}

    void accept(ExprVisitor& visitor) override {
        visitor.visit(shared_from_this());
    }

    std::shared_ptr<Expr> expression;
};

struct Literal : Expr, public std::enable_shared_from_this<Literal> {
    Literal(std::variant<std::monostate, int, double, std::string, bool> value)
        : value(value) {}

    void accept(ExprVisitor& visitor) override {
        visitor.visit(shared_from_this());
    }

    std::variant<std::monostate, int, double, std::string, bool> value;
};

struct Unary : Expr, public std::enable_shared_from_this<Unary> {
    Unary(Token op, std::shared_ptr<Expr> right)
        : op(op), right(right) {}

    void accept(ExprVisitor& visitor) override {
        visitor.visit(shared_from_this());
    }

    Token op;
    std::shared_ptr<Expr> right;
};

struct Variable : Expr, public std::enable_shared_from_this<Variable> {
    Variable(Token name)
        : name(name) {}

    void accept(ExprVisitor& visitor) override {
        visitor.visit(shared_from_this());
    }

    Token name;
};

struct Assign : Expr, public std::enable_shared_from_this<Assign> {
    Assign(Token name, std::shared_ptr<Expr> value)
        : name(name), value(value) {}

    void accept(ExprVisitor& visitor) override {
        visitor.visit(shared_from_this());
    }

    Token name;
    std::shared_ptr<Expr> value;
};

struct Postfix : Expr, public std::enable_shared_from_this<Postfix> {
    Postfix(std::shared_ptr<Expr> left, Token op)
        : left(left), op(op) {}

    void accept(ExprVisitor& visitor) override {
        visitor.visit(shared_from_this());
    }

    std::shared_ptr<Expr> left;
    Token op;
};

struct CallExpr : Expr, public std::enable_shared_from_this<CallExpr> {
    CallExpr(std::shared_ptr<Expr> callee, Token paren, std::vector<std::shared_ptr<Expr>> arguments)
        : callee(callee), paren(paren), arguments(arguments) {}

    void accept(ExprVisitor& visitor) override {
        visitor.visit(shared_from_this());
    }

    std::shared_ptr<Expr> callee;
    Token paren; // For error reporting
    std::vector<std::shared_ptr<Expr>> arguments;
};

struct GetExpr : Expr, public std::enable_shared_from_this<GetExpr> {
    GetExpr(std::shared_ptr<Expr> object, Token name)
        : object(object), name(name) {}

    void accept(ExprVisitor& visitor) override {
        visitor.visit(shared_from_this());
    }

    std::shared_ptr<Expr> object;
    Token name;
};

struct SetExpr : Expr, public std::enable_shared_from_this<SetExpr> {
    SetExpr(std::shared_ptr<Expr> object, Token name, std::shared_ptr<Expr> value)
        : object(object), name(name), value(value) {}

    void accept(ExprVisitor& visitor) override {
        visitor.visit(shared_from_this());
    }

    std::shared_ptr<Expr> object;
    Token name;
    std::shared_ptr<Expr> value;
};

struct ThisExpr : Expr, public std::enable_shared_from_this<ThisExpr> {
    ThisExpr(Token keyword)
        : keyword(keyword) {}

    void accept(ExprVisitor& visitor) override {
        visitor.visit(shared_from_this());
    }

    Token keyword;
};

// Statements
struct Block;
struct ExpressionStmt;
struct VarStmt;
struct IfStmt;
struct WhileStmt;
struct DoWhileStmt;
struct FunctionStmt;
struct ReturnStmt;
struct ClassStmt;

class StmtVisitor {
public:
    virtual ~StmtVisitor() = default;
    virtual void visit(const std::shared_ptr<Block>& stmt) = 0;
    virtual void visit(const std::shared_ptr<ExpressionStmt>& stmt) = 0;
    virtual void visit(const std::shared_ptr<VarStmt>& stmt) = 0;
    virtual void visit(const std::shared_ptr<IfStmt>& stmt) = 0;
    virtual void visit(const std::shared_ptr<WhileStmt>& stmt) = 0;
    virtual void visit(const std::shared_ptr<DoWhileStmt>& stmt) = 0;
    virtual void visit(const std::shared_ptr<FunctionStmt>& stmt) = 0;
    virtual void visit(const std::shared_ptr<ReturnStmt>& stmt) = 0;
    virtual void visit(const std::shared_ptr<ClassStmt>& stmt) = 0;
};

class Stmt {
public:
    virtual ~Stmt() = default;
    virtual void accept(StmtVisitor& visitor) = 0;
};

struct Block : Stmt, public std::enable_shared_from_this<Block> {
    Block(std::vector<std::shared_ptr<Stmt>> statements)
        : statements(statements) {}

    void accept(StmtVisitor& visitor) override {
        visitor.visit(shared_from_this());
    }

    std::vector<std::shared_ptr<Stmt>> statements;
};

struct ExpressionStmt : Stmt, public std::enable_shared_from_this<ExpressionStmt> {
    ExpressionStmt(std::shared_ptr<Expr> expression)
        : expression(expression) {}

    void accept(StmtVisitor& visitor) override {
        visitor.visit(shared_from_this());
    }

    std::shared_ptr<Expr> expression;
};

struct VarStmt : Stmt, public std::enable_shared_from_this<VarStmt> {
    VarStmt(Token name, std::shared_ptr<Expr> initializer)
        : name(name), initializer(initializer) {}

    void accept(StmtVisitor& visitor) override {
        visitor.visit(shared_from_this());
    }

    Token name;
    std::shared_ptr<Expr> initializer;
};

struct IfStmt : Stmt, public std::enable_shared_from_this<IfStmt> {
    IfStmt(std::shared_ptr<Expr> condition, std::shared_ptr<Stmt> thenBranch, std::shared_ptr<Stmt> elseBranch)
        : condition(condition), thenBranch(thenBranch), elseBranch(elseBranch) {}

    void accept(StmtVisitor& visitor) override {
        visitor.visit(shared_from_this());
    }

    std::shared_ptr<Expr> condition;
    std::shared_ptr<Stmt> thenBranch;
    std::shared_ptr<Stmt> elseBranch;
};

struct WhileStmt : Stmt, public std::enable_shared_from_this<WhileStmt> {
    WhileStmt(std::shared_ptr<Expr> condition, std::shared_ptr<Stmt> body)
        : condition(condition), body(body) {}

    void accept(StmtVisitor& visitor) override {
        visitor.visit(shared_from_this());
    }

    std::shared_ptr<Expr> condition;
    std::shared_ptr<Stmt> body;
};

struct DoWhileStmt : Stmt, public std::enable_shared_from_this<DoWhileStmt> {
    DoWhileStmt(std::shared_ptr<Stmt> body, std::shared_ptr<Expr> condition)
        : body(body), condition(condition) {}

    void accept(StmtVisitor& visitor) override {
        visitor.visit(shared_from_this());
    }

    std::shared_ptr<Stmt> body;
    std::shared_ptr<Expr> condition;
};

struct FunctionStmt : Stmt, public std::enable_shared_from_this<FunctionStmt> {
    FunctionStmt(Token name, std::vector<Token> params, std::vector<std::shared_ptr<Stmt>> body)
        : name(name), params(params), body(body) {}

    void accept(StmtVisitor& visitor) override {
        visitor.visit(shared_from_this());
    }

    Token name;
    std::vector<Token> params;
    std::vector<std::shared_ptr<Stmt>> body;
};

struct ReturnStmt : Stmt, public std::enable_shared_from_this<ReturnStmt> {
    ReturnStmt(Token keyword, std::shared_ptr<Expr> value)
        : keyword(keyword), value(value) {}

    void accept(StmtVisitor& visitor) override {
        visitor.visit(shared_from_this());
    }

    Token keyword; // For error reporting
    std::shared_ptr<Expr> value;
};

struct ClassStmt : Stmt, public std::enable_shared_from_this<ClassStmt> {
    ClassStmt(Token name, std::vector<std::shared_ptr<FunctionStmt>> methods)
        : name(name), methods(methods) {}

    void accept(StmtVisitor& visitor) override {
        visitor.visit(shared_from_this());
    }

    Token name;
    std::vector<std::shared_ptr<FunctionStmt>> methods;
};


} // namespace nota
