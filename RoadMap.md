# Nota Compiler Development Roadmap

This document outlines the plan for developing a professional-grade Nota compiler, designed for industrial production environments. The development is divided into several key phases.

## Phase 1: Project Setup

- **Objective:** Establish a solid foundation for the C++ project.
- **Tasks:**
    - [ ] Initialize C++ project structure.
    - [ ] Set up CMake as the build system.
    - [ ] Integrate a testing framework (e.g., GoogleTest).
    - [ ] Define basic data structures for tokens and AST nodes.
    - [ ] Set up a simple command-line interface (CLI) to accept a `.nota` file as input.

## Phase 2: Lexical Analysis (Lexer)

- **Objective:** Convert Nota source code into a stream of tokens.
- **Tasks:**
    - [ ] Implement a lexer to recognize keywords (`App`, `Row`, `Item`, etc.).
    - [ ] Recognize identifiers, numbers, strings, and symbols (`{`, `}`, `:`, `;`).
    - [ ] Handle single-line (`//`) and multi-line (`/* */`) comments.
    - [ ] Write unit tests for the lexer.

## Phase 3: Syntactic Analysis (Parser)

- **Objective:** Build an Abstract Syntax Tree (AST) from the token stream.
- **Tasks:**
    - [ ] Design the AST structure to represent Nota components, properties, and their hierarchy.
    - [ ] Implement a parser to process the token stream and construct the AST.
    - [ ] Implement error handling for syntax errors.
    - [ ] Write unit tests for the parser, including valid and invalid syntax examples.

## Phase 4: Code Generation

- **Objective:** Traverse the AST to generate HTML and CSS output.
- **Tasks:**
    - [ ] Implement a code generator that traverses the AST.
    - [ ] Generate corresponding HTML tags for basic components (`App` -> `<body>`, `Row` -> `<div>`, etc.).
    - [ ] Generate CSS classes and rules based on component properties (`width`, `color`, etc.).
    - [ ] Handle the nested structure of components correctly.
    - [ ] Implement a snapshot testing strategy: compile sample `.nota` files and compare the output against expected HTML/CSS files.

## Phase 5: Advanced Features

- **Objective:** Implement the full feature set of the Nota language.
- **Tasks:**
    - [ ] **Custom Components:** Implement `Item` for defining and instantiating custom components.
    - [ ] **Property Overriding:** Allow instances to override default properties of custom components.
    - [ ] **Positioning:** Implement `x`, `y`, and `position` properties, ensuring parent elements get `position: relative`.
    - [ ] **Stacking:** Implement the `index` property for controlling `z-index`.
    - [ ] **Child Access:** (Stretch Goal) Implement the `this.children[index]` syntax for property modification. This requires more complex semantic analysis.

## Phase 6: Toolchain and Optimization

- **Objective:** Finalize the compiler and prepare it for production use.
- **Tasks:**
    - [ ] Enhance the CLI with more options (e.g., output directory, verbosity).
    - [ ] Optimize the generated CSS (e.g., by grouping common styles).
    - [ ] Write comprehensive documentation for the language and the compiler.
    - [ ] Performance testing and profiling of the compiler.
