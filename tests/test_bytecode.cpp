#include <gtest/gtest.h>
#include "Chunk.h"
#include "Disassembler.h"
#include <sstream>

TEST(BytecodeTest, DisassemblesSimpleChunk) {
    Chunk chunk;
    size_t constant = chunk.add_constant(1.2);
    chunk.write(static_cast<uint8_t>(OpCode::OP_CONSTANT), 1);
    chunk.write(constant, 1);
    chunk.write(static_cast<uint8_t>(OpCode::OP_RETURN), 2);

    Disassembler disassembler;

    // Redirect cout to a stringstream to capture the output
    std::stringstream buffer;
    std::streambuf* old_cout = std::cout.rdbuf(buffer.rdbuf());

    disassembler.disassemble_chunk(chunk, "Test Chunk");

    // Restore cout
    std::cout.rdbuf(old_cout);

    std::string expected_output =
        "== Test Chunk ==\n"
        "0000 0001 OP_CONSTANT 0 ''\n"
        "0002 0002 OP_RETURN\n";

    EXPECT_EQ(buffer.str(), expected_output);
}
