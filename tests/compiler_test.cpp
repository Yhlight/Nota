#include <gtest/gtest.h>
#include "vm/compiler.hpp"
#include "lexer.hpp"
#include "parser.hpp"
#include "vm/chunk.hpp"

using namespace nota;

// Helper function to compile source and get the chunk
std::unique_ptr<Chunk> compileSource(const std::string& source) {
    Lexer lexer(source);
    std::vector<Token> tokens;
    for (Token token = lexer.scanToken(); token.type != TokenType::END_OF_FILE; token = lexer.scanToken()) {
        tokens.push_back(token);
    }
    tokens.push_back(lexer.scanToken());

    Parser parser(tokens);
    auto statements = parser.parse();

    Compiler compiler;
    return compiler.compile(statements);
}

TEST(CompilerTest, SimpleArithmetic) {
    std::string source = "1 + 2 * 3";
    auto chunk = compileSource(source);

    const auto& code = chunk->getCode();
    const auto& constants = chunk->getConstants();

    ASSERT_EQ(code.size(), 10);
    EXPECT_EQ(code[0], (uint8_t)OpCode::OP_CONSTANT);
    EXPECT_EQ(code[2], (uint8_t)OpCode::OP_CONSTANT);
    EXPECT_EQ(code[4], (uint8_t)OpCode::OP_CONSTANT);
    EXPECT_EQ(code[6], (uint8_t)OpCode::OP_MULTIPLY);
    EXPECT_EQ(code[7], (uint8_t)OpCode::OP_ADD);
    EXPECT_EQ(code[8], (uint8_t)OpCode::OP_POP);
    EXPECT_EQ(code[9], (uint8_t)OpCode::OP_RETURN);
}

TEST(CompilerTest, VariableDeclaration) {
    std::string source = "let a = 10";
    auto chunk = compileSource(source);

    const auto& code = chunk->getCode();
    const auto& constants = chunk->getConstants();

    ASSERT_EQ(code.size(), 3);
    EXPECT_EQ(code[0], (uint8_t)OpCode::OP_CONSTANT);
    EXPECT_EQ(code[2], (uint8_t)OpCode::OP_RETURN);
}
