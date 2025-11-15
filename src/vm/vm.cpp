#include "vm.hpp"
#include "debug.hpp"
#include <iostream>

VM::VM() {
    stack_top = stack;
}

InterpretResult VM::run() {
#define READ_BYTE() (*ip++)
#define READ_CONSTANT() (chunk->get_constants()[READ_BYTE()])

    for (;;) {
#ifdef DEBUG_TRACE_EXECUTION
        disassemble_instruction(*chunk, (int)(ip - &chunk->get_code()[0]));
#endif
        uint8_t instruction;
        switch (instruction = READ_BYTE()) {
            case OP_CONSTANT: {
                Value constant = READ_CONSTANT();
                push(constant);
                break;
            }
            case OP_RETURN: {
                print_value(pop());
                std::cout << std::endl;
                return INTERPRET_OK;
            }
        }
    }

#undef READ_BYTE
#undef READ_CONSTANT
}

InterpretResult VM::interpret(Chunk& chunk) {
    this->chunk = &chunk;
    this->ip = &this->chunk->get_code()[0];
    return run();
}

void VM::push(Value value) {
    *stack_top = value;
    stack_top++;
}

Value VM::pop() {
    stack_top--;
    return *stack_top;
}
