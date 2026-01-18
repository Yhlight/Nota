# Conductor 2: MVP Code Generation

## Objectives
Implement the backend to transform the AST into valid HTML and CSS, focusing on the fundamental built-in components.

## Tasks

### 2.1 Code Generator Structure
*   Create `CodeGen` class implementing the AST Visitor interface.
*   Setup output buffering for HTML and CSS.

### 2.2 Built-in Component Mapping
*   Implement mapping logic for `App`:
    *   Generates `<body class="nota-app">`.
    *   Adds default CSS reset (margin 0, padding 0, box-sizing border-box).
*   Implement mapping for `Row` & `Col`:
    *   Generates `div` with flexbox styles.
    *   `Row`: `display: flex; flex-direction: row;`.
    *   `Col`: `display: flex; flex-direction: column;`.
*   Implement mapping for `Rect`:
    *   Generates `div` with `display: block`.
*   Implement mapping for `Text`:
    *   Generates `span` or `div`.
    *   Handles `text` property content.

### 2.3 Basic Property Mapping
*   Map Nota properties to CSS rules:
    *   `width`, `height`: Handle numbers as `px`, `%` as is.
    *   `color`: Maps to `background-color`.
    *   `border`: Direct mapping.
    *   `radius`: Maps to `border-radius`.
    *   `padding`: Direct mapping.
    *   `spacing`: Maps to `gap` (only valid for Row/Col).

### 2.4 HTML/CSS Synthesis
*   Generate a single `.html` file containing:
    *   `<!DOCTYPE html>`
    *   `<head>` with embedded `<style>` block for generated CSS.
    *   `<body>` containing the DOM tree.
*   CSS Class Generation:
    *   Strategy to generate unique class names or use inline styles for instance-specific properties?
    *   *Decision*: Use base classes for component types (e.g., `.nota-row`) and unique IDs or utility classes for specific property overrides. For MVP, inline styles for properties defined in the instance is acceptable and easiest.

## Acceptance Criteria
*   Input: A `.nota` file with nested `App`, `Row`, `Col`, `Rect`, `Text` and basic styling.
*   Output: An HTML file.
*   Verification: Opening the HTML file in a browser renders the UI correctly according to the defined properties (layout, colors, dimensions).
