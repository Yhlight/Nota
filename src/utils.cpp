#include "utils.h"
#include <sstream>

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

}
