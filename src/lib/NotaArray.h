#pragma once

#include <vector>
#include <any>
#include <string>

class NotaArray {
public:
    std::vector<std::any> elements;

    std::string toString() const {
        std::string result = "[";
        for (size_t i = 0; i < elements.size(); ++i) {
            // This is a simplification. A full implementation would need
            // to convert each element's 'any' to a string representation.
            result += "element";
            if (i < elements.size() - 1) {
                result += ", ";
            }
        }
        result += "]";
        return result;
    }
};
