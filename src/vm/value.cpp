#include "value.hpp"
#include <iostream>

void print_value(Value value) {
    std::visit([](auto&& arg) {
        std::cout << arg;
    }, value);
}
