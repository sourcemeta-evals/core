#!/bin/sh
# Run: ROOT=<worktree of evalon/core-imple-b7cd8e16 with sourcemeta_core_jsonld built> verify/repro/c9_direction_in_1_0.sh  (1.0 mode rejects the term definition; 1.1 mode applies it)
P="$(dirname "$0")/probe.sh"
D='{"@context":{"p":{"@id":"http://e/p","@direction":"rtl"}},"p":"hello"}'
"$P" --v1.0 "$D"
"$P" "$D"
