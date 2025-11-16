#include "Parser.h"
#include <stdexcept>

namespace nota {

    Parser::Parser(Lexer& lexer)
        : lexer(lexer), current_token(lexer.next_token()) {}

    void Parser::advance() {
        previous_token = current_token;
        current_token = lexer.next_token();
    }

    void Parser::consume(TokenType type, const std::string& message) {
        if (current_token.type == type) {
            advance();
            return;
        }
        throw std::runtime_error(message);
    }

    bool Parser::match(TokenType type) {
        if (current_token.type == type) {
            advance();
            return true;
        }
        return false;
    }

    std::vector<std::unique_ptr<ast::Stmt>> Parser::parse() {
        std::vector<std::unique_ptr<ast::Stmt>> statements;
        while (current_token.type != TokenType::Eof) {
            statements.push_back(statement());
        }
        return statements;
    }

    std::unique_ptr<ast::Stmt> Parser::statement() {
        if (match(TokenType::Let) || match(TokenType::Mut)) {
            return var_declaration();
        }
        // Other statements will go here

        // Skip tokens until the next statement
        while(current_token.type != TokenType::Newline && current_token.type != TokenType::Eof)
        {
            advance();
        }
        if(current_token.type == TokenType::Newline) advance();

        return nullptr;
    }

    std::unique_ptr<ast::Stmt> Parser::var_declaration() {
        consume(TokenType::Identifier, "Expect variable name.");
        Token name = previous_token;

        std::unique_ptr<ast::Expr> initializer = nullptr;
        if (match(TokenType::Equal)) {
            initializer = expression();
        }

        consume(TokenType::Newline, "Expect newline after variable declaration.");
        return std::make_unique<ast::VarDeclStmt>(name, std::move(initializer));
    }

    std::unique_ptr<ast::Expr> Parser::expression() {
        return primary();
    }

    std::unique_ptr<ast::Expr> Parser::primary() {
        if (match(TokenType::Number)) {
            return std::make_unique<ast::LiteralExpr>(previous_token);
        }
        // Other primary expressions will go here
        return nullptr;
    }

} // namespace nota
