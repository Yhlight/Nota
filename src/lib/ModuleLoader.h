#pragma once

#include "Interpreter.h"
#include <memory>
#include <string>

namespace nota {

class ModuleLoader {
public:
    ModuleLoader(Interpreter& interpreter);

    std::shared_ptr<Environment> load(const std::string& path);

private:
    Interpreter& interpreter_;
};

} // namespace nota
