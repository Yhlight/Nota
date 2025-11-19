#include "core/Chunk.hpp"
#include "core/NotaValue.hpp"

namespace nota {
namespace core {

void Chunk::Write(uint8_t byte) {
    code.push_back(byte);
}

size_t Chunk::AddConstant(const NotaValue& value) {
    constants.push_back(value);
    return constants.size() - 1;
}

} // namespace core
} // namespace nota
