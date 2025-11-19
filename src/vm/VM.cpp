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

    for (;;) {
        uint8_t instruction;
        switch (instruction = READ_BYTE()) {
        case static_cast<uint8_t>(core::OpCode::OP_CONSTANT): {
            core::NotaValue constant = READ_CONSTANT();
            *stack_top_ = constant;
            stack_top_++;
            break;
        }
        case static_cast<uint8_t>(core::OpCode::OP_RETURN): {
            // For now, just print the top of the stack.
            stack_top_--;
            std::cout << "Result: " << stack_top_->AsInt() << std::endl;
            return InterpretResult::OK;
        }
        }
    }

#undef READ_BYTE
#undef READ_CONSTANT
}

} // namespace vm
} // namespace nota
