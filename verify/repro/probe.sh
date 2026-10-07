#!/bin/sh
# Run: verify/repro/probe.sh [--v1.0] '<json document>'  (ROOT=<checkout with ./build> selects another worktree; needs the sourcemeta_core_jsonld target built there)
set -eu
HERE="$(cd "$(dirname "$0")" && pwd)"
ROOT="${ROOT:-$(cd "$HERE/../.." && pwd)}"
BIN="$ROOT/build/verify_probe"
if [ ! -x "$BIN" ] || [ "$HERE/probe.cc" -nt "$BIN" ] || [ "$ROOT/build/src/core/jsonld/libsourcemeta_core_jsonld.a" -nt "$BIN" ]; then
  CXX="$(grep '^CMAKE_CXX_COMPILER:' "$ROOT/build/CMakeCache.txt" | cut -d= -f2)"
  INCLUDES="$(find "$ROOT/src" -type d -name include | sed 's/^/-I/' | tr '\n' ' ')"
  EXPORTS="$(find "$ROOT/build/src" -name '*_export.h' -exec dirname {} \; | xargs -n1 dirname | xargs -n1 dirname | sort -u | sed 's/^/-I/' | tr '\n' ' ')"
  LIBS="$(find "$ROOT/build/src" "$ROOT/build" -maxdepth 4 -name '*.a' ! -name 'libg*' | sort -u | tr '\n' ' ')"
  # shellcheck disable=SC2086
  "$CXX" -std=c++23 -g $INCLUDES $EXPORTS "$HERE/probe.cc" -Wl,--start-group $LIBS -Wl,--end-group -o "$BIN"
fi
exec "$BIN" "$@"
