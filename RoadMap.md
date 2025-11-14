# Nota Project Roadmap

## Phase 1: Project Setup and Core Infrastructure

- [ ] Set up the basic project structure with source, test, and build directories.
- [ ] Configure CMake for the project.
- [ ] Implement a Python build script to automate the build process.
- [ ] Establish a testing framework (e.g., Google Test).

## Phase 2: Lexer and Parser

- [ ] Implement the lexer to tokenize the Nota source code.
- [ ] Implement the parser using an LL(k) recursive descent algorithm.
- [ ] Define the Abstract Syntax Tree (AST) for the Nota language.
- [ ] Write unit tests for the lexer and parser.

## Phase 3: Bytecode Virtual Machine

- [ ] Design the bytecode instruction set.
- [ ] Implement the bytecode compiler to translate the AST to bytecode.
- [ ] Implement the virtual machine to execute the bytecode.
- [ ] Write unit tests for the virtual machine.

## Phase 4: Language Features

- [ ] Implement variable declarations (`let`, `mut`).
- [ ] Implement basic data types (`int`, `float`, `bool`, `string`).
- [ ] Implement control flow structures (`if-else`, `match`, `while`, `for`, `do-while`).
- [ ] Implement functions and lambdas.
- [ ] Implement classes and objects.
- [ ] Implement modules and packages.

## Phase 5: Standard Library

- [ ] Develop a basic standard library with essential I/O and data structure functionalities.
