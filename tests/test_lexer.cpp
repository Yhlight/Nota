#include "vendor/doctest.h"
#include "Lexer.h"

TEST_CASE("Lexer tokenizes a simple let statement") {
    std::string source = "let a = 10";
    nota::Lexer lexer(source);
    std::vector<nota::Token> tokens = lexer.scanTokens();

    REQUIRE(tokens.size() == 5);
    CHECK(tokens[0].type == nota::TokenType::LET);
    CHECK(tokens[1].type == nota::TokenType::IDENTIFIER);
    CHECK(tokens[1].lexeme == "a");
    CHECK(tokens[2].type == nota::TokenType::ASSIGN);
    CHECK(tokens[3].type == nota::TokenType::INTEGER);
    CHECK(std::get<int>(tokens[3].literal) == 10);
    CHECK(tokens[4].type == nota::TokenType::END_OF_FILE);
}

TEST_CASE("Lexer tokenizes all single-character tokens") {
    std::string source = "(){}[],:;+-*/%";
    nota::Lexer lexer(source);
    std::vector<nota::Token> tokens = lexer.scanTokens();

    REQUIRE(tokens.size() == 15);
    CHECK(tokens[0].type == nota::TokenType::LPAREN);
    CHECK(tokens[1].type == nota::TokenType::RPAREN);
    CHECK(tokens[2].type == nota::TokenType::LBRACE);
    CHECK(tokens[3].type == nota::TokenType::RBRACE);
    CHECK(tokens[4].type == nota::TokenType::LBRACKET);
    CHECK(tokens[5].type == nota::TokenType::RBRACKET);
    CHECK(tokens[6].type == nota::TokenType::COMMA);
    CHECK(tokens[7].type == nota::TokenType::COLON);
    CHECK(tokens[8].type == nota::TokenType::SEMICOLON);
    CHECK(tokens[9].type == nota::TokenType::PLUS);
    CHECK(tokens[10].type == nota::TokenType::MINUS);
    CHECK(tokens[11].type == nota::TokenType::STAR);
    CHECK(tokens[12].type == nota::TokenType::SLASH);
    CHECK(tokens[13].type == nota::TokenType::PERCENT);
}

TEST_CASE("Lexer tokenizes compound assignment operators") {
    std::string source = "+= -= *= /= %=";
    nota::Lexer lexer(source);
    std::vector<nota::Token> tokens = lexer.scanTokens();

    REQUIRE(tokens.size() == 6);
    CHECK(tokens[0].type == nota::TokenType::PLUS_ASSIGN);
    CHECK(tokens[1].type == nota::TokenType::MINUS_ASSIGN);
    CHECK(tokens[2].type == nota::TokenType::STAR_ASSIGN);
    CHECK(tokens[3].type == nota::TokenType::SLASH_ASSIGN);
    CHECK(tokens[4].type == nota::TokenType::PERCENT_ASSIGN);
    CHECK(tokens[5].type == nota::TokenType::END_OF_FILE);
}

TEST_CASE("Lexer tokenizes multi-character tokens") {
    std::string source = "== != < > <= >=";
    nota::Lexer lexer(source);
    std::vector<nota::Token> tokens = lexer.scanTokens();

    REQUIRE(tokens.size() == 7);
    CHECK(tokens[0].type == nota::TokenType::EQUALS);
    CHECK(tokens[1].type == nota::TokenType::NOT_EQUALS);
    CHECK(tokens[2].type == nota::TokenType::LESS);
    CHECK(tokens[3].type == nota::TokenType::GREATER);
    CHECK(tokens[4].type == nota::TokenType::LESS_EQUALS);
    CHECK(tokens[5].type == nota::TokenType::GREATER_EQUALS);
    CHECK(tokens[6].type == nota::TokenType::END_OF_FILE);
}

TEST_CASE("Lexer tokenizes string literals") {
    std::string source = "\"hello world\"";
    nota::Lexer lexer(source);
    std::vector<nota::Token> tokens = lexer.scanTokens();

    REQUIRE(tokens.size() == 2);
    CHECK(tokens[0].type == nota::TokenType::STRING);
    CHECK(tokens[0].lexeme == "\"hello world\"");
    CHECK(std::get<std::string>(tokens[0].literal) == "hello world");
    CHECK(tokens[1].type == nota::TokenType::END_OF_FILE);
}

TEST_CASE("Lexer handles invalid characters") {
    std::string source = "#";
    nota::Lexer lexer(source);
    lexer.scanTokens();
    CHECK(lexer.hadError());
}

TEST_CASE("Lexer tokenizes newlines") {
    std::string source = "let a = 1\nlet b = 2";
    nota::Lexer lexer(source);
    std::vector<nota::Token> tokens = lexer.scanTokens();

    // let, a, =, 1, \n, let, b, =, 2, EOF
    REQUIRE(tokens.size() == 10);
    CHECK(tokens[3].type == nota::TokenType::INTEGER);
    CHECK(tokens[4].type == nota::TokenType::NEWLINE);
    CHECK(tokens[5].type == nota::TokenType::LET);
}

TEST_CASE("Lexer tokenizes ! and !=") {
    std::string source = "! !=";
    nota::Lexer lexer(source);
    std::vector<nota::Token> tokens = lexer.scanTokens();

    REQUIRE(tokens.size() == 3);
    CHECK(tokens[0].type == nota::TokenType::NOT);
    CHECK(tokens[1].type == nota::TokenType::NOT_EQUALS);
    CHECK(tokens[2].type == nota::TokenType::END_OF_FILE);
}

TEST_CASE("Lexer handles unterminated strings") {
    std::string source = "\"hello world";
    nota::Lexer lexer(source);
    lexer.scanTokens();
    CHECK(lexer.hadError());
}
