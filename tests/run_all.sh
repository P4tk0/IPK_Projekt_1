#!/bin/bash
# ============================================================
# Runs all test suites and prints aggregated statistics
# ============================================================

TESTS=(
    "args|./tests/arg_parser_tests.sh"
    #"tcp|./tests/tcp_tests.sh"
    #"udp|./tests/udp_tests.sh"
)

TOTAL_PASS=0
TOTAL_FAIL=0

for entry in "${TESTS[@]}"; do
    name="${entry%%|*}"
    script="${entry##*|}"

    echo "=============================="
    echo " Suite: $name"
    echo "=============================="

    if [ ! -x "$script" ]; then
        echo "[SKIP] $script not found or not executable"
        continue
    fi

    output=$("$script" 2>&1)
    echo "$output"

    suite_pass=$(echo "$output" | grep -c '^\[PASS\]')
    suite_fail=$(echo "$output" | grep -c '^\[FAIL\]')

    TOTAL_PASS=$((TOTAL_PASS + suite_pass))
    TOTAL_FAIL=$((TOTAL_FAIL + suite_fail))
done

echo ""
echo "=============================="
echo " TOTAL: $TOTAL_PASS passed, $TOTAL_FAIL failed"
echo "=============================="

[ "$TOTAL_FAIL" -eq 0 ] && exit 0 || exit 1