#include "Nota.h"
#include <iostream>
#include <string>

void runPrompt() {
    std::cout << "> ";
    std::string line;
    while (std::getline(std::cin, line)) {
        Nota::run(line);
        Nota::hadError = false;
        std::cout << "> ";
    }
}

int main(int argc, char* argv[]) {
    if (argc > 2) {
        std::cout << "Usage: nota [script]" << std::endl;
        return 64;
    } else if (argc == 2) {
        // We don't support running files yet, but we will in the future.
    } else {
        runPrompt();
    }
    return 0;
}
