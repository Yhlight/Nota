#!/bin/bash
set -e

cmake --build build

./build/src/nota tests/test_structs.nota -o output_structs.html

if [ ! -f output_structs.html ]; then
    echo "Error: output_structs.html not generated"
    exit 1
fi

echo "Verifying Structs..."

# Check Class Definition
if ! grep -q "class Point {" output_structs.html; then
    echo "Error: Struct definition not found"
    exit 1
fi

if ! grep -q "this.x = x;" output_structs.html; then
    echo "Error: Struct field assignment not found"
    exit 1
fi

# Check Instantiation in style/attribute (even if invalid CSS, it verifies parsing)
# "pt: new Point(10, 20)"
if ! grep -q "new Point(10, 20)" output_structs.html; then
    echo "Error: Struct instantiation not found"
    exit 1
fi

echo "Structs Test Passed!"
rm output_structs.html
