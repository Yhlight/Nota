#!/bin/bash
set -e

cmake --build build

./build/src/nota tests/test_fixes.nota -o output_fixes.html

if [ ! -f output_fixes.html ]; then
    echo "Error: output_fixes.html not generated"
    exit 1
fi

echo "Verifying Fixes..."

# Check CSS Math: calc(100px * 2) (Right side unitless)
if ! grep -q "width: calc(100px \* 2)" output_fixes.html; then
    echo "Error: CSS Math fix failed. Expected 'calc(100px * 2)'"
    cat output_fixes.html
    exit 1
fi

# Check HTML Escaping: "alert(&quot;&lt;Safe&gt;&quot;)"
# Note: onclick is attribute. Value is "alert(\"<Safe>\")".
# Escaped: "alert(&quot;&lt;Safe&gt;&quot;)"
# Grep needs escaping too.
# The attribute value in HTML file will be: onclick="alert(&quot;&lt;Safe&gt;&quot;)"
if ! grep -q "onclick=\"alert(&quot;&lt;Safe&gt;&quot;)\"" output_fixes.html; then
    echo "Error: HTML Escaping failed."
    cat output_fixes.html
    exit 1
fi

echo "Fixes Test Passed!"
rm output_fixes.html
