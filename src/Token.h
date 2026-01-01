#ifndef NOTA_COMPILER_TOKEN_H
#define NOTA_COMPILER_TOKEN_H

#include <string>
#include <variant>

// 定义 Token 的类型
enum class TokenType {
    // 特殊类型
    ILLEGAL, // 非法字符
    END_OF_FILE, // 文件结束

    // 标识符 + 字面量
    IDENTIFIER, // my_component, width
    INTEGER,    // 123
    STRING,     // "hello"
    PERCENTAGE, // 100%

    // 运算符
    ASSIGN,   // =
    DOT,      // .
    SLASH,    // /

    // 分隔符
    LBRACE,   // {
    RBRACE,   // }
    LBRACKET, // [
    RBRACKET, // ]
    COLON,    // :
    SEMICOLON,// ;
    COMMA,    // ,

    // 关键字 (未来可能扩展)
    // 暂时将所有组件名和属性名视为 IDENTIFIER
    KEYWORD_ITEM, // Item
};

// Token 结构体，用于存储词法分析的结果
struct Token {
    TokenType type;
    std::string literal;
    int line = 0;
    int column = 0;
};

#endif //NOTA_COMPILER_TOKEN_H
