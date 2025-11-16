#include "Lexer.h"

namespace nota {

    Lexer::Lexer(const std::string& source)
        : source(source), current(0), line(1), column(1) {}

    Token Lexer::next_token() {
        if (current >= source.length()) {
            return {TokenType::Eof, "", line, column};
        }

        char c = source[current];

        // Single-line comments
        if (c == '/' && current + 1 < source.length() && source[current + 1] == '/') {
            int start = current;
            while (current < source.length() && source[current] != '\n') {
                current++;
            }
            return {TokenType::Comment, source.substr(start, current - start), line, column};
        }

        // Multi-line comments
        if (c == '/' && current + 1 < source.length() && source[current + 1] == '*') {
            int start = current;
            current += 2; // Skip '/*'
            while (current + 1 < source.length() && (source[current] != '*' || source[current + 1] != '/')) {
                if (source[current] == '\n') {
                    line++;
                    column = 1;
                } else {
                    column++;
                }
                current++;
            }
            current += 2; // Skip '*/'
            return {TokenType::Comment, source.substr(start, current - start), line, column};
        }

        // For now, just return EOF for everything else
        return {TokenType::Eof, "", line, column};
    }

} // namespace nota
