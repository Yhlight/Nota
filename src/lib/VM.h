#ifndef NOTA_VM_H
#define NOTA_VM_H

#include "Chunk.h"
#include "Value.h"
#include <vector>

namespace nota {

enum class InterpretResult {
    INTERPRET_OK,
    INTERPRET_COMPILE_ERROR,
    INTERPRET_RUNTIME_ERROR
};

class VM {
public:
    VM();
    InterpretResult interpret(Chunk* chunk);

    std::vector<Value> stack;
private:
    void resetStack();
    void push(Value value);
    Value pop();

    Chunk* chunk;
    int ip; // Instruction pointer
};

} // namespace nota

#endif // NOTA_VM_H
