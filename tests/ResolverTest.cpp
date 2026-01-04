#include <gtest/gtest.h>
#include "Lexer.h"
#include "Parser.h"
#include "Resolver.h"
#include <vector>
#include <memory>

TEST(ResolverTest, SimpleComponentWithId) {
    std::string source = "App { id: myApp }";
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.scan_tokens();
    Parser parser(tokens);
    std::vector<std::unique_ptr<Stmt>> statements = parser.parse();

    Resolver resolver;
    // The test passes if this doesn't throw an exception or crash.
    // A more robust test would inspect the resolver's state, but for now,
    // this verifies the traversal logic doesn't have obvious errors.
    ASSERT_NO_THROW(resolver.resolve(statements));
}

TEST(ResolverTest, PropertyReference) {
    std::string source = "App { id: myApp width: myApp.width }";
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.scan_tokens();
    Parser parser(tokens);
    std::vector<std::unique_ptr<Stmt>> statements = parser.parse();

    Resolver resolver;
    ASSERT_NO_THROW(resolver.resolve(statements));
}
