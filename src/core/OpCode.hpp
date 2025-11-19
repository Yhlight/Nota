#pragma once

#include <cstdint>

namespace nota {
namespace core {

enum class OpCode : uint8_t {
    OP_CONSTANT,
    OP_ADD,
    OP_SUBTRACT,
    OP_MULTIPLY,
    OP_DIVIDE,
    OP_RETURN,
};

} // namespace core
} // namespace nota
