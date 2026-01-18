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

std::shared_ptr<ProgramNode> Parser::parse() {
    auto program = std::make_shared<ProgramNode>();

    while (peek().type != TokenType::EOF_TOKEN) {
        Token t = peek();
        if (t.type == TokenType::KEYWORD_IMPORT) {
            program->statements.push_back(parseImport());
        } else if (t.type == TokenType::KEYWORD_EXPORT) {
            // Treat export as a modifier, for now consume and then expect component
            advance();
            program->statements.push_back(parseComponent());
        } else if (t.type >= TokenType::KEYWORD_ITEM && t.type <= TokenType::KEYWORD_TEXT) {
            program->statements.push_back(parseComponent());
        } else if (t.type == TokenType::IDENTIFIER) {
            // Could be Component Identifier { ... } (Custom component)
            // or maybe a property if we allowed properties at top level? (No, standard says no)
            // Assume it's a component type.
             program->statements.push_back(parseComponent());
        } else if (t.type == TokenType::SEMICOLON) {
            advance(); // Ignore top-level semicolons
        } else {
             throw std::runtime_error("Unexpected token at top level: " + t.value);
        }
    }
    return program;
}

std::shared_ptr<ImportNode> Parser::parseImport() {
    consume(TokenType::KEYWORD_IMPORT, "Expect 'import'");
    std::string path;
    std::string alias;

    Token t = peek();
    if (t.type == TokenType::STRING_LITERAL) {
        path = advance().value;
    } else if (t.type == TokenType::IDENTIFIER) {
        path = advance().value;
        // Handle dotted module names: import Std.UI.Button;
        while (match(TokenType::DOT)) {
            path += "." + consume(TokenType::IDENTIFIER, "Expect identifier after '.'").value;
        }
    } else {
        throw std::runtime_error("Expect string literal or identifier after import");
    }

    // Check for 'as' (which would need a keyword or just identifier?)
    // Nota.md example: import "ui.nota" as ui;
    // We don't have KEYWORD_AS. Check if next token is identifier "as"?
    // Or maybe just look for identifier if we decide 'as' is not a reserved keyword.
    // For now, let's look for identifier "as".
    if (peek().type == TokenType::IDENTIFIER && peek().value == "as") {
        advance(); // consume "as"
        alias = consume(TokenType::IDENTIFIER, "Expect alias identifier").value;
    }

    if (peek().type == TokenType::SEMICOLON) {
        advance();
    }

    return std::make_shared<ImportNode>(path, alias);
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
                 throw std::runtime_error("Unexpected token after identifier " + t.value);
            }
        } else if (t.type >= TokenType::KEYWORD_ITEM && t.type <= TokenType::KEYWORD_TEXT) {
            // Built-in component or Item
            node->children.push_back(parseComponent());
        } else if (t.type == TokenType::KEYWORD_PROPERTY) {
            advance(); // consume property
            Token typeTok = advance();
            Token nameTok = consume(TokenType::IDENTIFIER, "Expect property name");
            consume(TokenType::COLON, "Expect ':'");
            auto val = parseValue();
            auto prop = std::make_shared<PropertyNode>("property " + typeTok.value + " " + nameTok.value, val);
             node->children.push_back(prop);
        } else if (t.type == TokenType::SEMICOLON) {
            advance(); // Ignore semicolons
        } else {
             throw std::runtime_error("Unexpected token in component body: " + t.value);
        }
    }

    consume(TokenType::RBRACE, "Expect '}' after component body");
    return node;
}

std::shared_ptr<PropertyNode> Parser::parseProperty() {
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

    if (t.type == TokenType::IDENTIFIER) {
        advance();
        return std::make_shared<ValueNode>(t);
    }

    throw std::runtime_error("Unexpected value token: " + t.value);
}
