#!/bin/sh
# Run: verify/repro/c3_c5_type_protected_false.sh  (on the audited branch; line 1 should succeed like line 3 but raises Invalid context nullification)
P="$(dirname "$0")/probe.sh"
"$P" '{"@context":[{"@protected":true,"@type":{"@container":"@set","@protected":false}},null],"@type":"http://e/T"}'
echo '--- contrast: same shape without the context default succeeds'
"$P" '{"@context":[{"@type":{"@container":"@set","@protected":false}},null],"@type":"http://e/T"}'
echo '--- contrast: an ordinary term honours explicit @protected: false under the same default'
"$P" '{"@context":[{"@protected":true,"t":{"@id":"http://e/t","@protected":false}},null],"@type":"http://e/T"}'
echo '--- the @type definition is treated as protected for redefinition too'
"$P" '{"@context":[{"@protected":true,"@type":{"@container":"@set","@protected":false}},{"@type":{"@protected":true}}],"@type":"http://e/T"}'
