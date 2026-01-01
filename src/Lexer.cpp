#include "Lexer.h"
#include <cctype>

Lexer::Lexer(std::string input) : input(std::move(input)) {
    // 初始化，读取第一个字符
    readChar();
}

void Lexer::readChar() {
    if (readPosition >= input.length()) {
        ch = 0; // 0 是 ASCII 的 NUL 字符，通常用来表示文件结束 (EOF)
    } else {
        ch = input[readPosition];
    }
    position = readPosition;
    readPosition++;
}

char Lexer::peekChar() const {
    if (readPosition >= input.length()) {
        return 0;
    }
    return input[readPosition];
}

void Lexer::skipWhitespace() {
    while (isspace(ch)) {
        readChar();
    }
}

std::string Lexer::readIdentifier() {
    int startPosition = position;
    // 标识符可以包含字母、数字和下划线
    while (isalnum(ch) || ch == '_') {
        readChar();
    }
    return input.substr(startPosition, position - startPosition);
}

std::string Lexer::readNumber() {
    int startPosition = position;
    while (isdigit(ch)) {
        readChar();
    }
    return input.substr(startPosition, position - startPosition);
}

std::string Lexer::readString() {
    readChar(); // 跳过开头的 "
    int startPosition = position;
    while (ch != '"' && ch != 0) {
        readChar();
    }
    return input.substr(startPosition, position - startPosition);
}


Token Lexer::NextToken() {
    Token tok;

    skipWhitespace();

    switch (ch) {
        case '{':
            tok = {TokenType::LBRACE, "{"};
            break;
        case '}':
            tok = {TokenType::RBRACE, "}"};
            break;
        case ':':
            tok = {TokenType::COLON, ":"};
            break;
        case ';':
            tok = {TokenType::SEMICOLON, ";"};
            break;
        case '.':
            tok = {TokenType::DOT, "."};
            break;
        case '=':
             tok = {TokenType::ASSIGN, "="};
            break;
        case '"':
            tok.type = TokenType::STRING;
            tok.literal = readString();
            break;
        case '/':
            if (peekChar() == '/') { // 单行注释
                readChar(); // consume '/'
                readChar(); // consume '/'
                while(ch != '\n' && ch != 0) {
                    readChar();
                }
                return NextToken(); // 返回下一个有效 token
            } else if (peekChar() == '*') { // 多行注释
                readChar(); // consume '/'
                readChar(); // consume '*'
                while(!(ch == '*' && peekChar() == '/') && ch != 0) {
                    readChar();
                }
                readChar(); // consume '*'
                readChar(); // consume '/'
                return NextToken(); // 返回下一个有效 token
            }
            else {
                tok = {TokenType::SLASH, "/"};
            }
            break;
        case 0:
            tok = {TokenType::END_OF_FILE, ""};
            break;
        default:
            if (isalpha(ch) || ch == '_') {
                tok.literal = readIdentifier();
                if (tok.literal == "Item") {
                    tok.type = TokenType::KEYWORD_ITEM;
                } else {
                    tok.type = TokenType::IDENTIFIER; // Nota 中所有关键字都当做标识符处理
                }
                return tok; // readIdentifier 已经移动了指针, 所以直接返回
            } else if (isdigit(ch)) {
                tok.literal = readNumber();
                if (ch == '%') {
                    tok.literal += '%';
                    tok.type = TokenType::PERCENTAGE;
                    readChar(); // consume '%'
                } else {
                    tok.type = TokenType::INTEGER;
                }
                return tok; // readNumber 已经移动了指针, 所以直接返回
            } else {
                tok = {TokenType::ILLEGAL, std::string(1, ch)};
            }
            break;
    }

    readChar(); // 指向下一个字符，为下次调用 NextToken() 做准备
    return tok;
}
