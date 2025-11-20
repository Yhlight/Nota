#pragma once

#include <vector>

namespace nota {

enum class OpCode {
    // ... to be defined
};

class VM {
public:
    VM();
    void execute(const std::vector<OpCode>& bytecode);
};

} // namespace nota
