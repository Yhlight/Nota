#include <gtest/gtest.h>
#include "VM.h"

TEST(VMTest, ExecutesSimpleBytecode) {
    Chunk chunk;
    size_t constant = chunk.add_constant(1.2);
    chunk.write(static_cast<uint8_t>(OpCode::OP_CONSTANT), 1);
    chunk.write(constant, 1);
    chunk.write(static_cast<uint8_t>(OpCode::OP_NEGATE), 1);
    chunk.write(static_cast<uint8_t>(OpCode::OP_RETURN), 2);

    VM vm;
    InterpretResult result = vm.interpret(chunk);

    EXPECT_EQ(result, InterpretResult::OK);
}
