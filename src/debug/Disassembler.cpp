#include "debug/Disassembler.hpp"
#include "core/OpCode.hpp"
#include <iostream>

namespace nota {
namespace debug {

void Disassembler::DisassembleChunk(const core::Chunk& chunk, const std::string& name) {
    std::cout << "== " << name << " ==" << std::endl;

    for (size_t offset = 0; offset < chunk.GetCode().size();) {
        offset = DisassembleInstruction(chunk, offset);
    }
}

size_t Disassembler::DisassembleInstruction(const core::Chunk& chunk, size_t offset) {
    std::cout << offset << " ";

    uint8_t instruction = chunk.GetCode()[offset];
    switch (static_cast<core::OpCode>(instruction)) {
    case core::OpCode::OP_CONSTANT:
        return ConstantInstruction("OP_CONSTANT", chunk, offset);
    case core::OpCode::OP_RETURN:
        return SimpleInstruction("OP_RETURN", offset);
    default:
        std::cout << "Unknown opcode " << instruction << std::endl;
        return offset + 1;
    }
}

size_t Disassembler::ConstantInstruction(const std::string& name, const core::Chunk& chunk, size_t offset) {
    uint8_t constant_index = chunk.GetCode()[offset + 1];
    std::cout << name << " " << static_cast<int>(constant_index) << " '";
    // Assuming AsInt for simplicity. A full implementation would handle all types.
    std::cout << chunk.GetConstants()[constant_index].AsInt();
    std::cout << "'" << std::endl;
    return offset + 2;
}

size_t Disassembler::SimpleInstruction(const std::string& name, size_t offset) {
    std::cout << name << std::endl;
    return offset + 1;
}

} // namespace debug
} // namespace nota
