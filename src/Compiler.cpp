#include "Compiler.h"
#include <stdexcept>

namespace nota {

void Compiler::emit_byte(uint8_t byte) {
    chunk.code.push_back(byte);
}

void Compiler::emit_bytes(uint8_t byte1, uint8_t byte2) {
    emit_byte(byte1);
    emit_byte(byte2);
}

uint8_t Compiler::make_constant(Value value) {
    chunk.constants.push_back(value);
    return chunk.constants.size() - 1;
}

void Compiler::emit_constant(Value value) {
    emit_bytes((uint8_t)OpCode::Constant, make_constant(value));
}

Chunk Compiler::compile(const std::vector<std::unique_ptr<ast::Stmt>> &statements) {
    for (const auto &stmt : statements) {
        stmt->accept(*this);
    }
    emit_byte((uint8_t)OpCode::Return);
    return chunk;
}

std::any Compiler::visit(ast::LiteralExpr &expr) {
    switch (expr.value.type) {
        case TokenType::Integer:
            emit_constant(std::stoll(expr.value.lexeme));
            break;
        case TokenType::Float:
            emit_constant(std::stod(expr.value.lexeme));
            break;
        case TokenType::String:
            emit_constant(expr.value.lexeme);
            break;
        case TokenType::True:
            emit_constant(true);
            break;
        case TokenType::False:
            emit_constant(false);
            break;
        default:
            break;
    }
    return {};
}

std::any Compiler::visit(ast::BinaryExpr &expr) {
    expr.left->accept(*this);
    expr.right->accept(*this);

    switch (expr.op.type) {
        case TokenType::Plus:
            emit_byte((uint8_t)OpCode::Add);
            break;
        case TokenType::Minus:
            emit_byte((uint8_t)OpCode::Subtract);
            break;
        case TokenType::Star:
            emit_byte((uint8_t)OpCode::Multiply);
            break;
        case TokenType::Slash:
            emit_byte((uint8_t)OpCode::Divide);
            break;
        default:
            break;
    }
    return {};
}

// Empty implementations for other visit methods
std::any Compiler::visit(ast::UnaryExpr &expr) {
    expr.right->accept(*this);
    switch (expr.op.type) {
        case TokenType::Minus:
            emit_byte((uint8_t)OpCode::Negate);
            break;
        default:
            break;
    }
    return {};
}
std::any Compiler::visit(ast::VariableExpr &expr) { return {}; }
std::any Compiler::visit(ast::AssignExpr &expr) { return {}; }
std::any Compiler::visit(ast::PostfixExpr &expr) { return {}; }
std::any Compiler::visit(ast::CallExpr &expr) { return {}; }
std::any Compiler::visit(ast::LambdaExpr &expr) { return {}; }
std::any Compiler::visit(ast::ArrayLiteralExpr &expr) { return {}; }
std::any Compiler::visit(ast::SubscriptExpr &expr) { return {}; }
std::any Compiler::visit(ast::GetExpr &expr) { return {}; }
std::any Compiler::visit(ast::SetExpr &expr) { return {}; }
std::any Compiler::visit(ast::ThisExpr &expr) { return {}; }

std::any Compiler::visit(ast::VarDeclStmt &stmt) { return {}; }
std::any Compiler::visit(ast::BlockStmt &stmt) { return {}; }
std::any Compiler::visit(ast::IfStmt &stmt) { return {}; }
std::any Compiler::visit(ast::WhileStmt &stmt) { return {}; }
std::any Compiler::visit(ast::DoWhileStmt &stmt) { return {}; }
std::any Compiler::visit(ast::ExpressionStmt &stmt) {
    stmt.expression->accept(*this);
    return {};
}
std::any Compiler::visit(ast::ForEachStmt &stmt) { return {}; }
std::any Compiler::visit(ast::ForStmt &stmt) { return {}; }
std::any Compiler::visit(ast::MatchStmt &stmt) { return {}; }
std::any Compiler::visit(ast::FuncDeclStmt &stmt) { return {}; }
std::any Compiler::visit(ast::ReturnStmt &stmt) { return {}; }
std::any Compiler::visit(ast::ClassDeclStmt &stmt) { return {}; }
std::any Compiler::visit(ast::ImportStmt &stmt) { return {}; }
std::any Compiler::visit(ast::PackageDeclStmt &stmt) { return {}; }


} // namespace nota
