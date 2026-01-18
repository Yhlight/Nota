#include "Parser.h"
#include <iostream>

Parser::Parser(std::vector<Token> tokens) : tokens(std::move(tokens)) {}

const Token& Parser::peek() const {
    return tokens[current];
}

const Token& Parser::previous() const {
    return tokens[current - 1];
}

bool Parser::isAtEnd() const {
    return peek().type == TokenType::EndOfFile;
}

bool Parser::match(TokenType type) {
    if (isAtEnd()) return false;
    if (peek().type == type) {
        current++;
        return true;
    }
    return false;
}

Token Parser::consume(TokenType type, const std::string& message) {
    if (match(type)) {
        return previous();
    }
    // Simple error handling for now
    std::cerr << "Error at " << peek().line << ":" << peek().column << " - " << message << std::endl;
    throw std::runtime_error(message);
}

// Parsing Logic
// Component -> Identifier '{' (Property | Component)* '}'
// Property -> Identifier ':' (String | Number | Identifier)

std::unique_ptr<ComponentNode> Parser::parse() {
    if (match(TokenType::Identifier)) {
        auto node = std::make_unique<ComponentNode>();
        node->type = previous().value;

        consume(TokenType::LBrace, "Expect '{' after component type.");

        while (!isAtEnd() && peek().type != TokenType::RBrace) {
            if (peek().type == TokenType::Identifier) {
                // Could be a property or a child component
                // Lookahead needed?
                // Property: Identifier : Value
                // Component: Identifier { ... }

                if (current + 1 < tokens.size() && tokens[current + 1].type == TokenType::Colon) {
                    // It's a property
                    Token name = consume(TokenType::Identifier, "Expect property name.");
                    consume(TokenType::Colon, "Expect ':' after property name.");

                    std::string value;
                    if (match(TokenType::String) || match(TokenType::Number) || match(TokenType::Identifier)) {
                        value = previous().value;
                    } else {
                        throw std::runtime_error("Expect property value.");
                    }

                    // Handle multi-token values (e.g. position: left top)
                    // This is a naive heuristic: if the next token is also an Identifier and not followed by Colon or Brace, consume it.
                    // Actually, Nota grammar isn't fully strict here, but for "position: left top", we have two identifiers.
                    // The Lexer will see "left" then "top".
                    // The current parser loop expects "Prop: Value". "Value" is one token.
                    // We need to consume subsequent identifiers if they look like part of the value.

                    while (peek().type == TokenType::Identifier || peek().type == TokenType::Number) {
                        // Check if it's the start of a new property or component
                        if (current + 1 < tokens.size() && (tokens[current+1].type == TokenType::Colon || tokens[current+1].type == TokenType::LBrace)) {
                            break;
                        }

                        // It's likely part of the value (e.g. "1px solid black" or "left top")
                        value += " " + consume(peek().type, "Consume value part").value;
                    }

                    node->properties.push_back(PropertyNode{name.value, value});

                    // Optional semicolon? Not strictly enforced in example, but good to handle if present
                    match(TokenType::Semicolon);

                } else if (current + 1 < tokens.size() && tokens[current + 1].type == TokenType::LBrace) {
                    // It's a child component
                    node->children.push_back(parse());
                } else {
                     throw std::runtime_error("Unexpected token in component body.");
                }
            } else {
                throw std::runtime_error("Expect property or child component.");
            }
        }

        consume(TokenType::RBrace, "Expect '}' after component body.");
        return node;
    }
    return nullptr;
}
