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

size_t Compiler::emit_jump(uint8_t instruction) {
    emit_byte(instruction);
    emit_byte(0xff);
    emit_byte(0xff);
    return chunk.code.size() - 2;
}

void Compiler::patch_jump(size_t offset) {
    // -2 to adjust for the size of the jump offset itself.
    size_t jump = chunk.code.size() - offset - 2;

    chunk.code[offset] = (jump >> 8) & 0xff;
    chunk.code[offset + 1] = jump & 0xff;
}

void Compiler::emit_loop(size_t loop_start) {
    emit_byte((uint8_t)OpCode::Jump);

    size_t offset = chunk.code.size() - loop_start + 2;
    emit_byte((offset >> 8) & 0xff);
    emit_byte(offset & 0xff);
}


Chunk Compiler::compile(const std::vector<std::unique_ptr<ast::Stmt>> &statements, bool is_repl) {
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
            emit_byte((uint8_t)OpCode::True);
            break;
        case TokenType::False:
            emit_byte((uint8_t)OpCode::False);
            break;
        case TokenType::None:
            emit_byte((uint8_t)OpCode::None);
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
        case TokenType::BangEqual:
            emit_byte((uint8_t)OpCode::NotEqual);
            break;
        case TokenType::EqualEqual:
            emit_byte((uint8_t)OpCode::Equal);
            break;
        case TokenType::Greater:
            emit_byte((uint8_t)OpCode::Greater);
            break;
        case TokenType::GreaterEqual:
            emit_byte((uint8_t)OpCode::GreaterEqual);
            break;
        case TokenType::Less:
            emit_byte((uint8_t)OpCode::Less);
            break;
        case TokenType::LessEqual:
            emit_byte((uint8_t)OpCode::LessEqual);
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
        case TokenType::Bang:
            emit_byte((uint8_t)OpCode::Not);
            break;
        default:
            break;
    }
    return {};
}
std::any Compiler::visit(ast::VariableExpr &expr) {
    uint8_t name_index = make_constant(expr.name.lexeme);
    emit_bytes((uint8_t)OpCode::GetGlobal, name_index);
    return {};
}
std::any Compiler::visit(ast::AssignExpr &expr) {
    expr.value->accept(*this);

    if (auto var = dynamic_cast<ast::VariableExpr*>(expr.name.get())) {
        uint8_t name_index = make_constant(var->name.lexeme);
        emit_bytes((uint8_t)OpCode::SetGlobal, name_index);
    } else {
        throw std::runtime_error("Invalid assignment target.");
    }
    return {};
}
std::any Compiler::visit(ast::PostfixExpr &expr) { return {}; }
std::any Compiler::visit(ast::CallExpr &expr) { return {}; }
std::any Compiler::visit(ast::LambdaExpr &expr) { return {}; }
std::any Compiler::visit(ast::ArrayLiteralExpr &expr) { return {}; }
std::any Compiler::visit(ast::SubscriptExpr &expr) { return {}; }
std::any Compiler::visit(ast::GetExpr &expr) { return {}; }
std::any Compiler::visit(ast::SetExpr &expr) { return {}; }
std::any Compiler::visit(ast::ThisExpr &expr) { return {}; }

std::any Compiler::visit(ast::VarDeclStmt &stmt) {
    if (stmt.initializer) {
        stmt.initializer->accept(*this);
    } else {
        emit_byte((uint8_t)OpCode::None);
    }

    uint8_t name_index = make_constant(stmt.name.lexeme);
    emit_bytes((uint8_t)OpCode::DefineGlobal, name_index);
    return {};
}
std::any Compiler::visit(ast::BlockStmt &stmt) {
    for (const auto &s : stmt.statements) {
        s->accept(*this);
    }
    return {};
}
std::any Compiler::visit(ast::IfStmt &stmt) {
    stmt.condition->accept(*this);

    size_t then_jump = emit_jump((uint8_t)OpCode::JumpIfFalse);
    emit_byte((uint8_t)OpCode::Pop); // Pop the condition

    stmt.then_branch->accept(*this);

    size_t else_jump = emit_jump((uint8_t)OpCode::Jump);

    patch_jump(then_jump);

    if (stmt.else_branch) {
        stmt.else_branch->accept(*this);
    }

    patch_jump(else_jump);

    return {};
}
std::any Compiler::visit(ast::WhileStmt &stmt) { return {}; }
std::any Compiler::visit(ast::DoWhileStmt &stmt) { return {}; }
std::any Compiler::visit(ast::ExpressionStmt &stmt) {
    stmt.expression->accept(*this);
    emit_byte((uint8_t)OpCode::Pop);
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
