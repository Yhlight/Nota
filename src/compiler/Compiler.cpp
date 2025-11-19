#include "compiler/Compiler.hpp"
#include "parser/Parser.hpp"
#include "core/NotaValue.hpp"
#include "core/NotaObject.hpp"

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
    } else if (auto let_stmt = dynamic_cast<const core::LetStatement*>(node)) {
        CompileLetStatement(let_stmt);
    } else if (auto mut_stmt = dynamic_cast<const core::MutStatement*>(node)) {
        CompileMutStatement(mut_stmt);
    } else if (auto literal = dynamic_cast<const core::IntegerLiteral*>(node)) {
        CompileIntegerLiteral(literal);
    } else if (auto literal = dynamic_cast<const core::FloatLiteral*>(node)) {
        CompileFloatLiteral(literal);
    } else if (auto literal = dynamic_cast<const core::BooleanLiteral*>(node)) {
        CompileBooleanLiteral(literal);
    } else if (auto infix = dynamic_cast<const core::InfixExpression*>(node)) {
        CompileInfixExpression(infix);
    } else if (auto ident = dynamic_cast<const core::Identifier*>(node)) {
        CompileIdentifier(ident);
    }
}

void Compiler::CompileIntegerLiteral(const core::IntegerLiteral* literal) {
    core::NotaValue value(literal->value);
    size_t const_idx = compiling_chunk_->AddConstant(value);
    compiling_chunk_->Write(core::OP_CONSTANT);
    compiling_chunk_->Write(const_idx);
}

void Compiler::CompileFloatLiteral(const core::FloatLiteral* literal) {
    core::NotaValue value(literal->value);
    size_t const_idx = compiling_chunk_->AddConstant(value);
    compiling_chunk_->Write(core::OP_CONSTANT);
    compiling_chunk_->Write(const_idx);
}

void Compiler::CompileBooleanLiteral(const core::BooleanLiteral* literal) {
    core::NotaValue value(literal->value);
    size_t const_idx = compiling_chunk_->AddConstant(value);
    compiling_chunk_->Write(core::OP_CONSTANT);
    compiling_chunk_->Write(const_idx);
}

void Compiler::CompileInfixExpression(const core::InfixExpression* expression) {
    if (expression->op == "=") {
        CompileAssignmentExpression(expression);
        return;
    }

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
    } else if (expression->op == "==") {
        compiling_chunk_->Write(core::OP_EQUAL);
    } else if (expression->op == "!=") {
        compiling_chunk_->Write(core::OP_NOT_EQUAL);
    } else if (expression->op == "<") {
        compiling_chunk_->Write(core::OP_LESS);
    } else if (expression->op == "<=") {
        compiling_chunk_->Write(core::OP_LESS_EQUAL);
    } else if (expression->op == ">") {
        compiling_chunk_->Write(core::OP_GREATER);
    } else if (expression->op == ">=") {
        compiling_chunk_->Write(core::OP_GREATER_EQUAL);
    }
}

void Compiler::CompileAssignmentExpression(const core::InfixExpression* expression) {
    CompileNode(expression->right.get());
    auto ident = dynamic_cast<const core::Identifier*>(expression->left.get());
    if (!ident) {
        // Handle error: left side of assignment must be an identifier
        return;
    }
    core::NotaValue value(ident->value);
    size_t const_idx = compiling_chunk_->AddConstant(value);
    compiling_chunk_->Write(core::OP_SET_GLOBAL);
    compiling_chunk_->Write(const_idx);
}

void Compiler::CompileLetStatement(const core::LetStatement* stmt) {
    CompileNode(stmt->value.get());
    core::NotaValue value(stmt->name->value);
    size_t const_idx = compiling_chunk_->AddConstant(value);
    compiling_chunk_->Write(core::OP_DEFINE_IMMUTABLE_GLOBAL);
    compiling_chunk_->Write(const_idx);
}

void Compiler::CompileMutStatement(const core::MutStatement* stmt) {
    CompileNode(stmt->value.get());
    core::NotaValue value(stmt->name->value);
    size_t const_idx = compiling_chunk_->AddConstant(value);
    compiling_chunk_->Write(core::OP_DEFINE_MUTABLE_GLOBAL);
    compiling_chunk_->Write(const_idx);
}

void Compiler::CompileIdentifier(const core::Identifier* ident) {
    core::NotaValue value(ident->value);
    size_t const_idx = compiling_chunk_->AddConstant(value);
    compiling_chunk_->Write(core::OP_GET_GLOBAL);
    compiling_chunk_->Write(const_idx);
}

} // namespace compiler
} // namespace nota
