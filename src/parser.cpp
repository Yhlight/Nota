#include "parser.h"
#include <stdexcept>
#include <iostream>
#include <memory>

std::shared_ptr<ExpressionNode> Parser::parsePrimary() {
    Token t = peek();

    if (t.type == TokenType::NUMBER_LITERAL ||
        t.type == TokenType::STRING_LITERAL ||
        t.type == TokenType::HEX_COLOR_LITERAL) {
        advance();
        std::string val = t.value;

        while (peek().type == TokenType::IDENTIFIER || peek().type == TokenType::NUMBER_LITERAL) {
             if (peek().line != t.line) break;

             Token next = peek();
             if (t.type == TokenType::NUMBER_LITERAL && next.type == TokenType::IDENTIFIER) {
             } else {
                 val += " ";
             }
             val += next.value;
             t = advance();
        }

        Token combined{TokenType::STRING_LITERAL, val, 0, 0};
        return std::make_shared<LiteralNode>(combined);
    }

    if (t.type == TokenType::IDENTIFIER) {
        advance();
        std::string name = t.value;
        bool isDotted = false;
        while (match(TokenType::DOT)) {
            isDotted = true;
            Token part = consume(TokenType::IDENTIFIER, "Expect identifier after '.'");
            name += "." + part.value;
        }

        if (peek().type == TokenType::LPAREN) {
            consume(TokenType::LPAREN, "Expect '('");
            auto node = std::make_shared<StructInstantiationNode>(name);
            if (peek().type != TokenType::RPAREN) {
                do {
                    node->arguments.push_back(parseExpression());
                } while (match(TokenType::COMMA));
            }
            consume(TokenType::RPAREN, "Expect ')'");
            return node;
        }

        // Object Literal / Component in Expression: "State { ... }"
        if (peek().type == TokenType::LBRACE) {
            auto node = std::make_shared<ComponentNode>(name); // 'name' is the Type
            consume(TokenType::LBRACE, "Expect '{'");

            while (peek().type != TokenType::RBRACE && peek().type != TokenType::EOF_TOKEN) {
                Token t = peek();
                if (t.type == TokenType::IDENTIFIER) {
                    if (peek(1).type == TokenType::COLON) {
                        node->children.push_back(parseProperty());
                    } else if (peek(1).type == TokenType::LBRACE) {
                        node->children.push_back(parseComponent());
                    } else if (peek(1).type == TokenType::DOT) {
                        int lookahead = 1;
                        while (peek(lookahead).type == TokenType::DOT) {
                             lookahead++;
                             if (peek(lookahead).type == TokenType::IDENTIFIER) lookahead++; else break;
                        }
                        if (peek(lookahead).type == TokenType::COLON) node->children.push_back(parseProperty());
                        else if (peek(lookahead).type == TokenType::LBRACE) node->children.push_back(parseComponent());
                        else throw std::runtime_error("Unexpected token after dotted identifier");
                    } else {
                        throw std::runtime_error("Unexpected token after identifier " + t.value);
                    }
                } else if (t.type == TokenType::KEYWORD_IF) {
                    node->children.push_back(parseIf());
                } else if (t.type >= TokenType::KEYWORD_ITEM && t.type <= TokenType::KEYWORD_TEXT) {
                    node->children.push_back(parseComponent());
                } else if (t.type == TokenType::KEYWORD_PROPERTY) {
                     advance(); Token typeTok = advance(); Token nameTok = consume(TokenType::IDENTIFIER, "Prop name");
                     consume(TokenType::COLON, ":");
                     auto val = parseExpression();
                     node->children.push_back(std::make_shared<PropertyNode>("property " + typeTok.value + " " + nameTok.value, val));
                } else if (t.type == TokenType::SEMICOLON) {
                    advance();
                } else {
                     throw std::runtime_error("Unexpected token in component expression body: " + t.value);
                }
            }
            consume(TokenType::RBRACE, "Expect '}'");
            return node;
        }

        if (isDotted) {
             return std::make_shared<ReferenceNode>(name);
        }

        std::string val = name;
        bool merged = false;
        Token last = t;

        while (peek().type == TokenType::IDENTIFIER || peek().type == TokenType::NUMBER_LITERAL) {
             if (peek().line != last.line) break;

             Token next = peek();
             val += " " + next.value;
             last = advance();
             merged = true;
        }

        if (merged) {
             Token combined{TokenType::STRING_LITERAL, val, 0, 0};
             return std::make_shared<LiteralNode>(combined);
        } else {
             return std::make_shared<ReferenceNode>(name);
        }
    }

    if (t.type == TokenType::LBRACE) {
        return parseBlock();
    }

    if (t.type == TokenType::LBRACKET) {
        return parseList();
    }

    if (t.type == TokenType::KEYWORD_STATE ||
        t.type == TokenType::KEYWORD_ITEM ||
        (t.type >= TokenType::KEYWORD_APP && t.type <= TokenType::KEYWORD_TEXT)) {
        return parseComponent();
    }

    throw std::runtime_error("Unexpected expression token: " + t.value);
}

std::shared_ptr<ListNode> Parser::parseList() {
    consume(TokenType::LBRACKET, "Expect '['");
    auto node = std::make_shared<ListNode>();

    if (peek().type != TokenType::RBRACKET) {
        do {
            node->elements.push_back(parseExpression());
        } while (match(TokenType::COMMA));
    }

    consume(TokenType::RBRACKET, "Expect ']'");
    return node;
}

std::shared_ptr<ComponentNode> Parser::parseIf() {
    consume(TokenType::KEYWORD_IF, "Expect 'if'");
    bool hasParen = match(TokenType::LPAREN);
    auto condition = parseExpression();
    if (hasParen) consume(TokenType::RPAREN, "Expect ')' after condition");

    auto node = std::make_shared<ConditionalNode>(condition);

    consume(TokenType::LBRACE, "Expect '{' start of if-block");
    while (peek().type != TokenType::RBRACE && peek().type != TokenType::EOF_TOKEN) {
        Token t = peek();
        if (t.type == TokenType::IDENTIFIER) {
             if (peek(1).type == TokenType::COLON) {
                 node->thenBranch.push_back(parseProperty());
             } else if (peek(1).type == TokenType::LBRACE) {
                 node->thenBranch.push_back(parseComponent());
             } else if (peek(1).type == TokenType::DOT) {
                 int lookahead = 1;
                 while (peek(lookahead).type == TokenType::DOT) {
                     lookahead++;
                     if (peek(lookahead).type == TokenType::IDENTIFIER) lookahead++; else break;
                 }
                 if (peek(lookahead).type == TokenType::COLON) node->thenBranch.push_back(parseProperty());
                 else if (peek(lookahead).type == TokenType::LBRACE) node->thenBranch.push_back(parseComponent());
                 else throw std::runtime_error("Unexpected token after dotted identifier");
             } else {
                 throw std::runtime_error("Unexpected token after identifier " + t.value);
             }
        } else if (t.type >= TokenType::KEYWORD_ITEM && t.type <= TokenType::KEYWORD_TEXT) {
            node->thenBranch.push_back(parseComponent());
        } else if (t.type == TokenType::KEYWORD_IF) {
            node->thenBranch.push_back(parseIf());
        } else if (t.type == TokenType::SEMICOLON) {
            advance();
        } else {
             throw std::runtime_error("Unexpected token in if-block: " + t.value);
        }
    }
    consume(TokenType::RBRACE, "Expect '}' end of if-block");

    if (match(TokenType::KEYWORD_ELSE)) {
        consume(TokenType::LBRACE, "Expect '{' start of else-block");
        while (peek().type != TokenType::RBRACE && peek().type != TokenType::EOF_TOKEN) {
            Token t = peek();
            if (t.type == TokenType::IDENTIFIER) {
                 if (peek(1).type == TokenType::COLON) {
                     node->elseBranch.push_back(parseProperty());
                 } else if (peek(1).type == TokenType::LBRACE) {
                     node->elseBranch.push_back(parseComponent());
                 } else if (peek(1).type == TokenType::DOT) {
                     int lookahead = 1;
                     while (peek(lookahead).type == TokenType::DOT) {
                         lookahead++;
                         if (peek(lookahead).type == TokenType::IDENTIFIER) lookahead++; else break;
                     }
                     if (peek(lookahead).type == TokenType::COLON) node->elseBranch.push_back(parseProperty());
                     else if (peek(lookahead).type == TokenType::LBRACE) node->elseBranch.push_back(parseComponent());
                     else throw std::runtime_error("Unexpected token after dotted identifier");
                 } else {
                     throw std::runtime_error("Unexpected token after identifier " + t.value);
                 }
            } else if (t.type >= TokenType::KEYWORD_ITEM && t.type <= TokenType::KEYWORD_TEXT) {
                node->elseBranch.push_back(parseComponent());
            } else if (t.type == TokenType::KEYWORD_IF) {
                node->elseBranch.push_back(parseIf());
            } else if (t.type == TokenType::SEMICOLON) {
                advance();
            } else {
                 throw std::runtime_error("Unexpected token in else-block: " + t.value);
            }
        }
        consume(TokenType::RBRACE, "Expect '}' end of else-block");
    }

    return node;
}

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
        if (t.type == TokenType::KEYWORD_PACKAGE) {
            program->statements.push_back(parsePackage());
        } else if (t.type == TokenType::KEYWORD_IMPORT) {
            program->statements.push_back(parseImport());
        } else if (t.type == TokenType::KEYWORD_STRUCT) {
            program->statements.push_back(parseStruct());
        } else if (t.type == TokenType::KEYWORD_EXPORT) {
            advance();
            if (peek().type == TokenType::KEYWORD_STRUCT) {
                program->statements.push_back(parseStruct());
            } else {
                program->statements.push_back(parseComponent());
            }
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

std::shared_ptr<PackageNode> Parser::parsePackage() {
    consume(TokenType::KEYWORD_PACKAGE, "Expect 'package'");
    std::string name = consume(TokenType::IDENTIFIER, "Expect package name").value;
    while (match(TokenType::DOT)) {
        name += "." + consume(TokenType::IDENTIFIER, "Expect identifier after '.'").value;
    }
    consume(TokenType::SEMICOLON, "Expect ';'");
    return std::make_shared<PackageNode>(name);
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
    } else if (first.type == TokenType::IDENTIFIER) {
        type = first.value;
        // Support dotted names (Module.Component)
        while (match(TokenType::DOT)) {
            type += "." + consume(TokenType::IDENTIFIER, "Expect identifier after '.'").value;
        }
    } else if ((first.type >= TokenType::KEYWORD_APP && first.type <= TokenType::KEYWORD_TEXT) ||
                first.type == TokenType::KEYWORD_STATE) {
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
                 // Look ahead to distinguish Prop (ID.ID:) vs Component (ID.ID {)
                 int lookahead = 1;
                 // We are at ID (t). peek(1) is DOT.
                 while (peek(lookahead).type == TokenType::DOT) {
                     lookahead++; // Consume DOT
                     if (peek(lookahead).type == TokenType::IDENTIFIER) {
                         lookahead++; // Consume ID
                     } else {
                         break;
                     }
                 }

                 if (peek(lookahead).type == TokenType::COLON) {
                     node->children.push_back(parseProperty());
                 } else if (peek(lookahead).type == TokenType::LBRACE) {
                     node->children.push_back(parseComponent());
                 } else {
                     throw std::runtime_error("Unexpected token sequence after dotted identifier");
                 }
            } else {
                 throw std::runtime_error("Unexpected token after identifier " + t.value);
            }
        } else if (t.type == TokenType::KEYWORD_IF) {
            node->children.push_back(parseIf());
        } else if (t.type == TokenType::KEYWORD_DELEGATE) {
            node->children.push_back(parseDelegate());
        } else if (t.type == TokenType::KEYWORD_FOR) {
            node->children.push_back(parseFor());
        } else if (t.type >= TokenType::KEYWORD_ITEM && t.type <= TokenType::KEYWORD_TEXT) {
            node->children.push_back(parseComponent());
        } else if (t.type == TokenType::KEYWORD_PROPERTY) {
            advance();
            Token typeTok = advance();
            std::string typeName = typeTok.value;

            // Handle generic type: list<Rect>
            // Note: Our lexer might produce multiple tokens for "list < Rect >"
            // Actually, we expect IDENTIFIER "list", then check for LESS.
            // But we already advanced `typeTok`.

            if (typeName == "list" && peek().type == TokenType::IDENTIFIER) {
                 // Check if it's actually a generic list? Nota syntax: "list<Rect>" or just "list"?
                 // Nota.md says: "list<Rect> list_value"
                 // Lexer splits list < Rect >.
                 // So we need to check for LESS.
                 // Wait, parser loop has advanced `typeTok`. Next is `nameTok` usually.
                 // If next is LESS, then consume generic args.
            }

            if (match(TokenType::LESS)) {
                typeName += "<";
                // Consume generics
                while (peek().type != TokenType::GREATER && peek().type != TokenType::EOF_TOKEN) {
                    Token part = advance();
                    typeName += part.value;
                }
                consume(TokenType::GREATER, "Expect '>' after generic type");
                typeName += ">";
            }

            Token nameTok = consume(TokenType::IDENTIFIER, "Expect property name");

            std::shared_ptr<ExpressionNode> val = nullptr;
            if (match(TokenType::COLON)) {
                val = parseExpression();
            }

            auto prop = std::make_shared<PropertyNode>("property " + typeName + " " + nameTok.value, val);
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

std::shared_ptr<StructDefinitionNode> Parser::parseStruct() {
    consume(TokenType::KEYWORD_STRUCT, "Expect 'Struct'");
    std::string name = consume(TokenType::IDENTIFIER, "Expect struct name").value;
    auto node = std::make_shared<StructDefinitionNode>(name);

    consume(TokenType::LBRACE, "Expect '{'");

    while (peek().type != TokenType::RBRACE && peek().type != TokenType::EOF_TOKEN) {
        if (peek().type == TokenType::IDENTIFIER) {
             Token type = advance();
             Token name = consume(TokenType::IDENTIFIER, "Expect field name");

             std::shared_ptr<ExpressionNode> defaultVal = nullptr;
             if (match(TokenType::EQUAL) || match(TokenType::COLON)) {
                 defaultVal = parseExpression();
             }

             consume(TokenType::SEMICOLON, "Expect ';'");
             node->fields.push_back({type.value, name.value, defaultVal});
        } else {
             throw std::runtime_error("Unexpected token in Struct definition: " + peek().value);
        }
    }

    consume(TokenType::RBRACE, "Expect '}'");
    return node;
}

std::shared_ptr<DelegateNode> Parser::parseDelegate() {
    consume(TokenType::KEYWORD_DELEGATE, "Expect 'delegate'");
    consume(TokenType::LBRACKET, "Expect '[' for events");

    std::vector<std::string> events;
    do {
        events.push_back(consume(TokenType::IDENTIFIER, "Expect event name").value);
    } while (match(TokenType::COMMA));

    consume(TokenType::RBRACKET, "Expect ']'");

    // Expect "for" keyword. Wait, "for" is a keyword.
    // Lexer parses "for" as KEYWORD_FOR.
    // parseDelegate should consume it.
    consume(TokenType::KEYWORD_FOR, "Expect 'for'");

    std::string target = consume(TokenType::IDENTIFIER, "Expect target identifier").value;

    if (peek().type == TokenType::SEMICOLON) advance();

    return std::make_shared<DelegateNode>(events, target);
}

std::shared_ptr<ForNode> Parser::parseFor() {
    consume(TokenType::KEYWORD_FOR, "Expect 'for'");
    consume(TokenType::LPAREN, "Expect '('");
    std::string iterator = consume(TokenType::IDENTIFIER, "Expect iterator name").value;

    // Check for "in". Lexer might treat it as identifier or if we add keyword?
    // Current lexer doesn't have "in". So it's IDENTIFIER "in".
    if (peek().type == TokenType::IDENTIFIER && peek().value == "in") {
        advance();
    } else {
        throw std::runtime_error("Expect 'in' after iterator");
    }

    std::string listName = consume(TokenType::IDENTIFIER, "Expect list name").value;
    // Support dotted list name? e.g. parent.list
    while (match(TokenType::DOT)) {
        listName += "." + consume(TokenType::IDENTIFIER, "Expect identifier").value;
    }

    consume(TokenType::RPAREN, "Expect ')'");

    auto node = std::make_shared<ForNode>(iterator, listName);

    consume(TokenType::LBRACE, "Expect '{'");
    while (peek().type != TokenType::RBRACE && peek().type != TokenType::EOF_TOKEN) {
        Token t = peek();
        if (t.type >= TokenType::KEYWORD_ITEM && t.type <= TokenType::KEYWORD_TEXT) {
            node->body.push_back(parseComponent());
        } else if (t.type == TokenType::IDENTIFIER) {
             // Maybe component or property?
             // Inside 'for', we expect components usually.
             // But if someone does "property..."?
             if (peek(1).type == TokenType::LBRACE) {
                 node->body.push_back(parseComponent());
             } else {
                 // For now only support components in for loop body
                 // Skip or error?
                 // Let's assume Component
                 node->body.push_back(parseComponent());
             }
        } else {
             throw std::runtime_error("Unexpected token in for loop body: " + t.value);
        }
    }
    consume(TokenType::RBRACE, "Expect '}'");

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
    if (peek().type == TokenType::LPAREN) {
        advance();
        auto expr = parseExpression();
        consume(TokenType::RPAREN, "Expect ')' after expression");
        return expr;
    }
    return parsePrimary();
}

std::shared_ptr<ExpressionNode> Parser::parseBlock() {
    consume(TokenType::LBRACE, "Expect '{' start of block");
    std::string code;
    int braceDepth = 1;

    while (braceDepth > 0 && peek().type != TokenType::EOF_TOKEN) {
        Token t = advance();
        if (t.type == TokenType::LBRACE) braceDepth++;
        if (t.type == TokenType::RBRACE) {
            braceDepth--;
            if (braceDepth == 0) break;
        }

        if (t.type == TokenType::STRING_LITERAL) {
            code += "\"" + t.value + "\" "; // Re-add quotes for strings
        } else {
            code += t.value + " ";
        }
    }

    Token codeToken{TokenType::STRING_LITERAL, code, 0, 0};
    return std::make_shared<LiteralNode>(codeToken);
}
