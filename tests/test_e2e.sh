#!/bin/bash
set -e

# Compile first
cmake --build build

# Create test file
cat <<EOF > e2e_test.nota
App {
    width: 100%
    height: 100%
    color: #f0f0f0

    Rect {
        width: 100
        height: 100
        color: red
        radius: 10
    }
}
EOF

# Run compiler
./build/src/nota e2e_test.nota -o output.html

# Verify output exists
if [ ! -f output.html ]; then
    echo "Error: output.html not generated"
    exit 1
fi

# Verify content (basic check)
if ! grep -q "class=\"nota-App\"" output.html; then
    echo "Error: App class not found"
    exit 1
fi

if ! grep -q "background-color: red;" output.html; then
    echo "Error: background-color: red not found"
    exit 1
fi

if ! grep -q "border-radius: 10px;" output.html; then
    echo "Error: border-radius: 10px not found"
    exit 1
fi

echo "E2E Test Passed!"
rm e2e_test.nota output.html
