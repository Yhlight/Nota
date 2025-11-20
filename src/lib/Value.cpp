#include "Value.h"
#include <iostream>
#include <string>

namespace nota {

void printValue(Value value) {
    if (value.type() == typeid(double)) {
        std::cout << std::any_cast<double>(value);
    } else if (value.type() == typeid(bool)) {
        std::cout << (std::any_cast<bool>(value) ? "true" : "false");
    } else if (value.type() == typeid(nullptr)) {
        std::cout << "nil";
    } else if (value.type() == typeid(std::string)) {
        std::cout << std::any_cast<std::string>(value);
    } else {
        std::cout << "Unknown value type";
    }
}

} // namespace nota
