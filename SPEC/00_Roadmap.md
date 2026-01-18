# Nota Development Roadmap

This document outlines the development plan for the Nota language compiler, broken down into "Conductors" (Phases).

## Conductors

### [Conductor 1: Genesis (Core Compiler)](./01_Genesis.md)
**Goal:** Establish the project foundation and compile a basic `App` with `Rect` and `Text` to HTML/CSS.
- Project Setup (CMake, C++20).
- Lexer & Parser for basic structure.
- Code Generation for static attributes (`width`, `height`, `color`).

### [Conductor 2: The Architect (Layout)](./02_Layout.md)
**Goal:** Implement the layout system and positioning.
- `Row` and `Col` components (Flexbox).
- Relative and Absolute positioning (`x`, `y`, `position`).
- Box Model nuances.

### [Conductor 3: The Component Model](./03_Components.md)
**Goal:** Enable reusable components and property relationships.
- Custom `Item` definitions.
- Property overriding.
- `id` references and hierarchy access (`parent`, `this`).

### [Conductor 4: The Logic](./04_Logic.md)
**Goal:** Add expressions, control flow, and types.
- Type system (`int`, `string`, `bool`).
- Expressions (Arithmetic, Logical).
- Conditional rendering (`if/else`).
- Module Imports.

### [Conductor 5: The Interaction](./05_Interaction.md)
**Goal:** Bring the interface to life with events and state.
- Event handling (`onClick`, `onHover`).
- State management (`State` objects).
- JS Runtime integration.

### [Conductor 6: Refinement](./06_Refinement.md)
**Goal:** Advanced features and polish.
- Structs (Custom Value Types).
- Lists.
- Packages and Exports.
- Optimizations.
