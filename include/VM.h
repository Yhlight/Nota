#ifndef NOTA_VM_H
#define NOTA_VM_H

#include "Bytecode.h"
#include "Value.h"
#include <vector>
#include <map>

namespace nota {

enum class InterpretResult {
    Ok,
    CompileError,
    RuntimeError
};

class VM {
public:
    InterpretResult interpret(const Chunk &chunk);
    Value pop();
    size_t stack_size() const { return stack.size(); }

private:
    Chunk chunk;
    std::vector<Value> stack;
    std::map<std::string, Value> globals;
    uint8_t *ip;
};

} // namespace nota

#endif // NOTA_VM_H
