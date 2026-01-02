#pragma once

#include "ast/AST.h"
#include "lexer/Lexer.h"
#include <vector>
#include <string>

class Parser {
public:
    Parser(Lexer& lexer);

    RootNode parse();
    const std::vector<std::string>& errors() const;

private:
    void advance();
    bool match(TokenType type);
    void consume(TokenType type, const std::string& message);
    bool check(TokenType type) const;

    std::unique_ptr<ComponentNode> parse_component();
    PropertyNode parse_property();
    ASTValue parse_value();

    void synchronize();
    void error(const std::string& message);

    Lexer& lexer_;
    Token current_;
    Token previous_;
    bool had_error_ = false;
    std::vector<std::string> errors_;
};
