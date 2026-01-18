# Conductor 5: Modules & Logic

## Objectives
Implement the module system (`import`/`export`), events, and conditional logic.

## Tasks

### 5.1 Module System
*   Implement `import` statement handling.
    *   Path-based imports (`"path/to/file.nota"`).
    *   Module-based imports (`Module.Name`).
*   Implement `export` keyword.
*   Resolve file paths and read external files.
*   Manage dependencies to avoid cycles.

### 5.2 Events & JavaScript
*   Parse event handlers (`onClick`, `onHover`).
*   Extract the JS string.
*   Generate JS event listeners in the output.
    *   `element.addEventListener('click', function() { ... })`.

### 5.3 Conditional Rendering
*   Implement `if (condition) { ... } else { ... }`.
*   This likely requires a runtime component or complex CSS/JS toggling.
    *   *Approach*: Generate JS that watches the condition and adds/removes DOM nodes or toggles `display: none`.

### 5.4 Custom Types & Structs
*   Support `Struct` definition.
*   Support typed custom properties (`int`, `string`, `Struct`).

### 5.5 States
*   Implement `State` objects and transitions.
*   Generate JS to manage state changes and update styles.

## Acceptance Criteria
*   Can import components from other files.
*   Clicking a button triggers the defined JS alert/console log.
*   Changing a boolean property toggles visibility of an element via `if/else`.
