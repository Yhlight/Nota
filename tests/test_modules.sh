#!/bin/bash
set -e

cmake --build build

./build/src/nota tests/test_modules.nota -o output_modules.html

if [ ! -f output_modules.html ]; then
    echo "Error: output_modules.html not generated"
    exit 1
fi

echo "Verifying Modules and Events..."

# Check module import and expansion
if ! grep -q "class=\"nota-ui.Button\"" output_modules.html; then
    echo "Error: ui.Button class not found (import failed?)"
    exit 1
fi

if ! grep -q "Click Me" output_modules.html; then
    echo "Error: 'Click Me' text from module not found"
    exit 1
fi

# Check Events with HTML entity escaping
# onClick="alert(&quot;Clicked!&quot;)"
if ! grep -q "onclick=\"alert(&quot;Clicked!&quot;)\"" output_modules.html; then
    echo "Error: onclick attribute not found or incorrect escaping"
    exit 1
fi

# onmouseenter="console . log ( &quot;Hovering&quot; ) ; "
if ! grep -q "onmouseenter=\"console . log ( &quot;Hovering&quot; ) ; \"" output_modules.html; then
    echo "Error: onmouseenter attribute incorrect."
    exit 1
fi

echo "Modules Test Passed!"
rm output_modules.html
