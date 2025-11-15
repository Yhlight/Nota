#pragma once

#include "chunk.hpp"
#include "value.hpp"
#include <vector>

#define STACK_MAX 256

enum InterpretResult {
    INTERPRET_OK,
    INTERPRET_COMPILE_ERROR,
    INTERPRET_RUNTIME_ERROR
};

class VM {
public:
    VM();
    InterpretResult interpret(Chunk& chunk);
    void push(Value value);
    Value pop();

private:
    InterpretResult run();
    Chunk* chunk;
    uint8_t* ip;
    Value stack[STACK_MAX];
    Value* stack_top;
};
