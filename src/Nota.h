#ifndef NOTA_H
#define NOTA_H

#include <string>
#include <vector>
#include "Interpreter.h"
#include "RuntimeError.h"


class Nota {
public:
    static Interpreter interpreter;
    static bool hadError;
    static bool hadRuntimeError;

    static void run(const std::string& source);
    static void error(int line, const std::string& message);
    static void runtimeError(const RuntimeError& error);
    static void report(int line, const std::string& where, const std::string& message);
};


#endif // NOTA_H
