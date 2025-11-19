#ifndef NOTA_OPCODE_HPP
#define NOTA_OPCODE_HPP

#include <cstdint>

namespace nota {
namespace core {

enum OpCode : uint8_t {
    OP_RETURN,
    OP_CONSTANT,
    OP_ADD,
    OP_SUBTRACT,
    OP_MULTIPLY,
    OP_DIVIDE,
    OP_DEFINE_IMMUTABLE_GLOBAL,
    OP_DEFINE_MUTABLE_GLOBAL,
    OP_GET_GLOBAL,
    OP_SET_GLOBAL,
    OP_EQUAL,
    OP_NOT_EQUAL,
    OP_GREATER,
    OP_GREATER_EQUAL,
    OP_LESS,
    OP_LESS_EQUAL,
};

} // namespace core
} // namespace nota

#endif // NOTA_OPCODE_HPP
