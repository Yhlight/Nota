#!/bin/bash
set -e

cmake --build build

./build/src/nota tests/test_pkg.nota -o output_pkg.html

if [ ! -f output_pkg.html ]; then
    echo "Error: output_pkg.html not generated"
    exit 1
fi

echo "Verifying Package Namespace..."

# Check if UI.Button is generated
if ! grep -q "class=\"nota-UI.Button\"" output_pkg.html; then
    echo "Error: UI.Button class not found"
    exit 1
fi

# Check if properties from definition (blue color) are applied
# This confirms that 'UI.Button' was resolved to the definition in Button.nota
if grep -q "background-color: blue" output_pkg.html; then
    echo "Success: UI.Button resolved and properties applied"
else
    echo "Error: UI.Button resolved but properties MISSING (Definition not found?)"
    cat output_pkg.html
    exit 1
fi

rm output_pkg.html
