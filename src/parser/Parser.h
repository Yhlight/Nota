#pragma once

#include "ast/AST.h"
#include "core/Error.h"
#include "lexer/Lexer.h"
#include <string>
#include <vector>

class Parser {
public:
    Parser(Lexer& lexer);
    const std::vector<CompilerError>& errors() const;
    RootNode parse();

private:
    // Utility methods
    void advance();
    bool match(TokenType type);
    void consume(TokenType type, const std::string& message);
    bool check(TokenType type) const;
    void synchronize();
    void error(const std::string& message);

    // Parsing methods
    template <typename Node>
    void parse_component_body(Node& node);
    std::unique_ptr<ItemNode> parse_item_definition();
    std::unique_ptr<ComponentNode> parse_component();
    PropertyNode parse_property();
    ASTValue parse_value();
    std::unique_ptr<Expression> parse_expression();
    std::unique_ptr<Expression> parse_term();
    std::unique_ptr<Expression> parse_factor();
    std::unique_ptr<Expression> parse_unary();
    std::unique_ptr<Expression> parse_call();
    std::unique_ptr<Expression> parse_primary();
    AssignmentNode parse_assignment(std::unique_ptr<Expression> target);

    // State
    Lexer& lexer_;
    Token current_;
    Token previous_;
    bool had_error_ = false;
    std::vector<CompilerError> errors_;
};
