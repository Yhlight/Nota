#ifndef NOTA_PARSER_HPP
#define NOTA_PARSER_HPP

#include "lexer.hpp"
#include <vector>

namespace nota {

    class Parser {
    public:
        Parser(const std::vector<Token>& tokens);
        void parse();
    private:
        std::vector<Token> tokens;
        int current = 0;
    };

} // namespace nota

#endif // NOTA_PARSER_HPP
