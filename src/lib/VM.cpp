#include "VM.h"
#include <iostream>
#include <cstdint>

namespace nota {

VM::VM() {
    resetStack();
}

void VM::resetStack() {
    stack.clear();
    ip = 0;
}

void VM::push(Value value) {
    stack.push_back(value);
}

Value VM::pop() {
    Value value = stack.back();
    stack.pop_back();
    return value;
}

InterpretResult VM::interpret(Chunk* chunk) {
    this->chunk = chunk;
    this->ip = 0;

    for (;;) {
        if (ip >= chunk->code.size()) {
            return InterpretResult::INTERPRET_OK;
        }
        uint8_t instruction = chunk->code[ip++];
        switch (static_cast<OpCode>(instruction)) {
            case OpCode::OP_RETURN: {
                printValue(pop());
                std::cout << std::endl;
                return InterpretResult::INTERPRET_OK;
            }
            case OpCode::OP_CONSTANT: {
                Value constant = chunk->constants[chunk->code[ip++]];
                push(constant);
                break;
            }
            case OpCode::OP_NEGATE: {
                if (stack.back().type() != typeid(double)) {
                    // runtimeError("Operand must be a number.");
                    return InterpretResult::INTERPRET_RUNTIME_ERROR;
                }
                double value = std::any_cast<double>(pop());
                push(-value);
                break;
            }
            // Other opcodes will be implemented later
        }
    }
}

} // namespace nota
