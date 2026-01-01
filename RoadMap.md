# Nota Compiler Development Roadmap

This document outlines the development plan for the Nota compiler, a high-performance compiler that transforms `.nota` files into production-ready HTML and CSS. The project is divided into several key phases:

## Phase 1: Project Setup
- **Objective:** Establish the foundational infrastructure for the project.
- **Key Tasks:**
    - Initialize a Git repository.
    - Set up a C++20 build system using CMake.
    - Integrate the GoogleTest framework for unit testing.
    - Create a `.gitignore` file to exclude build artifacts.

## Phase 2: Lexical Analysis (Lexer)
- **Objective:** Develop a lexer to convert Nota source code into a stream of tokens.
- **Key Tasks:**
    - Define all possible tokens in the Nota language (e.g., identifiers, keywords, symbols, literals).
    - Implement the lexer to handle basic language features like comments, strings, and numbers.
    - Write unit tests to verify the lexer's accuracy.

## Phase 3: Syntactic Analysis (Parser)
- **Objective:** Build a parser that constructs an Abstract Syntax Tree (AST) from the lexer's token stream.
- **Key Tasks:**
    - Define the structure of the AST nodes.
    - Implement a recursive descent parser to recognize the Nota grammar.
    - Develop error handling for syntax errors.
    - Write unit tests to validate the AST structure.

## Phase 4: Code Generation
- **Objective:** Create a generator that traverses the AST to produce HTML and CSS code.
- **Key Tasks:**
    - Implement the logic to translate AST nodes into corresponding HTML tags and CSS rules.
    - Handle component properties and their conversion to CSS styles.
    - Manage the parent-child relationships between components.
    - Implement a snapshot testing strategy to verify the generated output.

## Phase 5: Advanced Features
- **Objective:** Implement advanced language features to enhance Nota's capabilities.
- **Key Tasks:**
    - **Custom Components:** Add support for defining and using custom components (`Item`).
    - **Property Overriding:** Allow custom component properties to be overridden.
    - **Child Component Access:** Enable access to child components.
    - **Relative Positioning:** Implement `x`, `y`, and `position` properties for layout control.
    - **Z-Indexing:** Add support for the `index` property to manage stacking order.

## Phase 6: Toolchain and Optimization
- **Objective:** Improve the compiler's performance and usability.
- **Key Tasks:**
    - **Command-Line Interface (CLI):** Develop a CLI for the compiler.
    - **Performance Optimization:** Profile and optimize the compiler's performance.
    - **Documentation:** Write comprehensive documentation for the Nota language and compiler.
