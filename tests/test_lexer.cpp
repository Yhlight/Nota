#define DOCTEST_CONFIG_IMPLEMENT
#include "doctest.h"
#include "Lexer.h"
#include "Token.h"
#include <vector>

using namespace nota;

TEST_CASE("Lexer can tokenize single-character tokens") {
    std::string source = "(){}[],.-+*/%:";
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.scanTokens();

    std::vector<TokenType> expected_types = {
        TokenType::LEFT_PAREN, TokenType::RIGHT_PAREN,
        TokenType::LEFT_BRACE, TokenType::RIGHT_BRACE,
        TokenType::LEFT_BRACKET, TokenType::RIGHT_BRACKET,
        TokenType::COMMA, TokenType::DOT, TokenType::MINUS,
        TokenType::PLUS, TokenType::STAR, TokenType::SLASH,
        TokenType::MODULO, TokenType::COLON, TokenType::END_OF_FILE
    };

    REQUIRE(tokens.size() == expected_types.size());

    for (size_t i = 0; i < tokens.size(); ++i) {
        CHECK(tokens[i].type == expected_types[i]);
    }
}

TEST_CASE("Lexer can tokenize multi-character tokens") {
    std::string source = "! != = == > >= < <= += -= *= /= %= ++ --";
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.scanTokens();

    std::vector<TokenType> expected_types = {
        TokenType::BANG, TokenType::BANG_EQUAL,
        TokenType::EQUAL, TokenType::EQUAL_EQUAL,
        TokenType::GREATER, TokenType::GREATER_EQUAL,
        TokenType::LESS, TokenType::LESS_EQUAL,
        TokenType::PLUS_EQUAL, TokenType::MINUS_EQUAL,
        TokenType::STAR_EQUAL, TokenType::SLASH_EQUAL,
        TokenType::MODULO_EQUAL,
        TokenType::PLUS_PLUS, TokenType::MINUS_MINUS,
        TokenType::END_OF_FILE
    };

    REQUIRE(tokens.size() == expected_types.size());

    for (size_t i = 0; i < tokens.size(); ++i) {
        CHECK(tokens[i].type == expected_types[i]);
    }
}

TEST_CASE("Lexer can tokenize literals") {
    std::string source = "\"hello\" 123 12.34";
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.scanTokens();

    std::vector<TokenType> expected_types = {
        TokenType::STRING, TokenType::INTEGER, TokenType::FLOAT,
        TokenType::END_OF_FILE
    };

    REQUIRE(tokens.size() == expected_types.size());

    CHECK(tokens[0].type == expected_types[0]);
    CHECK(tokens[0].lexeme == "\"hello\"");
    CHECK(tokens[1].type == expected_types[1]);
    CHECK(tokens[1].lexeme == "123");
    CHECK(tokens[2].type == expected_types[2]);
    CHECK(tokens[2].lexeme == "12.34");
}

TEST_CASE("Lexer can tokenize keywords") {
    std::string source = "let mut if else end match while for do fn return class new this import as package none true false";
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.scanTokens();

    std::vector<TokenType> expected_types = {
        TokenType::LET, TokenType::MUT, TokenType::IF, TokenType::ELSE,
        TokenType::END, TokenType::MATCH, TokenType::WHILE, TokenType::FOR,
        TokenType::DO, TokenType::FN, TokenType::RETURN, TokenType::CLASS,
        TokenType::NEW, TokenType::THIS, TokenType::IMPORT, TokenType::AS,
        TokenType::PACKAGE, TokenType::NONE, TokenType::TRUE, TokenType::FALSE,
        TokenType::END_OF_FILE
    };

    REQUIRE(tokens.size() == expected_types.size());

    for (size_t i = 0; i < tokens.size(); ++i) {
        CHECK(tokens[i].type == expected_types[i]);
    }
}

TEST_CASE("Lexer can handle comments") {
    std::string source = R"(
        // this is a comment
        let a = 10 // another comment
        /* this is a
           multi-line comment */
        let b = 20
    )";
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.scanTokens();

    std::vector<TokenType> expected_types = {
        TokenType::NEWLINE,
        TokenType::NEWLINE,
        TokenType::LET, TokenType::IDENTIFIER, TokenType::EQUAL, TokenType::INTEGER,
        TokenType::NEWLINE,
        TokenType::NEWLINE,
        TokenType::LET, TokenType::IDENTIFIER, TokenType::EQUAL, TokenType::INTEGER,
        TokenType::NEWLINE,
        TokenType::END_OF_FILE
    };

    REQUIRE(tokens.size() == expected_types.size());

    for (size_t i = 0; i < tokens.size(); ++i) {
        CHECK(tokens[i].type == expected_types[i]);
    }
}

int main(int argc, char** argv) {
    doctest::Context context;
    context.applyCommandLine(argc, argv);
    int res = context.run();
    if (context.shouldExit()) {
        return res;
    }
    return res;
}
