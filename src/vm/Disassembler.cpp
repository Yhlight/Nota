#include "Disassembler.h"
#include <iostream>
#include <iomanip>

void Disassembler::disassemble_chunk(const Chunk& chunk, const std::string& name) {
    std::cout << "== " << name << " ==" << std::endl;

    for (size_t offset = 0; offset < chunk.get_code().size();) {
        offset = disassemble_instruction(chunk, offset);
    }
}

size_t Disassembler::disassemble_instruction(const Chunk& chunk, size_t offset) {
    std::cout << std::setw(4) << std::setfill('0') << offset << " ";
    if (offset > 0 && chunk.get_line(offset) == chunk.get_line(offset - 1)) {
        std::cout << "   | ";
    } else {
        std::cout << std::setw(4) << chunk.get_line(offset) << " ";
    }

    OpCode instruction = static_cast<OpCode>(chunk.get_code()[offset]);
    switch (instruction) {
        case OpCode::OP_CONSTANT:
            return constant_instruction("OP_CONSTANT", chunk, offset);
        case OpCode::OP_NEGATE:
            return simple_instruction("OP_NEGATE", offset);
        case OpCode::OP_NOT:
            return simple_instruction("OP_NOT", offset);
        case OpCode::OP_ADD:
            return simple_instruction("OP_ADD", offset);
        case OpCode::OP_SUBTRACT:
            return simple_instruction("OP_SUBTRACT", offset);
        case OpCode::OP_MULTIPLY:
            return simple_instruction("OP_MULTIPLY", offset);
        case OpCode::OP_DIVIDE:
            return simple_instruction("OP_DIVIDE", offset);
        case OpCode::OP_MODULO:
            return simple_instruction("OP_MODULO", offset);
        case OpCode::OP_EQ:
            return simple_instruction("OP_EQ", offset);
        case OpCode::OP_NEQ:
            return simple_instruction("OP_NEQ", offset);
        case OpCode::OP_GT:
            return simple_instruction("OP_GT", offset);
        case OpCode::OP_GTE:
            return simple_instruction("OP_GTE", offset);
        case OpCode::OP_LT:
            return simple_instruction("OP_LT", offset);
        case OpCode::OP_LTE:
            return simple_instruction("OP_LTE", offset);
        case OpCode::OP_NULL:
            return simple_instruction("OP_NULL", offset);
        case OpCode::OP_TRUE:
            return simple_instruction("OP_TRUE", offset);
        case OpCode::OP_FALSE:
            return simple_instruction("OP_FALSE", offset);
        case OpCode::OP_RETURN:
            return simple_instruction("OP_RETURN", offset);
        default:
            std::cout << "Unknown opcode: " << static_cast<int>(instruction) << std::endl;
            return offset + 1;
    }
}

size_t Disassembler::simple_instruction(const std::string& name, size_t offset) {
    std::cout << name << std::endl;
    return offset + 1;
}

size_t Disassembler::constant_instruction(const std::string& name, const Chunk& chunk, size_t offset) {
    uint8_t constant_index = chunk.get_code()[offset + 1];
    std::cout << name << " " << static_cast<int>(constant_index) << " '";
    // In a real implementation, you would print the value from the constants pool.
    // For now, we'll just print the index.
    std::cout << "'" << std::endl;
    return offset + 2;
}
