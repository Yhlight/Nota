#include "vm/VM.hpp"
#include "compiler/Compiler.hpp"
#include <iostream>

namespace nota {
namespace vm {

InterpretResult VM::Interpret(const std::string& source) {
    compiler::Compiler compiler;
    if (!compiler.Compile(source, chunk_)) {
        return InterpretResult::COMPILE_ERROR;
    }

    stack_.resize(256); // Pre-allocate stack
    stack_top_ = &stack_[0];

    return Run();
}

InterpretResult VM::Run() {
    auto ip = chunk_.GetCode().data();

#define READ_BYTE() (*ip++)
#define READ_CONSTANT() (chunk_.GetConstants()[READ_BYTE()])
#define BINARY_OP(op) \
    do { \
        auto b = (stack_top_ - 1)->AsInt(); \
        auto a = (stack_top_ - 2)->AsInt(); \
        stack_top_ -= 2; \
        *stack_top_ = core::NotaValue(a op b); \
        stack_top_++; \
    } while (false)

    for (;;) {
        uint8_t instruction;
        switch (instruction = READ_BYTE()) {
        case static_cast<uint8_t>(core::OpCode::OP_CONSTANT): {
            core::NotaValue constant = READ_CONSTANT();
            *stack_top_ = constant;
            stack_top_++;
            break;
        }
        case static_cast<uint8_t>(core::OpCode::OP_ADD):      BINARY_OP(+); break;
        case static_cast<uint8_t>(core::OpCode::OP_SUBTRACT): BINARY_OP(-); break;
        case static_cast<uint8_t>(core::OpCode::OP_MULTIPLY): BINARY_OP(*); break;
        case static_cast<uint8_t>(core::OpCode::OP_DIVIDE):   BINARY_OP(/); break;
        case static_cast<uint8_t>(core::OpCode::OP_RETURN): {
            stack_top_--;
            this->last_popped_ = *stack_top_;
            return InterpretResult::OK;
        }
        }
    }

#undef READ_BYTE
#undef READ_CONSTANT
#undef BINARY_OP
}

core::NotaValue VM::GetLastPopped() const {
    return this->last_popped_;
}

} // namespace vm
} // namespace nota
