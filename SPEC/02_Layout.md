# Conductor 2: The Architect (Layout)

## Objective
Implement the core layout engines (Flexbox wrappers) and positioning systems defined in Nota.

## Scope
1.  **Layout Components**:
    - `Row`: Maps to `display: flex; flex-direction: row;`.
    - `Col`: Maps to `display: flex; flex-direction: column;`.

2.  **Positioning Properties**:
    - `x`, `y`: Maps to `left`, `top` with `position: absolute`.
    - `position` (Nota keyword): Maps to CSS constraints or custom calculation.
    - Ensure parent containers get `position: relative`.

3.  **Box Model Attributes**:
    - `padding`: Map to CSS `padding`.
    - `spacing`: Map to CSS `gap` (for Row/Col).
    - `radius`: Map to CSS `border-radius`.
    - `border`: Map to CSS `border`.

## Technical Challenges
- Handling the interaction between Flexbox layout (`Row`/`Col`) and absolute positioning (`x`/`y`) within them.
- Calculating `position: center` or similar Nota-specific alignments if they don't map directly to CSS.

## Deliverables
- Support for `Row` and `Col` in parser and codegen.
- Correct CSS generation for positioning.
