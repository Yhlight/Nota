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
        auto root = parser.parse();

        if (root) {
            CodeGen codegen;
            std::string htmlContent = codegen.generateHTML(*root);
            std::string cssContent = codegen.generateCSS(*root);

            std::ofstream outFile("index.html");
            outFile << "<!DOCTYPE html>\n<html>\n<head>\n";
            outFile << "<style>\n" << cssContent << "\n</style>\n";
            outFile << "</head>\n";
            outFile << htmlContent << "\n";
            outFile << "</html>\n";

            std::cout << "Successfully compiled to index.html" << std::endl;
        } else {
            std::cerr << "Error: Parsing failed." << std::endl;
            return 1;
        }

    } catch (const std::exception& e) {
        std::cerr << "Compilation Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
