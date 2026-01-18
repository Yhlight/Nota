#ifndef NOTA_PARSER_HPP
#define NOTA_PARSER_HPP

#include "Token.hpp"
#include "AST/AstNodes.hpp"
#include <vector>
#include <memory>

class Parser {
public:
    Parser(const std::vector<Token>& tokens);
    std::unique_ptr<ComponentNode> parse();

private:
    const std::vector<Token>& tokens;
    size_t current = 0;

    bool isAtEnd();
    Token advance();
    Token peek();

    // Parsing helpers
    std::unique_ptr<ComponentNode> parseComponent();
    std::unique_ptr<PropertyNode> parseProperty();
    bool match(TokenType type);
    Token consume(TokenType type, const std::string& message);
};

#endif // NOTA_PARSER_HPP
