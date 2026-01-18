# Conductor 4: Layout & Positioning

## Objectives
Implement advanced CSS layout features including absolute/relative positioning, Z-index, and property math.

## Tasks

### 4.1 Relative & Absolute Positioning
*   Map `x`, `y` to CSS `left`, `top`.
*   Ensure parent has `position: relative`.
*   Ensure child has `position: absolute` when `x` or `y` are used (or `position: relative` if it's just flow offset, but Nota docs imply explicit positioning).
*   Implement `position` property (anchor points like `left top`).

### 4.2 Z-Index
*   Map `index` property to CSS `z-index`.

### 4.3 Math Expressions
*   Update Parser to support complex expressions in property values (e.g., `parent.width / 2 - 10`).
*   Update Code Generator to output `calc()` or pre-calculate values if constant.

## Acceptance Criteria
*   Components can be positioned explicitly using `x`, `y`.
*   `index` correctly controls stacking order.
*   Mathematical expressions in properties are correctly evaluated or transpiled.
