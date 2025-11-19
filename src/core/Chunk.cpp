#include "core/Chunk.hpp"

namespace nota {
namespace core {

void Chunk::Write(uint8_t byte, int line) {
    code_.push_back(byte);
    lines_.push_back(line);
}

size_t Chunk::AddConstant(NotaValue value) {
    constants_.push_back(value);
    return constants_.size() - 1;
}

int Chunk::GetLine(size_t offset) const {
    // This is a simple implementation. A more efficient one would use run-length encoding.
    return lines_[offset];
}

} // namespace core
} // namespace nota
