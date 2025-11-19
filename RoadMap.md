# Nota Development Roadmap

This document outlines the development plan for the Nota programming language.

## Stage 1: Core Infrastructure

- [x] Set up the basic project structure with `src`, `tests`, and `scripts` directories.
- [x] Initialize CMake for the project.
- [x] Create a Python build script.
- [x] Set up Google Test for TDD.
- [x] Implement the core data structures for the language (e.g., `NotaValue`, `NotaObject`).
- [ ] Implement a basic memory management system (garbage collection).

## Stage 2: From Source to AST

- [x] Implement the lexer to tokenize the source code.
- [x] Define the Abstract Syntax Tree (AST) nodes.
- [x] Implement the parser to generate an AST from the token stream.

## Stage 3: Bytecode Virtual Machine

- [x] Define the bytecode instruction set.
- [x] Implement a compiler to translate the AST into bytecode.
- [x] Implement the virtual machine to execute the bytecode.
- [x] Implement basic stack and frame management in the VM.

## Stage 4: Foundational Language Features

- [x] Implement variable declarations (`let`, `mut`) in the parser.
- [x] Implement integer literals.
- [x] Implement arithmetic operators.
- [x] Implement `let` and `mut` functionality in the compiler and VM.
- [x] Implement other basic data types (`float`, `bool`).
- [ ] Implement `string` data type.
- [x] Implement comparison and equality operators.
- [ ] Implement logical operators (`&&`, `||`, `!`).
- [ ] Implement control flow structures (`if`, `else`, `match`).
- [ ] Implement loop structures (`while`, `for`, `for-each`, `do-while`).
- [ ] Implement functions and function calls.
- [ ] Implement classes and objects.
- [ ] Implement modules and packages.

## Stage 5: Standard Library

- [ ] Implement a basic set of standard library functions.
- [ ] Implement I/O operations.
- [ ] Implement string manipulation functions.
- [ ] Implement data structure manipulation functions.

## Stage 6: Advanced Features and Optimization

- [ ] Implement lambda functions.
- [ ] Implement FFI (Foreign Function Interface).
- [ ] Optimize the VM's performance.
- [ ] Add more advanced language features as needed.
