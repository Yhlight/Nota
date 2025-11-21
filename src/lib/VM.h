#ifndef NOTA_VM_H
#define NOTA_VM_H

#include "Chunk.h"
#include "Value.h"
#include <vector>
#include <string>
#include <unordered_map>
#include <memory>

namespace nota {

class NotaFunction;

struct CallFrame {
    std::shared_ptr<NotaFunction> function;
    int ip;
    int stackTop;
};

enum class InterpretResult {
    INTERPRET_OK,
    INTERPRET_COMPILE_ERROR,
    INTERPRET_RUNTIME_ERROR
};

class VM {
public:
    VM();
    InterpretResult interpret(const std::string& source);

    std::vector<Value> stack;
    std::unordered_map<std::string, Value> globals;
    Value lastPopped;
private:
    InterpretResult run();
    void runtimeError(const std::string& message);
    void resetStack();
    void push(Value value);
    Value pop();
    bool call(std::shared_ptr<NotaFunction> function, int argCount);

    std::vector<CallFrame> callStack;
};

} // namespace nota

#endif // NOTA_VM_H
