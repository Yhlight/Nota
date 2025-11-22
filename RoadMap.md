# Nota Development Roadmap

## Phase 1: Project Foundation & Core Components (Complete)

- [x] Set up the basic project structure (`src`, `tests`).
- [x] Create a Python build script (`build.py`) to automate the CMake process.
- [x] Configure CMake for the project.
- [x] Set up the Doctest framework for unit testing.
- [x] Implement a simple "Hello, World!" example.
- [x] Establish the initial structure for the Lexer, Parser, and VM.
- [x] Refactor project structure for modularity (`src/lib`, `src/app`).

## Phase 2: Lexer and Parser Implementation (Complete)

- [x] Implement a basic lexer to tokenize the source code.
- [x] Implement a recursive descent parser for expressions.
- [x] Implement parser for statements (variable declarations).
- [ ] Set up the bytecode VM structure.

## Phase 3: Language Features - Statements and Control Flow

- [x] Implement control flow structures (if/else).
- [x] Implement loops (while).
- [x] Implement other loop structures (for, do-while).
- [x] Implement functions (declaration and call).
- [x] Implement function return statements.
- [x] Implement classes and objects.
- [x] Implement modules and packages.
- [x] Implement garbage collection.
- [x] Implement lambda functions.
- [x] Implement FFI.
- [x] Implement arrays.

## Phase 3 In-Progress
- [x] Implement logical operators.
- [x] Implement for-each loops.

## Phase 4: Advanced Features

## Phase 5: Standard Library and Tooling

- [x] **Current Focus: Develop a basic standard library.**
  - [x] Implement `print()` for basic output.
  - [x] Implement `clock()` for performance measurement.
- [x] Create a command-line interface (CLI) for the Nota interpreter.
- [x] Write comprehensive documentation.
