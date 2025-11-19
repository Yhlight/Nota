#ifndef NOTA_VM_HPP
#define NOTA_VM_HPP

#include "core/Chunk.hpp"
#include "core/NotaValue.hpp"
#include <vector>
#include <string>

namespace nota {
namespace vm {

class VM {
public:
    enum InterpretResult {
        INTERPRET_OK,
        INTERPRET_COMPILE_ERROR,
        INTERPRET_RUNTIME_ERROR
    };

    InterpretResult Interpret(const std::string& source);
    core::NotaValue Pop();

private:
    core::Chunk* chunk_ = nullptr;
    uint8_t* ip_ = nullptr;
    std::vector<core::NotaValue> stack_;
};

} // namespace vm
} // namespace nota

#endif // NOTA_VM_HPP
