#pragma once

#include "ast.h"
#include <string>
#include <memory>

namespace Compiler {
    std::shared_ptr<ProgramNode> parseFile(const std::string& path);
}
