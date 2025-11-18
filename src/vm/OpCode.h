#pragma once

#include <cstdint>

enum class OpCode : uint8_t {
    // Constants
    OP_CONSTANT,

    // Unary operations
    OP_NEGATE,
    OP_NOT,

    // Binary operations
    OP_ADD,
    OP_SUBTRACT,
    OP_MULTIPLY,
    OP_DIVIDE,
    OP_MODULO,

    OP_EQ,
    OP_NEQ,
    OP_GT,
    OP_GTE,
    OP_LT,
    OP_LTE,

    // Literals
    OP_NULL,
    OP_TRUE,
    OP_FALSE,

    // Misc
    OP_RETURN,
};
