#pragma once

#include "OpCode.h"
#include <vector>
#include <variant>

using Value = std::variant<double, bool, std::nullptr_t>;

class Chunk {
public:
    void write(uint8_t byte, int line);
    size_t add_constant(Value value);

    const std::vector<uint8_t>& get_code() const;
    const std::vector<Value>& get_constants() const;
    int get_line(size_t offset) const;

private:
    std::vector<uint8_t> code_;
    std::vector<Value> constants_;
    std::vector<int> lines_;
};
