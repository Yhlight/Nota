#!/bin/bash
set -e

cmake --build build

./build/src/nota tests/test_comments_color.nota -o output_cc.html

if [ ! -f output_cc.html ]; then
    echo "Error: output_cc.html not generated"
    exit 1
fi

echo "Verifying Comments and Hex Colors..."

if grep -q "background-color: #ff0000" output_cc.html; then
    echo "[OK] Hex color found"
else
    echo "[FAIL] Hex color missing"
    exit 1
fi

if grep -q "data-hex=\"#F0F0F0\"" output_cc.html; then
    echo "[OK] Hex string property found"
else
    echo "[FAIL] Hex string property missing"
    exit 1
fi

echo "Comments & Color Test Passed"
rm output_cc.html
