#ifndef NOTA_BYTECODE_H
#define NOTA_BYTECODE_H

#include "Value.h"
#include <vector>
#include <cstdint>

namespace nota {

enum class OpCode : uint8_t {
    Return,
    Constant,
    Negate,
    Add,
    Subtract,
    Multiply,
    Divide,
    True,
    False,
    Not,
    Equal,
    NotEqual,
    Greater,
    Less,
    GreaterEqual,
    LessEqual,
};

struct Chunk {
    std::vector<uint8_t> code;
    std::vector<Value> constants;
};

} // namespace nota

#endif // NOTA_BYTECODE_H
