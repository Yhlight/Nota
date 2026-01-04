#ifndef NOTA_LEXER_H
#define NOTA_LEXER_H

#include "Token.h"
#include <vector>
#include <string>

namespace nota {

    class Lexer {
        std::string source;
        std::vector<Token> tokens;
        int start = 0;
        int current = 0;
        int line = 1;

        bool is_at_end();
        char advance();
        char peek();
        char peek_next();
        void add_token(TokenType type);
        void add_token(TokenType type, const std::string& literal);
        void scan_token();
        void number();
        void string();

    public:
        explicit Lexer(std::string source);
        std::vector<Token> scan_tokens();
    };

} // namespace nota

#endif //NOTA_LEXER_H
