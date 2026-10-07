#!/bin/sh
# Run: verify/repro/c1_joint_entry_pointer.sh  (on the audited branch after building sourcemeta_core_jsonld; every pointer printed stops at the enclosing object instead of naming a conflicting entry)
P="$(dirname "$0")/probe.sh"
"$P" '{"http://example.com/p": {"@value": "x", "@id": "http://example.com/y"}}'
"$P" '{"http://example.com/p": {"@value": "x", "@language": "en", "@type": "http://example.com/T"}}'
"$P" '{"http://example.com/p": {"@list": ["a"], "@id": "http://example.com/x"}}'
"$P" '{"http://example.com/p": {"@list": ["a"], "@set": ["b"]}}'
"$P" '{"http://example.com/p": {"@set": ["a"], "@type": "http://example.com/T"}}'
echo '--- contrast: Colliding keywords, the joint-entry error the task names, reports the later entry'
"$P" '{"@context":{"id2":"@id"},"http://example.com/p":{"@id":"http://a/1","id2":"http://a/2"}}'
