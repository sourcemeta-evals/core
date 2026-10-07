#!/bin/sh
# Run: ROOT=<configured worktree of evalon/core-imple-109f07fc> verify/repro/c10_pragma_trace.sh  (preprocesses a consumer of <sourcemeta/core/jsonld.h> with _MSC_VER defined and replays every `#pragma warning` for 4251/4275 in order; the last line is the state consumer code sees)
set -eu
ROOT="${ROOT:-$(cd "$(dirname "$0")/../.." && pwd)}"
CXX="$(grep '^CMAKE_CXX_COMPILER:' "$ROOT/build/CMakeCache.txt" | cut -d= -f2)"
INCLUDES="$(find "$ROOT/src" -type d -name include | sed 's/^/-I/' | tr '\n' ' ')"
EXPORTS="$(find "$ROOT/build/src" -name '*_export.h' -exec dirname {} \; | xargs -n1 dirname | xargs -n1 dirname | sort -u | sed 's/^/-I/' | tr '\n' ' ')"
TMP="$(mktemp -d)"
printf '#include <sourcemeta/core/jsonld.h>\nint consumer_code_starts_here;\n' > "$TMP/consumer.cc"
# MSVC-only system headers that the _MSC_VER path includes are stubbed out (they carry no warning pragmas of ours)
mkdir "$TMP/stubs"
for header in intrin.h; do : > "$TMP/stubs/$header"; done
# shellcheck disable=SC2086
"$CXX" -std=c++23 -E -D_MSC_VER=1940 -I"$TMP/stubs" $INCLUDES $EXPORTS "$TMP/consumer.cc" 2>"$TMP/stderr.log" |
  awk -v root="$ROOT/" '
    /^# [0-9]+ "/ { file = $3; gsub(/"/, "", file); sub(root, "", file); next }
    /#pragma warning/ && /42(51|75)/ {
      for (w = 4251; w <= 4275; w += 24) if (index($0, w)) state[w] = ($0 ~ /disable/) ? "disabled" : "default"
      printf "%-70s %-40s => 4251=%s 4275=%s\n", file, $0, state[4251] ? state[4251] : "default", state[4275] ? state[4275] : "default"
    }
    /consumer_code_starts_here/ { printf "AT CONSUMER CODE: 4251=%s 4275=%s\n", state[4251] ? state[4251] : "default", state[4275] ? state[4275] : "default" }'
grep -c "error" "$TMP/stderr.log" | sed "s/^/preprocessor error lines: /"; grep -B2 -A3 "error" "$TMP/stderr.log" | head -20; rm -rf "$TMP"
