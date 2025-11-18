#include "VM.h"
#include <iostream>

VM::VM() : chunk_(nullptr), ip_(nullptr) {}

InterpretResult VM::interpret(const Chunk& chunk) {
    chunk_ = &chunk;
    ip_ = chunk_->get_code().data();
    return run();
}

void VM::push(Value value) {
    stack_.push_back(value);
}

Value VM::pop() {
    Value value = stack_.back();
    stack_.pop_back();
    return value;
}

InterpretResult VM::run() {
#define READ_BYTE() (*ip_++)
#define READ_CONSTANT() (chunk_->get_constants()[READ_BYTE()])

    for (;;) {
        uint8_t instruction = READ_BYTE();
        switch (static_cast<OpCode>(instruction)) {
            case OpCode::OP_RETURN: {
                return InterpretResult::OK;
            }
            case OpCode::OP_CONSTANT: {
                Value constant = READ_CONSTANT();
                push(constant);
                break;
            }
            case OpCode::OP_NEGATE: {
                if (stack_.empty() || !std::holds_alternative<double>(stack_.back())) {
                    std::cerr << "Runtime error: Operand must be a number." << std::endl;
                    return InterpretResult::RUNTIME_ERROR;
                }
                double value = std::get<double>(pop());
                push(-value);
                break;
            }
            default:
                std::cerr << "Unknown opcode: " << instruction << std::endl;
                return InterpretResult::RUNTIME_ERROR;
        }
    }

#undef READ_BYTE
#undef READ_CONSTANT
}
