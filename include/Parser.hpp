#ifndef NOTA_PARSER_HPP
#define NOTA_PARSER_HPP

#include "Token.hpp"
#include "AST.hpp"
#include <vector>
#include <memory>
#include <stdexcept>

namespace Nota {

class Parser {
public:
    explicit Parser(std::vector<Token> tokens);

    std::unique_ptr<ComponentNode> Parse();
    const std::vector<std::string>& Errors() const { return errors_; }

private:
    // Parsing methods for different grammar rules
    std::unique_ptr<ComponentNode> ParseComponent();
    std::unique_ptr<PropertyNode> ParseProperty();
    std::unique_ptr<Expr> ParseExpression();
    std::unique_ptr<Expr> ParseTerm();
    std::unique_ptr<Expr> ParseFactor();
    std::unique_ptr<Expr> ParsePrimary();

    // Helper methods
    bool IsAtEnd() const;
    Token Advance();
    Token Peek() const;
    Token Previous() const;
    bool Match(const std::vector<TokenType>& types);
    Token Consume(TokenType type, const std::string& message);
    void Synchronize();

private:
    std::vector<Token> tokens_;
    int current_ = 0;
    std::vector<std::string> errors_;

    // Exception for error handling
    class ParseError : public std::runtime_error {
    public:
        ParseError() : std::runtime_error("") {}
    };

    ParseError Error(const Token& token, const std::string& message);
};

} // namespace Nota

#endif // NOTA_PARSER_HPP
