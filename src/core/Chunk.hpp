#ifndef NOTA_CHUNK_HPP
#define NOTA_CHUNK_HPP

#include "OpCode.hpp"
#include <vector>

namespace nota {
namespace core {

class NotaValue; // Forward declaration

class Chunk {
public:
    void Write(uint8_t byte);
    size_t AddConstant(const NotaValue& value);

    std::vector<uint8_t> code;
    std::vector<NotaValue> constants;
};

} // namespace core
} // namespace nota

#endif // NOTA_CHUNK_HPP
