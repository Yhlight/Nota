#pragma once
#include <string>

namespace Utils {
    std::string escapeHTML(const std::string& data);
    std::string resolveImportPath(const std::string& path);
}
