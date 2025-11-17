#include "ModuleManager.h"
#include "Lexer.h"
#include "Parser.h"
#include <fstream>
#include <sstream>
#include <vector>

namespace nota {

const std::vector<std::unique_ptr<ast::Stmt>> &ModuleManager::resolve_import(const std::string &path) {
    size_t double_colon_pos = path.find("::");
    if (double_colon_pos != std::string::npos) {
        std::string package_name = path.substr(0, double_colon_pos);
        std::string module_name = path.substr(double_colon_pos + 2);
        if (packages.count(package_name)) {
            std::string package_path = packages.at(package_name);
            std::string module_path = package_path + "/" + module_name + ".nota";
            return load_module(module_path);
        } else {
            throw std::runtime_error("Package not found: " + package_name);
        }
    }
    return load_module(path);
}

void ModuleManager::register_package(const std::string &name, const std::string &path) {
    packages[name] = path;
}

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
