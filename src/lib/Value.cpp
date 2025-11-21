#include "Value.h"
#include "Object.h"
#include <iostream>
#include <string>
#include <memory>

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
    } else if (value.type() == typeid(std::shared_ptr<NotaFunction>)) {
        std::cout << "<fn " << std::any_cast<std::shared_ptr<NotaFunction>>(value)->name << ">";
    }
    else {
        std::cout << "Unknown value type";
    }
}

} // namespace nota
