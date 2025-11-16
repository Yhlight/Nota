#ifndef NOTA_TOKEN_H
#define NOTA_TOKEN_H

#include <string>

namespace nota {

    enum class TokenType {
        // End of file
        Eof,

        // Comments
        Comment,

        // Keywords
        Let,
        Mut,

        // Etc...
    };

    struct Token {
        TokenType type;
        std::string lexeme;
        int line;
        int column;
    };

} // namespace nota

#endif // NOTA_TOKEN_H
