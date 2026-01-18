#pragma once
#include "AST.h"
#include <string>

class CodeGen {
public:
    std::string generateHTML(const ComponentNode& root);
    std::string generateCSS(const ComponentNode& root);
};
