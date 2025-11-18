#pragma once

#include "Chunk.h"
#include <string>

class Disassembler {
public:
    void disassemble_chunk(const Chunk& chunk, const std::string& name);
    size_t disassemble_instruction(const Chunk& chunk, size_t offset);

private:
    size_t simple_instruction(const std::string& name, size_t offset);
    size_t constant_instruction(const std::string& name, const Chunk& chunk, size_t offset);
};
