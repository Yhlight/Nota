#pragma once

#include "Chunk.h"
#include <vector>
#include <unordered_map>
#include <string>

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
    Value peek(int distance = 0);

    const Chunk* chunk_;
    const uint8_t* ip_;
    std::vector<Value> stack_;
    std::unordered_map<std::string, Value> globals_;
};
