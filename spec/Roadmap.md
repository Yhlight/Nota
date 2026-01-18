# Nota Compiler Development Roadmap

This document outlines the phased development plan for the Nota compiler, a C++20-based compiler that transforms `.nota` source files into production-ready HTML, CSS, and JavaScript.

## Phase 1: Project Setup & Core Infrastructure

- **Objective**: Establish the foundational infrastructure for the compiler project.
- **Key Tasks**:
    1.  **Build System Setup**: Configure CMake to manage the build process.
    2.  **Directory Structure**: Create directories for source (`src`), tests (`tests`), and shared headers (`src/include`).
    3.  **Testing Framework**: Integrate GoogleTest for unit and integration testing.
    4.  **Version Control**: Initialize a Git repository and create a `.gitignore` file.
    5.  **Main Executable**: Define the main `nota-compiler` executable.

## Phase 2: Lexical Analysis (Lexer)

- **Objective**: Develop a lexer to tokenize `.nota` source code.
- **Key Tasks**:
    1.  **Token Definition**: Define the `Token` and `TokenType` structures.
    2.  **Lexer Implementation**: Implement the `Lexer` class to handle:
        -   Whitespace and comments (single-line and multi-line).
        -   Keywords (`Item`, `import`, `if`, etc.).
        -   Identifiers, literals (numbers, strings), and symbols (`{`, `}`, `:`, `(`, `)`).
    3.  **Unit Testing**: Write comprehensive tests for the lexer.

## Phase 3: Syntactic Analysis (Parser) & AST

- **Objective**: Create a parser to build an Abstract Syntax Tree (AST) from the token stream.
- **Key Tasks**:
    1.  **AST Node Definition**: Define the class hierarchy for AST nodes (`ComponentNode`, `PropertyNode`, `ExpressionNode`, etc.).
    2.  **Parser Implementation**: Implement a recursive descent parser to recognize the Nota grammar.
    3.  **Error Handling**: Implement basic syntax error reporting.
    4.  **Unit Testing**: Write tests to validate the parser's correctness.

## Phase 4: Code Generation (HTML/CSS/JS)

- **Objective**: Develop a code generator to traverse the AST and produce the final output.
- **Key Tasks**:
    1.  **Generator Implementation**: Create a `Generator` class that recursively traverses the AST.
    2.  **HTML Generation**: Translate `ComponentNode` instances into corresponding HTML elements.
    3.  **CSS Generation**: Translate component properties (e.g., `width`, `color`, `radius`) into CSS rules.
    4.  **Initial Integration**: Test the end-to-end compilation of basic `.nota` files.

## Phase 5: Advanced Features I (Custom Components, Property Binding, Layout)

- **Objective**: Implement core language features related to component composition and layout.
- **Key Tasks**:
    1.  **Custom Components**: Implement the `Item` keyword for defining and instantiating custom components.
    2.  **Property Overriding & Referencing**: Support property overriding and referencing (e.g., `box.width / 2`).
    3.  **Layout System**: Implement the `x`, `y`, `position`, and `index` properties for component positioning.
    4.  **Integration Testing**: Write tests for these new features.

## Phase 6: Advanced Features II (Modules, Events, State Management, Conditional Rendering, Type System)

- **Objective**: Implement advanced features for building dynamic and interactive applications.
- **Key Tasks**:
    1.  **Module System**: Implement `import`, `package`, and `export` functionality.
    2.  **Event Handling**: Implement event handling (`onClick`, `onHover`) and event delegation.
    3.  **State Management**: Implement the `State` component and state-based property changes.
    4.  **Conditional Rendering**: Implement `if`/`else` blocks for conditional rendering.
    5.  **Type System**: Implement the Nota type system, including custom `Struct` definitions.
    6.  **Integration Testing**: Add comprehensive tests for all advanced features.

## Phase 7: Toolchain & Finalization

- **Objective**: Polish the compiler and prepare it for production use.
- **Key Tasks**:
    1.  **Command-Line Interface (CLI)**: Develop a user-friendly CLI for the compiler.
    2.  **Error Reporting**: Enhance error reporting with detailed messages and source code context.
    3.  **Performance Optimization**: Profile and optimize the compiler's performance.
    4.  **Documentation**: Write comprehensive documentation for the Nota language and compiler.
    5.  **Acceptance Testing**: Perform final acceptance testing against the `Nota.md` specification.
