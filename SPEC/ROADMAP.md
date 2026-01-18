# Nota Development Roadmap

This document outlines the development plan for the Nota language compiler, divided into "Conductors" (milestones).

## Conductor 1: Foundation
**Goal:** Establish the project structure and implement a minimal compiler capable of processing a basic `App` component and generating HTML/CSS.

-   **Infrastructure:** CMake build system, Google Test integration.
-   **Lexer:** Tokenize basic Nota syntax (Identifier, Number, String, Braces, Colon).
-   **Parser:** Build an AST for `App`, `Component` (e.g., `Rect`), and simple property assignments.
-   **Generator:** Generate HTML (with inline styles or `<style>` block) for `App` and nested `Rect` components.
-   **Target:** `App { width: 100%; height: 100%; color: red; }` -> Red background page.

## Conductor 2: Layout & Basic Components
**Goal:** Implement core UI components and the flexbox-based layout system.

-   **Components:** `Row`, `Col` (mapping to Flexbox), `Text`.
-   **Properties:** `spacing` (gap), `padding`.
-   **Types:** Handle unitless numbers as `px` and percentages.
-   **Generator:** Generate correct CSS for flex layouts (`display: flex`, `flex-direction`).

## Conductor 3: Property System & Expressions
**Goal:** Implement a robust property system with strict typing and referencing.

-   **Types:** `int`, `double`, `string`, `bool`.
-   **References:** Support `id` and accessing properties of other components (e.g., `width: box.width / 2`).
-   **Expressions:** Basic arithmetic and logic expressions in property values.

## Conductor 4: Components & Reusability
**Goal:** Enable users to define and reuse custom components.

-   **`Item`:** Support defining custom components using `Item Name { ... }`.
-   **Inheritance:** Components inheriting properties from their definition.
-   **Overrides:** Instances overriding default properties.
-   **Keywords:** `this` and `parent`.
-   **Structure:** "Box Model" implementation (border, padding, content).

## Conductor 5: Positioning & Layering
**Goal:** Implement precise positioning features.

-   **Relative:** `x`, `y` coordinates relative to parent.
-   **Anchor:** `position` property (9-grid system: left top, center, etc.).
-   **Z-Index:** `index` property for layering control.
-   **CSS Gen:** Handle `position: relative/absolute` mixed with flexbox where appropriate.

## Conductor 6: Interaction & Events
**Goal:** Add interactivity and Javascript runtime integration.

-   **Runtime:** Include a minimal JS runtime library.
-   **Events:** `onClick`, `onHover`.
-   **Delegation:** `delegate` keyword for event bubbling/capture control.
-   **Generator:** Bind JS event listeners to generated HTML elements.

## Conductor 7: Logic & State Management
**Goal:** Implement dynamic behavior within the UI.

-   **States:** `states` array and `State` object.
-   **Transitions:** Basic state switching logic.
-   **Conditionals:** `if (expr) { ... } else { ... }` for conditional rendering.
-   **Custom Properties:** User-defined properties (`property type name: value`).

## Conductor 8: Modules & Packaging
**Goal:** Scale the language to support multiple files and libraries.

-   **Import:** Support `import "path/file.nota"` and `import Module`.
-   **Package:** Package namespacing and resolution.
-   **Export:** `export` keyword.
-   **Struct:** Custom value types (`Struct Color { ... }`).

## Conductor 9: Production Polish
**Goal:** Finalize the compiler for production use.

-   **Optimization:** CSS minification, dead code elimination.
-   **Error Handling:** meaningful error messages with line numbers.
-   **Coverage:** Ensure 100% syntax support from `Nota.md`.
-   **Testing:** Comprehensive integration tests.
