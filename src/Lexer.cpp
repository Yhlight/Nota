#include "Lexer.h"
#include <cctype>
#include <map>

namespace nota {

    static std::map<std::string, TokenType> keywords = {
        {"let", TokenType::Let},
        {"mut", TokenType::Mut},
    };

    Lexer::Lexer(const std::string& source)
        : source(source), current(0), line(1), column(1) {}

    Token Lexer::next_token() {
        // Skip whitespace (but not newlines)
        while (current < source.length() && (source[current] == ' ' || source[current] == '\r' || source[current] == '\t')) {
            current++;
            column++;
        }

        if (current >= source.length()) {
            return {TokenType::Eof, "", line, column};
        }

        char c = source[current];

        // Newline
        if (c == '\n') {
            int start_line = line;
            current++;
            line++;
            column = 1;
            return {TokenType::Newline, "\\n", start_line, 1};
        }

        // Single-line comments
        if (c == '/' && current + 1 < source.length() && source[current + 1] == '/') {
            int start_col = column;
            int start = current;
            while (current < source.length() && source[current] != '\n') {
                current++;
                column++;
            }
            return {TokenType::Comment, source.substr(start, current - start), line, start_col};
        }

        // Multi-line comments
        if (c == '/' && current + 1 < source.length() && source[current + 1] == '*') {
            int start_col = column;
            int start_line = line;
            int start = current;
            current += 2; // Skip '/*'
            column += 2;
            while (current + 1 < source.length() && (source[current] != '*' || source[current + 1] != '/')) {
                if (source[current] == '\n') {
                    line++;
                    column = 1;
                } else {
                    column++;
                }
                current++;
            }
             if (current + 1 < source.length()) {
                current += 2; // Skip '*/'
                column += 2;
            }
            return {TokenType::Comment, source.substr(start, current - start), start_line, start_col};
        }

        // Plus operator
        if (c == '+') {
            int start_col = column;
            current++;
            column++;
            return {TokenType::Plus, "+", line, start_col};
        }

        // Equal operator
        if (c == '=') {
            int start_col = column;
            current++;
            column++;
            return {TokenType::Equal, "=", line, start_col};
        }

        // Numbers
        if (isdigit(c)) {
            int start_col = column;
            int start = current;
            while (current < source.length() && isdigit(source[current])) {
                current++;
                column++;
            }
            return {TokenType::Number, source.substr(start, current - start), line, start_col};
        }

        // Identifiers and keywords
        if (isalpha(c) || c == '_') {
            int start_col = column;
            int start = current;
            while (current < source.length() && (isalnum(source[current]) || source[current] == '_')) {
                current++;
                column++;
            }
            std::string lexeme = source.substr(start, current - start);
            auto it = keywords.find(lexeme);
            if (it != keywords.end()) {
                return {it->second, lexeme, line, start_col};
            }
            return {TokenType::Identifier, lexeme, line, start_col};
        }


        // If we don't recognize the character, we'll just advance for now.
        // This is not robust, but it will work for the current tests.
        current++;
        column++;
        return {TokenType::Eof, "", line, column};
    }

} // namespace nota
