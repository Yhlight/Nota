#!/bin/bash
set -e

cmake --build build

./build/src/nota tests/test_positioning.nota -o output_pos.html

if [ ! -f output_pos.html ]; then
    echo "Error: output_pos.html not generated"
    exit 1
fi

echo "Verifying Positioning..."

# Check Top Left
# x: 10 -> left: 10px. y: 10 -> top: 10px. position: left top (removed from css)
if ! grep -q "left: 10px" output_pos.html; then
    echo "Error: Top Left positioning failed (left: 10px)"
    exit 1
fi

# Check Bottom Right
# position: right bottom. x: 10 -> right: 10px. y: 10 -> bottom: 10px.
if ! grep -q "right: 10px" output_pos.html; then
    echo "Error: Bottom Right positioning failed (right: 10px)"
    exit 1
fi
if ! grep -q "bottom: 10px" output_pos.html; then
    echo "Error: Bottom Right positioning failed (bottom: 10px)"
    exit 1
fi

# Check Center
# position: center. x: 0. left: 50% + 0px. top: 50% + 0px. transform...
# x is 0 -> 0px.
if ! grep -q "left: calc(50% + 0px)" output_pos.html; then
    echo "Error: Center positioning failed (left calc)"
    cat output_pos.html
    exit 1
fi
if ! grep -q "transform: translate(-50%, -50%)" output_pos.html; then
    echo "Error: Center positioning failed (transform)"
    exit 1
fi

# Check Z-Index
if ! grep -q "z-index: 2" output_pos.html; then
    echo "Error: Z-Index failed"
    exit 1
fi

echo "Positioning Test Passed!"
rm output_pos.html
