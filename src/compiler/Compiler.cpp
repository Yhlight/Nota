#include "compiler/Compiler.hpp"
#include "lexer/Lexer.hpp"

namespace nota {
namespace compiler {

bool Compiler::Compile(const std::string& source, core::Chunk& chunk) {
    lexer::Lexer lexer(source);
    parser::Parser parser(lexer);
    compiling_chunk_ = &chunk;

    auto program = parser.ParseProgram();
    if (!parser.Errors().empty()) {
        return false;
    }

    CompileProgram(program.get());

    return true;
}

void Compiler::CompileProgram(const core::Program* program) {
    for (const auto& stmt : program->statements) {
        CompileStatement(stmt.get());
    }
}

void Compiler::CompileStatement(const core::Statement* stmt) {
    if (auto expr_stmt = dynamic_cast<const core::ExpressionStatement*>(stmt)) {
        CompileExpression(expr_stmt->expression.get());
        EmitReturn();
    }
}

void Compiler::CompileExpression(const core::Expression* expr) {
    if (auto int_lit = dynamic_cast<const core::IntegerLiteral*>(expr)) {
        core::NotaValue value(int_lit->value);
        EmitConstant(value);
    }
}

void Compiler::EmitByte(uint8_t byte) {
    compiling_chunk_->Write(byte, 0); // TODO: Pass line number
}

void Compiler::EmitReturn() {
    EmitByte(static_cast<uint8_t>(core::OpCode::OP_RETURN));
}

void Compiler::EmitConstant(core::NotaValue value) {
    EmitByte(static_cast<uint8_t>(core::OpCode::OP_CONSTANT));
    EmitByte(compiling_chunk_->AddConstant(value));
}

} // namespace compiler
} // namespace nota
