#pragma once

#include <string>

struct SourceLocation {
    std::string file_path;
    int line = 1;
    int column = 1;
};
