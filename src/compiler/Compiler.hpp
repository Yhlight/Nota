#pragma once

#include "core/Chunk.hpp"
#include "parser/Parser.hpp"
#include <string>

namespace nota {
namespace compiler {

class Compiler {
public:
    bool Compile(const std::string& source, core::Chunk& chunk);

private:
};

} // namespace compiler
} // namespace nota
