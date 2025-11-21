#include "Compiler.h"

namespace nota {

Chunk Compiler::compile(const std::vector<std::unique_ptr<Stmt>>& statements) {
    for (const auto& stmt : statements) {
        stmt->accept(*this);
    }
    return chunk;
}

void Compiler::emitByte(uint8_t byte, int line) {
    chunk.write(byte, line);
}

void Compiler::emitBytes(uint8_t byte1, uint8_t byte2, int line) {
    emitByte(byte1, line);
    emitByte(byte2, line);
}

int Compiler::emitJump(uint8_t instruction, int line) {
    emitByte(instruction, line);
    emitByte(0xff, line);
    emitByte(0xff, line);
    return chunk.code.size() - 2;
}

void Compiler::patchJump(int offset) {
    int jump = chunk.code.size() - offset - 2;
    chunk.patch(offset, jump);
}


uint8_t Compiler::makeConstant(Value value) {
    int constant = chunk.addConstant(value);
    // For now, we assume we won't have more than 256 constants.
    return static_cast<uint8_t>(constant);
}

void Compiler::emitConstant(Value value, int line) {
    emitByte(static_cast<uint8_t>(OpCode::OP_CONSTANT), line);
    emitByte(makeConstant(value), line);
}

void Compiler::beginScope() {
    scopeDepth.push_back(0);
}

void Compiler::endScope() {
    scopeDepth.pop_back();
}


// Expression Visitor Implementations
std::any Compiler::visitBinaryExpr(const Binary& expr) {
    expr.left->accept(*this);
    expr.right->accept(*this);

    switch (expr.op.type) {
        case TokenType::PLUS:          emitByte(static_cast<uint8_t>(OpCode::OP_ADD), expr.op.line); break;
        case TokenType::MINUS:         emitByte(static_cast<uint8_t>(OpCode::OP_SUBTRACT), expr.op.line); break;
        case TokenType::STAR:          emitByte(static_cast<uint8_t>(OpCode::OP_MULTIPLY), expr.op.line); break;
        case TokenType::SLASH:         emitByte(static_cast<uint8_t>(OpCode::OP_DIVIDE), expr.op.line); break;
        case TokenType::EQUAL_EQUAL:   emitByte(static_cast<uint8_t>(OpCode::OP_EQUAL), expr.op.line); break;
        case TokenType::BANG_EQUAL:    emitByte(static_cast<uint8_t>(OpCode::OP_EQUAL), expr.op.line);
                                       emitByte(static_cast<uint8_t>(OpCode::OP_NOT), expr.op.line); break;
        case TokenType::GREATER:       emitByte(static_cast<uint8_t>(OpCode::OP_GREATER), expr.op.line); break;
        case TokenType::GREATER_EQUAL: emitByte(static_cast<uint8_t>(OpCode::OP_LESS), expr.op.line);
                                       emitByte(static_cast<uint8_t>(OpCode::OP_NOT), expr.op.line); break;
        case TokenType::LESS:          emitByte(static_cast<uint8_t>(OpCode::OP_LESS), expr.op.line); break;
        case TokenType::LESS_EQUAL:    emitByte(static_cast<uint8_t>(OpCode::OP_GREATER), expr.op.line);
                                       emitByte(static_cast<uint8_t>(OpCode::OP_NOT), expr.op.line); break;
        default: break; // Unreachable.
    }
    return {};
}

std::any Compiler::visitGroupingExpr(const Grouping& expr) {
    expr.expression->accept(*this);
    return {};
}

std::any Compiler::visitLiteralExpr(const Literal& expr) {
    emitConstant(expr.value, expr.line);
    return {};
}

std::any Compiler::visitUnaryExpr(const Unary& expr) {
    expr.right->accept(*this);
    switch (expr.op.type) {
        case TokenType::MINUS: emitByte(static_cast<uint8_t>(OpCode::OP_NEGATE), expr.op.line); break;
        case TokenType::BANG:  emitByte(static_cast<uint8_t>(OpCode::OP_NOT), expr.op.line); break;
        default: break; // Unreachable.
    }
    return {};
}

std::any Compiler::visitVariableExpr(const VariableExpr& expr) {
    uint8_t name = makeConstant(expr.name.lexeme);
    emitBytes(static_cast<uint8_t>(OpCode::OP_GET_GLOBAL), name, expr.name.line);
    return {};
}

std::any Compiler::visitAssignExpr(const AssignExpr& expr) {
    expr.value->accept(*this);
    uint8_t name = makeConstant(expr.name.lexeme);
    emitBytes(static_cast<uint8_t>(OpCode::OP_SET_GLOBAL), name, expr.name.line);
    return {};
}


// Statement Visitor Implementations
void Compiler::visitExpressionStmt(const ExpressionStmt& stmt) {
    stmt.expression->accept(*this);
    // We might want to pop the value off the stack here, but for now we'll
    // leave it for potential use in a REPL.
}

void Compiler::visitVarDeclStmt(const VarDeclStmt& stmt) {
    if (stmt.initializer) {
        stmt.initializer->accept(*this);
    } else {
        // Default initialize to nil.
        emitByte(static_cast<uint8_t>(OpCode::OP_NIL), stmt.name.line);
    }

    uint8_t name = makeConstant(stmt.name.lexeme);
    emitBytes(static_cast<uint8_t>(OpCode::OP_DEFINE_GLOBAL), name, stmt.name.line);
}

void Compiler::visitBlockStmt(const BlockStmt& stmt) {
    beginScope();
    for (const auto& statement : stmt.statements) {
        statement->accept(*this);
    }
    endScope();
}

void Compiler::visitIfStmt(const IfStmt& stmt) {
    stmt.condition->accept(*this);

    int thenJump = emitJump(static_cast<uint8_t>(OpCode::OP_JUMP_IF_FALSE), 0);
    stmt.thenBranch->accept(*this);

    int elseJump = emitJump(static_cast<uint8_t>(OpCode::OP_JUMP), 0);

    patchJump(thenJump);
    if (stmt.elseBranch) {
        stmt.elseBranch->accept(*this);
    }

    patchJump(elseJump);
}

} // namespace nota
