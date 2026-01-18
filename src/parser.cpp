#include "parser.h"
#include <stdexcept>
#include <iostream>

Parser::Parser(const std::vector<Token>& tokens) : tokens(tokens), pos(0) {}

Token Parser::peek(int offset) const {
    if (pos + offset >= tokens.size()) return tokens.back(); // EOF
    return tokens[pos + offset];
}

Token Parser::advance() {
    if (pos < tokens.size()) return tokens[pos++];
    return tokens.back();
}

bool Parser::match(TokenType type) {
    if (peek().type == type) {
        advance();
        return true;
    }
    return false;
}

Token Parser::consume(TokenType type, const std::string& message) {
    if (peek().type == type) return advance();
    throw std::runtime_error("Parse Error at " + std::to_string(peek().line) + ":" + std::to_string(peek().column) + " - " + message + ". Got " + peek().value);
}

std::shared_ptr<ComponentNode> Parser::parse() {
    // Top level should be a Component (e.g., App or Item)
    return parseComponent();
}

std::shared_ptr<ComponentNode> Parser::parseComponent() {
    std::string type;
    std::string name;

    Token first = advance();
    // Expect Identifier or Keyword (App, Row, etc, or Item)
    if (first.type == TokenType::KEYWORD_ITEM) {
        type = "Item";
        // Item might have a name: Item Box { ... }
        if (peek().type == TokenType::IDENTIFIER) {
            name = advance().value;
        }
    } else if (first.type == TokenType::IDENTIFIER ||
               (first.type >= TokenType::KEYWORD_APP && first.type <= TokenType::KEYWORD_TEXT)) {
        type = first.value;
    } else {
        throw std::runtime_error("Unexpected token for Component start: " + first.value);
    }

    auto node = std::make_shared<ComponentNode>(type, name);

    consume(TokenType::LBRACE, "Expect '{' after component name");

    while (peek().type != TokenType::RBRACE && peek().type != TokenType::EOF_TOKEN) {
        // Decide if Property or Nested Component
        // Lookahead:
        // IDENTIFIER : ... -> Property
        // IDENTIFIER { ... -> Component
        // KEYWORD ... -> Component (usually)
        // KEYWORD_PROPERTY ... -> Property definition (custom prop)

        Token t = peek();
        if (t.type == TokenType::IDENTIFIER) {
            if (peek(1).type == TokenType::COLON) {
                node->children.push_back(parseProperty());
            } else if (peek(1).type == TokenType::LBRACE) {
                node->children.push_back(parseComponent());
            } else if (peek(1).type == TokenType::DOT) {
                 // id.prop: val
                 node->children.push_back(parseProperty());
            } else {
                 // Maybe "Identifier Identifier" (custom type property definition?)
                 // For now assume Syntax Error if not : or {
                 throw std::runtime_error("Unexpected token after identifier " + t.value);
            }
        } else if (t.type >= TokenType::KEYWORD_ITEM && t.type <= TokenType::KEYWORD_TEXT) {
            // Built-in component or Item
            node->children.push_back(parseComponent());
        } else if (t.type == TokenType::KEYWORD_PROPERTY) {
            // property type name: value
            // We treat this as a PropertyNode but maybe specialized?
            // For now, let's just parse it as a generic property line or ignore custom prop defs in MVP
            // Let's implement basic "property type name: value" parsing
            advance(); // consume property
            // type
            Token typeTok = advance();
            // name
            Token nameTok = consume(TokenType::IDENTIFIER, "Expect property name");
            consume(TokenType::COLON, "Expect ':'");
            auto val = parseValue();
            // We might store this differently, but for now reuse PropertyNode
            auto prop = std::make_shared<PropertyNode>("property " + typeTok.value + " " + nameTok.value, val);
             node->children.push_back(prop);
        } else if (t.type == TokenType::SEMICOLON) {
            advance(); // Ignore semicolons
        } else {
             // Unknown, maybe comment or error
             throw std::runtime_error("Unexpected token in component body: " + t.value);
        }
    }

    consume(TokenType::RBRACE, "Expect '}' after component body");
    return node;
}

std::shared_ptr<PropertyNode> Parser::parseProperty() {
    // Name can be ID or ID.ID
    Token nameTok = advance();
    std::string name = nameTok.value;

    while (match(TokenType::DOT)) {
        Token part = consume(TokenType::IDENTIFIER, "Expect identifier after '.'");
        name += "." + part.value;
    }

    consume(TokenType::COLON, "Expect ':' after property name");
    auto value = parseValue();
    return std::make_shared<PropertyNode>(name, value);
}

std::shared_ptr<ASTNode> Parser::parseValue() {
    Token t = peek();
    if (t.type == TokenType::STRING_LITERAL ||
        t.type == TokenType::NUMBER_LITERAL ||
        t.type == TokenType::HEX_COLOR_LITERAL) {
        advance();
        return std::make_shared<ValueNode>(t);
    }

    // Identifiers (references)
    if (t.type == TokenType::IDENTIFIER) {
        advance();
        // Check for math or dots?
        // For MVP, just return the identifier as a value
        return std::make_shared<ValueNode>(t);
    }

    throw std::runtime_error("Unexpected value token: " + t.value);
}
