#pragma once

#include "core/NotaValue.hpp"
#include "core/OpCode.hpp"
#include <vector>

namespace nota {
namespace core {

class Chunk {
public:
    void Write(uint8_t byte, int line);
    size_t AddConstant(NotaValue value);

    const std::vector<uint8_t>& GetCode() const { return code_; }
    const std::vector<NotaValue>& GetConstants() const { return constants_; }
    int GetLine(size_t offset) const;

private:
    std::vector<uint8_t> code_;
    std::vector<NotaValue> constants_;
    std::vector<int> lines_;
};

} // namespace core
} // namespace nota
