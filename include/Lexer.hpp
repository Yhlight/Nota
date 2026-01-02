#ifndef NOTA_LEXER_HPP
#define NOTA_LEXER_HPP

#include "Token.hpp"
#include <string>
#include <vector>

namespace Nota {

class Lexer {
public:
    explicit Lexer(const std::string& source);

    std::vector<Token> ScanTokens();

private:
    void ScanToken();
    char Advance();
    bool IsAtEnd();
    bool Match(char expected);
    char Peek();
    char PeekNext();
    void HandleString();
    void HandleNumber();
    void HandleIdentifier();
    void HandleComment();

    Token MakeToken(TokenType type) const;

private:
    const std::string& source_;
    std::vector<Token> tokens_;
    int start_ = 0;
    int current_ = 0;
    int line_ = 1;
    int column_ = 1;
};

} // namespace Nota

#endif // NOTA_LEXER_HPP
