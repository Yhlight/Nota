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

    std::string file_path = path;
    if (file_path.rfind(".nota") == std::string::npos) {
        file_path += ".nota";
    }
    return load_module(file_path);
}

void ModuleManager::register_package(const std::string &name, const std::string &path) {
    packages[name] = path;
}

void ModuleManager::add_search_path(const std::string &path) {
    search_paths.push_back(path);
}

void ModuleManager::reset() {
    modules.clear();
    packages.clear();
    search_paths.clear();
}

const std::vector<std::unique_ptr<ast::Stmt>> &ModuleManager::load_module(const std::string &path) {
    if (modules.count(path)) {
        return modules.at(path);
    }

    std::vector<std::string> paths_to_try;
    paths_to_try.push_back(path);
    for (const auto &search_path : search_paths) {
        paths_to_try.push_back(search_path + "/" + path);
    }

    for (const auto &p : paths_to_try) {
        std::ifstream file(p);
        if (file.is_open()) {
            std::stringstream buffer;
            buffer << file.rdbuf();
            std::string source = buffer.str();

            Lexer lexer(source);
            Parser parser(lexer);
            auto statements = parser.parse();

            modules[path] = std::move(statements);
            return modules[path];
        }
    }

    throw std::runtime_error("Could not open file: " + path);
}

} // namespace nota
