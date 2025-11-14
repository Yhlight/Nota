#ifndef NOTA_LEXER_HPP
#define NOTA_LEXER_HPP

#include "Token.hpp"
#include <string>
#include <vector>

namespace nota {

    class Lexer {
    public:
        Lexer(const std::string& source);

        std::vector<Token> scanTokens();

    private:
        void scanToken();
        char advance();
        bool isAtEnd();
        char peek();
        char peekNext();
        void number();
        void identifier();

        std::string m_source;
        std::vector<Token> m_tokens;
        int m_start = 0;
        int m_current = 0;
        int m_line = 1;
        int m_column = 1;
    };

} // namespace nota

#endif // NOTA_LEXER_HPP
