#ifndef NOTA_LEXER_HPP
#define NOTA_LEXER_HPP

#include <string>
#include <vector>
#include <iostream>

namespace nota {

    enum class TokenType {
        // Single-character tokens.
        LEFT_PAREN, RIGHT_PAREN, LEFT_BRACE, RIGHT_BRACE,
        LEFT_BRACKET, RIGHT_BRACKET,
        COMMA, DOT, MINUS, PLUS, SLASH, STAR, PERCENT, SEMICOLON,

        // One or two character tokens.
        BANG, BANG_EQUAL,
        EQUAL, EQUAL_EQUAL,
        GREATER, GREATER_EQUAL,
        LESS, LESS_EQUAL,
        PLUS_EQUAL, MINUS_EQUAL, STAR_EQUAL, SLASH_EQUAL, PERCENT_EQUAL,
        AND, OR,
        BITWISE_AND, BITWISE_OR, BITWISE_XOR, BITWISE_NOT,
        LEFT_SHIFT, RIGHT_SHIFT,

        // Literals.
        IDENTIFIER, STRING, NUMBER,

        // Keywords.
        CLASS, ELSE, FUNC, FOR, IF, LET, MUT,
        RETURN, WHILE, END, MATCH, DO, IMPORT, PACKAGE,
        TRUE, FALSE,

        // Misc
        COLON,
        ARROW,

        // End of file.
        EOF_TOKEN
    };

    inline std::ostream& operator<<(std::ostream& os, const TokenType& type) {
        os << static_cast<int>(type);
        return os;
    }

    struct Token {
        TokenType type;
        std::string lexeme;
        int line;

        friend std::ostream& operator<<(std::ostream& os, const Token& token) {
            os << "Token(" << token.type << ", " << token.lexeme << ", " << token.line << ")";
            return os;
        }
    };

    class Lexer {
    public:
        Lexer(const std::string& source);
        std::vector<Token> scan_tokens();
    private:
        void scan_token();
        char advance();
        void add_token(TokenType type);
        void add_token(TokenType type, const std::string& literal);
        bool match(char expected);
        char peek();
        char peek_next();
        void string();
        void number();
        void identifier();

        std::string source;
        std::vector<Token> tokens;
        int start = 0;
        int current = 0;
        int line = 1;
    };

} // namespace nota

#endif // NOTA_LEXER_HPP
