#include "Parser.h"

namespace nota {

    Parser::Parser(const std::vector<Token>& tokens) : tokens(tokens) {}

    bool Parser::is_at_end() {
        return peek().type == TokenType::END_OF_FILE;
    }

    Token Parser::peek() {
        return tokens[current];
    }

    Token Parser::previous() {
        return tokens[current - 1];
    }

    Token Parser::advance() {
        if (!is_at_end()) current++;
        return previous();
    }

    bool Parser::check(TokenType type) {
        if (is_at_end()) return false;
        return peek().type == type;
    }

    bool Parser::match(std::vector<TokenType> types) {
        for (TokenType type : types) {
            if (check(type)) {
                advance();
                return true;
            }
        }
        return false;
    }

    Token Parser::consume(TokenType type, const std::string& message) {
        if (check(type)) return advance();
        // In a real parser, we would throw an error here.
        // For now, we'll just return the current token.
        return peek();
    }

    std::unique_ptr<ast::Stmt> Parser::declaration() {
        if (match({TokenType::IDENTIFIER})) {
            Token name = previous();
            consume(TokenType::LEFT_BRACE, "Expect '{' after component name.");
            // For now, we don't parse the body.
            std::vector<std::unique_ptr<ast::Stmt>> body;
            consume(TokenType::RIGHT_BRACE, "Expect '}' after component body.");
            return std::make_unique<ast::ComponentStmt>(name, std::move(body));
        }
        // In a real parser, we would handle other declaration types here.
        return nullptr;
    }


    std::vector<std::unique_ptr<ast::Stmt>> Parser::parse() {
        std::vector<std::unique_ptr<ast::Stmt>> statements;
        while (!is_at_end()) {
            statements.push_back(declaration());
        }
        return statements;
    }

} // namespace nota
