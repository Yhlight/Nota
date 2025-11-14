# Nota Project Roadmap

This document tracks the progress of the Nota programming language implementation.

## Phase 1: Project Setup and Core Language Features

- [x] Project structure setup (CMake, C++ source, tests)
- [x] Python build script
- [x] Initial lexer and parser implementation
- [x] Support for `let` and `mut` variable declarations
- [x] Support for basic data types: `int`, `float`, `bool`, `string`
- [x] Unit tests for core features

## Phase 2: Control Flow and Functions

- [ ] Implement `if`/`else`/`else if` conditional statements
- [ ] Implement `match` statements
- [ ] Implement `while`, `for`, `for-each`, and `do-while` loops
- [ ] Implement function declaration and calls
- [ ] Implement lambda functions

## Phase 3: Advanced Data Types and Modules

- [ ] Implement dynamic and static arrays
- [ ] Implement basic class support (declaration, instantiation)
- [ ] Implement module system with `import`
- [ ] Implement package system

## Phase 4: Bytecode and Virtual Machine

- [ ] Design the Nota bytecode instruction set
- [ ] Implement the bytecode compiler
- [ ] Implement the Nota Virtual Machine (NVM)
- [ ] Integrate the NVM with the parser

## Phase 5: Standard Library and Tooling

- [ ] Develop a core standard library
- [ ] Create a command-line interface (CLI) for the Nota interpreter
- [ ] Implement a debugger
- [ ] Create documentation
