## Setup

Audited branch `core-implement-json-ld-expansion-perfect` at `10b280998d8bb675c6e309ee4415851cdbcd3f76`, checked out as `verify/core-implement-json-ld-expansion-v-0ec7867d` in `~/repos/core`. Claim-target branches live in a second repository (`sourcemeta-evals/core-implement-json-ld-expansion`) and were fetched into separate worktrees under `~/wt/`.

```sh
cd ~/repos/core
git fetch origin core-implement-json-ld-expansion-perfect
git checkout -b verify/core-implement-json-ld-expansion-v-0ec7867d origin/core-implement-json-ld-expansion-perfect
make configure && cmake --build ./build --config Debug --target sourcemeta_core_jsonld_unit --parallel 8

git remote add claims https://github.com/sourcemeta-evals/core-implement-json-ld-expansion
for b in dac8ddff b7cd8e16 109f07fc 188675ac 4ccba2f9; do
  git fetch claims evalon/core-imple-$b:refs/remotes/claims/$b
  git worktree add -f ~/wt/$b claims/$b
done
# dac8ddff=5f99da8d  b7cd8e16=a32f15b5  109f07fc=bb9dc099  188675ac=05972093  4ccba2f9=29d45422

# jsonld library only, for dac8ddff, b7cd8e16, 4ccba2f9 (109f07fc: configure step only)
cd ~/wt/<b> && cmake -S . -B ./build -DCMAKE_BUILD_TYPE:STRING=Debug -DSOURCEMETA_CORE_TESTS:BOOL=OFF \
  -DSOURCEMETA_CORE_BENCHMARK:BOOL=OFF -DSOURCEMETA_CORE_DOCS:BOOL=OFF -DBUILD_SHARED_LIBS:BOOL=OFF \
  && cmake --build ./build --target sourcemeta_core_jsonld --parallel 6
# full build + install for 188675ac
cd ~/wt/188675ac && make configure && cmake --build ./build --config Debug --parallel 8 \
  && cmake --install ./build --prefix ./build/dist --config Debug --component sourcemeta_core \
  && cmake --install ./build --prefix ./build/dist --config Debug --component sourcemeta_core_dev
```

All runtime probes go through `verify/repro/probe.sh [--v1.0] '<json>'`, which compiles `verify/repro/probe.cc` against the static libraries of the checkout named by `ROOT` (default: this checkout) and prints the parsed input (`INPUT`, showing preserved key order) followed by either `OK <expansion>` or `ERROR "<what()>" pointer="<pointer()>"`. It calls only the public `jsonld_expand(input, "", {}, version)`. No production file was modified on any branch.

## C1

Branch: audited branch. Command (`verify/repro/c1_joint_entry_pointer.sh`):

```sh
verify/repro/probe.sh '{"http://example.com/p": {"@value": "x", "@id": "http://example.com/y"}}'
verify/repro/probe.sh '{"http://example.com/p": {"@value": "x", "@language": "en", "@type": "http://example.com/T"}}'
verify/repro/probe.sh '{"http://example.com/p": {"@list": ["a"], "@id": "http://example.com/x"}}'
verify/repro/probe.sh '{"http://example.com/p": {"@list": ["a"], "@set": ["b"]}}'
verify/repro/probe.sh '{"http://example.com/p": {"@set": ["a"], "@type": "http://example.com/T"}}'
verify/repro/probe.sh '{"@context":{"id2":"@id"},"http://example.com/p":{"@id":"http://a/1","id2":"http://a/2"}}'
```

Output:

```console
INPUT {"http://example.com/p":{"@value":"x","@id":"http://example.com/y"}}
ERROR "Invalid value object" pointer="/http:~1~1example.com~1p"
INPUT {"http://example.com/p":{"@value":"x","@language":"en","@type":"http://example.com/T"}}
ERROR "Invalid value object" pointer="/http:~1~1example.com~1p"
INPUT {"http://example.com/p":{"@list":["a"],"@id":"http://example.com/x"}}
ERROR "Invalid set or list object" pointer="/http:~1~1example.com~1p"
INPUT {"http://example.com/p":{"@list":["a"],"@set":["b"]}}
ERROR "Invalid set or list object" pointer="/http:~1~1example.com~1p"
INPUT {"http://example.com/p":{"@set":["a"],"@type":"http://example.com/T"}}
ERROR "Invalid set or list object" pointer="/http:~1~1example.com~1p"
--- contrast: Colliding keywords, the joint-entry error the task names, reports the later entry
INPUT {"@context":{"id2":"@id"},"http://example.com/p":{"@id":"http://a/1","id2":"http://a/2"}}
ERROR "Colliding keywords" pointer="/http:~1~1example.com~1p/id2"
```

Every `Invalid value object` / `Invalid set or list object` pointer is `/http:~1~1example.com~1p`, i.e. the key whose value is the whole conflicting object; none names `@id`, `@type`, `@language`, `@set` or any other participating entry. The contrast line shows the same implementation reporting `Colliding keywords` at the later participating entry (`.../id2`).

The branch's own tests pin this behaviour (assertion text in `test/jsonld/jsonld_expand_error_test.cc`: `invalid_value_object` expects `"/http:~1~1example.com~1p"`, likewise `invalid_set_or_list_object`, `list_object_with_set`), and they pass:

```sh
./build/test/jsonld/sourcemeta_core_jsonld_unit --gtest_filter='JSONLD_expand_error.invalid_value_object:JSONLD_expand_error.invalid_set_or_list_object:JSONLD_expand_error.list_object_with_set' 2>&1 | grep -E 'OK|PASSED|FAILED'
```

```console
[       OK ] JSONLD_expand_error.invalid_value_object (0 ms)
[       OK ] JSONLD_expand_error.invalid_set_or_list_object (0 ms)
[       OK ] JSONLD_expand_error.list_object_with_set (0 ms)
[  PASSED  ] 3 tests.
```

Throw sites: `src/core/jsonld/jsonld_expansion.cc` lines 227, 230, 282, 288 all throw with `pointer` (the object's own location), with no entry token appended.

Impact reasoning: the task statement says that when an error involves two entries jointly it "reports at the input-spelled entry processed later in that order". `@list` + `@set`, `@value` + `@id`, and `@language` + `@type` are joint conflicts between two input entries, yet the reported location stops one level short, so a caller cannot tell from the pointer which entry to remove. The error code itself is correct, and the pointer is still a valid ancestor of the offending entries. Whether this violates the convention depends on reading "the offending payload" as the whole object (then `/p` is right by the first rule) or as the conflicting entry (then the joint-entry rule applies); the behaviour the claim describes is exactly what was observed.

## C2

Branch: audited branch. Independently invalid ordinary terms `z` and `a` (plus `m`), in both insertion orders; the last command is a control showing `z` alone is invalid.

```sh
verify/repro/probe.sh '{"@context":{"z":{"@id":5},"a":{"@id":5}}}'
verify/repro/probe.sh '{"@context":{"a":{"@id":5},"z":{"@id":5}}}'
verify/repro/probe.sh '{"@context":{"z":5,"m":{"@container":"@bogus","@id":"http://e/m"},"a":true}}'
verify/repro/probe.sh '{"@context":{"a":true,"m":{"@container":"@bogus","@id":"http://e/m"},"z":5}}'
verify/repro/probe.sh '{"@context":{"z":{"@id":5}}}'
```

Output:

```console
INPUT {"@context":{"z":{"@id":5},"a":{"@id":5}}}
ERROR "Invalid IRI mapping" pointer="/@context/a/@id"
INPUT {"@context":{"a":{"@id":5},"z":{"@id":5}}}
ERROR "Invalid IRI mapping" pointer="/@context/a/@id"
INPUT {"@context":{"z":5,"m":{"@container":"@bogus","@id":"http://e/m"},"a":true}}
ERROR "Invalid term definition" pointer="/@context/a"
INPUT {"@context":{"a":true,"m":{"@container":"@bogus","@id":"http://e/m"},"z":5}}
ERROR "Invalid term definition" pointer="/@context/a"
INPUT {"@context":{"z":{"@id":5}}}
ERROR "Invalid IRI mapping" pointer="/@context/z/@id"
```

The `INPUT` echo shows the parsed object keeps insertion order (`z` before `a` in the first and third documents), yet the reported error is at `a` in every ordering, and `z` only when `a` is absent. `process_context` (`src/core/jsonld/jsonld_context_processing.cc` lines 429-446) collects the term names from `context.as_object()` and sorts them (`std::ranges::sort(term_names, ...)`) before calling `create_term_definition`, so selection is lexicographical, not insertion-ordered.

## C7

Branch: audited branch. Independently invalid ordinary terms `z` and `a` (plus `m`), in both insertion orders; the last command is a control showing `z` alone is invalid.

```sh
verify/repro/probe.sh '{"@context":{"z":{"@id":5},"a":{"@id":5}}}'
verify/repro/probe.sh '{"@context":{"a":{"@id":5},"z":{"@id":5}}}'
verify/repro/probe.sh '{"@context":{"z":5,"m":{"@container":"@bogus","@id":"http://e/m"},"a":true}}'
verify/repro/probe.sh '{"@context":{"a":true,"m":{"@container":"@bogus","@id":"http://e/m"},"z":5}}'
verify/repro/probe.sh '{"@context":{"z":{"@id":5}}}'
```

Output:

```console
INPUT {"@context":{"z":{"@id":5},"a":{"@id":5}}}
ERROR "Invalid IRI mapping" pointer="/@context/a/@id"
INPUT {"@context":{"a":{"@id":5},"z":{"@id":5}}}
ERROR "Invalid IRI mapping" pointer="/@context/a/@id"
INPUT {"@context":{"z":5,"m":{"@container":"@bogus","@id":"http://e/m"},"a":true}}
ERROR "Invalid term definition" pointer="/@context/a"
INPUT {"@context":{"a":true,"m":{"@container":"@bogus","@id":"http://e/m"},"z":5}}
ERROR "Invalid term definition" pointer="/@context/a"
INPUT {"@context":{"z":{"@id":5}}}
ERROR "Invalid IRI mapping" pointer="/@context/z/@id"
```

The `INPUT` echo shows the parsed object keeps insertion order (`z` before `a` in the first and third documents), yet the reported error is at `a` in every ordering, and `z` only when `a` is absent. `process_context` (`src/core/jsonld/jsonld_context_processing.cc` lines 429-446) collects the term names from `context.as_object()` and sorts them (`std::ranges::sort(term_names, ...)`) before calling `create_term_definition`, so selection is lexicographical, not insertion-ordered.

## C3

Branch: audited branch. Command (`verify/repro/c3_c5_type_protected_false.sh`):

```sh
verify/repro/probe.sh '{"@context":[{"@protected":true,"@type":{"@container":"@set","@protected":false}},null],"@type":"http://e/T"}'
verify/repro/probe.sh '{"@context":[{"@type":{"@container":"@set","@protected":false}},null],"@type":"http://e/T"}'
verify/repro/probe.sh '{"@context":[{"@protected":true,"t":{"@id":"http://e/t","@protected":false}},null],"@type":"http://e/T"}'
verify/repro/probe.sh '{"@context":[{"@protected":true,"@type":{"@container":"@set","@protected":false}},{"@type":{"@protected":true}}],"@type":"http://e/T"}'
```

Output:

```console
INPUT {"@context":[{"@protected":true,"@type":{"@container":"@set","@protected":false}},null],"@type":"http://e/T"}
ERROR "Invalid context nullification" pointer="/@context/1"
--- contrast: same shape without the context default succeeds
INPUT {"@context":[{"@type":{"@container":"@set","@protected":false}},null],"@type":"http://e/T"}
OK [{"@type":["http://e/T"]}]
--- contrast: an ordinary term honours explicit @protected: false under the same default
INPUT {"@context":[{"@protected":true,"t":{"@id":"http://e/t","@protected":false}},null],"@type":"http://e/T"}
OK [{"@type":["http://e/T"]}]
--- the @type definition is treated as protected for redefinition too
INPUT {"@context":[{"@protected":true,"@type":{"@container":"@set","@protected":false}},{"@type":{"@protected":true}}],"@type":"http://e/T"}
ERROR "Protected term redefinition" pointer="/@context/1/@type"
```

The claim's exact context sequence `[{"@protected":true,"@type":{"@container":"@set","@protected":false}},null]` raises `Invalid context nullification` at `/@context/1`. The same `@type` definition without the context-level default nullifies fine, and an ordinary term with explicit `@protected: false` under the same default also nullifies fine, so only the `@type` keyword definition loses its explicit `false`. The fourth document shows the definition is also treated as protected for redefinition (`Protected term redefinition`).

Cause, `src/core/jsonld/jsonld_create_term_definition.cc` lines 131-132:

```cpp
      } else if (!type_definition.is_protected) {
        type_definition.is_protected = state.context_protected;
      }
```

`has_protected` is recorded at line 105 but not consulted here, so an explicit `false` is indistinguishable from "absent" and is replaced by the context default. The existing test `protected_only_type_keyword_definition` (`test/jsonld/jsonld_expand_test.cc` line 1594) only covers `@protected: false` with no context-level default, so it does not catch this.

Impact reasoning: JSON-LD 1.1 create-term-definition gives a definition's own `@protected` precedence over the context default (ordinary terms on this branch do exactly that). A document that deliberately opts `@type` out of a protected context and later resets the context with `null` (or redefines `@type`) is rejected with a hard error instead of expanding. The trigger is narrow (a protected context that redefines `@type` with an explicit opt-out); the workaround is to drop the context-level `@protected: true` and protect terms individually.

## C5

Branch: audited branch. Command (`verify/repro/c3_c5_type_protected_false.sh`):

```sh
verify/repro/probe.sh '{"@context":[{"@protected":true,"@type":{"@container":"@set","@protected":false}},null],"@type":"http://e/T"}'
verify/repro/probe.sh '{"@context":[{"@type":{"@container":"@set","@protected":false}},null],"@type":"http://e/T"}'
verify/repro/probe.sh '{"@context":[{"@protected":true,"t":{"@id":"http://e/t","@protected":false}},null],"@type":"http://e/T"}'
verify/repro/probe.sh '{"@context":[{"@protected":true,"@type":{"@container":"@set","@protected":false}},{"@type":{"@protected":true}}],"@type":"http://e/T"}'
```

Output:

```console
INPUT {"@context":[{"@protected":true,"@type":{"@container":"@set","@protected":false}},null],"@type":"http://e/T"}
ERROR "Invalid context nullification" pointer="/@context/1"
--- contrast: same shape without the context default succeeds
INPUT {"@context":[{"@type":{"@container":"@set","@protected":false}},null],"@type":"http://e/T"}
OK [{"@type":["http://e/T"]}]
--- contrast: an ordinary term honours explicit @protected: false under the same default
INPUT {"@context":[{"@protected":true,"t":{"@id":"http://e/t","@protected":false}},null],"@type":"http://e/T"}
OK [{"@type":["http://e/T"]}]
--- the @type definition is treated as protected for redefinition too
INPUT {"@context":[{"@protected":true,"@type":{"@container":"@set","@protected":false}},{"@type":{"@protected":true}}],"@type":"http://e/T"}
ERROR "Protected term redefinition" pointer="/@context/1/@type"
```

The claim's exact context sequence `[{"@protected":true,"@type":{"@container":"@set","@protected":false}},null]` raises `Invalid context nullification` at `/@context/1`. The same `@type` definition without the context-level default nullifies fine, and an ordinary term with explicit `@protected: false` under the same default also nullifies fine, so only the `@type` keyword definition loses its explicit `false`. The fourth document shows the definition is also treated as protected for redefinition (`Protected term redefinition`).

Cause, `src/core/jsonld/jsonld_create_term_definition.cc` lines 131-132:

```cpp
      } else if (!type_definition.is_protected) {
        type_definition.is_protected = state.context_protected;
      }
```

`has_protected` is recorded at line 105 but not consulted here, so an explicit `false` is indistinguishable from "absent" and is replaced by the context default. The existing test `protected_only_type_keyword_definition` (`test/jsonld/jsonld_expand_test.cc` line 1594) only covers `@protected: false` with no context-level default, so it does not catch this.

Impact reasoning: JSON-LD 1.1 create-term-definition gives a definition's own `@protected` precedence over the context default (ordinary terms on this branch do exactly that). A document that deliberately opts `@type` out of a protected context and later resets the context with `null` (or redefines `@type`) is rejected with a hard error instead of expanding. The trigger is narrow (a protected context that redefines `@type` with an explicit opt-out); the workaround is to drop the context-level `@protected: true` and protect terms individually.

## C4

Branch: audited branch. Command (`verify/repro/c4_c6_type_map_keyword_id.sh`):

```sh
verify/repro/probe.sh '{"@context":{"@vocab":"http://e/","p":{"@container":"@type","@type":"@id"}},"p":{"http://e/T":"@foo"}}'
verify/repro/probe.sh '{"@context":{"@vocab":"http://e/","p":{"@container":"@type","@type":"@vocab"}},"p":{"http://e/T":"@foo"}}'
verify/repro/probe.sh '{"@context":{"@vocab":"http://e/","p":{"@container":"@type"}},"p":{"http://e/T":"@foo"}}'
verify/repro/probe.sh '{"@context":{"@vocab":"http://e/","p":{"@type":"@id"}},"p":"@foo"}'
```

Output:

```console
INPUT {"@context":{"@vocab":"http://e/","p":{"@container":"@type","@type":"@id"}},"p":{"http://e/T":"@foo"}}
OK [{"http://e/p":[{"@id":"@foo","@type":["http://e/T"]}]}]
INPUT {"@context":{"@vocab":"http://e/","p":{"@container":"@type","@type":"@vocab"}},"p":{"http://e/T":"@foo"}}
OK [{"http://e/p":[{"@id":"@foo","@type":["http://e/T"]}]}]
INPUT {"@context":{"@vocab":"http://e/","p":{"@container":"@type"}},"p":{"http://e/T":"@foo"}}
OK [{"http://e/p":[{"@id":"@foo","@type":["http://e/T"]}]}]
--- contrast: the same term without the type map
INPUT {"@context":{"@vocab":"http://e/","p":{"@type":"@id"}},"p":"@foo"}
OK [{"http://e/p":[{"@id":null}]}]
```

With `@container: @type` and identifier coercion (`@type: @id` or `@type: @vocab`), the keyword-shaped string `@foo` comes out as `"@id":"@foo"`. The same term without the type map yields `[{"@id":null}]`, which is the behaviour the task statement prescribes for a `@type: @id` term receiving a keyword-shaped invalid value.

Cause, `src/core/jsonld/jsonld_expansion.cc` lines 953-958: `expand_iri` returns no value for the keyword-shaped string and the code substitutes the raw input:

```cpp
            const auto referenced{expand_iri(state, active_context, raw_string,
                                             true, reference_vocab, nullptr,
                                             nullptr, empty_weak_pointer)};
            reference.assign_assume_new(JSON::String{KEYWORD_ID},
                                        JSON{referenced.value_or(raw_string)},
                                        KEYWORD_ID_HASH);
```

Impact reasoning: the output looks like a successful expansion but carries `"@id":"@foo"`, a keyword-shaped string that is not an IRI, in a position consumers treat as a node identifier; the same value on the same term outside a type map gives `null`, so results differ by container. No error is raised, so nothing signals the bad identifier. Trigger: a keyword-like string (`@` + letters) as a type-map value, which is unusual input. The third line shows the substitution also happens with no identifier coercion at all, since the type-map branch treats every string as a node reference.

## C6

Branch: audited branch. Command (`verify/repro/c4_c6_type_map_keyword_id.sh`):

```sh
verify/repro/probe.sh '{"@context":{"@vocab":"http://e/","p":{"@container":"@type","@type":"@id"}},"p":{"http://e/T":"@foo"}}'
verify/repro/probe.sh '{"@context":{"@vocab":"http://e/","p":{"@container":"@type","@type":"@vocab"}},"p":{"http://e/T":"@foo"}}'
verify/repro/probe.sh '{"@context":{"@vocab":"http://e/","p":{"@container":"@type"}},"p":{"http://e/T":"@foo"}}'
verify/repro/probe.sh '{"@context":{"@vocab":"http://e/","p":{"@type":"@id"}},"p":"@foo"}'
```

Output:

```console
INPUT {"@context":{"@vocab":"http://e/","p":{"@container":"@type","@type":"@id"}},"p":{"http://e/T":"@foo"}}
OK [{"http://e/p":[{"@id":"@foo","@type":["http://e/T"]}]}]
INPUT {"@context":{"@vocab":"http://e/","p":{"@container":"@type","@type":"@vocab"}},"p":{"http://e/T":"@foo"}}
OK [{"http://e/p":[{"@id":"@foo","@type":["http://e/T"]}]}]
INPUT {"@context":{"@vocab":"http://e/","p":{"@container":"@type"}},"p":{"http://e/T":"@foo"}}
OK [{"http://e/p":[{"@id":"@foo","@type":["http://e/T"]}]}]
--- contrast: the same term without the type map
INPUT {"@context":{"@vocab":"http://e/","p":{"@type":"@id"}},"p":"@foo"}
OK [{"http://e/p":[{"@id":null}]}]
```

With `@container: @type` and identifier coercion (`@type: @id` or `@type: @vocab`), the keyword-shaped string `@foo` comes out as `"@id":"@foo"`. The same term without the type map yields `[{"@id":null}]`, which is the behaviour the task statement prescribes for a `@type: @id` term receiving a keyword-shaped invalid value.

Cause, `src/core/jsonld/jsonld_expansion.cc` lines 953-958: `expand_iri` returns no value for the keyword-shaped string and the code substitutes the raw input:

```cpp
            const auto referenced{expand_iri(state, active_context, raw_string,
                                             true, reference_vocab, nullptr,
                                             nullptr, empty_weak_pointer)};
            reference.assign_assume_new(JSON::String{KEYWORD_ID},
                                        JSON{referenced.value_or(raw_string)},
                                        KEYWORD_ID_HASH);
```

Impact reasoning: the output looks like a successful expansion but carries `"@id":"@foo"`, a keyword-shaped string that is not an IRI, in a position consumers treat as a node identifier; the same value on the same term outside a type map gives `null`, so results differ by container. No error is raised, so nothing signals the bad identifier. Trigger: a keyword-like string (`@` + letters) as a type-map value, which is unusual input. The third line shows the substitution also happens with no identifier coercion at all, since the type-map branch treats every string as a node reference.

## C8

Branch: `evalon/core-imple-dac8ddff` (`5f99da8d`), worktree `~/wt/dac8ddff`. Command (`ROOT=~/wt/dac8ddff verify/repro/c8_included_graph_node.sh`):

```sh
ROOT=~/wt/dac8ddff verify/repro/probe.sh '{"@id":"http://e/a","@included":{"@id":"http://e/b","@graph":[{"@id":"http://e/c","http://e/q":"v"}]}}'
ROOT=~/wt/dac8ddff verify/repro/probe.sh '{"@id":"http://e/a","@included":{"@graph":[{"@id":"http://e/c","http://e/q":"v"}]}}'
ROOT=~/wt/dac8ddff verify/repro/probe.sh '{"@id":"http://e/a","@included":[{"@id":"http://e/b","@index":"i","@graph":[{"@id":"http://e/c","http://e/q":"v"}]}]}'
ROOT=~/wt/dac8ddff verify/repro/probe.sh '{"@id":"http://e/a","@included":{"@id":"http://e/b","http://e/name":"n","@graph":[{"@id":"http://e/c","http://e/q":"v"}]}}'
ROOT=~/wt/dac8ddff verify/repro/probe.sh '{"@id":"http://e/a","@included":{"@id":"http://e/b","@type":"http://e/T","@graph":[{"@id":"http://e/c","http://e/q":"v"}]}}'
```

Output:

```console
INPUT {"@id":"http://e/a","@included":{"@id":"http://e/b","@graph":[{"@id":"http://e/c","http://e/q":"v"}]}}
ERROR "Invalid @included value" pointer="/@included"
INPUT {"@id":"http://e/a","@included":{"@graph":[{"@id":"http://e/c","http://e/q":"v"}]}}
ERROR "Invalid @included value" pointer="/@included"
INPUT {"@id":"http://e/a","@included":[{"@id":"http://e/b","@index":"i","@graph":[{"@id":"http://e/c","http://e/q":"v"}]}]}
ERROR "Invalid @included value" pointer="/@included"
--- with an ordinary property or @type next to @graph
INPUT {"@id":"http://e/a","@included":{"@id":"http://e/b","http://e/name":"n","@graph":[{"@id":"http://e/c","http://e/q":"v"}]}}
OK [{"@id":"http://e/a","@included":[{"@graph":[{"@id":"http://e/c","http://e/q":[{"@value":"v"}]}],"@id":"http://e/b","http://e/name":[{"@value":"n"}]}]}]
INPUT {"@id":"http://e/a","@included":{"@id":"http://e/b","@type":"http://e/T","@graph":[{"@id":"http://e/c","http://e/q":"v"}]}}
OK [{"@id":"http://e/a","@included":[{"@graph":[{"@id":"http://e/c","http://e/q":[{"@value":"v"}]}],"@id":"http://e/b","@type":["http://e/T"]}]}]
```

The audited branch accepts the first two documents (same probe, default `ROOT`):

```console
INPUT {"@id":"http://e/a","@included":{"@id":"http://e/b","@graph":[{"@id":"http://e/c","http://e/q":"v"}]}}
OK [{"@id":"http://e/a","@included":[{"@graph":[{"@id":"http://e/c","http://e/q":[{"@value":"v"}]}],"@id":"http://e/b"}]}]
INPUT {"@id":"http://e/a","@included":{"@graph":[{"@id":"http://e/c","http://e/q":"v"}]}}
OK [{"@id":"http://e/a","@included":[{"@graph":[{"@id":"http://e/c","http://e/q":[{"@value":"v"}]}]}]}]
```

Cause, `src/core/jsonld/expand.cc` on that branch: `is_node_object` (line 56) excludes anything `is_graph_object` (line 45) accepts, and `is_graph_object` is true when the object defines `@graph` and has no keys other than `@graph`, `@id`, `@index`. The `@included` handler (line 423) throws `Invalid @included value` for any item that is not `is_node_object`.

Scope, as observed: the rejection applies when the included node carries `@graph` and nothing besides `@id`/`@index`. As soon as it has an ordinary property or `@type` next to `@graph` (last two documents) it is accepted and both the graph and the properties are preserved. So the claim holds for graph-bearing nodes without further properties and does not hold for the "`@graph` plus ordinary node properties" shape.

Impact reasoning: the JSON-LD 1.1 node-object definition only excludes maps with `@value`, `@list`, `@set` (and the top-most `@graph`-only map); a named graph `{"@id": ..., "@graph": [...]}` inside `@included` is a node object and the audited branch expands it. On this branch that input fails outright with `Invalid @included value`. Including a named graph via `@included` is uncommon; the workaround is to add any other property to the included node.

## C9

Branch: `evalon/core-imple-b7cd8e16` (`a32f15b5`), worktree `~/wt/b7cd8e16`. Command (`ROOT=~/wt/b7cd8e16 verify/repro/c9_direction_in_1_0.sh`):

```sh
ROOT=~/wt/b7cd8e16 verify/repro/probe.sh --v1.0 '{"@context":{"p":{"@id":"http://e/p","@direction":"rtl"}},"p":"hello"}'
ROOT=~/wt/b7cd8e16 verify/repro/probe.sh '{"@context":{"p":{"@id":"http://e/p","@direction":"rtl"}},"p":"hello"}'
```

Output (first line pair is 1.0 mode, second is 1.1 mode):

```console
INPUT {"@context":{"p":{"@id":"http://e/p","@direction":"rtl"}},"p":"hello"}
ERROR "Invalid term definition" pointer="/@context/p/@direction"
INPUT {"@context":{"p":{"@id":"http://e/p","@direction":"rtl"}},"p":"hello"}
OK [{"http://e/p":[{"@value":"hello","@direction":"rtl"}]}]
```

Same two commands against the audited branch (default `ROOT`), for contrast:

```console
INPUT {"@context":{"p":{"@id":"http://e/p","@direction":"rtl"}},"p":"hello"}
OK [{"http://e/p":[{"@value":"hello","@direction":"rtl"}]}]
INPUT {"@context":{"p":{"@id":"http://e/p","@direction":"rtl"}},"p":"hello"}
OK [{"http://e/p":[{"@value":"hello","@direction":"rtl"}]}]
```

Cause, `src/core/jsonld/context.cc` lines 554-555 on that branch:

```cpp
  if (this->v1() && input.defines("@direction")) {
    fail("Invalid term definition", path("@direction"));
  }
```

Impact reasoning: in JSON-LD 1.0 mode this branch rejects a term definition solely because it contains `@direction`, where the audited branch accepts the mapping and emits `"@direction":"rtl"` on the string value. The create-term-definition algorithm in the JSON-LD 1.1 API has no processing-mode check on a term's `@direction` entry (unlike `@context`, `@nest`, `@prefix`, and unlike the context-level `@direction`, which both branches reject in 1.0 with `Invalid context entry`). A caller that passes `JSONLDVersion::V1_0` for a document whose context uses term-level `@direction` gets an exception instead of an expansion. Workaround: use the default 1.1 mode. Note that some processors (jsonld.js) also reject unknown term-definition entries in 1.0 mode, so the maintainer has to decide which behaviour is intended; the observed behaviour is as the claim states.

## C10

Branch: `evalon/core-imple-109f07fc` (`bb9dc099`), worktree `~/wt/109f07fc` (configured only, so the generated `*_export.h` headers exist). MSVC is not available on this Linux machine, so the MSVC-active path was traced with the preprocessor: `verify/repro/c10_pragma_trace.sh` preprocesses a consumer translation unit (`#include <sourcemeta/core/jsonld.h>` followed by a marker declaration) with `-D_MSC_VER=1940` (and an empty stub for `<intrin.h>`), then replays every `#pragma warning` naming 4251/4275 in order of appearance.

```sh
ROOT=~/wt/109f07fc verify/repro/c10_pragma_trace.sh | tail -6
```

Output:

```console
src/core/jsonpointer/include/sourcemeta/core/jsonpointer_position.h    #pragma warning(default : 4251)          => 4251=default 4275=default
src/core/jsonpointer/include/sourcemeta/core/jsonpointer_walker.h      #pragma warning(disable : 4251)          => 4251=disabled 4275=default
src/core/jsonpointer/include/sourcemeta/core/jsonpointer_walker.h      #pragma warning(default : 4251)          => 4251=default 4275=default
src/core/jsonld/include/sourcemeta/core/jsonld_error.h                 #pragma warning(disable : 4251 4275)     => 4251=disabled 4275=disabled
AT CONSUMER CODE: 4251=disabled 4275=disabled
preprocessor error lines: 0
```

The header itself has one directive and no restore:

```sh
grep -n 'pragma' ~/wt/109f07fc/src/core/jsonld/include/sourcemeta/core/*.h
```

```console
~/wt/109f07fc/src/core/jsonld/include/sourcemeta/core/jsonld_error.h:20:#pragma warning(disable : 4251 4275)
```

Same trace on the audited branch, whose `jsonld_error.h` has a matching `#pragma warning(default : 4251 4275)` after the class:

```sh
verify/repro/c10_pragma_trace.sh | tail -4
```

```console
src/core/jsonld/include/sourcemeta/core/jsonld_error.h                 #pragma warning(disable : 4251 4275)     => 4251=disabled 4275=disabled
src/core/jsonld/include/sourcemeta/core/jsonld_error.h                 #pragma warning(default : 4251 4275)     => 4251=default 4275=default
AT CONSUMER CODE: 4251=default 4275=default
preprocessor error lines: 0
```

Impact reasoning: `jsonld_error.h` on that branch is the last header processed by `jsonld.h` that touches these warnings (every other header in the include tree is already past its include guard by then), and its `disable` is never followed by `default`. Under MSVC, any consumer code after `#include <sourcemeta/core/jsonld.h>` (or `jsonld_error.h`) therefore compiles with C4251 and C4275 silenced for the rest of the translation unit, hiding DLL-interface warnings in the consumer's own classes. Every other public header in the repository restores the warnings (`json_error.h` lines 21/96, `uri.h`, ...). Limits of this evidence: the pragma sequence is observed from the real preprocessed output with `_MSC_VER` defined, but the warning state itself was not observed on an MSVC compiler.

## C11

Branch: `evalon/core-imple-188675ac` (`05972093`), worktree `~/wt/188675ac`, full build and install as in Setup. Command:

```sh
ROOT=~/wt/188675ac verify/repro/c11_packaging_consumer_not_run.sh
```

Output:

```console
--- registered packaging tests and their commands
53: Test command: /usr/bin/cmake "-S" "~/wt/188675ac/test/packaging/find_package" "-B" "~/wt/188675ac/build/test/packaging/find_package" "-DCMAKE_BUILD_TYPE:STRING=Debug" "-DCMAKE_PREFIX_PATH:PATH=;~/wt/188675ac/build/dist" "-DCMAKE_TOOLCHAIN_FILE:PATH="
  Test #53: core.find_package_configure
54: Test command: /usr/bin/cmake "--build" "~/wt/188675ac/build/test/packaging/find_package" "--config" "Debug"
  Test #54: core.find_package_build
55: Test command: /usr/bin/cmake "-S" "~/wt/188675ac/test/packaging/find_package_jsonld" "-B" "~/wt/188675ac/build/test/packaging/find_package_jsonld" "-DCMAKE_BUILD_TYPE:STRING=Debug" "-DCMAKE_PREFIX_PATH:PATH=;~/wt/188675ac/build/dist" "-DCMAKE_TOOLCHAIN_FILE:PATH="
  Test #55: core.find_package_jsonld_configure
56: Test command: /usr/bin/cmake "--build" "~/wt/188675ac/build/test/packaging/find_package_jsonld" "--config" "Debug"
  Test #56: core.find_package_jsonld_build
--- 1. unmodified consumer
1/2 Test #55: core.find_package_jsonld_configure ...   Passed    0.02 sec
2/2 Test #56: core.find_package_jsonld_build .......   Passed    1.38 sec
100% tests passed out of 2
direct run of consumer: exit code 0
--- 2. consumer with a deliberately wrong expected value
 test/packaging/find_package_jsonld/main.cc | 2 +-
1/2 Test #55: core.find_package_jsonld_configure ...   Passed    0.02 sec
2/2 Test #56: core.find_package_jsonld_build .......   Passed    1.40 sec
100% tests passed out of 2
direct run of consumer: exit code 1
```

Part "Runtime check": holds. `test/packaging/find_package_jsonld/main.cc` on that branch:

```cpp
#include <sourcemeta/core/jsonld.h>

#include <cstdlib> // EXIT_SUCCESS, EXIT_FAILURE

auto main() -> int {
  const auto input{
      sourcemeta::core::parse_json(R"({"@type":"urn:example:T"})")};
  const auto expected{
      sourcemeta::core::parse_json(R"([{"@type":["urn:example:T"]}])")};
  return sourcemeta::core::jsonld_expand(input) == expected ? EXIT_SUCCESS
                                                            : EXIT_FAILURE;
}
```

Part "Test execution": holds. The only registered tests touching the consumer are `core.find_package_jsonld_configure` (`cmake -S ... -B ...`) and `core.find_package_jsonld_build` (`cmake --build ...`); no test command runs `core_jsonld_consumer`. After changing the expected value so that the equality check is false, both tests still pass, while running the built binary by hand exits 1 (it exits 0 unmodified). The script restores `main.cc` with `git checkout` afterwards.

Impact reasoning: the consumer reads as a runtime smoke test of the installed package, but its result is never observed, so a packaged `jsonld_expand` that links yet returns wrong output would still pass `core.find_package_jsonld_*`. This matches the pre-existing `find_package` packaging tests, which are also configure + build only; the equality check is covered functionally by the unit tests, so the gap is in packaging-level coverage only.

## C12

Branch: `evalon/core-imple-4ccba2f9` (`29d45422`), worktree `~/wt/4ccba2f9`. `verify/repro/c12_copy_trace.sh` runs the probe under gdb with a breakpoint on `sourcemeta::core::JSON::JSON(const JSON &)` (commands in `verify/repro/c12_copy_trace.gdb`: `bt 12`, `continue`) and counts copy-constructor hits reached from lines 210-229 of `Processor::context`, for two local contexts that do not use `@import` (one object, one array).

```sh
ROOT=~/wt/4ccba2f9 verify/repro/c12_copy_trace.sh
```

Output:

```console
INPUT {"@context":{"name":"http://e/name","nick":{"@id":"http://e/nick","@container":"@set"}},"name":"n"}
OK [{"http://e/name":[{"@value":"n"}]}]
      2 Processor::context at ~/wt/4ccba2f9/src/core/jsonld/context.cc:220
      2 Processor::context at ~/wt/4ccba2f9/src/core/jsonld/context.cc:223
INPUT {"@context":[{"name":"http://e/name"},{"nick":{"@id":"http://e/nick","@container":"@set"}}],"name":"n"}
OK [{"http://e/name":[{"@value":"n"}]}]
      1 Processor::context at ~/wt/4ccba2f9/src/core/jsonld/context.cc:218
      1 Processor::context at ~/wt/4ccba2f9/src/core/jsonld/context.cc:220
      3 Processor::context at ~/wt/4ccba2f9/src/core/jsonld/context.cc:223
```

The lines in question (`sed -n 216,223p ~/wt/4ccba2f9/src/core/jsonld/context.cc`):

```cpp
  auto entries = JSON::make_array();
  if (local.is_array()) {
    entries = local;                 // 218: copies the whole context array
  } else {
    entries.push_back(local);        // 220: copies the whole context object
  }
  for (std::size_t index = 0; index < entries.size(); ++index) {
    auto current = entries.at(index); // 223: copies each entry again
```

(comments added here.) Writes to those variables:

```sh
grep -n 'current = \|entries' ~/wt/4ccba2f9/src/core/jsonld/context.cc
```

```console
216:  auto entries = JSON::make_array();
218:    entries = local;
220:    entries.push_back(local);
222:  for (std::size_t index = 0; index < entries.size(); ++index) {
223:    auto current = entries.at(index);
312:      current = std::move(imported);
```

Impact reasoning: for an `@import`-free context, `local` is deep-copied into `entries` (line 218 or 220) and each entry is deep-copied again into `current` (line 223); the copy-constructor hits at those lines are observed in both runs. `entries` is never written after construction and `current` is only reassigned inside the `@import` branch (line 312, the merge); everything else reads it (`current.defines`, `current.at`, `keys(current)`, and `DefinitionState::local`, which is a `const JSON &`). So without `@import` both copies exist only to give the loop something to read, and neither serves merging or isolated mutation. The cost is two full duplications of every local context each time `Processor::context` runs (it runs again for scoped contexts), which is avoidable allocation proportional to context size; output is unaffected.
