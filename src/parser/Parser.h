#pragma once

#include "ast/AST.h"
#include "lexer/Lexer.h"
#include "core/Error.h"
#include <vector>
#include <string>

class Parser {
public:
    Parser(Lexer& lexer);

    RootNode parse();
    const std::vector<CompilerError>& errors() const;

private:
    void advance();
    bool match(TokenType type);
    void consume(TokenType type, const std::string& message);
    bool check(TokenType type) const;

    template <typename Node>
    void parse_component_body(Node& node);

    std::unique_ptr<ItemNode> parse_item_definition();
    std::unique_ptr<ComponentNode> parse_component();
    PropertyNode parse_property();
    ASTValue parse_value();
    std::unique_ptr<Expression> parse_expression();
    std::unique_ptr<Expression> parse_primary();
    AssignmentNode parse_assignment(std::unique_ptr<Expression> target);


    void synchronize();
    void error(const std::string& message);

    Lexer& lexer_;
    Token current_;
    Token previous_;
    bool had_error_ = false;
    std::vector<CompilerError> errors_;
};
