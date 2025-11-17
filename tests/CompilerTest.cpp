#include "gtest/gtest.h"
#include "Compiler.h"
#include "Lexer.h"
#include "Parser.h"

using namespace nota;

TEST(CompilerTest, SimpleExpression) {
    std::string source = "1 + 2\n";
    Lexer lexer(source);
    Parser parser(lexer);
    auto stmts = parser.parse();
    Compiler compiler;
    Chunk chunk = compiler.compile(stmts);

    ASSERT_EQ(chunk.code.size(), 6);
    EXPECT_EQ(chunk.code[0], (uint8_t)OpCode::Constant);
    EXPECT_EQ(chunk.code[2], (uint8_t)OpCode::Constant);
    EXPECT_EQ(chunk.code[4], (uint8_t)OpCode::Add);
    EXPECT_EQ(chunk.code[5], (uint8_t)OpCode::Return);
}
