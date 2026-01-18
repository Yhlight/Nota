# Nota Compiler Development Roadmap

This document outlines the phased development plan for the Nota compiler. Each phase, or "Conductor," represents a major milestone in the project, building upon the previous one to create a production-ready compiler.

## Phase 1: Project Setup

**Goal:** Establish the foundational infrastructure for the project.

- **Tasks:**
  - Initialize a C++ project using CMake.
  - Set up the directory structure (`src`, `tests`, `include`).
  - Integrate the GoogleTest framework for unit testing.
  - Configure a basic build and test pipeline.
  - Create a `.gitignore` file to exclude build artifacts.

## Phase 2: Lexical Analysis

**Goal:** Implement the lexer to convert Nota source code into a stream of tokens.

- **Tasks:**
  - Define the complete set of tokens for the Nota language (keywords, identifiers, literals, operators, etc.).
  - Implement the `Lexer` class to read a `.nota` file and produce a list of tokens.
  - Handle basic lexical constructs like comments (single-line and multi-line) and whitespace.
  - Write comprehensive unit tests for the lexer to ensure it correctly tokenizes various source code examples.

## Phase 3: Syntactic Analysis

**Goal:** Implement the parser to build an Abstract Syntax Tree (AST) from the token stream.

- **Tasks:**
  - Define the structure of the AST nodes to represent the grammatical structure of the Nota language.
  - Implement the `Parser` class to consume the token stream from the lexer and build the AST.
  - Implement parsing for basic language features, such as component definitions, properties, and nested components.
  - Implement error handling for syntax errors.
  - Write unit tests for the parser to verify the correctness of the generated AST.

## Phase 4: Code Generation

**Goal:** Implement the code generator to produce basic HTML and CSS from the AST.

- **Tasks:**
  - Implement a `Generator` class that traverses the AST.
  - Generate corresponding HTML elements for basic Nota components (`App`, `Row`, `Col`, `Rect`, `Text`).
  - Generate CSS rules for basic properties (`width`, `height`, `color`, `spacing`, `padding`).
  - Write integration tests that compile a `.nota` file and verify the generated HTML and CSS output.

## Phase 5: Advanced Features (Component Model)

**Goal:** Implement the core component model features.

- **Tasks:**
  - Implement support for custom `Item`-based components.
  - Implement property overriding for component instances.
  - Implement property referencing (e.g., `width: box.width / 2`).
  - Implement child component access (e.g., `this.children[0]`).
  - Write extensive tests for all component model features.

## Phase 6: Advanced Features (Interactivity)

**Goal:** Implement features that make Nota interfaces interactive.

- **Tasks:**
  - Implement event handling (`onClick`, `onHover`).
  - Implement event delegation.
  - Implement state management (`states`, `State`, `when`).
  - Implement conditional rendering (`if`/`else`).
  - Write tests for all interactivity features.

## Phase 7: Modularization

**Goal:** Implement the module system for code organization.

- **Tasks:**
  - Implement the `import` statement for both file paths and module names.
  - Implement the `export` statement to control component visibility.
  - Implement the `package` statement for organizing modules.
  - Implement namespace resolution and conflict handling.
  - Write tests for the entire module system.

## Phase 8: Type System

**Goal:** Implement the static type system.

- **Tasks:**
  - Implement the basic types (`int`, `double`, `bool`, `string`, `void`, `list`, `State`).
  - Implement custom value types using `Struct`.
  - Implement type checking for properties and expressions.
  - Implement type inference where appropriate.
  - Write tests for the type system.

## Phase 9: Toolchain & Optimization

**Goal:** Finalize the compiler and prepare it for production use.

- **Tasks:**
  - Create a command-line interface (CLI) for the compiler.
  - Implement detailed error reporting with line numbers and context.
  - Optimize the generated code (e.g., CSS minification, JavaScript bundling).
  - Write comprehensive documentation for the language and the compiler.
  - Perform end-to-end testing of the entire compiler with a complex application.
