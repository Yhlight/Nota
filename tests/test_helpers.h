#pragma once

#include "Interpreter.h"
#include "Lexer.h"
#include "Parser.h"
#include "VM.h"
#include "NotaObjects.h"
#include <string>
#include <filesystem>
#include <fstream>
#include <stdexcept>
#include <variant>

// A variant type for test results to avoid returning dangling Object pointers.
using TestResult = std::variant<std::monostate, int, double, bool, std::string>;

namespace doctest {
template <>
struct StringMaker<TestResult> {
    static String convert(const TestResult& value) {
        std::ostringstream oss;
        std::visit([&](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::monostate>) {
                oss << "monostate";
            } else if constexpr (std::is_same_v<T, std::string>) {
                oss << '"' << arg << '"';
            } else {
                oss << arg;
            }
        }, value);
        return oss.str().c_str();
    }
};
} // namespace doctest


// Helper function to run a piece of Nota code and return the result.
inline TestResult run(const std::string& source_code) {
    nota::VM vm;
    nota::Interpreter interpreter(vm);

    nota::Lexer lexer(source_code);
    auto tokens = lexer.scanTokens();
    if (lexer.hadError()) {
        throw std::runtime_error("Lexer error");
    }

    nota::Parser parser(tokens);
    auto stmts = parser.parse();
    if (parser.hadError()) {
        throw std::runtime_error("Parser error");
    }

    nota::Value result_value;
    try {
        interpreter.interpret(stmts);
        // If the code doesn't explicitly return, get the value of 'result'
        if (interpreter.getEnvironment()->isDefined("result")) {
             result_value = interpreter.getEnvironment()->get({nota::TokenType::IDENTIFIER, "result", {}, -1});
        }
    } catch (const nota::Interpreter::ReturnControl& r) {
        result_value = r.value;
    } catch (const nota::Interpreter::RuntimeError&) {
        throw; // Re-throw runtime errors for CHECK_THROWS
    }

    // Convert nota::Value to TestResult to safely transport values out of the VM's scope.
    if (std::holds_alternative<std::monostate>(result_value)) return std::monostate{};
    if (std::holds_alternative<int>(result_value)) return std::get<int>(result_value);
    if (std::holds_alternative<double>(result_value)) return std::get<double>(result_value);
    if (std::holds_alternative<bool>(result_value)) return std::get<bool>(result_value);
    if (std::holds_alternative<nota::Object*>(result_value)) {
        auto* obj = std::get<nota::Object*>(result_value);
        if (obj && obj->type == nota::ObjectType::STRING) {
            return static_cast<nota::NotaString*>(obj)->value;
        }
    }

    return std::monostate{};
}

// Helper function to create a test file
inline void createTestFile(const std::string& path, const std::string& content) {
    std::filesystem::create_directories(std::filesystem::path(path).parent_path());
    std::ofstream file(path);
    file << content;
}
