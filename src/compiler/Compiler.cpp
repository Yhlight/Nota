#include "compiler/Compiler.hpp"
#include "lexer/Lexer.hpp"

namespace nota {
namespace compiler {

bool Compiler::Compile(const std::string& source, core::Chunk& chunk) {
    lexer::Lexer lexer(source);
    parser::Parser parser(lexer);

    auto program = parser.ParseProgram();
    if (!parser.Errors().empty()) {
        return false;
    }

    // TODO: Implement the compilation logic here.

    return true;
}

} // namespace compiler
} // namespace nota
