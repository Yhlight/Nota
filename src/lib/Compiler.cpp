#include "Compiler.h"

namespace nota {

Chunk Compiler::compile(const std::unique_ptr<Expr>& expr) {
    expr->accept(*this);
    return chunk;
}

std::any Compiler::visitBinaryExpr(const Binary& expr) {
    expr.left->accept(*this);
    expr.right->accept(*this);

    switch (expr.op.type) {
        case TokenType::PLUS:          chunk.write(static_cast<uint8_t>(OpCode::OP_ADD), expr.op.line); break;
        case TokenType::MINUS:         chunk.write(static_cast<uint8_t>(OpCode::OP_SUBTRACT), expr.op.line); break;
        case TokenType::STAR:          chunk.write(static_cast<uint8_t>(OpCode::OP_MULTIPLY), expr.op.line); break;
        case TokenType::SLASH:         chunk.write(static_cast<uint8_t>(OpCode::OP_DIVIDE), expr.op.line); break;
        case TokenType::EQUAL_EQUAL:   chunk.write(static_cast<uint8_t>(OpCode::OP_EQUAL), expr.op.line); break;
        case TokenType::BANG_EQUAL:    chunk.write(static_cast<uint8_t>(OpCode::OP_EQUAL), expr.op.line);
                                       chunk.write(static_cast<uint8_t>(OpCode::OP_NOT), expr.op.line); break;
        case TokenType::GREATER:       chunk.write(static_cast<uint8_t>(OpCode::OP_GREATER), expr.op.line); break;
        case TokenType::GREATER_EQUAL: chunk.write(static_cast<uint8_t>(OpCode::OP_LESS), expr.op.line);
                                       chunk.write(static_cast<uint8_t>(OpCode::OP_NOT), expr.op.line); break;
        case TokenType::LESS:          chunk.write(static_cast<uint8_t>(OpCode::OP_LESS), expr.op.line); break;
        case TokenType::LESS_EQUAL:    chunk.write(static_cast<uint8_t>(OpCode::OP_GREATER), expr.op.line);
                                       chunk.write(static_cast<uint8_t>(OpCode::OP_NOT), expr.op.line); break;
        default: break; // Unreachable.
    }
    return {};
}

std::any Compiler::visitGroupingExpr(const Grouping& expr) {
    expr.expression->accept(*this);
    return {};
}

std::any Compiler::visitLiteralExpr(const Literal& expr) {
    int constant = chunk.addConstant(expr.value);
    chunk.write(static_cast<uint8_t>(OpCode::OP_CONSTANT), expr.line);
    chunk.write(constant, expr.line);
    return {};
}

std::any Compiler::visitUnaryExpr(const Unary& expr) {
    expr.right->accept(*this);
    switch (expr.op.type) {
        case TokenType::MINUS: chunk.write(static_cast<uint8_t>(OpCode::OP_NEGATE), expr.op.line); break;
        case TokenType::BANG:  chunk.write(static_cast<uint8_t>(OpCode::OP_NOT), expr.op.line); break;
        default: break; // Unreachable.
    }
    return {};
}

} // namespace nota
