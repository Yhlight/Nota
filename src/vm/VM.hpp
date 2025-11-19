#pragma once

#include "core/Chunk.hpp"
#include "core/NotaValue.hpp"
#include <string>
#include <vector>

namespace nota {
namespace vm {

enum class InterpretResult {
    OK,
    COMPILE_ERROR,
    RUNTIME_ERROR,
};

class VM {
public:
    InterpretResult Interpret(const std::string& source);
    core::NotaValue GetLastPopped() const;

private:
    InterpretResult Run();

    core::Chunk chunk_;
    std::vector<core::NotaValue> stack_;
    core::NotaValue* stack_top_ = nullptr;
    core::NotaValue last_popped_;
};

} // namespace vm
} // namespace nota
