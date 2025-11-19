#include "Parser.hpp"
#include <stdexcept>

namespace nota {
namespace parser {

Parser::Parser(lexer::Lexer& lexer) {
    tokens_ = lexer.Tokenize();
    cur_pos_ = 0;

    NextToken();
    NextToken();

    RegisterPrefix(core::TokenType::IDENTIFIER, &Parser::ParseIdentifier);
    RegisterPrefix(core::TokenType::INTEGER, &Parser::ParseIntegerLiteral);
    RegisterPrefix(core::TokenType::BANG, &Parser::ParsePrefixExpression);
    RegisterPrefix(core::TokenType::MINUS, &Parser::ParsePrefixExpression);

    RegisterInfix(core::TokenType::PLUS, &Parser::ParseInfixExpression);
    RegisterInfix(core::TokenType::MINUS, &Parser::ParseInfixExpression);
    RegisterInfix(core::TokenType::SLASH, &Parser::ParseInfixExpression);
    RegisterInfix(core::TokenType::ASTERISK, &Parser::ParseInfixExpression);
    RegisterInfix(core::TokenType::EQ, &Parser::ParseInfixExpression);
    RegisterInfix(core::TokenType::NOT_EQ, &Parser::ParseInfixExpression);
    RegisterInfix(core::TokenType::LT, &Parser::ParseInfixExpression);
    RegisterInfix(core::TokenType::GT, &Parser::ParseInfixExpression);
    RegisterInfix(core::TokenType::ASSIGN, &Parser::ParseAssignmentExpression);

    precedences_ = {
        {core::TokenType::ASSIGN, ASSIGNMENT},
        {core::TokenType::EQ, EQUALS},
        {core::TokenType::NOT_EQ, EQUALS},
        {core::TokenType::LT, LESSGREATER},
        {core::TokenType::GT, LESSGREATER},
        {core::TokenType::PLUS, SUM},
        {core::TokenType::MINUS, SUM},
        {core::TokenType::SLASH, PRODUCT},
        {core::TokenType::ASTERISK, PRODUCT},
    };
}

std::unique_ptr<core::Program> Parser::ParseProgram() {
    auto program = std::make_unique<core::Program>();

    while (cur_token_.type != core::TokenType::END_OF_FILE) {
        if (cur_token_.type == core::TokenType::NEWLINE) {
            NextToken();
            continue;
        }
        auto stmt = ParseStatement();
        if (stmt) {
            program->statements.push_back(std::move(stmt));
        }
        NextToken();
    }
    return program;
}

const std::vector<std::string>& Parser::Errors() const {
    return errors_;
}

void Parser::NextToken() {
    cur_token_ = peek_token_;
    if (cur_pos_ < tokens_.size()) {
        peek_token_ = tokens_[cur_pos_];
    } else {
        peek_token_ = {core::TokenType::END_OF_FILE, "", cur_token_.line};
    }
    cur_pos_++;
}

bool Parser::ExpectPeek(core::TokenType type) {
    if (peek_token_.type == type) {
        NextToken();
        return true;
    }
    errors_.push_back("Expected next token to be " + std::to_string((int)type) +
                      ", got " + std::to_string((int)peek_token_.type) + " instead");
    return false;
}

std::unique_ptr<core::Statement> Parser::ParseStatement() {
    switch (cur_token_.type) {
        case core::TokenType::LET:
            return ParseLetStatement();
        case core::TokenType::MUT:
            return ParseMutStatement();
        case core::TokenType::IF:
            return ParseIfStatement();
        default:
            return ParseExpressionStatement();
    }
}

std::unique_ptr<core::LetStatement> Parser::ParseLetStatement() {
    auto stmt = std::make_unique<core::LetStatement>();
    stmt->token = cur_token_;

    if (!ExpectPeek(core::TokenType::IDENTIFIER)) return nullptr;

    stmt->name = std::make_unique<core::Identifier>();
    stmt->name->token = cur_token_;
    stmt->name->value = std::string(cur_token_.literal);

    if (!ExpectPeek(core::TokenType::ASSIGN)) return nullptr;

    NextToken();
    stmt->value = ParseExpression(LOWEST);

    return stmt;
}

std::unique_ptr<core::MutStatement> Parser::ParseMutStatement() {
    auto stmt = std::make_unique<core::MutStatement>();
    stmt->token = cur_token_;

    if (!ExpectPeek(core::TokenType::IDENTIFIER)) return nullptr;

    stmt->name = std::make_unique<core::Identifier>();
    stmt->name->token = cur_token_;
    stmt->name->value = std::string(cur_token_.literal);

    if (!ExpectPeek(core::TokenType::ASSIGN)) return nullptr;

    NextToken();
    stmt->value = ParseExpression(LOWEST);

    return stmt;
}

std::unique_ptr<core::Statement> Parser::ParseExpressionStatement() {
    auto stmt = std::make_unique<core::ExpressionStatement>();
    stmt->token = cur_token_;
    stmt->expression = ParseExpression(LOWEST);
    return stmt;
}

std::unique_ptr<core::Expression> Parser::ParseExpression(Precedence precedence) {
    auto prefix_it = prefix_parse_fns_.find(cur_token_.type);
    if (prefix_it == prefix_parse_fns_.end()) {
        errors_.push_back("No prefix parse function for " + std::string(cur_token_.literal));
        return nullptr;
    }
    auto left_exp = (this->*(prefix_it->second))();

    while (peek_token_.type != core::TokenType::END_OF_FILE && precedence < PeekPrecedence()) {
        auto infix_it = infix_parse_fns_.find(peek_token_.type);
        if (infix_it == infix_parse_fns_.end()) {
            return left_exp;
        }
        NextToken();
        left_exp = (this->*(infix_it->second))(std::move(left_exp));
    }

    return left_exp;
}

std::unique_ptr<core::Expression> Parser::ParseIdentifier() {
    auto ident = std::make_unique<core::Identifier>();
    ident->token = cur_token_;
    ident->value = std::string(cur_token_.literal);
    return ident;
}

std::unique_ptr<core::Expression> Parser::ParseIntegerLiteral() {
    auto literal = std::make_unique<core::IntegerLiteral>();
    literal->token = cur_token_;
    try {
        literal->value = std::stoll(std::string(cur_token_.literal));
    } catch (const std::invalid_argument& e) {
        errors_.push_back("Could not parse " + std::string(cur_token_.literal) + " as integer");
        return nullptr;
    }
    return literal;
}

std::unique_ptr<core::Expression> Parser::ParsePrefixExpression() {
    auto expression = std::make_unique<core::PrefixExpression>();
    expression->token = cur_token_;
    expression->op = std::string(cur_token_.literal);

    NextToken();
    expression->right = ParseExpression(PREFIX);

    return expression;
}

std::unique_ptr<core::Expression> Parser::ParseAssignmentExpression(std::unique_ptr<core::Expression> left) {
    auto expression = std::make_unique<core::AssignmentExpression>();
    expression->token = cur_token_;
    expression->name = std::unique_ptr<core::Identifier>(static_cast<core::Identifier*>(left.release()));

    auto precedence = CurPrecedence();
    NextToken();
    expression->value = ParseExpression(precedence);

    return expression;
}

std::unique_ptr<core::Expression> Parser::ParseInfixExpression(std::unique_ptr<core::Expression> left) {
    auto expression = std::make_unique<core::InfixExpression>();
    expression->token = cur_token_;
    expression->op = std::string(cur_token_.literal);
    expression->left = std::move(left);

    auto precedence = CurPrecedence();
    NextToken();
    expression->right = ParseExpression(precedence);

    return expression;
}

std::unique_ptr<core::Statement> Parser::ParseIfStatement() {
    auto stmt = std::make_unique<core::IfStatement>();
    stmt->token = cur_token_;

    NextToken();
    stmt->condition = ParseExpression(LOWEST);

    while (cur_token_.type == core::TokenType::NEWLINE) {
        NextToken();
    }

    stmt->consequence = ParseBlockStatement();

    if (peek_token_.type == core::TokenType::ELSE) {
        NextToken();
        NextToken();

        if (cur_token_.type == core::TokenType::IF) {
            stmt->alternative = ParseIfStatement();
        } else {
            stmt->alternative = ParseBlockStatement();
        }
    }

    return stmt;
}

std::unique_ptr<core::BlockStatement> Parser::ParseBlockStatement() {
    auto block = std::make_unique<core::BlockStatement>();
    block->token = cur_token_;

    NextToken();

    while (cur_token_.type != core::TokenType::END && cur_token_.type != core::TokenType::END_OF_FILE) {
        if (cur_token_.type == core::TokenType::NEWLINE) {
            NextToken();
            continue;
        }
        auto stmt = ParseStatement();
        if (stmt) {
            block->statements.push_back(std::move(stmt));
        }
        NextToken();
    }

    return block;
}

Precedence Parser::PeekPrecedence() {
    if (precedences_.count(peek_token_.type)) {
        return precedences_[peek_token_.type];
    }
    return LOWEST;
}

Precedence Parser::CurPrecedence() {
    if (precedences_.count(cur_token_.type)) {
        return precedences_[cur_token_.type];
    }
    return LOWEST;
}

void Parser::RegisterPrefix(core::TokenType type, PrefixParseFn fn) {
    prefix_parse_fns_[type] = fn;
}

void Parser::RegisterInfix(core::TokenType type, InfixParseFn fn) {
    infix_parse_fns_[type] = fn;
}

} // namespace parser
} // namespace nota
