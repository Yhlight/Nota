#!/bin/bash
set -e

cmake --build build

./build/src/nota tests/test_exports.nota -o output_exports.html

if [ ! -f output_exports.html ]; then
    echo "Error: output_exports.html not generated"
    exit 1
fi

echo "Verifying Exports..."

# Check Internal
# Internal was NOT exported.
# SemanticAnalyzer should NOT register P1.Internal.
# CodeGen will treat P1.Internal as a standard div (fallback) but won't apply properties from definition?
# Actually, if it's not registered, `isCustom` is false.
# CodeGen output: <div class="nota-P1.Internal">...</div>
# But it won't have the properties from Internal definition (width 10, height 10, color gray).
# Let's check for properties.

if grep -q "background-color: gray" output_exports.html; then
    echo "Error: Internal component properties should not be applied (it wasn't exported)"
    exit 1
fi

# Check Public
# Should have properties (width 20, height 20, color green).
if ! grep -q "background-color: green" output_exports.html; then
    echo "Error: Public component properties not found (export failed?)"
    exit 1
fi

echo "Export Test Passed!"
rm output_exports.html
