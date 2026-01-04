#include "Parser.h"
#include "AST/Expr.h"

namespace nota {

    Parser::Parser(const std::vector<Token>& tokens) : tokens(tokens) {}

    bool Parser::is_at_end() { return peek().type == TokenType::END_OF_FILE; }
    Token Parser::peek() { return tokens[current]; }
    Token Parser::previous() { return tokens[current - 1]; }
    Token Parser::advance() { if (!is_at_end()) current++; return previous(); }
    bool Parser::check(TokenType type) { if (is_at_end()) return false; return peek().type == type; }

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
        // Proper error handling will be added later.
        return peek();
    }

    std::unique_ptr<ast::Expr> Parser::primary() {
        if (match({TokenType::NUMBER, TokenType::STRING})) {
            return std::make_unique<ast::LiteralExpr>(previous());
        }
        // Proper error handling will be added later.
        return nullptr;
    }

    std::unique_ptr<ast::Expr> Parser::expression() {
        return primary();
    }

    std::unique_ptr<ast::Stmt> Parser::propertyDeclaration() {
        Token name = previous();
        consume(TokenType::COLON, "Expect ':' after property name.");
        std::unique_ptr<ast::Expr> value = expression();
        return std::make_unique<ast::PropertyStmt>(name, std::move(value));
    }

    std::unique_ptr<ast::Stmt> Parser::statement() {
        if (match({TokenType::IDENTIFIER})) {
            // This could be a component or a property. For now, assume property.
            return propertyDeclaration();
        }
        // Handle other statement types later.
        return nullptr;
    }

    std::unique_ptr<ast::Stmt> Parser::declaration() {
        if (check(TokenType::IDENTIFIER) && tokens[current + 1].type == TokenType::LEFT_BRACE) {
             Token name = advance();
             consume(TokenType::LEFT_BRACE, "Expect '{' after component name.");
             std::vector<std::unique_ptr<ast::Stmt>> body;
             while (!check(TokenType::RIGHT_BRACE) && !is_at_end()) {
                 body.push_back(statement());
             }
             consume(TokenType::RIGHT_BRACE, "Expect '}' after component body.");
             return std::make_unique<ast::ComponentStmt>(name, std::move(body));
        }
        return statement();
    }

    std::vector<std::unique_ptr<ast::Stmt>> Parser::parse() {
        std::vector<std::unique_ptr<ast::Stmt>> statements;
        while (!is_at_end()) {
            auto decl = declaration();
            if (decl) {
                statements.push_back(std::move(decl));
            }
        }
        return statements;
    }

} // namespace nota
