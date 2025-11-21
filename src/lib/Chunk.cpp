#include "Chunk.h"

namespace nota {

void Chunk::write(uint8_t byte, int line) {
    code.push_back(byte);
    lines.push_back(line);
}

void Chunk::write(uint16_t bytes, int line) {
    code.push_back((bytes >> 8) & 0xFF);
    code.push_back(bytes & 0xFF);
    lines.push_back(line);
    lines.push_back(line);
}


int Chunk::addConstant(Value value) {
    constants.push_back(value);
    return constants.size() - 1;
}

void Chunk::patch(int offset, uint16_t value) {
    code[offset] = (value >> 8) & 0xFF;
    code[offset + 1] = value & 0xFF;
}

} // namespace nota
