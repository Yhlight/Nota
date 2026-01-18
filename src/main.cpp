#include <iostream>
#include <fstream>
#include <sstream>
#include "Lexer.h"
#include "Parser.h"
#include "CodeGen.h"

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: nota <input_file>" << std::endl;
        return 1;
    }

    std::string filepath = argv[1];
    std::ifstream file(filepath);
    if (!file.is_open()) {
        std::cerr << "Error: Could not open file " << filepath << std::endl;
        return 1;
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string source = buffer.str();

    try {
        Lexer lexer(source);
        auto tokens = lexer.tokenize();

        Parser parser(tokens);
        auto nodes = parser.parseAll();

        if (!nodes.empty()) {
            CodeGen codegen;
            std::string htmlContent;
            std::string cssContent;

            // Generate content for all top-level nodes (usually just App, but could be others)
            // Note: Components defined but not used are filtered out or handled by registry.
            // Nodes returned here are top-level instances.

            for (const auto& node : nodes) {
                htmlContent += codegen.generateHTML(*node) + "\n";
                // Base CSS is static for now, so we just need it once.
                // But if we had per-component CSS generation (not inline), we'd merge it.
            }
            // Just get base CSS from the first node or generic.
            cssContent = codegen.generateCSS(*nodes[0]);

            std::ofstream outFile("index.html");
            outFile << "<!DOCTYPE html>\n<html>\n<head>\n";
            outFile << "<style>\n" << cssContent << "\n</style>\n";
            outFile << "</head>\n";
            outFile << htmlContent << "\n";
            outFile << "</html>\n";

            std::cout << "Successfully compiled to index.html" << std::endl;
        } else {
            // It's possible to have a valid file with only definitions and no App,
            // but for now we consider it "Parsing failed" or "No output".
            std::cout << "No top-level components to render." << std::endl;
        }

    } catch (const std::exception& e) {
        std::cerr << "Compilation Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
