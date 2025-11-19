#pragma once

#include <cstdint>

namespace nota {
namespace core {

enum class OpCode : uint8_t {
    OP_CONSTANT,
    OP_RETURN,
};

} // namespace core
} // namespace nota
