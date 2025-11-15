#pragma once

#include "bytecode.hpp"
#include "value.hpp"
#include <vector>

class Chunk {
public:
    void write(uint8_t byte, int line);
    int add_constant(Value value);

    std::vector<uint8_t>& get_code();
    std::vector<Value>& get_constants();
    std::vector<int>& get_lines();

private:
    std::vector<uint8_t> code;
    std::vector<Value> constants;
    std::vector<int> lines;
};
