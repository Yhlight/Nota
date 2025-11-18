#include "Chunk.h"

void Chunk::write(uint8_t byte, int line) {
    code_.push_back(byte);
    lines_.push_back(line);
}

size_t Chunk::add_constant(Value value) {
    constants_.push_back(value);
    return constants_.size() - 1;
}

const std::vector<uint8_t>& Chunk::get_code() const {
    return code_;
}

const std::vector<Value>& Chunk::get_constants() const {
    return constants_;
}

int Chunk::get_line(size_t offset) const {
    return lines_[offset];
}
