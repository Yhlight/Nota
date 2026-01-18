# Conductor 5: The Interaction

## Objective
Make Nota applications interactive using Event handling and State management.

## Scope
1.  **Events**:
    - `onClick`, `onHover`.
    - JS Code embedding: `onClick: "alert('Click!')"`.
    - Event Delegation: `delegate [event] for target`.

2.  **State Management**:
    - `State` object definition.
    - `states` array.
    - `when` condition: `when: ClickEvent`.
    - Automatic state transitions.

3.  **Runtime Library (JS)**:
    - A lightweight JS runtime included in the output.
    - Handles event binding.
    - Manages state updates and DOM manipulation.

## Technical Details
- The compiler acts as a transpiler here, embedding the user's JS code into the HTML/JS output.
- The Runtime library needs to be robust and minimal.

## Deliverables
- Interactive elements (Buttons responding to clicks).
- State-based visual changes (Hover effects via State).
