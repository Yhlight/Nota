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
    const std::vector<std::unique_ptr<ast::Stmt>> &load_module(const std::string &path);

private:
    std::map<std::string, std::vector<std::unique_ptr<ast::Stmt>>> modules;
};

} // namespace nota

#endif // NOTA_MODULE_MANAGER_H
