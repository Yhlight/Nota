#include "ModuleManager.h"
#include "Lexer.h"
#include "Parser.h"
#include <fstream>
#include <sstream>
#include <vector>

namespace nota {

const std::vector<std::unique_ptr<ast::Stmt>> &ModuleManager::load_module(const std::string &path) {
    if (modules.count(path)) {
        return modules.at(path);
    }

    std::ifstream file(path);
    if (!file.is_open()) {
        throw std::runtime_error("Could not open file: " + path);
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string source = buffer.str();

    Lexer lexer(source);
    Parser parser(lexer);
    auto statements = parser.parse();

    modules[path] = std::move(statements);
    return modules[path];
}

} // namespace nota
