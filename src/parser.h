#ifndef NOTA_PARSER_H
#define NOTA_PARSER_H

#include "lexer.h"
#include "ast.h"
#include <memory>

enum Precedence {
    LOWEST,
    EQUALS,
    LESSGREATER,
    SUM,
    PRODUCT,
    PREFIX,
    CALL,
    INDEX
};

class Parser {
public:
    Parser(Lexer& lexer);
    std::unique_ptr<Program> parseProgram();

private:
    std::unique_ptr<Statement> parseStatement();
    std::unique_ptr<LetStatement> parseLetStatement();
    std::unique_ptr<Expression> parseExpression(Precedence precedence);

    void nextToken();
    bool currentTokenIs(TokenType type);
    bool peekTokenIs(TokenType type);
    bool expectPeek(TokenType type);

    Lexer& lexer;
    Token currentToken;
    Token peekToken;
};

#endif //NOTA_PARSER_H
