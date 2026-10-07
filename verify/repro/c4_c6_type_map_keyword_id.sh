#!/bin/sh
# Run: verify/repro/c4_c6_type_map_keyword_id.sh  (on the audited branch; the type-map lines emit "@id":"@foo", the plain @type:@id term emits "@id":null)
P="$(dirname "$0")/probe.sh"
"$P" '{"@context":{"@vocab":"http://e/","p":{"@container":"@type","@type":"@id"}},"p":{"http://e/T":"@foo"}}'
"$P" '{"@context":{"@vocab":"http://e/","p":{"@container":"@type","@type":"@vocab"}},"p":{"http://e/T":"@foo"}}'
"$P" '{"@context":{"@vocab":"http://e/","p":{"@container":"@type"}},"p":{"http://e/T":"@foo"}}'
echo '--- contrast: the same term without the type map'
"$P" '{"@context":{"@vocab":"http://e/","p":{"@type":"@id"}},"p":"@foo"}'
