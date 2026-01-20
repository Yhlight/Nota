#!/bin/bash
set -e

cmake --build build

./build/src/nota tests/transitive/A.nota -o output_transitive.html

if [ ! -f output_transitive.html ]; then
    echo "Error: output_transitive.html not generated"
    exit 1
fi

echo "Verifying Transitive Import..."

# We expect DeepRect properties (width: 999px) to be present.
# MidBox contains DeepRect. DeepRect is defined in C.
# If C is not loaded, DeepRect is unknown -> generic div -> no width 999.

if grep -q "width: 999px" output_transitive.html; then
    echo "Success: Transitive import working (Unexpectedly?)"
else
    echo "Failure: Transitive import properties missing (Expected)"
    # We exit 0 here because this is a reproduction script confirming the issue.
    # But for the final verification step, we will want it to succeed.
    # For now, I'll exit 1 to show it failed if I run it as a test?
    # No, I want to confirm failure first.
    # I'll print FAILURE but exit 0 so I can proceed.
fi

rm output_transitive.html
