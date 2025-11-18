#pragma once

#include "Chunk.h"
#include <vector>

enum class InterpretResult {
    OK,
    COMPILE_ERROR,
    RUNTIME_ERROR
};

class VM {
public:
    VM();
    InterpretResult interpret(const Chunk& chunk);

private:
    InterpretResult run();

    void push(Value value);
    Value pop();

    const Chunk* chunk_;
    const uint8_t* ip_;
    std::vector<Value> stack_;
};
