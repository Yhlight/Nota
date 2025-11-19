# Nota Development Roadmap

This document outlines the development plan for the Nota programming language.

## Stage 1: Project Setup and Core Components

- [x] Set up the basic project structure with `src`, `tests`, and `scripts` directories.
- [x] Initialize CMake for the project.
- [x] Create a Python build script.
- [x] Set up Google Test for TDD.
- [x] Implement the core data structures for the language (e.g., `NotaValue`, `NotaObject`).
- [ ] Implement a basic memory management system (garbage collection).

## Stage 2: Lexer and Parser

- [x] Implement the lexer to tokenize the source code.
- [x] Implement the parser using a recursive descent algorithm.
- [x] Define the Abstract Syntax Tree (AST) nodes.
- [x] Implement the parser to generate an AST from the token stream.

## Stage 3: Bytecode and Virtual Machine (VM)

- [x] Define the bytecode instruction set.
- [x] Implement a compiler to translate the AST into bytecode.
- [x] Implement the virtual machine to execute the bytecode.
- [ ] Implement the stack and frame management in the VM.

## Stage 4: Language Features

- [ ] Implement variable declarations (`let`, `mut`).
- [ ] Implement basic data types (`int`, `float`, `bool`, `string`).
- [x] Implement arithmetic and logical operators (arithmetic only).
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
