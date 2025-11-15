#pragma once

#include "chunk.hpp"
#include <string>

void disassemble_chunk(Chunk& chunk, const std::string& name);
int disassemble_instruction(Chunk& chunk, int offset);
