#include "parser.h"
#include <map>

Parser::Parser(Lexer& lexer) : lexer(lexer) {
    nextToken();
    nextToken();
}

std::unique_ptr<Program> Parser::parseProgram() {
    auto program = std::make_unique<Program>();

    while (currentToken.type != TokenType::END_OF_FILE) {
        auto stmt = parseStatement();
        if (stmt) {
            program->statements.push_back(std::move(stmt));
        }
    }

    return program;
}

void Parser::nextToken() {
    currentToken = peekToken;
    peekToken = lexer.nextToken();
}

bool Parser::currentTokenIs(TokenType type) {
    return currentToken.type == type;
}

bool Parser::peekTokenIs(TokenType type) {
    return peekToken.type == type;
}

bool Parser::expectPeek(TokenType type) {
    if (peekTokenIs(type)) {
        nextToken();
        return true;
    } else {
        return false;
    }
}

std::unique_ptr<Statement> Parser::parseStatement() {
    switch (currentToken.type) {
        case TokenType::LET:
        case TokenType::MUT:
            return parseLetStatement();
        default:
            nextToken();
            return nullptr;
    }
}

std::unique_ptr<LetStatement> Parser::parseLetStatement() {
    auto stmt = std::make_unique<LetStatement>();
    stmt->token = currentToken;

    if (!expectPeek(TokenType::IDENTIFIER)) {
        return nullptr;
    }

    stmt->name = currentToken;

    if (peekTokenIs(TokenType::COLON)) {
        nextToken(); // consume identifier
        nextToken(); // consume ':'
        stmt->type = std::make_unique<Type>();
        stmt->type->token = currentToken;
        stmt->type->name = currentToken.literal;
    }

    if (!expectPeek(TokenType::ASSIGN)) {
        return nullptr;
    }

    nextToken(); // Move past the '='

    stmt->value = parseExpression(LOWEST);

    // In Nota, newlines are the statement terminators.
    // We'll advance tokens until we find the next one.
    if (peekTokenIs(TokenType::SEMICOLON)) { // Supporting for debugging/testing
        nextToken();
    }


    return stmt;
}

std::unique_ptr<Expression> Parser::parseExpression(Precedence precedence) {
    // For now, we only support literals
    if (currentToken.type == TokenType::INTEGER || currentToken.type == TokenType::FLOAT || currentToken.type == TokenType::STRING || currentToken.type == TokenType::BOOL) {
        auto literal = std::make_unique<ExpressionLiteral>();
        literal->token = currentToken;
        nextToken();
        return literal;
    }

    return nullptr;
}
