#include <gtest/gtest.h>
#include "VM.h"
#include "Bytecode.h"

TEST(VM, TestSimpleAddition) {
    nota::Chunk chunk;
    chunk.constants.push_back(1LL);
    chunk.constants.push_back(2LL);
    chunk.code = {
        (uint8_t)nota::OpCode::Constant, 0,
        (uint8_t)nota::OpCode::Constant, 1,
        (uint8_t)nota::OpCode::Add,
        (uint8_t)nota::OpCode::Return
    };

    nota::VM vm;
    vm.interpret(chunk);

    EXPECT_EQ(std::get<long long>(vm.pop()), 3LL);
}
