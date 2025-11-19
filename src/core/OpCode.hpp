#ifndef NOTA_OPCODE_HPP
#define NOTA_OPCODE_HPP

#include <cstdint>

namespace nota {
namespace core {

enum OpCode : uint8_t {
    OP_RETURN,
    OP_CONSTANT,
};

} // namespace core
} // namespace nota

#endif // NOTA_OPCODE_HPP
