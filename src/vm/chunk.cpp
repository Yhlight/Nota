#include "chunk.hpp"

namespace nota {

    void Chunk::write(uint8_t byte, int line) {
        code.push_back(byte);
        lines.push_back(line);
    }

    size_t Chunk::addConstant(const LiteralValue& value) {
        constants.push_back(value);
        return constants.size() - 1;
    }

    int Chunk::getLine(size_t offset) const {
        // This is a simple implementation. A more efficient implementation
        // would use run-length encoding.
        return lines[offset];
    }

} // namespace nota
