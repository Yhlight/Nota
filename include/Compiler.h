#ifndef NOTA_COMPILER_H
#define NOTA_COMPILER_H

#include "AST/Stmt.h"
#include "Bytecode.h"
#include "AST/Expr.h"
#include "AST/Stmt.h"
#include <memory>
#include <vector>

namespace nota {

class Compiler : public ast::ExprVisitor, public ast::StmtVisitor {
public:
    Chunk compile(const std::vector<std::unique_ptr<ast::Stmt>> &statements);

private:
    Chunk chunk;

    void emit_byte(uint8_t byte);
    void emit_bytes(uint8_t byte1, uint8_t byte2);
    uint8_t make_constant(Value value);
    void emit_constant(Value value);

    std::any visit(ast::LiteralExpr &expr) override;
    std::any visit(ast::UnaryExpr &expr) override;
    std::any visit(ast::BinaryExpr &expr) override;
    std::any visit(ast::VariableExpr &expr) override;
    std::any visit(ast::AssignExpr &expr) override;
    std::any visit(ast::PostfixExpr &expr) override;
    std::any visit(ast::CallExpr &expr) override;
    std::any visit(ast::LambdaExpr &expr) override;
    std::any visit(ast::ArrayLiteralExpr &expr) override;
    std::any visit(ast::SubscriptExpr &expr) override;
    std::any visit(ast::GetExpr &expr) override;
    std::any visit(ast::SetExpr &expr) override;
    std::any visit(ast::ThisExpr &expr) override;

    std::any visit(ast::VarDeclStmt &stmt) override;
    std::any visit(ast::BlockStmt &stmt) override;
    std::any visit(ast::IfStmt &stmt) override;
    std::any visit(ast::WhileStmt &stmt) override;
    std::any visit(ast::DoWhileStmt &stmt) override;
    std::any visit(ast::ExpressionStmt &stmt) override;
    std::any visit(ast::ForEachStmt &stmt) override;
    std::any visit(ast::ForStmt &stmt) override;
    std::any visit(ast::MatchStmt &stmt) override;
    std::any visit(ast::FuncDeclStmt &stmt) override;
    std::any visit(ast::ReturnStmt &stmt) override;
    std::any visit(ast::ClassDeclStmt &stmt) override;
    std::any visit(ast::ImportStmt &stmt) override;
    std::any visit(ast::PackageDeclStmt &stmt) override;
};

} // namespace nota

#endif // NOTA_COMPILER_H
