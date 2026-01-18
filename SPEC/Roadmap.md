# Nota Development Roadmap

This document outlines the development plan for the Nota programming language, a compiled front-end interface language similar to QML. The project is divided into multiple "Conductors" (milestones) to ensure gradual and robust implementation.

## Project Goal
Build a production-ready prototype that compiles `.nota` files into standard HTML, CSS, and JavaScript. The prototype must support all syntax defined in `Nota.md`.

## Architecture Overview
The compiler will be implemented in C++20.
1.  **Lexer**: Tokenizes the input source.
2.  **Parser**: detailed recursive descent parser producing an AST.
3.  **Semantic Analyzer**: Resolves symbols, checks types, and handles imports.
4.  **Code Generator**: Transpiles AST to HTML/CSS/JS.

## Conductors

### [Conductor 1: Core Compiler Infrastructure](Conductor_1_Core_Compiler.md)
*   **Goal**: Establish the build system and basic compiler frontend.
*   **Scope**:
    *   CMake project setup.
    *   Lexer implementation.
    *   AST node definitions (basic hierarchy).
    *   Parser for basic syntax (Components, blocks, simple properties).
    *   Basic CLI entry point.

### [Conductor 2: MVP Code Generation](Conductor_2_Basic_Runtime_Codegen.md)
*   **Goal**: Generate a renderable HTML file from a simple Nota file.
*   **Scope**:
    *   Implement Code Generator backend.
    *   Support built-in components: `App`, `Row`, `Col`, `Rect`, `Text`.
    *   Support basic CSS property mapping (`width`, `height`, `color`).
    *   Output single HTML file.

### [Conductor 3: Component System & Properties](Conductor_3_Component_System.md)
*   **Goal**: Enable custom components and property hierarchy.
*   **Scope**:
    *   `Item` definition and instantiation.
    *   Property overrides.
    *   `id` property.
    *   Property references (e.g., `width: parent.width`).
    *   Semantic analysis for symbol resolution.

### [Conductor 4: Layout & Positioning](Conductor_4_Layout_Positioning.md)
*   **Goal**: Implement advanced layout capabilities.
*   **Scope**:
    *   Relative positioning (`x`, `y`).
    *   Anchor positioning (`position: left top`, etc.).
    *   Z-index (`index`).
    *   `padding`, `spacing`.
    *   Math expression parsing for property values.

### [Conductor 5: Modules & Logic](Conductor_5_Modules_Logic.md)
*   **Goal**: Support modular development and dynamic behavior.
*   **Scope**:
    *   `import` and `export` system.
    *   File system handling for modules.
    *   `if/else` conditional rendering.
    *   Events (`onClick`, `onHover`).
    *   Embedded JavaScript strings.
    *   States and transitions.
    *   Custom types (`Struct`).

## Testing Strategy
*   **Unit Tests**: Google Test for Lexer, Parser, and AST.
*   **Integration Tests**: Compile sample `.nota` files and verify output HTML structure (snapshot testing).
