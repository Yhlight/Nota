#include "debug.hpp"
#include <iostream>

static int simple_instruction(const std::string& name, int offset) {
    std::cout << name << std::endl;
    return offset + 1;
}

static int constant_instruction(const std::string& name, Chunk& chunk, int offset) {
    uint8_t constant_index = chunk.get_code()[offset + 1];
    std::cout << name << " ";
    print_value(chunk.get_constants()[constant_index]);
    std::cout << std::endl;
    return offset + 2;
}

void disassemble_chunk(Chunk& chunk, const std::string& name) {
    std::cout << "== " << name << " ==" << std::endl;
    for (int offset = 0; offset < chunk.get_code().size();) {
        offset = disassemble_instruction(chunk, offset);
    }
}

int disassemble_instruction(Chunk& chunk, int offset) {
    std::cout << offset << " ";
    if (offset > 0 && chunk.get_lines()[offset] == chunk.get_lines()[offset - 1]) {
        std::cout << "   | ";
    } else {
        std::cout << chunk.get_lines()[offset] << " ";
    }

    uint8_t instruction = chunk.get_code()[offset];
    switch (instruction) {
        case OP_CONSTANT:
            return constant_instruction("OP_CONSTANT", chunk, offset);
        case OP_RETURN:
            return simple_instruction("OP_RETURN", offset);
        default:
            std::cout << "Unknown opcode " << instruction << std::endl;
            return offset + 1;
    }
}
