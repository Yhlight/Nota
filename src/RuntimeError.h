#ifndef RUNTIME_ERROR_H
#define RUNTIME_ERROR_H

#include "Token.h"
#include <stdexcept>

class RuntimeError : public std::runtime_error {
public:
    const Token token;
    RuntimeError(const Token& token, const std::string& message)
        : std::runtime_error(message), token(token) {}
};

#endif // RUNTIME_ERROR_H
