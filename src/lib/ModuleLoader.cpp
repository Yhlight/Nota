#include "ModuleLoader.h"
#include "Lexer.h"
#include "Parser.h"
#include <fstream>
#include <sstream>

namespace nota {

ModuleLoader::ModuleLoader(Interpreter& interpreter)
    : interpreter_(interpreter) {}

std::shared_ptr<Environment> ModuleLoader::load(const std::string& path) {
    std::ifstream file(path);
    if (!file) {
        throw std::runtime_error("Could not open file: " + path);
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string source = buffer.str();

    Lexer lexer(source);
    auto tokens = lexer.scanTokens();
    Parser parser(tokens);
    auto statements = parser.parse();

    auto module_interpreter = std::make_shared<Interpreter>();
    module_interpreter->interpret(statements);

    return module_interpreter->getEnvironment();
}

} // namespace nota
