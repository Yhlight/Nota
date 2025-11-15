#include <gtest/gtest.h>
#include "parser.hpp"
#include "lexer.hpp"
#include "ast.hpp"

using namespace nota;

// Helper function to check for a binary expression
void checkBinaryExpr(const Expr* expr, TokenType opType) {
    ASSERT_NE(expr, nullptr);
    const auto* binaryExpr = dynamic_cast<const BinaryExpr*>(expr);
    ASSERT_NE(binaryExpr, nullptr);
    EXPECT_EQ(binaryExpr->op.type, opType);
}

// Helper function to check for a literal expression
template<typename T>
void checkLiteralExpr(const Expr* expr, T value) {
    ASSERT_NE(expr, nullptr);
    const auto* literalExpr = dynamic_cast<const LiteralExpr*>(expr);
    ASSERT_NE(literalExpr, nullptr);
    ASSERT_TRUE(std::holds_alternative<T>(literalExpr->value));
    EXPECT_EQ(std::get<T>(literalExpr->value), value);
}

TEST(ParserTest, SimpleAddition) {
    std::string source = "1 + 2";
    Lexer lexer(source);
    std::vector<Token> tokens;
    for (Token token = lexer.scanToken(); token.type != TokenType::END_OF_FILE; token = lexer.scanToken()) {
        tokens.push_back(token);
    }
    tokens.push_back(lexer.scanToken()); // Add EOF

    Parser parser(tokens);
    auto statements = parser.parse();

    ASSERT_EQ(statements.size(), 1);
    const auto* exprStmt = dynamic_cast<const ExpressionStmt*>(statements[0].get());
    ASSERT_NE(exprStmt, nullptr);

    const auto* binaryExpr = dynamic_cast<const BinaryExpr*>(exprStmt->expression.get());
    ASSERT_NE(binaryExpr, nullptr);
    EXPECT_EQ(binaryExpr->op.type, TokenType::PLUS);

    checkLiteralExpr(binaryExpr->left.get(), 1.0);
    checkLiteralExpr(binaryExpr->right.get(), 2.0);
}

TEST(ParserTest, VariableDeclaration) {
    std::string source = "let a = 10";
    Lexer lexer(source);
    std::vector<Token> tokens;
    for (Token token = lexer.scanToken(); token.type != TokenType::END_OF_FILE; token = lexer.scanToken()) {
        tokens.push_back(token);
    }
    tokens.push_back(lexer.scanToken());

    Parser parser(tokens);
    auto statements = parser.parse();

    ASSERT_EQ(statements.size(), 1);
    const auto* varDeclStmt = dynamic_cast<const VarDeclStmt*>(statements[0].get());
    ASSERT_NE(varDeclStmt, nullptr);
    EXPECT_EQ(varDeclStmt->name.lexeme, "a");
    EXPECT_FALSE(varDeclStmt->isMutable);

    checkLiteralExpr(varDeclStmt->initializer.get(), 10.0);
}

TEST(ParserTest, Assignment) {
    std::string source = "a = 20";
    Lexer lexer(source);
    std::vector<Token> tokens;
    for (Token token = lexer.scanToken(); token.type != TokenType::END_OF_FILE; token = lexer.scanToken()) {
        tokens.push_back(token);
    }
    tokens.push_back(lexer.scanToken());

    Parser parser(tokens);
    auto statements = parser.parse();

    ASSERT_EQ(statements.size(), 1);
    const auto* exprStmt = dynamic_cast<const ExpressionStmt*>(statements[0].get());
    ASSERT_NE(exprStmt, nullptr);

    const auto* assignExpr = dynamic_cast<const AssignExpr*>(exprStmt->expression.get());
    ASSERT_NE(assignExpr, nullptr);
    EXPECT_EQ(assignExpr->name.lexeme, "a");

    checkLiteralExpr(assignExpr->value.get(), 20.0);
}
