#pragma once

#include "Interpreter.h"
#include <memory>
#include <string>

namespace nota {

class ModuleLoader {
public:
    ModuleLoader(Interpreter& interpreter);

    std::shared_ptr<Interpreter> load(const std::string& path, VM& vm);

private:
    Interpreter& interpreter_;
};

} // namespace nota
