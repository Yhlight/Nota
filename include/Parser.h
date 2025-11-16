#ifndef NOTA_PARSER_H
#define NOTA_PARSER_H

#include "Lexer.h"

namespace nota {

    class Parser {
    public:
        Parser(Lexer& lexer);

        void parse();

    private:
        Lexer& lexer;
        Token current_token;
    };

} // namespace nota

#endif // NOTA_PARSER_H
