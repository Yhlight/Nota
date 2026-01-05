#include <iostream>
#include <fstream>
#include <sstream>
#include "Nota.h"

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

    Nota nota;
    std::string html_output;
    std::string css_output;
    nota.run(source, html_output, css_output);

    // Write output files
    std::string base_name = argv[2];
    std::ofstream html_file(base_name + ".html");
    html_file << html_output;

    std::ofstream css_file(base_name + ".css");
    css_file << css_output;

    std::cout << "Compilation successful. Output written to " << base_name << ".html and " << base_name << ".css" << std::endl;

    return 0;
}
