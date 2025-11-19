#pragma once

#include "core/Chunk.hpp"
#include <string>

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
    core::Chunk chunk_;
};

} // namespace vm
} // namespace nota
