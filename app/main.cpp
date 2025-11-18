#include <iostream>
#include "Tokenizer.h"
#include "Parser.h"
#include "Compiler.h"
#include "VM.h"

int main() {
    std::string source = "let a = 10\nlet b = 20\nprint a + b\n";

    Tokenizer tokenizer(source);
    std::vector<Token> tokens = tokenizer.tokenize();

    Parser parser(tokens);
    std::vector<std::unique_ptr<Stmt>> statements = parser.parse();

    Compiler compiler;
    Chunk chunk = compiler.compile(statements);

    VM vm;
    vm.interpret(chunk);

    return 0;
}
