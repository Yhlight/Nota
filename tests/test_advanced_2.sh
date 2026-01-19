#!/bin/bash
set -e

cmake --build build

./build/src/nota tests/test_advanced_2.nota -o output_advanced_2.html

if [ ! -f output_advanced_2.html ]; then
    echo "Error: output_advanced_2.html not generated"
    exit 1
fi

echo "Verifying Generic Properties and Interpolation..."

# Check property definition handling (currently ignored by CodeGen but verified by Parser success)
# If parser failed on "list<string>", we wouldn't get here.

# Check For Loop Interpolation Logic
# Loop 1: iterator 'item'
if ! grep -q "\${item}" output_advanced_2.html; then
    echo "Error: Loop 1 interpolation failed"
    exit 1
fi

# Loop 2: iterator 'it'
# Should find ${it}
if ! grep -q "\${it}" output_advanced_2.html; then
    echo "Error: Loop 2 interpolation failed"
    exit 1
fi

# Loop 3: iterator 'iter'
# Should find ${iter}
if ! grep -q "\${iter}" output_advanced_2.html; then
    echo "Error: Loop 3 interpolation failed"
    exit 1
fi

# Since we switched to AST-based interpolation, this should be robust.
# The previous string-replace hack would have replaced 'item' inside 'items' -> '${item}s' which is invalid if 'items' is the list.
# AST-based check: 'items' is a ReferenceNode(items). 'item' is the iterator.
# ExpressionStringVisitor will check if "items" starts with "item"?
# No, my logic in ExpressionStringVisitor was:
#
#            if (activeIterators->count(root)) {
#                ss << "${" << node.name << "}";
#                return;
#            }
#
# If iterator is 'item'.
# Reference 'items' (root='items').
# 'activeIterators' contains 'item'.
# 'items' != 'item'. So it prints "items". Correct.
# Reference 'item' (root='item').
# 'item' == 'item'. Prints "${item}". Correct.

echo "Advanced 2 Test Passed!"
rm output_advanced_2.html
