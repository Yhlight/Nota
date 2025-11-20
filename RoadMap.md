# Nota Development Roadmap

This document outlines the development plan for the Nota programming language.

## Phase 1: Core Language Features

*   [x] **Project Setup:**
    *   [x] Create a C++ project using CMake.
    *   [x] Set up a Python build script for CMake.
    *   [x] Configure `.gitignore` to exclude build artifacts.
*   [x] **Lexer:**
    *   [x] Implement a lexer to tokenize the Nota source code.
    *   [x] Support for all basic tokens (keywords, identifiers, operators, literals).
*   [ ] **Parser:**
    *   [ ] Implement a recursive descent parser.
    *   [ ] Support for basic expressions and statements.
    *   [ ] Abstract Syntax Tree (AST) generation.
*   [ ] **Bytecode Compiler:**
    *   [ ] Design the bytecode instruction set.
    *   [ ] Implement a compiler to translate the AST to bytecode.
*   [ ] **Virtual Machine (VM):**
    *   [ ] Implement a stack-based VM to execute the bytecode.
    *   [ ] Support for basic arithmetic and logical operations.
*   [ ] **Garbage Collection:**
    *   [ ] Implement a basic garbage collector.

## Phase 2: Advanced Language Features

*   [ ] **Functions:**
    *   [ ] Support for function declaration and calls.
    *   [ ] Support for lambda functions.
*   [ ] **Control Flow:**
    *   [ ] Support for `if/else` and `match` statements.
    *   [ ] Support for `while`, `for`, `for-each`, and `do-while` loops.
*   [ ] **Data Types:**
    *   [ ] Support for `int`, `float`, `bool`, and `string`.
    *   [ ] Support for arrays (static and dynamic).
    *   [ ] Support for type annotations.
*   [ ] **Classes and Objects:**
    *   [ ] Support for class declaration and instantiation.
    *   [ ] Support for methods and properties.
*   [ ] **Modules and Packages:**
    *   [ ] Support for importing modules.
    *   [ ] Support for creating packages.

## Phase 3: Standard Library and FFI

*   [ ] **Standard Library:**
    *   [ ] Implement a basic standard library with common utilities.
*   [ ] **Foreign Function Interface (FFI):**
    *   [ ] Implement an FFI to call C++ functions from Nota.

## Phase 4: Tooling and Ecosystem

*   [ ] **REPL:**
    *   [ ] Implement a Read-Eval-Print Loop (REPL) for interactive programming.
*   [ ] **Debugger:**
    *   [ ] Implement a basic debugger.
*   [ ] **Package Manager:**
    *   [ ] Implement a package manager for distributing and installing Nota libraries.
