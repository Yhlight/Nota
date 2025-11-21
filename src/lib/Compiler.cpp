#include "Compiler.h"

namespace nota {

Compiler::Compiler() = default;

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

void Compiler::emitLoop(int loopStart, int line) {
    emitByte(static_cast<uint8_t>(OpCode::OP_LOOP), line);

    int offset = chunk.code.size() - loopStart + 2;
    emitByte((offset >> 8) & 0xff, line);
    emitByte(offset & 0xff, line);
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
    symbolTable.beginScope();
}

void Compiler::endScope() {
    symbolTable.endScope();

    while (symbolTable.locals.size() > 0 &&
           symbolTable.locals.back().depth > symbolTable.scopeDepth) {
        emitByte(static_cast<uint8_t>(OpCode::OP_POP), 0); // Placeholder line
        symbolTable.locals.pop_back();
    }
}

void Compiler::declareVariable(Token name) {
    if (symbolTable.scopeDepth == 0) return;

    for (int i = symbolTable.locals.size() - 1; i >= 0; i--) {
        const Local& local = symbolTable.locals[i];
        if (local.depth != -1 && local.depth < symbolTable.scopeDepth) {
            break;
        }

        if (name.lexeme == local.name.lexeme) {
            // Error: Already a variable with this name in this scope.
        }
    }

    symbolTable.addLocal(name);
}

uint8_t Compiler::parseVariable(const std::string& errorMessage, int line) {
    // For now, we only support global variables.
    return makeConstant(errorMessage);
}

void Compiler::defineVariable(uint8_t global, int line) {
    if (symbolTable.scopeDepth > 0) {
        // Mark the latest local as initialized.
        symbolTable.locals.back().depth = symbolTable.scopeDepth;
        return;
    }

    emitBytes(static_cast<uint8_t>(OpCode::OP_DEFINE_GLOBAL), global, line);
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
    int arg = symbolTable.resolveLocal(expr.name.lexeme);
    if (arg != -1) {
        emitBytes(static_cast<uint8_t>(OpCode::OP_GET_LOCAL), arg, expr.name.line);
    } else {
        uint8_t name = makeConstant(expr.name.lexeme);
        emitBytes(static_cast<uint8_t>(OpCode::OP_GET_GLOBAL), name, expr.name.line);
    }
    return {};
}

std::any Compiler::visitAssignExpr(const AssignExpr& expr) {
    expr.value->accept(*this);
    int arg = symbolTable.resolveLocal(expr.name.lexeme);
    if (arg != -1) {
        emitBytes(static_cast<uint8_t>(OpCode::OP_SET_LOCAL), arg, expr.name.line);
    } else {
        uint8_t name = makeConstant(expr.name.lexeme);
        emitBytes(static_cast<uint8_t>(OpCode::OP_SET_GLOBAL), name, expr.name.line);
    }
    return {};
}


// Statement Visitor Implementations
void Compiler::visitExpressionStmt(const ExpressionStmt& stmt) {
    stmt.expression->accept(*this);
    emitByte(static_cast<uint8_t>(OpCode::OP_POP), 0); // Placeholder line
}

void Compiler::visitVarDeclStmt(const VarDeclStmt& stmt) {
    declareVariable(stmt.name);
    if (stmt.initializer) {
        stmt.initializer->accept(*this);
    } else {
        emitByte(static_cast<uint8_t>(OpCode::OP_NIL), stmt.name.line);
    }
    defineVariable(makeConstant(stmt.name.lexeme), stmt.name.line);
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

void Compiler::visitWhileStmt(const WhileStmt& stmt) {
    int loopStart = chunk.code.size();
    stmt.condition->accept(*this);

    int exitJump = emitJump(static_cast<uint8_t>(OpCode::OP_JUMP_IF_FALSE), 0);
    stmt.body->accept(*this);
    emitLoop(loopStart, 0);

    patchJump(exitJump);
}

void Compiler::visitForStmt(const ForStmt& stmt) {
    beginScope();
    if (stmt.initializer) {
        stmt.initializer->accept(*this);
    }

    int loopStart = chunk.code.size();
    int exitJump = -1;
    if (stmt.condition) {
        stmt.condition->accept(*this);
        exitJump = emitJump(static_cast<uint8_t>(OpCode::OP_JUMP_IF_FALSE), 0);
    }

    stmt.body->accept(*this);
    if (stmt.increment) {
        stmt.increment->accept(*this);
        emitByte(static_cast<uint8_t>(OpCode::OP_POP), 0);
    }

    emitLoop(loopStart, 0);

    if (exitJump != -1) {
        patchJump(exitJump);
    }

    endScope();
}

} // namespace nota
