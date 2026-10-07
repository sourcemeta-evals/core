#!/bin/sh
# Run: ROOT=<worktree of evalon/core-imple-188675ac after `make configure compile`> verify/repro/c11_packaging_consumer_not_run.sh  (breaks the consumer's expected value, shows the packaging tests still pass, then restores the file)
set -eu
cd "$ROOT"
MAIN=test/packaging/find_package_jsonld/main.cc
echo '--- registered packaging tests and their commands'
ctest --test-dir ./build -N -V -R 'find_package' | grep -E 'Test command|Test +#'
echo '--- 1. unmodified consumer'
ctest --test-dir ./build --build-config Debug -R 'find_package_jsonld' 2>&1 | grep -E 'Test +#|tests passed|tests failed'
set +e; ./build/test/packaging/find_package_jsonld/core_jsonld_consumer; echo "direct run of consumer: exit code $?"; set -e
echo '--- 2. consumer with a deliberately wrong expected value'
sed -i 's/urn:example:T"\]}\]/urn:example:WRONG"]}]/' "$MAIN"
git diff --stat -- "$MAIN" | head -1
ctest --test-dir ./build --build-config Debug -R 'find_package_jsonld' 2>&1 | grep -E 'Test +#|tests passed|tests failed'
set +e; ./build/test/packaging/find_package_jsonld/core_jsonld_consumer; echo "direct run of consumer: exit code $?"; set -e
git checkout -- "$MAIN"
