#include <iostream>
#include <fstream>
#include <sstream>
#include "lexer/Lexer.h"
#include "parser/Parser.h"
#include "analyzer/SemanticAnalyzer.h"
#include "codegen/CodeGenerator.h"

int main(int argc, char* argv[]) {
    if (argc < 2 || argc > 3) {
        std::cerr << "Usage: " << argv[0] << " <input_file.nota> [output_file.html]" << std::endl;
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

    Lexer lexer(source);
    Parser parser(lexer);
    RootNode ast = parser.parse();

    if (!parser.errors().empty()) {
        std::cerr << "Parser errors:\n";
        for (const auto& error : parser.errors()) {
            std::cerr << error << std::endl;
        }
        return 1;
    }

    SemanticAnalyzer analyzer;
    if (!analyzer.analyze(ast)) {
        std::cerr << "Semantic errors:\n";
        for (const auto& error : analyzer.errors()) {
            std::cerr << error << std::endl;
        }
        return 1;
    }

    CodeGenerator generator;
    std::string html_output = generator.generate(ast);

    std::string output_filename = (argc == 3) ? argv[2] : "output.html";
    std::ofstream output_file(output_filename);
    if (!output_file) {
        std::cerr << "Error: Could not open output file " << output_filename << std::endl;
        return 1;
    }

    output_file << html_output;
    std::cout << "Successfully compiled " << argv[1] << " to " << output_filename << std::endl;

    return 0;
}
