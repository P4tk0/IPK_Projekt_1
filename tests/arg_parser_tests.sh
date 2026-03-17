#!/bin/bash
# ============================================================
# Argument parser tests for ipk-L4-scan
# ============================================================

BIN="./ipk-L4-scan"
PASS=0
FAIL=0

# ── helpers ──────────────────────────────────────────────────

run() {
    # run <description> <expected_exit> <args...>
    local desc="$1"
    local expected_exit="$2"
    shift 2

    $BIN "$@" >/dev/null 2>&1
    local actual_exit=$?

    if [ "$actual_exit" -eq "$expected_exit" ]; then
        echo "[PASS] $desc"
        ((PASS++))
    else
        echo "[FAIL] $desc  (expected exit=$expected_exit, got exit=$actual_exit)"
        ((FAIL++))
    fi
}

run_stdout() {
    # run_stdout <description> <pattern> <args...>
    # asserts exit 0 AND that stdout matches grep pattern
    local desc="$1"
    local pattern="$2"
    shift 2

    local out
    out=$($BIN "$@" 2>/dev/null)
    local actual_exit=$?

    if [ "$actual_exit" -ne 0 ]; then
        echo "[FAIL] $desc  (expected exit=0, got exit=$actual_exit)"
        ((FAIL++))
    elif ! echo "$out" | grep -qE "$pattern"; then
        echo "[FAIL] $desc  (stdout did not match '$pattern')"
        echo "       stdout: $out"
        ((FAIL++))
    else
        echo "[PASS] $desc"
        ((PASS++))
    fi
}

run_stderr() {
    # run_stderr <description> <args...>
    # asserts non-zero exit AND something on stderr
    local desc="$1"
    shift

    local err
    err=$($BIN "$@" 2>&1 >/dev/null)
    local actual_exit=$?

    if [ "$actual_exit" -eq 0 ]; then
        echo "[FAIL] $desc  (expected non-zero exit, got 0)"
        ((FAIL++))
    elif [ -z "$err" ]; then
        echo "[FAIL] $desc  (expected stderr output, got none)"
        ((FAIL++))
    else
        echo "[PASS] $desc"
        ((PASS++))
    fi
}

# ── guard ────────────────────────────────────────────────────

if [ ! -x "$BIN" ]; then
    echo "Binary '$BIN' not found or not executable. Run 'make' first."
    exit 1
fi

echo "=============================="
echo " ipk-L4-scan  argument tests"
echo "=============================="

# ── 1. HELP ──────────────────────────────────────────────────
echo ""
echo "--- HELP ---"

run_stdout "short help flag"          "Usage|usage|-i" -h
run_stdout "long help flag"           "Usage|usage|-i" --help
run_stdout "-h wins over other flags" "Usage|usage|-i" -i eth0 -t 22 localhost -h

# ── 2. INTERFACE LIST ────────────────────────────────────────
echo ""
echo "--- INTERFACE LIST ---"

run_stdout "-i alone lists interfaces"          ".+" -i
run_stdout "-i as last arg (no value) → list"   ".+" -t 22 -i

# ── 3. VALID SCANS ───────────────────────────────────────────
echo ""
echo "--- VALID SCANS (exit 0) ---"

run "tcp single port"          0 -i eth0 -t 22 localhost
run "udp single port"          0 -i eth0 -u 53 localhost
run "tcp + udp together"       0 -i eth0 -t 80 -u 53 localhost
run "tcp port range"           0 -i eth0 -t 1-1024 localhost
run "tcp comma list"           0 -i eth0 -t 22,80,443 localhost
run "udp comma list"           0 -i eth0 -u 53,67 localhost
run "custom timeout -w"        0 -i eth0 -t 80 -w 3000 localhost
run "IPv4 address as host"     0 -i eth0 -t 80 127.0.0.1
run "IPv6 address as host"     0 -i eth0 -t 80 ::1
run "args in different order"  0 -t 22 localhost -i eth0
run "host before -i"           0 localhost -i eth0 -t 22

# ── 4. MISSING / INVALID ARGS ────────────────────────────────
echo ""
echo "--- INVALID / MISSING ARGS (non-zero exit + stderr) ---"

run_stderr "no arguments at all"
run_stderr "missing -i"              -t 22 localhost
run_stderr "missing HOST"            -i eth0 -t 22
run_stderr "no -t and no -u"         -i eth0 localhost
run_stderr "-t without value"        -i eth0 -t
run_stderr "-u without value"        -i eth0 -u
run_stderr "-w without value"        -i eth0 -t 22 -w localhost
run_stderr "-w zero"                 -i eth0 -t 22 -w 0 localhost
run_stderr "-w negative"             -i eth0 -t 22 -w -1 localhost
run_stderr "port out of range high"  -i eth0 -t 99999 localhost
run_stderr "port out of range zero"  -i eth0 -t 0 localhost
run_stderr "invalid port range lo>hi" -i eth0 -t 100-10 localhost
run_stderr "unknown flag"            -i eth0 -t 22 --unknown localhost
run_stderr "multiple HOST args"      -i eth0 -t 22 host1 host2

# ── summary ──────────────────────────────────────────────────
echo ""
echo "=============================="
echo " Results: $PASS passed, $FAIL failed"
echo "=============================="

[ "$FAIL" -eq 0 ] && exit 0 || exit 1