#include "compiler.hpp"
#include <stdexcept>

namespace nota {

    Compiler::Compiler() : chunk(std::make_unique<Chunk>()) {}

    std::unique_ptr<Chunk> Compiler::compile(const std::vector<std::unique_ptr<Stmt>>& statements) {
        for (const auto& statement : statements) {
            statement->accept(*this);
        }
        emitReturn();
        return std::move(chunk);
    }

    std::any Compiler::visit(const BinaryExpr& expr) {
        expr.left->accept(*this);
        expr.right->accept(*this);
        switch (expr.op.type) {
            case TokenType::PLUS:    emitByte((uint8_t)OpCode::OP_ADD); break;
            case TokenType::MINUS:   emitByte((uint8_t)OpCode::OP_SUBTRACT); break;
            case TokenType::STAR:    emitByte((uint8_t)OpCode::OP_MULTIPLY); break;
            case TokenType::SLASH:   emitByte((uint8_t)OpCode::OP_DIVIDE); break;
            case TokenType::EQUAL_EQUAL: emitByte((uint8_t)OpCode::OP_EQUAL); break;
            case TokenType::GREATER:     emitByte((uint8_t)OpCode::OP_GREATER); break;
            case TokenType::LESS:        emitByte((uint8_t)OpCode::OP_LESS); break;
            default: break; // Unreachable.
        }
        return {};
    }

    std::any Compiler::visit(const UnaryExpr& expr) {
        expr.right->accept(*this);
        switch (expr.op.type) {
            case TokenType::MINUS: emitByte((uint8_t)OpCode::OP_NEGATE); break;
            case TokenType::BANG:  emitByte((uint8_t)OpCode::OP_NOT); break;
            default: break; // Unreachable.
        }
        return {};
    }

    std::any Compiler::visit(const LiteralExpr& expr) {
        emitConstant(expr.value);
        return {};
    }

    std::any Compiler::visit(const GroupingExpr& expr) {
        expr.expression->accept(*this);
        return {};
    }

    std::any Compiler::visit(const VariableExpr& expr) {
        uint8_t arg = identifierConstant(expr.name);
        emitBytes((uint8_t)OpCode::OP_GET_GLOBAL, arg);
        return {};
    }

    std::any Compiler::visit(const AssignExpr& expr) {
        expr.value->accept(*this);
        uint8_t arg = identifierConstant(expr.name);
        emitBytes((uint8_t)OpCode::OP_SET_GLOBAL, arg);
        return {};
    }

    void Compiler::visit(const ExpressionStmt& stmt) {
        stmt.expression->accept(*this);
        emitByte((uint8_t)OpCode::OP_POP);
    }

    void Compiler::visit(const VarDeclStmt& stmt) {
        if (stmt.initializer) {
            stmt.initializer->accept(*this);
        } else {
            emitByte((uint8_t)OpCode::OP_NIL);
        }
        uint8_t arg = identifierConstant(stmt.name);
        emitBytes((uint8_t)OpCode::OP_DEFINE_GLOBAL, arg);
    }

    void Compiler::visit(const BlockStmt& stmt) {
        for (const auto& statement : stmt.statements) {
            statement->accept(*this);
        }
    }

    void Compiler::emitByte(uint8_t byte) {
        chunk->write(byte, 0); // Placeholder line number
    }

    void Compiler::emitBytes(uint8_t byte1, uint8_t byte2) {
        emitByte(byte1);
        emitByte(byte2);
    }

    void Compiler::emitConstant(const LiteralValue& value) {
        size_t constant = chunk->addConstant(value);
        if (constant > UINT8_MAX) {
            throw std::runtime_error("Too many constants in one chunk.");
        }
        emitBytes((uint8_t)OpCode::OP_CONSTANT, (uint8_t)constant);
    }

    void Compiler::emitReturn() {
        emitByte((uint8_t)OpCode::OP_RETURN);
    }

    uint8_t Compiler::identifierConstant(const Token& name) {
        size_t constant = chunk->addConstant(name.lexeme);
        if (constant > UINT8_MAX) {
            throw std::runtime_error("Too many constants in one chunk.");
        }
        return (uint8_t)constant;
    }

} // namespace nota
