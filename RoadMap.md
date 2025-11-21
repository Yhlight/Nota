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
- [ ] **Current Focus: Implement functions.**
- [ ] Implement classes and objects.

## Phase 4: Advanced Features

- [ ] Implement modules and packages.
- [ ] Implement garbage collection.
- [ ] Implement FFI.

## Phase 5: Standard Library and Tooling

- [ ] Develop a basic standard library.
- [ ] Create a command-line interface (CLI) for the Nota interpreter.
- [ ] Write comprehensive documentation.
