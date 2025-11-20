#ifndef NOTA_CHUNK_H
#define NOTA_CHUNK_H

#include "Value.h"
#include <vector>
#include <cstdint>

namespace nota {

enum class OpCode {
    OP_RETURN,
    OP_CONSTANT,
    OP_NEGATE,
    OP_ADD,
    OP_SUBTRACT,
    OP_MULTIPLY,
    OP_DIVIDE,
    OP_NIL,
    OP_TRUE,
    OP_FALSE,
    OP_NOT,
    OP_EQUAL,
    OP_GREATER,
    OP_LESS,
    OP_DEFINE_GLOBAL,
    OP_GET_GLOBAL,
    OP_SET_GLOBAL
};

class Chunk {
public:
    void write(uint8_t byte, int line);
    int addConstant(Value value);

    std::vector<uint8_t> code;
    std::vector<Value> constants;
    std::vector<int> lines;
};

} // namespace nota

#endif // NOTA_CHUNK_H
