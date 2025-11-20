#include "vendor/doctest.h"
#include "../src/lib/Chunk.h"
#include "../src/lib/VM.h"

TEST_CASE("testing the VM with OP_CONSTANT") {
    nota::Chunk chunk;
    nota::VM vm;

    int constant = chunk.addConstant(1.2);
    chunk.write(static_cast<uint8_t>(nota::OpCode::OP_CONSTANT), 1);
    chunk.write(constant, 1);

    vm.interpret(&chunk);

    CHECK(std::any_cast<double>(vm.stack.back()) == 1.2);
}

TEST_CASE("testing the VM with OP_NEGATE") {
    nota::Chunk chunk;
    nota::VM vm;

    int constant = chunk.addConstant(1.2);
    chunk.write(static_cast<uint8_t>(nota::OpCode::OP_CONSTANT), 1);
    chunk.write(constant, 1);
    chunk.write(static_cast<uint8_t>(nota::OpCode::OP_NEGATE), 1);

    vm.interpret(&chunk);

    CHECK(std::any_cast<double>(vm.stack.back()) == -1.2);
}
