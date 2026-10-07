#!/bin/sh
# Run: ROOT=<worktree of evalon/core-imple-4ccba2f9 with sourcemeta_core_jsonld built> verify/repro/c12_copy_trace.sh  (needs gdb; counts JSON copy-constructor calls reached from lines 210-229 of Processor::context (the entries/current set-up), by source line, for an @import-free local context)
set -eu
HERE="$(cd "$(dirname "$0")" && pwd)"
for DOC in \
  '{"@context":{"name":"http://e/name","nick":{"@id":"http://e/nick","@container":"@set"}},"name":"n"}' \
  '{"@context":[{"name":"http://e/name"},{"nick":{"@id":"http://e/nick","@container":"@set"}}],"name":"n"}'; do
  ROOT="$ROOT" "$HERE/probe.sh" "$DOC"
  gdb -q -batch -x "$HERE/c12_copy_trace.gdb" --args "$ROOT/build/verify_probe" "$DOC" 2>&1 |
    grep -o 'Processor::context .* at .*context.cc:2[12][0-9]$' | sed 's/ (this=.*) at / at /' | sort | uniq -c
done
