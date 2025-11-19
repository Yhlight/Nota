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
    EmitReturn();
}

void Compiler::CompileStatement(const core::Statement* stmt) {
    if (auto expr_stmt = dynamic_cast<const core::ExpressionStatement*>(stmt)) {
        CompileExpression(expr_stmt->expression.get());
    } else if (auto let_stmt = dynamic_cast<const core::LetStatement*>(stmt)) {
        CompileExpression(let_stmt->value.get());
        uint8_t global = ParseVariable(let_stmt->name->value);
        EmitByte(static_cast<uint8_t>(core::OpCode::OP_DEFINE_GLOBAL));
        EmitByte(global);
    } else if (auto mut_stmt = dynamic_cast<const core::MutStatement*>(stmt)) {
        CompileExpression(mut_stmt->value.get());
        uint8_t global = ParseVariable(mut_stmt->name->value);
        EmitByte(static_cast<uint8_t>(core::OpCode::OP_DEFINE_GLOBAL));
        EmitByte(global);
    }
}

void Compiler::CompileExpression(const core::Expression* expr) {
    if (auto int_lit = dynamic_cast<const core::IntegerLiteral*>(expr)) {
        core::NotaValue value(int_lit->value);
        EmitConstant(value);
    } else if (auto infix_expr = dynamic_cast<const core::InfixExpression*>(expr)) {
        CompileExpression(infix_expr->left.get());
        CompileExpression(infix_expr->right.get());

        if (infix_expr->op == "+") {
            EmitByte(static_cast<uint8_t>(core::OpCode::OP_ADD));
        } else if (infix_expr->op == "-") {
            EmitByte(static_cast<uint8_t>(core::OpCode::OP_SUBTRACT));
        } else if (infix_expr->op == "*") {
            EmitByte(static_cast<uint8_t>(core::OpCode::OP_MULTIPLY));
        } else if (infix_expr->op == "/") {
            EmitByte(static_cast<uint8_t>(core::OpCode::OP_DIVIDE));
        }
    } else if (auto ident = dynamic_cast<const core::Identifier*>(expr)) {
        uint8_t global = ParseVariable(ident->value);
        EmitByte(static_cast<uint8_t>(core::OpCode::OP_GET_GLOBAL));
        EmitByte(global);
    } else if (auto assign_expr = dynamic_cast<const core::AssignmentExpression*>(expr)) {
        CompileExpression(assign_expr->value.get());
        uint8_t global = ParseVariable(assign_expr->name->value);
        EmitByte(static_cast<uint8_t>(core::OpCode::OP_SET_GLOBAL));
        EmitByte(global);
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

uint8_t Compiler::ParseVariable(const std::string& name) {
    if (globals_.count(name)) {
        // Already defined
        return globals_[name];
    }

    uint8_t index = IdentifierConstant(name);
    globals_[name] = index;
    return index;
}

uint8_t Compiler::IdentifierConstant(const std::string& name) {
    return compiling_chunk_->AddConstant(core::NotaValue(new core::StringObject(name)));
}

} // namespace compiler
} // namespace nota
