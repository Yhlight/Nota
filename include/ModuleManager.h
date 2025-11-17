#ifndef NOTA_MODULE_MANAGER_H
#define NOTA_MODULE_MANAGER_H

#include "AST/Stmt.h"
#include <map>
#include <string>
#include <memory>
#include <vector>

namespace nota {

class ModuleManager {
public:
    const std::vector<std::unique_ptr<ast::Stmt>> &resolve_import(const std::string &path);
    void register_package(const std::string &name, const std::string &path);
    void add_search_path(const std::string &path);
    void reset();

private:
    const std::vector<std::unique_ptr<ast::Stmt>> &load_module(const std::string &path);
    std::map<std::string, std::vector<std::unique_ptr<ast::Stmt>>> modules;
    std::map<std::string, std::string> packages;
    std::vector<std::string> search_paths;
};

} // namespace nota

#endif // NOTA_MODULE_MANAGER_H
