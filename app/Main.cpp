#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include "Lexer.h"
#include "Parser.h"
#include "Resolver.h"
#include "Evaluator.h"
#include "CodeGenerator.h"

int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cerr << "Usage: nota-compiler <input_file> <output_basename>" << std::endl;
        return 1;
    }

    std::ifstream input_file(argv[1]);
    if (!input_file) {
        std::cerr << "Error: Could not open input file " << argv[1] << std::endl;
        return 1;
    }

    std::stringstream buffer;
    buffer << input_file.rdbuf();
    std::string source = buffer.str();

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

    // Write output files
    std::string base_name = argv[2];
    std::ofstream html_file(base_name + ".html");
    html_file << result.html;

    std::ofstream css_file(base_name + ".css");
    css_file << result.css;

    std::cout << "Compilation successful. Output written to " << base_name << ".html and " << base_name << ".css" << std::endl;

    return 0;
}
