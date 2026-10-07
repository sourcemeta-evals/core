#!/bin/sh
# Run: ROOT=<worktree of evalon/core-imple-dac8ddff with sourcemeta_core_jsonld built> verify/repro/c8_included_graph_node.sh  (first three documents are rejected, the last two with an extra property are accepted)
P="$(dirname "$0")/probe.sh"
"$P" '{"@id":"http://e/a","@included":{"@id":"http://e/b","@graph":[{"@id":"http://e/c","http://e/q":"v"}]}}'
"$P" '{"@id":"http://e/a","@included":{"@graph":[{"@id":"http://e/c","http://e/q":"v"}]}}'
"$P" '{"@id":"http://e/a","@included":[{"@id":"http://e/b","@index":"i","@graph":[{"@id":"http://e/c","http://e/q":"v"}]}]}'
echo '--- with an ordinary property or @type next to @graph'
"$P" '{"@id":"http://e/a","@included":{"@id":"http://e/b","http://e/name":"n","@graph":[{"@id":"http://e/c","http://e/q":"v"}]}}'
"$P" '{"@id":"http://e/a","@included":{"@id":"http://e/b","@type":"http://e/T","@graph":[{"@id":"http://e/c","http://e/q":"v"}]}}'
