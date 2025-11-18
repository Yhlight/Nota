#pragma once

#include "Chunk.h"
#include <vector>

class Object;

enum class InterpretResult {
    OK,
    COMPILE_ERROR,
    RUNTIME_ERROR
};

class VM {
public:
    VM();
    ~VM();
    InterpretResult interpret(const Chunk& chunk);

private:
    template <typename T, typename... Args>
    T* allocate_object(Args&&... args);

    InterpretResult run();

    void push(Value value);
    Value pop();

    const Chunk* chunk_;
    const uint8_t* ip_;
    std::vector<Value> stack_;
    Object* objects_ = nullptr;
};
