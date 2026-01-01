# Nota Compiler Development Roadmap

## Phase 1: Project Setup & Core Architecture (MVP)
*   **Task 1.1**: Set up a C++20 project using CMake.
*   **Task 1.2**: Integrate GoogleTest for unit testing.
*   **Task 1.3**: Implement a simple file reader for `.nota` files.
*   **Task 1.4**: Define the core Abstract Syntax Tree (AST) data structures.

## Phase 2: Lexical Analysis (Lexer)
*   **Task 2.1**: Implement the lexer to tokenize Nota source code.
    *   Handle keywords (`App`, `Row`, `Col`, `Item`, etc.).
    *   Handle identifiers, literals (numbers, strings, colors), and symbols (`{`, `}`, `:`, `;`).
    *   Support single-line and multi-line comments.
*   **Task 2.2**: Write comprehensive unit tests for the lexer.

## Phase 3: Syntactic Analysis (Parser)
*   **Task 3.1**: Implement the parser to build the AST from the token stream.
    *   Parse component definitions (`App { ... }`).
    *   Parse property assignments (`width: 100%`).
    *   Parse nested components (children).
*   **Task 3.2**: Write unit tests for the parser, including snapshot tests for the AST structure.

## Phase 4: Code Generation (Code Generator)
*   **Task 4.1**: Implement a basic code generator to traverse the AST and produce HTML and CSS.
    *   Generate corresponding HTML tags and CSS classes for basic components like `App`, `Row`, and `Col`.
*   **Task 4.2**: Implement the mapping from properties to CSS.
    *   Convert properties like `width`, `height`, `color`, `spacing`, and `padding`.
*   **Task 4.3**: Implement snapshot testing for the generated HTML and CSS code.

## Phase 5: Advanced Features
*   **Task 5.1**: Support the definition and instantiation of custom components (`Item`).
*   **Task 5.2**: Implement property overriding (`this.color = black`).
*   **Task 5.3**: Implement layout and positioning (`x`, `y`, `position`).
*   **Task 5.4**: Implement child component access (`this.children[0]...`).
*   **Task 5.5**: Implement layer control (`index` property).

## Phase 6: Toolchain & Optimization
*   **Task 6.1**: Create a command-line interface (CLI) to invoke the compiler.
*   **Task 6.2**: Improve the error-handling mechanism to provide clear error messages and source code locations.
*   **Task 6.3**: Performance analysis and optimization.
*   **Task 6.4**: Write comprehensive developer and user documentation.
