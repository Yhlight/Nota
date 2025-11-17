#ifndef NOTA_VM_H
#define NOTA_VM_H

#include "Bytecode.h"
#include "Value.h"
#include <vector>

namespace nota {

enum class InterpretResult {
    Ok,
    CompileError,
    RuntimeError
};

class VM {
public:
    InterpretResult interpret(const Chunk &chunk);

private:
    Chunk chunk;
    std::vector<Value> stack;
    uint8_t *ip;
};

} // namespace nota

#endif // NOTA_VM_H
