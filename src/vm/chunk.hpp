#ifndef NOTA_CHUNK_HPP
#define NOTA_CHUNK_HPP

#include <vector>
#include <cstdint>
#include "../ast.hpp" // For LiteralValue

namespace nota {

    enum class OpCode : uint8_t {
        OP_RETURN,
        OP_POP,
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
    };

    class Chunk {
    public:
        void write(uint8_t byte, int line);
        size_t addConstant(const LiteralValue& value);

        const std::vector<uint8_t>& getCode() const { return code; }
        const std::vector<LiteralValue>& getConstants() const { return constants; }
        int getLine(size_t offset) const;

    private:
        std::vector<uint8_t> code;
        std::vector<LiteralValue> constants;
        std::vector<int> lines;
    };

} // namespace nota

#endif // NOTA_CHUNK_HPP
