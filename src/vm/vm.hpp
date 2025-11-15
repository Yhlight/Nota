#ifndef NOTA_VM_HPP
#define NOTA_VM_HPP

#include "chunk.hpp"
#include "value.hpp"
#include <vector>
#include <unordered_map>
#include <string>
#include <memory>

namespace nota {

    enum class InterpretResult {
        OK,
        COMPILE_ERROR,
        RUNTIME_ERROR
    };

    class VM {
    public:
        VM();
        InterpretResult interpret(const std::string& source);

    private:
        InterpretResult run();
        void push(Value value);
        Value pop();
        Value peek(int distance);

        std::unique_ptr<Chunk> chunk;
        const uint8_t* ip;
        std::vector<Value> stack;
        std::unordered_map<std::string, Value> globals;
    };

} // namespace nota

#endif // NOTA_VM_HPP
