#ifndef NOTA_LEXER_HPP
#define NOTA_LEXER_HPP

#include <string>
#include <vector>

namespace nota {

    enum class TokenType {
        // ... to be filled in later
    };

    struct Token {
        TokenType type;
        std::string lexeme;
        int line;
    };

    class Lexer {
    public:
        Lexer(const std::string& source);
        std::vector<Token> scan_tokens();
    private:
        std::string source;
        std::vector<Token> tokens;
        int start = 0;
        int current = 0;
        int line = 1;
    };

} // namespace nota

#endif // NOTA_LEXER_HPP
