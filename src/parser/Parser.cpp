#include "Parser.h"
#include <stdexcept>

Parser::Parser(Lexer& lexer) : lexer_(lexer) {}

RootNode Parser::parse() {
    advance(); // Load the first token
    RootNode root;
    root.root_component = parse_component();
    return root;
}

const std::vector<std::string>& Parser::errors() const {
    return errors_;
}

void Parser::advance() {
    previous_ = current_;
    current_ = lexer_.next_token();
    while (current_.type == TokenType::UNEXPECTED) {
        error("Lexer error: " + std::string(current_.text));
        current_ = lexer_.next_token();
    }
}

bool Parser::match(TokenType type) {
    if (check(type)) {
        advance();
        return true;
    }
    return false;
}

void Parser::consume(TokenType type, const std::string& message) {
    if (check(type)) {
        advance();
        return;
    }
    error(message);
}

bool Parser::check(TokenType type) const {
    return current_.type == type;
}

std::unique_ptr<ComponentNode> Parser::parse_component() {
    if (!check(TokenType::IDENTIFIER)) {
        error("Expected a component type identifier.");
        return nullptr;
    }

    auto component = std::make_unique<ComponentNode>();
    component->type = current_;
    advance();

    consume(TokenType::LEFT_BRACE, "Expected '{' after component type.");

    while (!check(TokenType::RIGHT_BRACE) && !check(TokenType::END_OF_FILE)) {
        if (check(TokenType::IDENTIFIER)) {
            // Use a lookahead that skips whitespace to differentiate properties from child components.
            if (lexer_.peek_next_significant_char() == ':') {
                component->properties.push_back(parse_property());
            } else {
                component->children.push_back(parse_component());
            }
        } else {
            error("Unexpected token in component body.");
            advance(); // Skip the token to avoid an infinite loop
        }
    }

    consume(TokenType::RIGHT_BRACE, "Expected '}' after component body.");
    return component;
}

PropertyNode Parser::parse_property() {
    PropertyNode prop;
    prop.name = current_;
    advance();

    consume(TokenType::COLON, "Expected ':' after property name.");

    prop.value = parse_value();

    // Optionally consume a semicolon
    match(TokenType::SEMICOLON);

    return prop;
}

ASTValue Parser::parse_value() {
    if (match(TokenType::STRING)) {
        return LiteralNode{std::string(previous_.text), previous_};
    }
    if (match(TokenType::NUMBER)) {
        // The value is stored as a double in the AST.
        return LiteralNode{std::stod(std::string(previous_.text)), previous_};
    }
    // Handle identifiers for properties like 'position'.
    if (match(TokenType::IDENTIFIER)) {
        std::string value_text = std::string(previous_.text);
        // Check for a second identifier (e.g., 'left top').
        if (check(TokenType::IDENTIFIER)) {
            advance();
            value_text += " " + std::string(previous_.text);
        }
        return LiteralNode{value_text, previous_};
    }

    error("Expected a value (string, number, or identifier).");
    return LiteralNode{std::string(""), previous_}; // Return a dummy value
}

void Parser::synchronize() {
    advance();
    while (!check(TokenType::END_OF_FILE)) {
        if (previous_.type == TokenType::SEMICOLON) return;
        switch (current_.type) {
            case TokenType::ITEM:
            case TokenType::IDENTIFIER:
                return;
            default:
                break;
        }
        advance();
    }
}

void Parser::error(const std::string& message) {
    if (had_error_) return;
    had_error_ = true;
    std::string error_message = "Error at line " + std::to_string(previous_.line) +
                               ", column " + std::to_string(previous_.column) +
                               ": " + message;
    errors_.push_back(error_message);
}
