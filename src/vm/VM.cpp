#include "vm/VM.hpp"
#include "compiler/Compiler.hpp"
#include "parser/Parser.hpp"
#include "lexer/Lexer.hpp"

#define BINARY_OP(op) \
    do { \
        core::NotaValue b = Pop(); \
        core::NotaValue a = Pop(); \
        stack_.push_back(core::NotaValue(a.AsInt() op b.AsInt())); \
    } while (false)

namespace nota {
namespace vm {

VM::InterpretResult VM::Interpret(const std::string& source) {
    lexer::Lexer l(source);
    parser::Parser p(l);
    auto program = p.ParseProgram();
    if (!p.Errors().empty()) {
        return INTERPRET_COMPILE_ERROR;
    }

    core::Chunk chunk;
    compiler::Compiler compiler;
    if (!compiler.Compile(*program, chunk)) {
        return INTERPRET_COMPILE_ERROR;
    }

    chunk_ = &chunk;
    ip_ = &chunk_->code[0];

    for (;;) {
        uint8_t instruction = *ip_++;
        switch (instruction) {
            case core::OP_CONSTANT: {
                core::NotaValue constant = chunk_->constants[*ip_++];
                stack_.push_back(constant);
                break;
            }
            case core::OP_ADD:      BINARY_OP(+); break;
            case core::OP_SUBTRACT: BINARY_OP(-); break;
            case core::OP_MULTIPLY: BINARY_OP(*); break;
            case core::OP_DIVIDE:   BINARY_OP(/); break;
            case core::OP_RETURN: {
                return INTERPRET_OK;
            }
        }
    }
}

core::NotaValue VM::Pop() {
    core::NotaValue value = stack_.back();
    stack_.pop_back();
    return value;
}

} // namespace vm
} // namespace nota
