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

private:
    InterpretResult Run();

    core::Chunk chunk_;
    std::vector<core::NotaValue> stack_;
    core::NotaValue* stack_top_ = nullptr;
};

} // namespace vm
} // namespace nota
