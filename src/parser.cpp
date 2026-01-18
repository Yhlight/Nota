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
            advance();
            program->statements.push_back(parseComponent());
        } else if (t.type >= TokenType::KEYWORD_ITEM && t.type <= TokenType::KEYWORD_TEXT) {
            program->statements.push_back(parseComponent());
        } else if (t.type == TokenType::IDENTIFIER) {
             program->statements.push_back(parseComponent());
        } else if (t.type == TokenType::SEMICOLON) {
            advance();
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
        while (match(TokenType::DOT)) {
            path += "." + consume(TokenType::IDENTIFIER, "Expect identifier after '.'").value;
        }
    } else {
        throw std::runtime_error("Expect string literal or identifier after import");
    }

    if (peek().type == TokenType::IDENTIFIER && peek().value == "as") {
        advance();
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
    if (first.type == TokenType::KEYWORD_ITEM) {
        type = "Item";
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
                 node->children.push_back(parseProperty());
            } else {
                 throw std::runtime_error("Unexpected token after identifier " + t.value);
            }
        } else if (t.type >= TokenType::KEYWORD_ITEM && t.type <= TokenType::KEYWORD_TEXT) {
            node->children.push_back(parseComponent());
        } else if (t.type == TokenType::KEYWORD_PROPERTY) {
            advance();
            Token typeTok = advance();
            Token nameTok = consume(TokenType::IDENTIFIER, "Expect property name");
            consume(TokenType::COLON, "Expect ':'");
            auto val = parseExpression();
            auto prop = std::make_shared<PropertyNode>("property " + typeTok.value + " " + nameTok.value, val);
             node->children.push_back(prop);
        } else if (t.type == TokenType::SEMICOLON) {
            advance();
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
    auto value = parseExpression();
    return std::make_shared<PropertyNode>(name, value);
}

// Expression Parsing (Recursive Descent)
// Expression -> Term { (+|-) Term }
// Term       -> Factor { (*|/) Factor }
// Factor     -> Primary

std::shared_ptr<ExpressionNode> Parser::parseExpression() {
    auto left = parseTerm();

    while (peek().type == TokenType::PLUS || peek().type == TokenType::MINUS) {
        Token op = advance();
        auto right = parseTerm();
        left = std::make_shared<BinaryExpressionNode>(left, op, right);
    }
    return left;
}

std::shared_ptr<ExpressionNode> Parser::parseTerm() {
    auto left = parseFactor();

    while (peek().type == TokenType::STAR || peek().type == TokenType::SLASH) {
        Token op = advance();
        auto right = parseFactor();
        left = std::make_shared<BinaryExpressionNode>(left, op, right);
    }
    return left;
}

std::shared_ptr<ExpressionNode> Parser::parseFactor() {
    // Usually Factor handles unaries, grouping, but for now just Primary
    // Add support for parentheses? ( Expression )
    if (peek().type == TokenType::LPAREN) {
        advance();
        auto expr = parseExpression();
        consume(TokenType::RPAREN, "Expect ')' after expression");
        return expr;
    }
    return parsePrimary();
}

std::shared_ptr<ExpressionNode> Parser::parsePrimary() {
    Token t = peek();

    if (t.type == TokenType::NUMBER_LITERAL ||
        t.type == TokenType::STRING_LITERAL ||
        t.type == TokenType::HEX_COLOR_LITERAL) {
        advance();
        return std::make_shared<LiteralNode>(t);
    }

    if (t.type == TokenType::IDENTIFIER) {
        advance();
        std::string name = t.value;
        // Check for dotted access (reference chain)
        while (match(TokenType::DOT)) {
            Token part = consume(TokenType::IDENTIFIER, "Expect identifier after '.'");
            name += "." + part.value;
        }
        return std::make_shared<ReferenceNode>(name);
    }

    throw std::runtime_error("Unexpected expression token: " + t.value);
}
