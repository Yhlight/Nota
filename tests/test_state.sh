#!/bin/bash
set -e

cmake --build build

./build/src/nota tests/test_state.nota -o output_state.html

if [ ! -f output_state.html ]; then
    echo "Error: output_state.html not generated"
    exit 1
fi

echo "Verifying State Runtime..."

# Check that 'states' is NOT in the style attribute (it was before)
if grep -q "style=.*states:" output_state.html; then
    echo "Error: 'states' property should not be in CSS style attribute"
    grep "style=" output_state.html
    exit 1
fi

# Check for Runtime JS Injection
if ! grep -q "class NotaComponent" output_state.html; then
    echo "Error: Runtime JS class NotaComponent not found"
    exit 1
fi

# Check for Component Registration script
if ! grep -q "nota_elements\['mainBox'\] = new NotaComponent" output_state.html; then
    echo "Error: Component registration JS not found"
    exit 1
fi

# Check for State Configuration in JS
# Note: Spacing might vary in generated output
if ! grep -q "states: \[{ type: \"State\", name: down, when: ClickEvent, color: red }, { type: \"State\", name: hov, when: HoverEvent, color: green }\]" output_state.html; then
    echo "Error: State configuration JS not found or incorrect"
    cat output_state.html
    exit 1
fi

echo "State Runtime Test Passed!"
rm output_state.html
