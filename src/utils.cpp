#include "utils.h"
#include <sstream>
#include <algorithm>

namespace Utils {

std::string escapeHTML(const std::string& str) {
    std::stringstream ss;
    for (char c : str) {
        switch (c) {
            case '"': ss << "&quot;"; break;
            case '&': ss << "&amp;"; break;
            case '<': ss << "&lt;"; break;
            case '>': ss << "&gt;"; break;
            default: ss << c; break;
        }
    }
    return ss.str();
}

std::string resolveImportPath(const std::string& path) {
    // If path ends with .nota, it's a file path
    if (path.size() >= 5 && path.substr(path.size() - 5) == ".nota") {
        return path;
    }
    // Otherwise, assume it's a dotted module path: A.B.C -> A/B/C.nota
    std::string resolved = path;
    std::replace(resolved.begin(), resolved.end(), '.', '/');
    return resolved + ".nota";
}

}
