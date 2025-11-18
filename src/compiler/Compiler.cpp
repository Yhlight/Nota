#include "Compiler.h"

Chunk Compiler::compile(const std::vector<std::unique_ptr<Stmt>>& statements) {
    for (const auto& stmt : statements) {
        stmt->accept(*this);
    }
    chunk_.write(static_cast<uint8_t>(OpCode::OP_RETURN), 0); // Add a return at the end
    return chunk_;
}

std::any Compiler::visit(BinaryExpr& expr) {
    expr.left->accept(*this);
    expr.right->accept(*this);
    switch (expr.op.type) {
        case TokenType::PLUS:    chunk_.write(static_cast<uint8_t>(OpCode::OP_ADD), expr.op.location.line); break;
        case TokenType::MINUS:   chunk_.write(static_cast<uint8_t>(OpCode::OP_SUBTRACT), expr.op.location.line); break;
        case TokenType::STAR:    chunk_.write(static_cast<uint8_t>(OpCode::OP_MULTIPLY), expr.op.location.line); break;
        case TokenType::SLASH:   chunk_.write(static_cast<uint8_t>(OpCode::OP_DIVIDE), expr.op.location.line); break;
        case TokenType::PERCENT: chunk_.write(static_cast<uint8_t>(OpCode::OP_MODULO), expr.op.location.line); break;
        case TokenType::EQ:      chunk_.write(static_cast<uint8_t>(OpCode::OP_EQ), expr.op.location.line); break;
        case TokenType::NEQ:     chunk_.write(static_cast<uint8_t>(OpCode::OP_NEQ), expr.op.location.line); break;
        case TokenType::GT:      chunk_.write(static_cast<uint8_t>(OpCode::OP_GT), expr.op.location.line); break;
        case TokenType::GTE:     chunk_.write(static_cast<uint8_t>(OpCode::OP_GTE), expr.op.location.line); break;
        case TokenType::LT:      chunk_.write(static_cast<uint8_t>(OpCode::OP_LT), expr.op.location.line); break;
        case TokenType::LTE:     chunk_.write(static_cast<uint8_t>(OpCode::OP_LTE), expr.op.location.line); break;
        default: break; // Should not happen
    }
    return {};
}

std::any Compiler::visit(UnaryExpr& expr) {
    expr.right->accept(*this);
    switch (expr.op.type) {
        case TokenType::MINUS: chunk_.write(static_cast<uint8_t>(OpCode::OP_NEGATE), expr.op.location.line); break;
        case TokenType::NOT:   chunk_.write(static_cast<uint8_t>(OpCode::OP_NOT), expr.op.location.line); break;
        default: break; // Should not happen
    }
    return {};
}

std::any Compiler::visit(LiteralExpr& expr) {
    switch (expr.value.type) {
        case TokenType::INTEGER:
        case TokenType::FLOAT: {
            size_t constant = chunk_.add_constant(std::stod(expr.value.lexeme));
            chunk_.write(static_cast<uint8_t>(OpCode::OP_CONSTANT), expr.value.location.line);
            chunk_.write(constant, expr.value.location.line);
            break;
        }
        case TokenType::BOOL:
            if (expr.value.lexeme == "true") {
                chunk_.write(static_cast<uint8_t>(OpCode::OP_TRUE), expr.value.location.line);
            } else {
                chunk_.write(static_cast<uint8_t>(OpCode::OP_FALSE), expr.value.location.line);
            }
            break;
        case TokenType::NONE:
            chunk_.write(static_cast<uint8_t>(OpCode::OP_NULL), expr.value.location.line);
            break;
        default: break; // Should not happen
    }
    return {};
}

std::any Compiler::visit(VariableExpr& expr) {
    size_t global = chunk_.add_constant(expr.name.lexeme);
    chunk_.write(static_cast<uint8_t>(OpCode::OP_GET_GLOBAL), expr.name.location.line);
    chunk_.write(global, expr.name.location.line);
    return {};
}

std::any Compiler::visit(VarDeclStmt& stmt) {
    stmt.initializer->accept(*this);
    size_t global = chunk_.add_constant(stmt.name.lexeme);
    chunk_.write(static_cast<uint8_t>(OpCode::OP_DEFINE_GLOBAL), stmt.name.location.line);
    chunk_.write(global, stmt.name.location.line);
    return {};
}

std::any Compiler::visit(PrintStmt& stmt) {
    stmt.expression->accept(*this);
    chunk_.write(static_cast<uint8_t>(OpCode::OP_PRINT), 0); // Line number is not important here
    return {};
}

std::any Compiler::visit(ExprStmt& stmt) {
    stmt.expression->accept(*this);
    chunk_.write(static_cast<uint8_t>(OpCode::OP_POP), 0); // Line number is not important here
    return {};
}
