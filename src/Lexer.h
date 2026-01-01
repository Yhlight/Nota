#ifndef NOTA_COMPILER_LEXER_H
#define NOTA_COMPILER_LEXER_H

#include "Token.h"
#include <string>

class Lexer {
public:
    // 构造函数，接收源代码
    explicit Lexer(std::string input);

    // 获取下一个 Token
    Token NextToken();

private:
    // 辅助函数，用于读取字符和跳过空白
    void readChar();
    char peekChar() const;
    void skipWhitespace();

    // 辅助函数，用于读取完整的标识符、数字等
    std::string readIdentifier();
    std::string readNumber();
    std::string readString();

    std::string input;     // 源代码
    int position = 0;      // 当前字符位置 (指向当前字符)
    int readPosition = 0;  // 下一个读取位置 (指向当前字符的下一个)
    char ch = 0;           // 当前正在查看的字符
};

#endif //NOTA_COMPILER_LEXER_H
