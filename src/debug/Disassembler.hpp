#pragma once

#include "core/Chunk.hpp"
#include <string>

namespace nota {
namespace debug {

class Disassembler {
public:
    void DisassembleChunk(const core::Chunk& chunk, const std::string& name);
    size_t DisassembleInstruction(const core::Chunk& chunk, size_t offset);

private:
    size_t ConstantInstruction(const std::string& name, const core::Chunk& chunk, size_t offset);
    size_t SimpleInstruction(const std::string& name, size_t offset);
};

} // namespace debug
} // namespace nota
