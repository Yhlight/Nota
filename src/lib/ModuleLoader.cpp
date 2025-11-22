#include "ModuleLoader.h"
#include "Lexer.h"
#include "Parser.h"
#include <fstream>
#include <sstream>
#include <memory>
#include "Interpreter.h"

namespace nota {

ModuleLoader::ModuleLoader(Interpreter& interpreter)
    : interpreter_(interpreter) {}

std::shared_ptr<Interpreter> ModuleLoader::load(const std::string& path, VM& vm) {
    // TODO: Implement full module path resolution, including package name.
    // std::string package_name = interpreter_.getPackageName();
    // std::string full_path = resolve_path(package_name, path);
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

    auto module_interpreter = std::make_shared<Interpreter>(vm);
    module_interpreter->interpret(std::move(statements));

    return module_interpreter;
}

} // namespace nota
