#include "compiler.hpp"
#include <stdexcept>

Compiler::Compiler() {}

Chunk Compiler::compile(const std::vector<std::unique_ptr<Stmt>>& statements) {
    for (const auto& statement : statements) {
        statement->accept(*this);
    }
    return chunk;
}

void Compiler::visit(const ExpressionStmt& stmt) {
    stmt.expression->accept(*this);
}

void Compiler::visit(const LiteralExpr& expr) {
    Value value;
    switch (expr.value.type) {
        case TokenType::INTEGER:
            value = std::stod(expr.value.lexeme);
            break;
        case TokenType::FLOAT:
            value = std::stod(expr.value.lexeme);
            break;
        case TokenType::TRUE:
            value = true;
            break;
        case TokenType::FALSE:
            value = false;
            break;
        default:
            // Should not happen
            break;
    }
    chunk.write(OP_CONSTANT, expr.value.line);
    chunk.write(chunk.add_constant(value), expr.value.line);
}

// Placeholder implementations
void Compiler::visit(const LetStmt& stmt) {}
void Compiler::visit(const MutStmt& stmt) {}
void Compiler::visit(const BlockStmt& stmt) {}
void Compiler::visit(const IfStmt& stmt) {}
void Compiler::visit(const WhileStmt& stmt) {}
void Compiler::visit(const FunctionStmt& stmt) {}
void Compiler::visit(const ReturnStmt& stmt) {}
void Compiler::visit(const BinaryExpr& expr) {}
void Compiler::visit(const UnaryExpr& expr) {}
void Compiler::visit(const GroupingExpr& expr) {}
void Compiler::visit(const VariableExpr& expr) {}
void Compiler::visit(const CallExpr& expr) {}
