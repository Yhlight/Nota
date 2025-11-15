#include <gtest/gtest.h>
#include "../src/vm/compiler.hpp"
#include "../src/lexer.hpp"
#include "../src/parser.hpp"
#include "../src/vm/vm.hpp"

TEST(CompilerTest, CompilesLiteral) {
    std::string source = "123";
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.tokenize();
    Parser parser(tokens);
    std::vector<std::unique_ptr<Stmt>> statements = parser.parse();

    Compiler compiler;
    Chunk chunk = compiler.compile(statements);

    ASSERT_EQ(chunk.get_code().size(), 2);
    EXPECT_EQ(chunk.get_code()[0], OP_CONSTANT);
    EXPECT_EQ(chunk.get_code()[1], 0); // Index of the constant in the constant pool

    ASSERT_EQ(chunk.get_constants().size(), 1);
    EXPECT_EQ(std::get<double>(chunk.get_constants()[0]), 123);
}

TEST(VMTest, ExecutesConstant) {
    Compiler compiler;
    Chunk chunk;
    chunk.write(OP_CONSTANT, 1);
    chunk.write(chunk.add_constant(123.0), 1);
    chunk.write(OP_RETURN, 1);

    VM vm;
    InterpretResult result = vm.interpret(chunk);
    EXPECT_EQ(result, INTERPRET_OK);
}

TEST(VMTest, ExecutesArithmetic) {
    std::string source = "(5 - 3) * 4 / 2"; // Expected result: 4
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.tokenize();
    Parser parser(tokens);
    std::vector<std::unique_ptr<Stmt>> statements = parser.parse();

    Compiler compiler;
    Chunk chunk = compiler.compile(statements);
    chunk.write(OP_RETURN, 1);

    VM vm;
    testing::internal::CaptureStdout();
    InterpretResult result = vm.interpret(chunk);
    std::string output = testing::internal::GetCapturedStdout();

    EXPECT_EQ(result, INTERPRET_OK);
    EXPECT_EQ(output, "4\n");
}

TEST(CompilerTest, CompilesUnaryExpression) {
    std::string source = "-10";
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.tokenize();
    Parser parser(tokens);
    std::vector<std::unique_ptr<Stmt>> statements = parser.parse();

    Compiler compiler;
    Chunk chunk = compiler.compile(statements);

    ASSERT_EQ(chunk.get_code().size(), 3);
    EXPECT_EQ(chunk.get_code()[0], OP_CONSTANT);
    EXPECT_EQ(chunk.get_code()[2], OP_NEGATE);
}

TEST(CompilerTest, CompilesBinaryExpression) {
    std::string source = "5 + 5";
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.tokenize();
    Parser parser(tokens);
    std::vector<std::unique_ptr<Stmt>> statements = parser.parse();

    Compiler compiler;
    Chunk chunk = compiler.compile(statements);

    ASSERT_EQ(chunk.get_code().size(), 5);
    EXPECT_EQ(chunk.get_code()[0], OP_CONSTANT);
    EXPECT_EQ(chunk.get_code()[2], OP_CONSTANT);
    EXPECT_EQ(chunk.get_code()[4], OP_ADD);
}
