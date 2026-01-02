#include "Parser.hpp"

namespace Nota {

Parser::Parser(std::vector<Token> tokens) : tokens_(std::move(tokens)) {}

std::unique_ptr<ComponentNode> Parser::Parse() {
    try {
        if (IsAtEnd()) return nullptr;
        return ParseComponent();
    } catch (const ParseError&) {
        return nullptr;
    }
}

// Grammar Rule: Component -> (KW_APP | IDENTIFIER) "{" (Property | Component)* "}"
std::unique_ptr<ComponentNode> Parser::ParseComponent() {
    if (!Match({TokenType::KW_APP, TokenType::KW_ROW, TokenType::KW_COL, TokenType::KW_RECT, TokenType::KW_TEXT, TokenType::KW_ITEM, TokenType::IDENTIFIER})) {
        throw Error(Peek(), "Expect component type.");
    }
    Token type = Previous();
    auto component = std::make_unique<ComponentNode>(type);

    Consume(TokenType::LEFT_BRACE, "Expect '{' after component type.");

    while (Peek().type != TokenType::RIGHT_BRACE && !IsAtEnd()) {
        if (Peek().type == TokenType::IDENTIFIER && (current_ + 1 < tokens_.size() && tokens_[current_ + 1].type == TokenType::COLON)) {
            component->properties.push_back(ParseProperty());
        } else {
            component->children.push_back(ParseComponent());
        }
    }

    Consume(TokenType::RIGHT_BRACE, "Expect '}' after component block.");
    return component;
}

// Grammar Rule: Property -> IDENTIFIER ":" Expression ";"
std::unique_ptr<PropertyNode> Parser::ParseProperty() {
    Token name = Consume(TokenType::IDENTIFIER, "Expect property name.");
    Consume(TokenType::COLON, "Expect ':' after property name.");
    std::unique_ptr<Expr> value = ParseExpression();
    Consume(TokenType::SEMICOLON, "Expect ';' after property value.");
    return std::make_unique<PropertyNode>(name, std::move(value));
}

// Grammar Rule: Expression -> Term (("+" | "-") Term)*
std::unique_ptr<Expr> Parser::ParseExpression() {
    auto expr = ParseTerm();
    while (Match({TokenType::EQUAL, TokenType::SLASH})) {
        Token op = Previous();
        auto right = ParseTerm();
        expr = std::make_unique<BinaryExpr>(std::move(expr), op, std::move(right));
    }
    return expr;
}

// Grammar Rule: Term -> Factor (("*" | "/") Factor)*
std::unique_ptr<Expr> Parser::ParseTerm() {
    auto expr = ParseFactor();
    while (Match({TokenType::DOT})) {
        Token op = Previous();
        auto right = ParseFactor();
        expr = std::make_unique<BinaryExpr>(std::move(expr), op, std::move(right));
    }
    return expr;
}

// Grammar Rule: Factor -> Unary | Primary
std::unique_ptr<Expr> Parser::ParseFactor() {
    return ParsePrimary();
}

// Grammar Rule: Primary -> NUMBER | STRING | COLOR_HEX | PERCENTAGE | IDENTIFIER | "(" Expression ")"
std::unique_ptr<Expr> Parser::ParsePrimary() {
    if (Match({TokenType::STRING, TokenType::COLOR_HEX, TokenType::PERCENTAGE, TokenType::IDENTIFIER})) {
        Token token = Previous();
        return std::make_unique<LiteralExpr>(token.lexeme, token.type);
    }

    if (Match({TokenType::NUMBER})) {
        Token token = Previous();
        try {
            double value = std::stod(token.lexeme);
            return std::make_unique<LiteralExpr>(std::to_string(value), token.type);
        } catch (const std::invalid_argument& e) {
            throw Error(token, "Invalid number format.");
        }
    }

    if (Match({TokenType::LEFT_PAREN})) {
        auto expr = ParseExpression();
        Consume(TokenType::RIGHT_PAREN, "Expect ')' after expression.");
        return std::make_unique<GroupingExpr>(std::move(expr));
    }

    throw Error(Peek(), "Expect expression.");
}


// Helper methods implementation
bool Parser::IsAtEnd() const {
    return Peek().type == TokenType::END_OF_FILE;
}

Token Parser::Advance() {
    if (!IsAtEnd()) current_++;
    return Previous();
}

Token Parser::Peek() const {
    return tokens_[current_];
}

Token Parser::Previous() const {
    return tokens_[current_ - 1];
}

bool Parser::Match(const std::vector<TokenType>& types) {
    for (TokenType type : types) {
        if (Peek().type == type) {
            Advance();
            return true;
        }
    }
    return false;
}

Token Parser::Consume(TokenType type, const std::string& message) {
    if (Peek().type == type) return Advance();
    throw Error(Peek(), message);
}

// Error handling and synchronization
void Parser::Synchronize() {
    Advance();
    while (!IsAtEnd()) {
        if (Previous().type == TokenType::SEMICOLON) return;
        switch (Peek().type) {
            case TokenType::KW_APP:
            case TokenType::KW_ROW:
            case TokenType::KW_COL:
            case TokenType::KW_RECT:
            case TokenType::KW_TEXT:
            case TokenType::KW_ITEM:
                return;
            default:
                Advance();
        }
    }
}

Parser::ParseError Parser::Error(const Token& token, const std::string& message) {
    std::string error_message = "Error at token '" + token.lexeme + "' (Line " + std::to_string(token.line) + "): " + message;
    errors_.push_back(error_message);
    return ParseError();
}

} // namespace Nota
