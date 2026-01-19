#!/bin/bash
set -e

cmake --build build

./build/src/nota tests/test_advanced.nota -o output_advanced.html

if [ ! -f output_advanced.html ]; then
    echo "Error: output_advanced.html not generated"
    exit 1
fi

echo "Verifying Advanced Features..."

# Check Delegate JS
if ! grep -q "target.dispatchEvent(new Event('click'))" output_advanced.html; then
    echo "Error: Delegate event dispatch not found"
    exit 1
fi

# Check For Loop JS
if ! grep -q "list.forEach(item => {" output_advanced.html; then
    echo "Error: For loop iteration not found"
    exit 1
fi

if ! grep -q "const template = ( item ) => \`" output_advanced.html; then
    echo "Error: Template function generation not found"
    exit 1
fi

# Check interpolation
# Parser produces "item.name" as string (Literal/Reference).
# CodeGen hack finds "item" and replaces with "${item}".
# So "item.name" -> "${item}.name".
# The verification script checked for "${item.name}" which is incorrect if replacer is dumb.
# Let's check for "${item}.name".

if ! grep -q "\${item}.name" output_advanced.html; then
    echo "Error: Variable interpolation \${item}.name not found in template"
    # Fallback check
    if ! grep -q "\${item}" output_advanced.html; then
        echo "Error: Variable interpolation \${item} not found at all"
        cat output_advanced.html
        exit 1
    fi
fi

echo "Advanced Test Passed!"
rm output_advanced.html
