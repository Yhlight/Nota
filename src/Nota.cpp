#include "Nota.h"
#include "Lexer.h"
#include "Parser.h"
#include "Resolver.h"
#include "Evaluator.h"
#include "CodeGenerator.h"

Nota::Nota() {}

void Nota::run(const std::string& source, std::string& html_output, std::string& css_output) {
    // 1. Lexer
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.scan_tokens();

    // 2. Parser
    Parser parser(tokens);
    std::vector<std::unique_ptr<Stmt>> statements = parser.parse();

    // 3. Resolver
    Resolver resolver;
    resolver.resolve(statements);

    // 4. Evaluator
    Evaluator evaluator;
    evaluator.evaluate(statements);

    // 5. Code Generator
    CodeGenerator generator(resolver.get_custom_types(), evaluator.get_results());
    CompilationResult result = generator.generate(statements);

    html_output = result.html;
    css_output = result.css;
}
