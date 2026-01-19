#!/bin/bash
set -e

cmake --build build

./build/src/nota tests/test_package.nota -o output_package.html

if [ ! -f output_package.html ]; then
    echo "Error: output_package.html not generated"
    exit 1
fi

echo "Verifying Package Support..."

# Check if class name from package import is present
# Import alias is UI, component is Button. Registry stores "UI.Button".
if ! grep -q "class=\"nota-UI.Button\"" output_package.html; then
    echo "Error: Package component class not found"
    exit 1
fi

echo "Package Test Passed!"
rm output_package.html
