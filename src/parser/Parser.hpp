#pragma once

#include "core/AST.hpp"
#include "lexer/Lexer.hpp"
#include <map>
#include <vector>

namespace nota {
namespace parser {

enum Precedence {
    LOWEST,
    EQUALS,      // ==
    LESSGREATER, // > or <
    SUM,         // +
    PRODUCT,     // *
    PREFIX,      // -X or !X
    CALL,        // myFunction(X)
};

class Parser {
public:
    explicit Parser(lexer::Lexer& lexer);

    std::unique_ptr<core::Program> ParseProgram();
    const std::vector<std::string>& Errors() const;
    std::unique_ptr<core::Expression> ParseExpression(Precedence precedence);

private:
    using PrefixParseFn = std::unique_ptr<core::Expression> (Parser::*)();
    using InfixParseFn = std::unique_ptr<core::Expression> (Parser::*)(std::unique_ptr<core::Expression>);

    void NextToken();
    bool ExpectPeek(core::TokenType type);

    std::unique_ptr<core::Statement> ParseStatement();
    std::unique_ptr<core::LetStatement> ParseLetStatement();
    std::unique_ptr<core::MutStatement> ParseMutStatement();
    std::unique_ptr<core::Statement> ParseExpressionStatement();

    std::unique_ptr<core::Expression> ParseIdentifier();
    std::unique_ptr<core::Expression> ParseIntegerLiteral();
    std::unique_ptr<core::Expression> ParsePrefixExpression();
    std::unique_ptr<core::Expression> ParseInfixExpression(std::unique_ptr<core::Expression> left);
    std::unique_ptr<core::Expression> ParseGroupedExpression();

    Precedence PeekPrecedence();
    Precedence CurPrecedence();

    void RegisterPrefix(core::TokenType type, PrefixParseFn fn);
    void RegisterInfix(core::TokenType type, InfixParseFn fn);

    std::vector<core::Token> tokens_;
    size_t cur_pos_{0};
    core::Token cur_token_;
    core::Token peek_token_;
    std::vector<std::string> errors_;

    std::map<core::TokenType, PrefixParseFn> prefix_parse_fns_;
    std::map<core::TokenType, InfixParseFn> infix_parse_fns_;
    std::map<core::TokenType, Precedence> precedences_;
};

} // namespace parser
} // namespace nota
