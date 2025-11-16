#include "Nota.h"
#include "Lexer.h"
#include "Parser.h"
#include <iostream>

Interpreter Nota::interpreter;
bool Nota::hadError = false;
bool Nota::hadRuntimeError = false;

void Nota::run(const std::string& source) {
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.scanTokens();
    Parser parser(tokens);
    std::vector<std::unique_ptr<Stmt>> statements = parser.parse();

    // Stop if there was a syntax error.
    if (hadError) return;

    interpreter.interpret(statements);
}

void Nota::error(int line, const std::string& message) {
    report(line, "", message);
}

void Nota::runtimeError(const RuntimeError& error) {
    std::cerr << error.what() << "\n[line " << error.token.line << "]" << std::endl;
    hadRuntimeError = true;
}

void Nota::report(int line, const std::string& where, const std::string& message) {
    std::cerr << "[line " << line << "] Error" << where << ": " << message << std::endl;
    hadError = true;
}
