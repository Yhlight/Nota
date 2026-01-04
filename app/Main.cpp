#include <iostream>

int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cerr << "Usage: nota-compiler <input_file> <output_file>" << std::endl;
        return 1;
    }

    std::cout << "Input file: " << argv[1] << std::endl;
    std::cout << "Output file: " << argv[2] << std::endl;

    return 0;
}
