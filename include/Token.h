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
        Let, Mut, If, Else, Match, While, For, Do, Func, Class, Import, As, Package,
        Return, True, False, End,

        // Identifiers
        Identifier,

        // Literals
        Integer, Float, String,

        // Operators
        Plus, Minus, Star, Slash, Percent,
        Equal, EqualEqual, Bang, BangEqual,
        Greater, GreaterEqual, Less, LessEqual,
        Ampersand, Pipe, Caret, Tilde, LessLess, GreaterGreater,
        PlusEqual, MinusEqual, StarEqual, SlashEqual, PercentEqual,
        AmpersandAmpersand, PipePipe,
        PlusPlus, MinusMinus,

        // Punctuation
        Newline, Colon, LeftParen, RightParen, LeftBrace, RightBrace,
        LeftBracket, RightBracket, Comma, Dot,

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
