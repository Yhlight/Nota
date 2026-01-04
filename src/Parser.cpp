#include "Parser.h"
#include "TokenType.h"
#include <iostream>

Parser::Parser(std::vector<Token> tokens) : tokens(std::move(tokens)) {}

std::vector<std::unique_ptr<Stmt>> Parser::parse() {
    std::vector<std::unique_ptr<Stmt>> statements;
    while (!is_at_end()) {
        statements.push_back(declaration());
    }
    return statements;
}

std::unique_ptr<Stmt> Parser::declaration() {
    try {
        if (match({TokenType::PACKAGE})) {
            return package_declaration();
        }
        if (match({TokenType::IMPORT})) {
            return import_declaration();
        }
        if (match({TokenType::EXPORT})) {
            return export_declaration();
        }
        if (match({TokenType::ITEM})) {
            return item_declaration();
        }
        if (peek().type == TokenType::IDENTIFIER && (current + 1) < tokens.size() && tokens[current + 1].type == TokenType::LEFT_BRACE) {
            return component_declaration();
        }
        return property_declaration();
    } catch (const std::runtime_error& error) {
        synchronize();
        return nullptr;
    }
}

std::unique_ptr<Stmt> Parser::item_declaration() {
    Token name = consume(TokenType::IDENTIFIER, "Expect item name.");
    consume(TokenType::LEFT_BRACE, "Expect '{' after item name.");
    std::vector<std::unique_ptr<Stmt>> body;
    while (!check(TokenType::RIGHT_BRACE) && !is_at_end()) {
        body.push_back(declaration());
    }
    consume(TokenType::RIGHT_BRACE, "Expect '}' after item body.");
    return std::make_unique<ItemStmt>(name, std::move(body));
}

std::unique_ptr<Stmt> Parser::package_declaration() {
    Token name = consume(TokenType::IDENTIFIER, "Expect package name.");
    consume(TokenType::SEMICOLON, "Expect ';' after package name.");
    return std::make_unique<PackageStmt>(name);
}

std::unique_ptr<Stmt> Parser::import_declaration() {
    Token path = consume(TokenType::STRING, "Expect module path.");
    Token alias = Token{TokenType::IDENTIFIER, "", nullptr, 0};
    if (match({TokenType::AS})) {
        alias = consume(TokenType::IDENTIFIER, "Expect alias name.");
    }
    consume(TokenType::SEMICOLON, "Expect ';' after import statement.");
    return std::make_unique<ImportStmt>(path, alias);
}

std::unique_ptr<Stmt> Parser::export_declaration() {
    consume(TokenType::LEFT_BRACE, "Expect '{' after 'export'.");
    std::vector<Token> names;
    if (!check(TokenType::RIGHT_BRACE)) {
        do {
            names.push_back(consume(TokenType::IDENTIFIER, "Expect component name."));
        } while (match({TokenType::COMMA}));
    }
    consume(TokenType::RIGHT_BRACE, "Expect '}' after export list.");
    return std::make_unique<ExportStmt>(names);
}

std::unique_ptr<Stmt> Parser::component_declaration() {
    Token name = consume(TokenType::IDENTIFIER, "Expect component name.");
    consume(TokenType::LEFT_BRACE, "Expect '{' after component name.");
    std::vector<std::unique_ptr<Stmt>> body;
    while (!check(TokenType::RIGHT_BRACE) && !is_at_end()) {
        body.push_back(declaration());
    }
    consume(TokenType::RIGHT_BRACE, "Expect '}' after component body.");
    return std::make_unique<ComponentStmt>(name, std::move(body));
}

std::unique_ptr<Stmt> Parser::property_declaration() {
    Token name = consume(TokenType::IDENTIFIER, "Expect property name.");
    consume(TokenType::COLON, "Expect ':' after property name.");
    std::unique_ptr<Expr> value = expression();
    if (name.lexeme == "id") {
        if (auto* id_expr = dynamic_cast<IdentifierExpr*>(value.get())) {
            // This is a component id, not a property.
            // We'll handle this in the resolver.
            return std::make_unique<PropertyStmt>(name, std::move(value));
        }
    }
    return std::make_unique<PropertyStmt>(name, std::move(value));
}

std::unique_ptr<Expr> Parser::expression() {
    return term();
}

std::unique_ptr<Expr> Parser::term() {
    std::unique_ptr<Expr> expr = factor();

    while (match({TokenType::MINUS, TokenType::PLUS})) {
        Token op = previous();
        std::unique_ptr<Expr> right = factor();
        expr = std::make_unique<BinaryExpr>(std::move(expr), op, std::move(right));
    }

    return expr;
}

std::unique_ptr<Expr> Parser::factor() {
    std::unique_ptr<Expr> expr = unary();

    while (match({TokenType::SLASH, TokenType::STAR, TokenType::PERCENT})) {
        Token op = previous();
        std::unique_ptr<Expr> right = unary();
        expr = std::make_unique<BinaryExpr>(std::move(expr), op, std::move(right));
    }

    return expr;
}

std::unique_ptr<Expr> Parser::unary() {
    // For now, we don't have unary operators, so this just passes through to call().
    return call();
}

std::unique_ptr<Expr> Parser::call() {
    std::unique_ptr<Expr> expr = primary();

    while (true) {
        if (match({TokenType::DOT})) {
            Token name = consume(TokenType::IDENTIFIER, "Expect property name after '.'.");
            expr = std::make_unique<GetExpr>(std::move(expr), name);
        } else {
            break;
        }
    }

    return expr;
}

std::unique_ptr<Expr> Parser::primary() {
    if (match({TokenType::NUMBER, TokenType::STRING})) {
        return std::make_unique<LiteralExpr>(previous().literal);
    }
    if (match({TokenType::IDENTIFIER})) {
        return std::make_unique<IdentifierExpr>(previous());
    }

    throw std::runtime_error("Expect expression.");
}

bool Parser::match(const std::vector<TokenType>& types) {
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
    throw std::runtime_error(message);
}

bool Parser::check(TokenType type) {
    if (is_at_end()) return false;
    return peek().type == type;
}

Token Parser::advance() {
    if (!is_at_end()) current++;
    return previous();
}

bool Parser::is_at_end() {
    return peek().type == TokenType::END_OF_FILE;
}

Token Parser::peek() {
    return tokens[current];
}

Token Parser::previous() {
    return tokens[current - 1];
}

void Parser::synchronize() {
    advance();
    while (!is_at_end()) {
        if (previous().type == TokenType::SEMICOLON) return;
        switch (peek().type) {
            case TokenType::IDENTIFIER:
                return;
        }
        advance();
    }
}
