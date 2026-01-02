#pragma once

#include <string>

struct CompilerError {
    std::string message;
    int line;
    int column;
};
