#!/bin/bash
set -e

cmake --build build

echo "Compiling Demo..."
./build/src/nota tests/demo.nota -o demo.html

if [ ! -f demo.html ]; then
    echo "Error: demo.html not generated"
    exit 1
fi

echo "Verifying Demo Output..."

check_grep() {
    if grep -q "$1" demo.html; then
        echo "[OK] Found '$1'"
    else
        echo "[FAIL] Not found '$1'"
        exit 1
    fi
}

# Basic checks
check_grep "nota-App"
check_grep "nota-Button"
check_grep "nota-Card"

# Check Theme usage
# In demo.html, it was "padding: theme.spacing" (invalid CSS but compiled)
check_grep "padding: theme.spacing"

# Check Loop
# Since loop is dynamic JS, we check for the script
check_grep "const list = menuItems"
check_grep "template(item)"

# Check Delegate
check_grep "dispatchEvent"

# Check Custom Property Data Attribute (Theme)
check_grep "data-theme="

# Check that For Loop container generated
check_grep "id=\"nota-for-"

echo "Demo Passed!"
rm demo.html
