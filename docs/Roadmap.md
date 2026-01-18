# Nota Compiler Development Roadmap

This document outlines the phased development plan for the Nota compiler. Each phase builds upon the previous one, ensuring a structured and incremental development process.

## Phase 1: Project Setup

- **Objective:** Establish the foundational infrastructure for the project.
- **Key Tasks:**
    - Set up a robust build system using CMake.
    - Integrate a testing framework (e.g., GoogleTest).
    - Define the basic project structure (`src`, `tests`, `include`).
    - Implement a command-line interface for the compiler.

## Phase 2: Lexical Analysis

- **Objective:** Implement the lexer to tokenize Nota source code.
- **Key Tasks:**
    - Define all tokens required by the Nota language (keywords, identifiers, literals, operators, etc.).
    - Implement a `Lexer` class that takes a string of Nota source code and outputs a stream of tokens.
    - Add support for comments (single-line and multi-line).
    - Write comprehensive unit tests for the lexer.

## Phase 3: Syntactic Analysis

- **Objective:** Implement the parser to build an Abstract Syntax Tree (AST).
- **Key Tasks:**
    - Define the structure of the AST nodes.
    - Implement a `Parser` class that takes a stream of tokens and produces an AST.
    - Implement parsing for basic components (`App`, `Row`, `Col`, `Rect`, `Text`).
    - Implement parsing for properties (e.g., `width`, `height`, `color`).
    - Write comprehensive unit tests for the parser.

## Phase 4: Code Generation

- **Objective:** Implement the code generator to produce HTML, CSS, and JavaScript.
- **Key Tasks:**
    - Implement a `Generator` class that traverses the AST.
    - Generate the corresponding HTML structure for each component.
    - Generate CSS rules based on the component's properties.
    - Handle basic property types (numbers, strings, colors).
    - Write unit tests for the code generator.

## Phase 5: Advanced Features

- **Objective:** Implement more complex language features.
- **Key Tasks:**
    - **Custom Components:** Implement support for defining and using custom components with `Item`.
    - **Property Overriding and Referencing:** Add support for overriding properties and referencing them from other properties.
    - **Modules and Imports:** Implement the module system with `import`, `export`, and `package`.
    - **Event Handling:** Add support for events like `onClick` and `onHover`.
    - **State Management:** Implement the `states` and `State` feature.
    - **Conditional Rendering:** Add support for `if`/`else` blocks.
    - **Custom Types:** Implement support for custom value types with `Struct`.

## Phase 6: Toolchain and Optimization

- **Objective:** Improve the compiler's performance and developer experience.
- **Key Tasks:**
    - **Error Reporting:** Implement robust error reporting with clear and helpful messages.
    - **Source Maps:** Generate source maps to facilitate debugging.
    - **Performance Optimization:** Profile and optimize the compiler's performance.
    - **Documentation:** Write comprehensive documentation for the Nota language and the compiler.
