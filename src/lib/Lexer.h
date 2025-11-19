#pragma once

#include "Token.h"
#include <string>
#include <vector>

#include <vector>

struct NotaError {
    int line;
    std::string message;
};

class Lexer {
public:
    Lexer(std::string source);
    std::vector<Token> scanTokens();
    bool hasError() const;
    const std::vector<NotaError>& getErrors() const;

private:
    std::string source;
    std::vector<Token> tokens;
    std::vector<NotaError> errors;
    int start = 0;
    int current = 0;
    int line = 1;

    void error(const std::string& message);
    bool isAtEnd();
    char advance();
    void addToken(TokenType type);
    void addToken(TokenType type, const std::variant<std::monostate, int, double, std::string, bool>& literal);
    void scanToken();
    void identifier();
    void number();
    void string();
    char peek();
    char peekNext();
    bool match(char expected);
};
