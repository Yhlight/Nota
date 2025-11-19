#include "compiler/Compiler.hpp"
#include "parser/Parser.hpp"
#include "core/NotaValue.hpp"

namespace nota {
namespace compiler {

bool Compiler::Compile(const core::Program& program, core::Chunk& chunk) {
    compiling_chunk_ = &chunk;
    for (const auto& stmt : program.statements) {
        CompileNode(stmt.get());
    }
    compiling_chunk_->Write(core::OP_RETURN);
    return true;
}

void Compiler::CompileNode(const core::Node* node) {
    if (auto stmt = dynamic_cast<const core::ExpressionStatement*>(node)) {
        CompileNode(stmt->expression.get());
    } else if (auto literal = dynamic_cast<const core::IntegerLiteral*>(node)) {
        CompileIntegerLiteral(literal);
    } else if (auto infix = dynamic_cast<const core::InfixExpression*>(node)) {
        CompileInfixExpression(infix);
    }
}

void Compiler::CompileIntegerLiteral(const core::IntegerLiteral* literal) {
    core::NotaValue value(literal->value);
    size_t const_idx = compiling_chunk_->AddConstant(value);
    compiling_chunk_->Write(core::OP_CONSTANT);
    compiling_chunk_->Write(const_idx);
}

void Compiler::CompileInfixExpression(const core::InfixExpression* expression) {
    CompileNode(expression->left.get());
    CompileNode(expression->right.get());

    if (expression->op == "+") {
        compiling_chunk_->Write(core::OP_ADD);
    } else if (expression->op == "-") {
        compiling_chunk_->Write(core::OP_SUBTRACT);
    } else if (expression->op == "*") {
        compiling_chunk_->Write(core::OP_MULTIPLY);
    } else if (expression->op == "/") {
        compiling_chunk_->Write(core::OP_DIVIDE);
    }
}

} // namespace compiler
} // namespace nota
