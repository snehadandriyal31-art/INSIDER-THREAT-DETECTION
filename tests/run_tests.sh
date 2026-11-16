#!/usr/bin/env bash
# run_tests.sh — builds and runs every C unit-test suite. `make test` calls the same binaries.
set -e
cd "$(dirname "$0")/.."
make test
