#include "catch_amalgamated.hpp"
#include "Lexer.hpp"

TEST_CASE("Lexer tests", "[lexer]") {
    SECTION("Test single character tokens") {
        nota::Lexer lexer("+ - * / ( )");
        std::vector<nota::Token> tokens = lexer.scanTokens();

        REQUIRE(tokens.size() == 7);
        REQUIRE(tokens[0].type == nota::TokenType::PLUS);
        REQUIRE(tokens[1].type == nota::TokenType::MINUS);
        REQUIRE(tokens[2].type == nota::TokenType::STAR);
        REQUIRE(tokens[3].type == nota::TokenType::SLASH);
        REQUIRE(tokens[4].type == nota::TokenType::LEFT_PAREN);
        REQUIRE(tokens[5].type == nota::TokenType::RIGHT_PAREN);
        REQUIRE(tokens[6].type == nota::TokenType::END_OF_FILE);
    }

    SECTION("Test number literals") {
        nota::Lexer lexer("123 3.14");
        std::vector<nota::Token> tokens = lexer.scanTokens();

        REQUIRE(tokens.size() == 3);
        REQUIRE(tokens[0].type == nota::TokenType::NUMBER);
        REQUIRE(tokens[0].lexeme == "123");
        REQUIRE(tokens[1].type == nota::TokenType::NUMBER);
        REQUIRE(tokens[1].lexeme == "3.14");
        REQUIRE(tokens[2].type == nota::TokenType::END_OF_FILE);
    }

    SECTION("Test identifier literals") {
        nota::Lexer lexer("my_variable _another_var func1");
        std::vector<nota::Token> tokens = lexer.scanTokens();

        REQUIRE(tokens.size() == 4);
        REQUIRE(tokens[0].type == nota::TokenType::IDENTIFIER);
        REQUIRE(tokens[0].lexeme == "my_variable");
        REQUIRE(tokens[1].type == nota::TokenType::IDENTIFIER);
        REQUIRE(tokens[1].lexeme == "_another_var");
        REQUIRE(tokens[2].type == nota::TokenType::IDENTIFIER);
        REQUIRE(tokens[2].lexeme == "func1");
        REQUIRE(tokens[3].type == nota::TokenType::END_OF_FILE);
    }

    SECTION("Test a mix of tokens") {
        nota::Lexer lexer("x = 10 + (y * 2)");
        // The lexer doesn't know about '=' yet, so it will be unexpected.
        std::vector<nota::Token> tokens = lexer.scanTokens();

        REQUIRE(tokens.size() == 10);
        REQUIRE(tokens[0].type == nota::TokenType::IDENTIFIER);
        REQUIRE(tokens[0].lexeme == "x");
        REQUIRE(tokens[1].type == nota::TokenType::UNEXPECTED);
        REQUIRE(tokens[1].lexeme == "=");
        REQUIRE(tokens[2].type == nota::TokenType::NUMBER);
        REQUIRE(tokens[2].lexeme == "10");
        REQUIRE(tokens[3].type == nota::TokenType::PLUS);
        REQUIRE(tokens[4].type == nota::TokenType::LEFT_PAREN);
        REQUIRE(tokens[5].type == nota::TokenType::IDENTIFIER);
        REQUIRE(tokens[5].lexeme == "y");
        REQUIRE(tokens[6].type == nota::TokenType::STAR);
        REQUIRE(tokens[7].type == nota::TokenType::NUMBER);
        REQUIRE(tokens[7].lexeme == "2");
        REQUIRE(tokens[8].type == nota::TokenType::RIGHT_PAREN);
        REQUIRE(tokens[9].type == nota::TokenType::END_OF_FILE);
    }

    SECTION("Test unexpected characters") {
        nota::Lexer lexer("$ # @");
        std::vector<nota::Token> tokens = lexer.scanTokens();

        REQUIRE(tokens.size() == 4);
        REQUIRE(tokens[0].type == nota::TokenType::UNEXPECTED);
        REQUIRE(tokens[0].lexeme == "$");
        REQUIRE(tokens[1].type == nota::TokenType::UNEXPECTED);
        REQUIRE(tokens[1].lexeme == "#");
        REQUIRE(tokens[2].type == nota::TokenType::UNEXPECTED);
        REQUIRE(tokens[2].lexeme == "@");
        REQUIRE(tokens[3].type == nota::TokenType::END_OF_FILE);
    }
}
