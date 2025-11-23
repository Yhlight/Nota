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
    std::string full_path = path;
    if (full_path.rfind(".nota") == std::string::npos) {
        full_path += ".nota";
    }

    // TODO: Implement package-based resolution
    // For now, assume path is relative to current working directory

    std::ifstream file(full_path);
    if (!file) {
        throw std::runtime_error("Could not open file: " + full_path);
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
