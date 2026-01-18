#ifndef NOTA_PARSER_HPP
#define NOTA_PARSER_HPP

#include "AST.hpp"
#include "Token.hpp"
#include "ComponentRegistry.hpp"
#include <vector>
#include <memory>

class Parser {
public:
    Parser(const std::vector<Token>& tokens);
    std::unique_ptr<Program> parse();

private:
    const std::vector<Token>& tokens;
    int current = 0;

    std::unique_ptr<Statement> statement();
    std::unique_ptr<Statement> componentStatement();
    std::unique_ptr<Statement> propertyStatement();
    std::unique_ptr<Statement> componentDefinitionStatement();
    std::unique_ptr<Expression> expression();

    bool isAtEnd();
    Token peek();
    Token previous();
    Token advance();
    bool check(TokenType type);
    bool match(const std::vector<TokenType>& types);
    Token consume(TokenType type, const std::string& message);
};

#endif // NOTA_PARSER_HPP
