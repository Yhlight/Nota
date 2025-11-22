/**
 * @file Lexer.h
 * @brief The lexical analyzer for the Nota language.
 */

#pragma once

#include "Token.h"
#include <string>
#include <vector>

namespace nota {

/**
 * @class Lexer
 * @brief The lexical analyzer for the Nota language.
 */
class Lexer {
public:
    /**
     * @brief Construct a new Lexer.
     * @param source The source code to scan.
     */
    Lexer(const std::string& source);

    /**
     * @brief Scan all tokens from the source code.
     * @return A vector of tokens.
     */
    std::vector<Token> scanTokens();

    /**
     * @brief Check if the lexer encountered an error.
     * @return True if an error was encountered, false otherwise.
     */
    bool hadError() const { return hadError_; }

private:
    void scanToken();
    char advance();
    bool match(char expected);
    char peek() const;
    char peekNext() const;
    void string();
    void number();
    void identifier();
    bool isAtEnd() const;
    void addToken(TokenType type);
    void addToken(TokenType type, const std::variant<std::monostate, int, double, std::string, bool>& literal);
    bool isDigit(char c) const;
    bool isAlpha(char c) const;
    bool isAlphaNumeric(char c) const;
    void report(int line, const std::string& where, const std::string& message);

    const std::string source_;
    std::vector<Token> tokens_;
    size_t start_ = 0;
    size_t current_ = 0;
    int line_ = 1;
    bool hadError_ = false;
};

} // namespace nota
