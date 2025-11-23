#pragma once

#include "Token.h"
#include "NotaObjects.h"
#include <memory>
#include <vector>
#include <variant>
#include <optional>

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
struct ScopeAccessExpr;
struct LambdaExpr;
struct ArrayExpr;
struct SubscriptExpr;
struct LogicalExpr;
struct TypeExpr;
struct CastExpr;
struct Parameter;

struct Stmt;

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
    virtual void visit(const std::shared_ptr<ScopeAccessExpr>& expr) = 0;
    virtual void visit(const std::shared_ptr<LambdaExpr>& expr) = 0;
    virtual void visit(const std::shared_ptr<ArrayExpr>& expr) = 0;
    virtual void visit(const std::shared_ptr<SubscriptExpr>& expr) = 0;
    virtual void visit(const std::shared_ptr<LogicalExpr>& expr) = 0;
    virtual void visit(const std::shared_ptr<TypeExpr>& expr) = 0;
    virtual void visit(const std::shared_ptr<CastExpr>& expr) = 0;
};

class Expr : public std::enable_shared_from_this<Expr> {
public:
    virtual ~Expr() = default;
    virtual void accept(ExprVisitor& visitor) = 0;
};

struct Binary : Expr {
    Binary(std::shared_ptr<Expr> left, Token op, std::shared_ptr<Expr> right)
        : left(left), op(op), right(right) {}

    void accept(ExprVisitor& visitor) override {
        visitor.visit(std::static_pointer_cast<Binary>(shared_from_this()));
    }

    std::shared_ptr<Expr> left;
    Token op;
    std::shared_ptr<Expr> right;
};

struct Grouping : Expr {
    Grouping(std::shared_ptr<Expr> expression)
        : expression(expression) {}

    void accept(ExprVisitor& visitor) override {
        visitor.visit(std::static_pointer_cast<Grouping>(shared_from_this()));
    }

    std::shared_ptr<Expr> expression;
};

struct Literal : Expr {
    Literal(std::variant<std::monostate, int, double, std::string, bool> value)
        : value(value) {}

    void accept(ExprVisitor& visitor) override {
        visitor.visit(std::static_pointer_cast<Literal>(shared_from_this()));
    }

    std::variant<std::monostate, int, double, std::string, bool> value;
};

struct Unary : Expr {
    Unary(Token op, std::shared_ptr<Expr> right)
        : op(op), right(right) {}

    void accept(ExprVisitor& visitor) override {
        visitor.visit(std::static_pointer_cast<Unary>(shared_from_this()));
    }

    Token op;
    std::shared_ptr<Expr> right;
};

struct Variable : Expr {
    Variable(Token name)
        : name(name) {}

    void accept(ExprVisitor& visitor) override {
        visitor.visit(std::static_pointer_cast<Variable>(shared_from_this()));
    }

    Token name;
};

struct Assign : Expr {
    Assign(Token name, std::shared_ptr<Expr> value)
        : name(name), value(value) {}

    void accept(ExprVisitor& visitor) override {
        visitor.visit(std::static_pointer_cast<Assign>(shared_from_this()));
    }

    Token name;
    std::shared_ptr<Expr> value;
};

struct Postfix : Expr {
    Postfix(std::shared_ptr<Expr> left, Token op)
        : left(left), op(op) {}

    void accept(ExprVisitor& visitor) override {
        visitor.visit(std::static_pointer_cast<Postfix>(shared_from_this()));
    }

    std::shared_ptr<Expr> left;
    Token op;
};

struct CallExpr : Expr {
    CallExpr(std::shared_ptr<Expr> callee, Token paren, std::vector<std::shared_ptr<Expr>> arguments)
        : callee(callee), paren(paren), arguments(arguments) {}

    void accept(ExprVisitor& visitor) override {
        visitor.visit(std::static_pointer_cast<CallExpr>(shared_from_this()));
    }

    std::shared_ptr<Expr> callee;
    Token paren; // For error reporting
    std::vector<std::shared_ptr<Expr>> arguments;
};

struct GetExpr : Expr {
    GetExpr(std::shared_ptr<Expr> object, std::variant<Token, std::shared_ptr<Expr>> accessor)
        : object(object), accessor(accessor) {}

    void accept(ExprVisitor& visitor) override {
        visitor.visit(std::static_pointer_cast<GetExpr>(shared_from_this()));
    }

    std::shared_ptr<Expr> object;
    std::variant<Token, std::shared_ptr<Expr>> accessor;
};

struct SetExpr : Expr {
    SetExpr(std::shared_ptr<Expr> object, std::variant<Token, std::shared_ptr<Expr>> accessor, std::shared_ptr<Expr> value, Token token_for_error)
        : object(object), accessor(accessor), value(value), token_for_error(token_for_error) {}

    void accept(ExprVisitor& visitor) override {
        visitor.visit(std::static_pointer_cast<SetExpr>(shared_from_this()));
    }

    std::shared_ptr<Expr> object;
    std::variant<Token, std::shared_ptr<Expr>> accessor;
    std::shared_ptr<Expr> value;
    Token token_for_error;
};

struct ThisExpr : Expr {
    ThisExpr(Token keyword)
        : keyword(keyword) {}

    void accept(ExprVisitor& visitor) override {
        visitor.visit(std::static_pointer_cast<ThisExpr>(shared_from_this()));
    }

    Token keyword;
};

struct ScopeAccessExpr : Expr {
    ScopeAccessExpr(Token scope, Token member)
        : scope(scope), member(member) {}

    void accept(ExprVisitor& visitor) override {
        visitor.visit(std::static_pointer_cast<ScopeAccessExpr>(shared_from_this()));
    }

    Token scope;
    Token member;
};

struct LambdaExpr : Expr {
    LambdaExpr(std::vector<Parameter> params, std::vector<std::shared_ptr<Stmt>> body, std::shared_ptr<TypeExpr> return_type = nullptr) : params(params), body(body), return_type(return_type) {}
    void accept(ExprVisitor& visitor) override { visitor.visit(std::static_pointer_cast<LambdaExpr>(shared_from_this())); }
    std::vector<Parameter> params;
    std::vector<std::shared_ptr<Stmt>> body;
    std::shared_ptr<TypeExpr> return_type;
};

struct ArrayExpr : Expr {
    ArrayExpr(std::vector<std::shared_ptr<Expr>> elements) : elements(elements) {}
    void accept(ExprVisitor& visitor) override { visitor.visit(std::static_pointer_cast<ArrayExpr>(shared_from_this())); }
    std::vector<std::shared_ptr<Expr>> elements;
};

struct SubscriptExpr : Expr {
    SubscriptExpr(std::shared_ptr<Expr> object, std::shared_ptr<Expr> index, Token bracket)
        : object(object), index(index), bracket(bracket) {}
    void accept(ExprVisitor& visitor) override { visitor.visit(std::static_pointer_cast<SubscriptExpr>(shared_from_this())); }
    std::shared_ptr<Expr> object;
    std::shared_ptr<Expr> index;
    Token bracket; // For error reporting
};

struct LogicalExpr : Expr {
    LogicalExpr(std::shared_ptr<Expr> left, Token op, std::shared_ptr<Expr> right)
        : left(left), op(op), right(right) {}

    void accept(ExprVisitor& visitor) override {
        visitor.visit(std::static_pointer_cast<LogicalExpr>(shared_from_this()));
    }

    std::shared_ptr<Expr> left;
    Token op;
    std::shared_ptr<Expr> right;
};

struct TypeExpr : Expr {
    TypeExpr(Token name, std::shared_ptr<Expr> size = nullptr, bool is_array = false)
        : name(name), size(size), is_array(is_array) {}

    void accept(ExprVisitor& visitor) override {
        visitor.visit(std::static_pointer_cast<TypeExpr>(shared_from_this()));
    }

    Token name;
    std::shared_ptr<Expr> size; // For static arrays
    bool is_array;
};

struct CastExpr : Expr {
    CastExpr(std::shared_ptr<TypeExpr> type, std::shared_ptr<Expr> expression)
        : type(type), expression(expression) {}

    void accept(ExprVisitor& visitor) override {
        visitor.visit(std::static_pointer_cast<CastExpr>(shared_from_this()));
    }

    std::shared_ptr<TypeExpr> type;
    std::shared_ptr<Expr> expression;
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
struct ImportStmt;
struct PackageStmt;
struct ForEachStmt;
struct MatchStmt;
struct MatchCase;

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
    virtual void visit(const std::shared_ptr<ImportStmt>& stmt) = 0;
    virtual void visit(const std::shared_ptr<PackageStmt>& stmt) = 0;
    virtual void visit(const std::shared_ptr<ForEachStmt>& stmt) = 0;
    virtual void visit(const std::shared_ptr<MatchStmt>& stmt) = 0;
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
    VarStmt(Token name, std::shared_ptr<Expr> initializer, bool is_mutable, std::shared_ptr<TypeExpr> type = nullptr)
        : name(name), initializer(initializer), is_mutable(is_mutable), type(type) {}

    void accept(StmtVisitor& visitor) override {
        visitor.visit(shared_from_this());
    }

    Token name;
    std::shared_ptr<Expr> initializer;
    bool is_mutable;
    std::shared_ptr<TypeExpr> type;
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

struct Parameter {
    Token name;
    std::shared_ptr<TypeExpr> type;
};

struct FunctionStmt : Stmt, public std::enable_shared_from_this<FunctionStmt> {
    FunctionStmt(Token name, std::vector<Parameter> params, std::vector<std::shared_ptr<Stmt>> body, std::shared_ptr<TypeExpr> return_type = nullptr, bool is_static = false)
        : name(name), params(params), body(body), return_type(return_type), is_static(is_static) {}

    void accept(StmtVisitor& visitor) override {
        visitor.visit(shared_from_this());
    }

    Token name;
    std::vector<Parameter> params;
    std::vector<std::shared_ptr<Stmt>> body;
    std::shared_ptr<TypeExpr> return_type;
    bool is_static;
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
    ClassStmt(Token name, std::vector<std::shared_ptr<VarStmt>> properties, std::vector<std::shared_ptr<FunctionStmt>> methods)
        : name(name), properties(properties), methods(methods) {}

    void accept(StmtVisitor& visitor) override {
        visitor.visit(shared_from_this());
    }

    Token name;
    std::vector<std::shared_ptr<VarStmt>> properties;
    std::vector<std::shared_ptr<FunctionStmt>> methods;
};

struct ImportStmt : Stmt, public std::enable_shared_from_this<ImportStmt> {
    ImportStmt(Token path, std::optional<Token> alias)
        : path(path), alias(alias) {}

    void accept(StmtVisitor& visitor) override {
        visitor.visit(shared_from_this());
    }

    Token path;
    std::optional<Token> alias;
};

struct PackageStmt : Stmt, public std::enable_shared_from_this<PackageStmt> {
    PackageStmt(Token name)
        : name(name) {}

    void accept(StmtVisitor& visitor) override {
        visitor.visit(shared_from_this());
    }

    Token name;
};

struct ForEachStmt : Stmt, public std::enable_shared_from_this<ForEachStmt> {
    ForEachStmt(Token variable, std::shared_ptr<Expr> collection, std::shared_ptr<Stmt> body)
        : variable(variable), collection(collection), body(body) {}

    void accept(StmtVisitor& visitor) override {
        visitor.visit(shared_from_this());
    }

    Token variable;
    std::shared_ptr<Expr> collection;
    std::shared_ptr<Stmt> body;
};

struct MatchCase {
    std::vector<std::shared_ptr<Expr>> values;
    std::shared_ptr<Stmt> body;
    bool is_default;
};

struct MatchStmt : Stmt, public std::enable_shared_from_this<MatchStmt> {
    MatchStmt(std::shared_ptr<Expr> expression, std::vector<MatchCase> cases)
        : expression(expression), cases(cases) {}

    void accept(StmtVisitor& visitor) override {
        visitor.visit(shared_from_this());
    }

    std::shared_ptr<Expr> expression;
    std::vector<MatchCase> cases;
};


} // namespace nota
