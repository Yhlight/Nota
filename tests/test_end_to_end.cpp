#include <gtest/gtest.h>
#include "Compiler.h"
#include "Tokenizer.h"
#include "Parser.h"
#include "VM.h"
#include <sstream>

TEST(EndToEndTest, CompilesAndRunsSimpleProgram) {
    std::string source = "let a = 10\nprint a\n";
    Tokenizer tokenizer(source);
    std::vector<Token> tokens = tokenizer.tokenize();
    Parser parser(tokens);
    std::vector<std::unique_ptr<Stmt>> statements = parser.parse();
    Compiler compiler;
    Chunk chunk = compiler.compile(statements);

    // Redirect cout to a stringstream to capture the output
    std::stringstream buffer;
    std::streambuf* old_cout = std::cout.rdbuf(buffer.rdbuf());

    VM vm;
    InterpretResult result = vm.interpret(chunk);

    // Restore cout
    std::cout.rdbuf(old_cout);

    EXPECT_EQ(result, InterpretResult::OK);
    EXPECT_EQ(buffer.str(), "10\n");
}
