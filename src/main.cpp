#include <iostream>
#include <string>
#include <vector>
#include "Lexer.hpp"
#include "Parser.hpp"
#include "Generator.hpp"

int main() {
    std::string source = R"(
        App {
            Row {
                width: "100%"
                spacing: 20
                color: "#f0f0f0"

                Rect {
                    width: 50
                    height: 50
                    color: "blue"
                }
                Text {
                    text: "Welcome to Nota!"
                }
            }
        }
    )";

    try {
        Lexer lexer(source);
        std::vector<Token> tokens = lexer.scanTokens();

        Parser parser(tokens);
        std::unique_ptr<Program> program = parser.parse();

        if (program) {
            Generator generator;
            GeneratedCode result = generator.generate(*program);

            std::cout << "--- GENERATED HTML ---" << std::endl;
            std::cout << result.html << std::endl;
            std::cout << "\n--- GENERATED CSS ---" << std::endl;
            std::cout << result.css << std::endl;
        } else {
            std::cerr << "Failed to parse the program." << std::endl;
            return 1;
        }

    } catch (const std::exception& e) {
        std::cerr << "Compilation error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
