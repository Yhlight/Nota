# Nota Development Roadmap

This document outlines the development roadmap for the Nota programming language.

## Phase 1: Project Setup and Foundation (In Progress)

- [x] Set up the basic project structure (`src`, `tests`, `build`, `scripts`).
- [x] Create the initial `CMakeLists.txt` file.
- [x] Create a Python build script (`scripts/build.py`).
- [x] Create a simple "hello world" C++ source file (`src/main.cpp`).
- [x] Create an initial test case (`tests/test_main.cpp`).
- [x] Create this `RoadMap.md` file.
- [x] Update the `.gitignore` file.

## Phase 2: Lexer (Tokenizer)

- [ ] Implement the lexer to tokenize the Nota source code.
- [ ] Support for all basic tokens (keywords, identifiers, operators, literals, etc.).
- [ ] Write comprehensive tests for the lexer.

## Phase 3: Parser

- [ ] Implement the parser using a recursive descent (LL(k)) algorithm.
- [ ] Build the Abstract Syntax Tree (AST).
- [ ] Support for all language constructs (variables, expressions, statements, control flow, functions, classes).
- [ ] Write comprehensive tests for the parser.

## Phase 4: Bytecode and Virtual Machine (VM)

- [ ] Design the Nota bytecode instruction set.
- [ ] Implement the VM to execute the bytecode.
- [ ] Implement a simple stack-based VM.
- [ ] Implement garbage collection.
- [ ] Write comprehensive tests for the VM.

## Phase 5: Standard Library

- [ ] Implement a basic standard library.
- [ ] Include I/O operations, string manipulation, and basic data structures.
- [ ] Implement the FFI (Foreign Function Interface).

## Phase 6: Advanced Features

- [ ] Implement modules and packages.
- [ ] Implement advanced error handling and reporting.
- [ ] Performance optimizations.
