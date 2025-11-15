#include "chunk.hpp"

void Chunk::write(uint8_t byte, int line) {
    code.push_back(byte);
    lines.push_back(line);
}

int Chunk::add_constant(Value value) {
    constants.push_back(value);
    return constants.size() - 1;
}

std::vector<uint8_t>& Chunk::get_code() {
    return code;
}

std::vector<Value>& Chunk::get_constants() {
    return constants;
}

std::vector<int>& Chunk::get_lines() {
    return lines;
}
