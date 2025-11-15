#include "vm/vm.hpp"
#include <iostream>
#include <string>
#include <vector>

int main(int argc, char* argv[]) {
    nota::VM vm;
    std::string line;
    std::cout << "> ";
    while (std::getline(std::cin, line)) {
        vm.interpret(line);
        std::cout << "> ";
    }
    return 0;
}
