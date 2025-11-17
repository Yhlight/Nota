#ifndef NOTA_TEST_HELPER_H
#define NOTA_TEST_HELPER_H

#include <string>
#include "ModuleManager.h"

void interpret(const std::string& source);
void interpret_file(const std::string& path);
void interpret_with_search_path(const std::string &source, const std::string &search_path);
nota::ModuleManager &get_module_manager();

#endif // NOTA_TEST_HELPER_H
