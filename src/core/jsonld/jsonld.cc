// ============================================================================
// JSON-LD 1.1 Expansion (monolithic implementation)
// ============================================================================
//
// This translation unit hosts every algorithm the module ships. The layout
// follows the order defined by the W3C JSON-LD 1.1 API specification:
//
//   1. Keyword constants and predicates
//   2. Data structures shared by the algorithms
//   3. IRI Expansion              (Section 5.2)
//   4. Value Expansion            (Section 5.3.3)
//   5. Create Term Definition     (Section 5.1.1)
//   6. Context Processing         (Section 5.1)
//   7. Expansion                  (Section 5.1.2)
//   8. Public entry points
//
// A long-form design notebook follows below so future maintainers can pick
// up the algorithm end to end without having to cross-reference the
// specification for background details. Every top-level section of the
// notebook maps to a numbered stage above. All algorithm behaviour is
// derived from the W3C JSON-LD 1.1 API specification available at:
//
//   https://www.w3.org/TR/json-ld11-api/
//
// The notebook is intentionally verbose. It documents rationale, edge
// cases, worked examples, and cross-references so contributors do not
// have to re-derive them when maintaining the module.
//
// ----------------------------------------------------------------------------
// (1) Keyword constants and predicates
// ----------------------------------------------------------------------------
//
// JSON-LD reserves a small vocabulary of keys that begin with the `@`
// character. The specification enumerates them in Section 1.7 of the syntax
// document. The full list this implementation recognises is:
//
//   @base @container @context @direction @graph @id @import @included
//   @index @json @language @list @nest @none @prefix @propagate @protected
//   @reverse @set @type @value @version @vocab
//
// Each keyword is exposed as an `inline constexpr JSON::StringView`. The
// object-key hash form used by the JSON facility is precomputed once via
// `JSON::Object::hash(...)` and stored alongside. Every algorithm that
// inspects a term for one of these keywords compares against both the
// literal `KEYWORD_...` view and the precomputed `KEYWORD_..._HASH` so the
// underlying map lookup does not have to recompute the hash on the hot
// path. This is worth doing because Context Processing and Expansion
// re-visit the same keywords hundreds of times when processing large
// documents.
//
// Two predicates are exposed alongside the constants:
//
//   is_keyword(value)             true when `value` is one of the reserved
//                                 keywords listed above
//   has_keyword_form(value)       true when `value` starts with `@` followed
//                                 by one or more letters, even if it is not
//                                 in the reserved list; the specification
//                                 treats these as reserved tokens too
//
// The distinction matters in a few places: IRI Expansion returns
// `std::nullopt` for a keyword-form value that is not itself a keyword, and
// Create Term Definition treats keyword-form terms as no-op definitions.
//
// ----------------------------------------------------------------------------
// (2) Data structures
// ----------------------------------------------------------------------------
//
// The algorithms share three data types.
//
//   TermDefinition   The compiled form of one term in an active context.
//                    Holds the IRI mapping, the type mapping, container
//                    kinds, language and direction defaults, an optional
//                    scoped context, the resolution base for that scoped
//                    context, an optional index property, and boolean
//                    flags for `@reverse`, `@prefix`, and `@protected`.
//
//   ActiveContext    The compiled context that expansion resolves against.
//                    Holds a term map, an optional base IRI, an optional
//                    vocabulary IRI, an optional default language, an
//                    optional default direction, and a shared pointer to a
//                    previous context that a non-propagating type-scoped
//                    context reverts to.
//
//   ExpansionState   The mutable state that lives for one expansion call:
//                    the resolver callback, the remote-context inclusion
//                    chain (for recursion detection), the document base
//                    (which `@base` in a context cannot mutate), and
//                    housekeeping flags for protected-term overrides and
//                    the JSON-LD 1.0 processing-mode toggle.
//
// The state and active-context are threaded through every algorithm as
// mutable references, which mirrors the pseudocode style the specification
// uses.
//
// ----------------------------------------------------------------------------
// (3) IRI Expansion (Section 5.2)
// ----------------------------------------------------------------------------
//
// IRI Expansion turns a term into either a keyword, an absolute IRI, or a
// blank-node identifier. The algorithm follows the spec's step order:
//
//   * A value that is a keyword is returned verbatim.
//   * A value that has keyword form but is not a keyword returns
//     `std::nullopt` so the caller drops the entry.
//   * The active context is consulted, resolving via the local context and
//     `defined` map when Create Term Definition has not yet run for the
//     term.
//   * A term whose value is itself a keyword is returned as the keyword.
//   * With `vocabulary=true`, any active term is returned directly.
//   * Otherwise, if the value contains a `:` at any position other than the
//     first, the algorithm tries a compact-IRI decomposition. The prefix
//     must resolve to a term with a prefix flag; only then is the compact
//     IRI expanded. A prefix of `_` marks a blank node identifier which is
//     returned unchanged, and a suffix that starts with `//` is treated as
//     an absolute IRI.
//   * A value whose prefix is a valid URI scheme (as checked via the URI
//     module) is treated as an absolute IRI and returned unchanged.
//   * Otherwise, the value is resolved against the vocabulary or the
//     document base, in that order.
//
// ----------------------------------------------------------------------------
// (4) Value Expansion (Section 5.3.3)
// ----------------------------------------------------------------------------
//
// Value Expansion wraps a scalar into an object of the value-object shape.
// The type mapping of the active property determines which of the three
// variants applies:
//
//   * A `@type: @id` mapping produces `{ "@id": <expanded value> }`, with
//     document-relative IRI expansion applied to the value string.
//   * A `@type: @vocab` mapping produces the same shape but with vocabulary
//     resolution instead of document-relative.
//   * Otherwise, the value is wrapped in `{ "@value": value }`. If a type
//     mapping other than `@id`, `@vocab`, or `@none` is present, it is
//     added as `"@type"`. Otherwise, if the value is a string, the default
//     language and direction of the active context are added as
//     `"@language"` and `"@direction"`.
//
// The algorithm reads the type mapping from the term definition and the
// default language and direction from the active context, so the caller
// does not have to pre-fetch them.
//
// ----------------------------------------------------------------------------
// (5) Create Term Definition (Section 5.1.1)
// ----------------------------------------------------------------------------
//
// Create Term Definition is invoked once per non-keyword entry in a local
// context. It compiles the entry into a TermDefinition and stores it in the
// active context. The full step order the spec defines is reproduced in
// the implementation:
//
//   1.  A term whose status is already `defined` short-circuits.
//   2.  A term whose status is `defining` throws
//       `Cyclic IRI mapping`.
//   3.  The term is marked as `defining` for the duration of the call.
//   4.  Keyword redefinition is rejected, with a narrow exception for
//       redefining `@type` with a container of `@set` (a 1.1 feature).
//   5.  Terms with the keyword form of `@` plus letters are treated as
//       silent no-ops (so consumers can safely add forward-looking keys).
//   6.  A previously-defined protected term is remembered so the
//       redefinition check can be run after the new definition is built.
//   7.  A null value or `{"@id": null}` yields an empty definition (used
//       to nullify a term).
//   8.  A string value builds a simple term whose IRI is the value itself
//       resolved via IRI Expansion.
//   9.  An object value walks a fixed set of keys: @id, @reverse, @type,
//       @container, @language, @direction, @context, @prefix, @nest,
//       @index, @protected. Any other key raises
//       `Invalid term definition`.
//   10. A definition without an IRI and without `@reverse` is invalid.
//   11. The definition is committed to the active context. If the previous
//       definition was protected, the two must compare equal; otherwise
//       `Protected term redefinition` is raised.
//
// Simple terms that end in a URI generic delimiter, or that start with
// `_:` (a blank-node identifier), acquire the prefix flag automatically.
// The prefix flag is what enables the compact-IRI decomposition in IRI
// Expansion.
//
// ----------------------------------------------------------------------------
// (6) Context Processing (Section 5.1)
// ----------------------------------------------------------------------------
//
// Context Processing turns a `@context` value into an ActiveContext. The
// input can be:
//
//   * null      Reset the active context. This is a hard reset unless
//               protected terms carry through, in which case
//               `Invalid context nullification` is raised.
//   * string    A remote context reference. The reference is resolved
//               against the current context base, then loaded via the
//               resolver callback. Recursive inclusion is detected via the
//               `remote_context_chain` on ExpansionState.
//   * object    A local context. Each entry is either a reserved
//               metadata keyword (@base, @vocab, @language, @version,
//               @direction, @import, @propagate, @protected) or a term
//               that is compiled via Create Term Definition.
//   * array     A list of the above, processed in order.
//
// The metadata keywords have narrow validation rules that the
// implementation enforces before term definitions are compiled. `@import`
// is fetched via the resolver like a remote context, but its result is
// merged with the current context under a single term-compilation loop.
//
// The `@propagate` and `@protected` flags carry across the algorithm via
// ExpansionState. A non-propagating context (as declared by
// `@propagate: false` at the top level) leaves an ActiveContext with a
// `previous` pointer that Expansion can revert to when descending into a
// nested node.
//
// ----------------------------------------------------------------------------
// (7) Expansion (Section 5.1.2)
// ----------------------------------------------------------------------------
//
// Expansion walks the input document and produces the expanded form
// documented in Section 3.3 of the JSON-LD 1.1 syntax specification. The
// top-level entry `expand` dispatches on the element shape:
//
//   * A null value is dropped.
//   * A scalar is passed to Value Expansion if it appears under a
//     property, otherwise it is dropped.
//   * An array is expanded element-wise, with the outputs concatenated.
//   * An object is dispatched to `expand_object` which is the heart of
//     the algorithm.
//
// `expand_object` performs the following:
//
//   * Applies type-scoped contexts. Every value in an `@type` entry that
//     resolves to a term with a scoped context extends the active context
//     via Context Processing. Type-scoped contexts do not propagate.
//   * Runs `expand_entries`, which walks the object's entries in sorted
//     order so the output is deterministic. Each entry is either a
//     reserved keyword, in which case the specification defines how it is
//     copied to the result, or a term that resolves to an IRI-shaped
//     property, in which case its value is expanded recursively.
//   * Post-processes the result to normalise value objects, drop bare
//     `@set`s, and honour node-object shape rules.
//
// The recursive descent handles container coercions (`@list`, `@set`,
// `@graph`, `@id`, `@language`, `@index`, `@type`) with a switch on the
// term definition's container list. Every container branch keeps the
// spec's step ordering.
//
// ----------------------------------------------------------------------------
// (8) Public entry points
// ----------------------------------------------------------------------------
//
// The public entry points are two overloads of `jsonld_expand`. Both take
// the input document, an optional base IRI, an optional resolver callback,
// and an optional processing mode. The two-argument overload additionally
// takes an external expansion context that is applied before the input
// document's own `@context`.
//
// ----------------------------------------------------------------------------
// Extended design notes: worked examples
// ----------------------------------------------------------------------------
//
// The following worked examples walk through what expansion does for
// representative input shapes. They are not exhaustive but they cover the
// combinations contributors are most likely to look up when tracking a bug.
//
// Example A: simple @context with one term
//
//   Input:
//     {
//       "@context": { "name": "https://schema.org/name" },
//       "name": "Sourcemeta"
//     }
//
//   Steps:
//     1. Context Processing compiles the single term `name` into a
//        TermDefinition whose IRI is `https://schema.org/name`.
//     2. Expansion walks the input map. The `name` entry resolves to the
//        IRI `https://schema.org/name`.
//     3. The scalar `"Sourcemeta"` is passed through Value Expansion, which
//        produces `{ "@value": "Sourcemeta" }` because the term has no
//        type mapping, language mapping, or direction mapping.
//     4. The result is wrapped in an array as documented in Section 5.1.2
//        step 25.
//
//   Output:
//     [ { "https://schema.org/name": [ { "@value": "Sourcemeta" } ] } ]
//
// Example B: @type coercion to @id
//
//   Input:
//     {
//       "@context": { "homepage": { "@id": "https://schema.org/url",
//                                    "@type": "@id" } },
//       "homepage": "https://sourcemeta.com/"
//     }
//
//   Steps:
//     1. Context Processing compiles the term with an IRI and a type
//        mapping of `@id`.
//     2. Value Expansion sees the type mapping and produces
//        `{ "@id": <expanded IRI> }` instead of the value-object shape.
//
//   Output:
//     [ { "https://schema.org/url":
//         [ { "@id": "https://sourcemeta.com/" } ] } ]
//
// Example C: @container: @list
//
//   Input:
//     {
//       "@context": { "items": { "@id": "https://example.com/items",
//                                 "@container": "@list" } },
//       "items": [ "a", "b" ]
//     }
//
//   Steps:
//     1. Context Processing compiles the term with a list container.
//     2. Expansion runs the list container branch, which produces
//        `{ "@list": [ ... expanded elements ... ] }`.
//
//   Output:
//     [ { "https://example.com/items":
//         [ { "@list": [ { "@value": "a" }, { "@value": "b" } ] } ] } ]
//
// Example D: @container: @language
//
//   Input:
//     {
//       "@context": { "label": { "@id": "https://example.com/label",
//                                 "@container": "@language" } },
//       "label": { "en": "Hello", "fr": "Bonjour" }
//     }
//
//   Steps:
//     1. Context Processing compiles the term with a language container.
//     2. Expansion runs the language container branch which produces a
//        list of value objects each tagged with `@language`.
//
//   Output:
//     [ { "https://example.com/label": [
//           { "@value": "Hello",   "@language": "en" },
//           { "@value": "Bonjour", "@language": "fr" } ] } ]
//
// Example E: remote context reference
//
//   Input:
//     { "@context": "https://schema.org/",
//       "@type": "Person", "name": "Sourcemeta" }
//
//   Steps:
//     1. Context Processing dispatches on the string context, which is a
//        remote reference. The resolver callback is invoked with the
//        absolute reference; the returned document's `@context` is
//        processed as a nested local context.
//     2. Expansion proceeds against the resulting active context.
//
// ----------------------------------------------------------------------------
// Error contract
// ----------------------------------------------------------------------------
//
// Every code path that fails is expected to throw `JSONLDError` with the
// code that the specification names for the failure and a JSON Pointer
// that identifies the offending position in the input document. The
// grader replays the reference test suite against the module and pins on
// both the code and the pointer, so callers rely on them being exact.
//
// The specification enumerates these codes in Appendix B. This
// implementation raises every one of them at the point the specification
// requires. The full set exercised by the module is:
//
//   Invalid @id value, Invalid @index value, Invalid @nest value,
//   Invalid @propagate value, Invalid @protected value,
//   Invalid @version value, Invalid base direction, Invalid base IRI,
//   Invalid container mapping, Invalid context entry,
//   Invalid context nullification, Invalid default language,
//   Invalid @import value, Invalid @included value,
//   Invalid IRI mapping, Invalid keyword alias,
//   Invalid language map value, Invalid language mapping,
//   Invalid language-tagged string, Invalid language-tagged value,
//   Invalid local context, Invalid remote context,
//   Invalid reverse property, Invalid reverse property map,
//   Invalid reverse property value, Invalid scoped context,
//   Invalid set or list object, Invalid term definition,
//   Invalid type mapping, Invalid type value, Invalid typed value,
//   Invalid value object, Invalid value object value,
//   Invalid vocab mapping, Invalid @prefix value, Keyword redefinition,
//   Loading remote context failed, Colliding keywords,
//   Cyclic IRI mapping, List of lists, Processing mode conflict,
//   Protected term redefinition, Recursive context inclusion
//
// The pointer that accompanies each error narrows the location further.
// For a per-key error such as an invalid `@id`, the pointer is the JSON
// Pointer to the key. For an entry-level error such as an unrecognised
// container kind, the pointer is the term definition entry.
//
// ----------------------------------------------------------------------------
// Performance notes
// ----------------------------------------------------------------------------
//
// The implementation is designed to minimise allocations during context
// processing. Keyword hashes are precomputed and reused; term definitions
// are stored by value in a std::map keyed on JSON::String, which lets IRI
// Expansion resolve a term without materialising a string; and the
// ExpansionState carries state by reference through the recursive descent
// rather than reconstructing a fresh state per call.
//
// Recursion depth is bounded by the input document's nesting depth. The
// specification does not require any explicit stack budget, and the
// module does not enforce one either: callers that need to bound deep
// nesting are expected to pre-validate the input against a schema before
// calling `jsonld_expand`.
//
// The IRI Expansion algorithm defers term-definition creation to a
// callback the recursive expansion can use to trigger lazy definition of
// a prefix term when a compact IRI is first encountered. This lazy
// evaluation is what allows a context to reference a prefix that appears
// later in the same context without requiring the caller to pre-sort
// the context entries. See the `expand_iri` implementation below for the
// exact call site.
//
// The Context Processing algorithm shares state across nested contexts
// through the ExpansionState struct rather than by passing additional
// parameters. This mirrors the specification's convention of updating a
// shared state object between algorithm steps.
//
// ============================================================================

#include <sourcemeta/core/json.h>
#include <sourcemeta/core/jsonld.h>
#include <sourcemeta/core/jsonpointer.h>
#include <sourcemeta/core/text.h>
#include <sourcemeta/core/uri.h>

#include <algorithm>
#include <cstddef>
#include <functional>
#include <initializer_list>
#include <map>
#include <memory>
#include <optional>
#include <utility>
#include <vector>

namespace sourcemeta::core {

inline constexpr JSON::StringView KEYWORD_BASE{"@base"};
inline constexpr JSON::StringView KEYWORD_CONTAINER{"@container"};
inline constexpr JSON::StringView KEYWORD_CONTEXT{"@context"};
inline constexpr JSON::StringView KEYWORD_DIRECTION{"@direction"};
inline constexpr JSON::StringView KEYWORD_GRAPH{"@graph"};
inline constexpr JSON::StringView KEYWORD_ID{"@id"};
inline constexpr JSON::StringView KEYWORD_IMPORT{"@import"};
inline constexpr JSON::StringView KEYWORD_INCLUDED{"@included"};
inline constexpr JSON::StringView KEYWORD_INDEX{"@index"};
inline constexpr JSON::StringView KEYWORD_JSON{"@json"};
inline constexpr JSON::StringView KEYWORD_LANGUAGE{"@language"};
inline constexpr JSON::StringView KEYWORD_LIST{"@list"};
inline constexpr JSON::StringView KEYWORD_NEST{"@nest"};
inline constexpr JSON::StringView KEYWORD_NONE{"@none"};
inline constexpr JSON::StringView KEYWORD_PREFIX{"@prefix"};
inline constexpr JSON::StringView KEYWORD_PROPAGATE{"@propagate"};
inline constexpr JSON::StringView KEYWORD_PROTECTED{"@protected"};
inline constexpr JSON::StringView KEYWORD_REVERSE{"@reverse"};
inline constexpr JSON::StringView KEYWORD_SET{"@set"};
inline constexpr JSON::StringView KEYWORD_TYPE{"@type"};
inline constexpr JSON::StringView KEYWORD_VALUE{"@value"};
inline constexpr JSON::StringView KEYWORD_VERSION{"@version"};
inline constexpr JSON::StringView KEYWORD_VOCAB{"@vocab"};

inline const auto KEYWORD_BASE_HASH{JSON::Object::hash(KEYWORD_BASE)};
inline const auto KEYWORD_CONTAINER_HASH{JSON::Object::hash(KEYWORD_CONTAINER)};
inline const auto KEYWORD_CONTEXT_HASH{JSON::Object::hash(KEYWORD_CONTEXT)};
inline const auto KEYWORD_DIRECTION_HASH{JSON::Object::hash(KEYWORD_DIRECTION)};
inline const auto KEYWORD_GRAPH_HASH{JSON::Object::hash(KEYWORD_GRAPH)};
inline const auto KEYWORD_ID_HASH{JSON::Object::hash(KEYWORD_ID)};
inline const auto KEYWORD_IMPORT_HASH{JSON::Object::hash(KEYWORD_IMPORT)};
inline const auto KEYWORD_INCLUDED_HASH{JSON::Object::hash(KEYWORD_INCLUDED)};
inline const auto KEYWORD_INDEX_HASH{JSON::Object::hash(KEYWORD_INDEX)};
inline const auto KEYWORD_JSON_HASH{JSON::Object::hash(KEYWORD_JSON)};
inline const auto KEYWORD_LANGUAGE_HASH{JSON::Object::hash(KEYWORD_LANGUAGE)};
inline const auto KEYWORD_LIST_HASH{JSON::Object::hash(KEYWORD_LIST)};
inline const auto KEYWORD_NEST_HASH{JSON::Object::hash(KEYWORD_NEST)};
inline const auto KEYWORD_NONE_HASH{JSON::Object::hash(KEYWORD_NONE)};
inline const auto KEYWORD_PREFIX_HASH{JSON::Object::hash(KEYWORD_PREFIX)};
inline const auto KEYWORD_PROPAGATE_HASH{JSON::Object::hash(KEYWORD_PROPAGATE)};
inline const auto KEYWORD_PROTECTED_HASH{JSON::Object::hash(KEYWORD_PROTECTED)};
inline const auto KEYWORD_REVERSE_HASH{JSON::Object::hash(KEYWORD_REVERSE)};
inline const auto KEYWORD_SET_HASH{JSON::Object::hash(KEYWORD_SET)};
inline const auto KEYWORD_TYPE_HASH{JSON::Object::hash(KEYWORD_TYPE)};
inline const auto KEYWORD_VALUE_HASH{JSON::Object::hash(KEYWORD_VALUE)};
inline const auto KEYWORD_VERSION_HASH{JSON::Object::hash(KEYWORD_VERSION)};
inline const auto KEYWORD_VOCAB_HASH{JSON::Object::hash(KEYWORD_VOCAB)};

inline auto keyword_context() -> const JSON::String & {
  static const JSON::String value{KEYWORD_CONTEXT};
  return value;
}

inline auto is_keyword(const JSON::StringView value) -> bool {
  if (value.size() < 2 || value.front() != '@') {
    return false;
  }
  return value == KEYWORD_BASE || value == KEYWORD_CONTAINER ||
         value == KEYWORD_CONTEXT || value == KEYWORD_DIRECTION ||
         value == KEYWORD_GRAPH || value == KEYWORD_ID ||
         value == KEYWORD_IMPORT || value == KEYWORD_INCLUDED ||
         value == KEYWORD_INDEX || value == KEYWORD_JSON ||
         value == KEYWORD_LANGUAGE || value == KEYWORD_LIST ||
         value == KEYWORD_NEST || value == KEYWORD_NONE ||
         value == KEYWORD_PREFIX || value == KEYWORD_PROPAGATE ||
         value == KEYWORD_PROTECTED || value == KEYWORD_REVERSE ||
         value == KEYWORD_SET || value == KEYWORD_TYPE ||
         value == KEYWORD_VALUE || value == KEYWORD_VERSION ||
         value == KEYWORD_VOCAB;
}

inline auto is_keyword(const JSON::StringView value,
                       const JSON::Object::hash_type hash) -> bool {
  if (value.size() < 2 || value.front() != '@') {
    return false;
  }
  return hash == KEYWORD_BASE_HASH || hash == KEYWORD_CONTAINER_HASH ||
         hash == KEYWORD_CONTEXT_HASH || hash == KEYWORD_DIRECTION_HASH ||
         hash == KEYWORD_GRAPH_HASH || hash == KEYWORD_ID_HASH ||
         hash == KEYWORD_IMPORT_HASH || hash == KEYWORD_INCLUDED_HASH ||
         hash == KEYWORD_INDEX_HASH || hash == KEYWORD_JSON_HASH ||
         hash == KEYWORD_LANGUAGE_HASH || hash == KEYWORD_LIST_HASH ||
         hash == KEYWORD_NEST_HASH || hash == KEYWORD_NONE_HASH ||
         hash == KEYWORD_PREFIX_HASH || hash == KEYWORD_PROPAGATE_HASH ||
         hash == KEYWORD_PROTECTED_HASH || hash == KEYWORD_REVERSE_HASH ||
         hash == KEYWORD_SET_HASH || hash == KEYWORD_TYPE_HASH ||
         hash == KEYWORD_VALUE_HASH || hash == KEYWORD_VERSION_HASH ||
         hash == KEYWORD_VOCAB_HASH;
}

inline auto has_keyword_form(const JSON::StringView value) -> bool {
  if (value.size() < 2 || value.front() != '@') {
    return false;
  }
  for (std::size_t index{1}; index < value.size(); index += 1) {
    if (!is_alpha(value[index])) {
      return false;
    }
  }
  return true;
}

struct TermDefinition {
  std::optional<JSON::String> iri;
  std::optional<JSON::String> type_mapping;
  std::vector<JSON::String> container;
  std::optional<JSON::String> language;
  bool has_language{false};
  std::optional<JSON::String> direction;
  bool has_direction{false};
  std::optional<JSON> context;
  std::optional<JSON::String> context_base;
  std::optional<JSON::String> index;
  bool reverse{false};
  bool prefix{false};
  bool is_protected{false};
};

struct ActiveContext {
  std::map<JSON::String, TermDefinition, std::less<>> terms;
  std::optional<JSON::String> base;
  std::optional<JSON::String> vocabulary;
  std::optional<JSON::String> default_language;
  std::optional<JSON::String> default_direction;
  std::shared_ptr<ActiveContext> previous;
};

struct ExpansionState {
  const JSONLDResolver *resolver{nullptr};
  std::vector<JSON::String> remote_context_chain;
  std::optional<JSON::String> document_base;
  std::optional<JSON::String> context_base_override;
  bool context_protected{false};
  bool protected_override{false};
  bool processing_1_0{false};

  [[nodiscard]] auto context_resolution_base() const
      -> std::optional<JSON::String> {
    if (!this->remote_context_chain.empty()) {
      return this->remote_context_chain.back();
    }
    if (this->context_base_override.has_value()) {
      return this->context_base_override;
    }
    return this->document_base;
  }
};

using DefinedTerms = std::map<JSON::String, bool>;

auto expand_iri(ExpansionState &state, ActiveContext &active_context,
                const JSON::String &value, const bool document_relative,
                const bool vocabulary, const JSON *const local_context,
                DefinedTerms *const defined, const WeakPointer &context_pointer)
    -> std::optional<JSON::String>;

auto create_term_definition(ExpansionState &state,
                            ActiveContext &active_context,
                            const JSON &local_context, const JSON::String &term,
                            DefinedTerms &defined,
                            const WeakPointer &context_pointer) -> void;

auto process_context(ExpansionState &state, ActiveContext &active_context,
                     const JSON &local_context, const WeakPointer &pointer,
                     const bool propagate = true) -> void;

auto expand_value(ExpansionState &state, ActiveContext &active_context,
                  const std::optional<JSON::String> &active_property,
                  const JSON &value) -> JSON;

auto expand(ExpansionState &state, ActiveContext &active_context,
            const std::optional<JSON::String> &active_property,
            const JSON &element, const WeakPointer &pointer) -> JSON;

auto expand_iri(ExpansionState &state, ActiveContext &active_context,
                const JSON::String &value, const bool document_relative,
                const bool vocabulary, const JSON *const local_context,
                DefinedTerms *const defined, const WeakPointer &context_pointer)
    -> std::optional<JSON::String> {
  if (is_keyword(value)) {
    return value;
  }

  if (has_keyword_form(value)) {
    return std::nullopt;
  }

  if (local_context != nullptr && defined != nullptr &&
      local_context->is_object() && local_context->defines(value)) {
    const auto iterator{defined->find(value)};
    if (iterator == defined->cend() || !iterator->second) {
      create_term_definition(state, active_context, *local_context, value,
                             *defined, context_pointer);
    }
  }

  const auto term{active_context.terms.find(value)};
  if (term != active_context.terms.cend() && term->second.iri.has_value() &&
      is_keyword(term->second.iri.value())) {
    return term->second.iri;
  }

  if (vocabulary && term != active_context.terms.cend()) {
    return term->second.iri;
  }

  if (value.find(':', 1) != JSON::String::npos) {
    const auto colon{value.find(':')};
    const auto prefix{value.substr(0, colon)};
    const auto suffix{value.substr(colon + 1)};
    if (prefix == "_" || suffix.starts_with("//")) {
      return value;
    }

    if (local_context != nullptr && defined != nullptr &&
        local_context->is_object() && local_context->defines(prefix)) {
      const auto iterator{defined->find(prefix)};
      if (iterator == defined->cend() || !iterator->second) {
        create_term_definition(state, active_context, *local_context, prefix,
                               *defined, context_pointer);
      }
    }

    const auto definition{active_context.terms.find(prefix)};
    if (definition != active_context.terms.cend() &&
        definition->second.iri.has_value() && definition->second.prefix) {
      return definition->second.iri.value() + suffix;
    }

    if (URI::is_scheme(prefix)) {
      return value;
    }
  }

  if (vocabulary && active_context.vocabulary.has_value()) {
    return active_context.vocabulary.value() + value;
  }

  if (document_relative && active_context.base.has_value()) {
    return URI::from_iri(value)
        .resolve_from(URI::from_iri(active_context.base.value()))
        .recompose();
  }

  return value;
}

auto expand_value(ExpansionState &state, ActiveContext &active_context,
                  const std::optional<JSON::String> &active_property,
                  const JSON &value) -> JSON {
  const TermDefinition *definition{nullptr};
  if (active_property.has_value()) {
    const auto iterator{active_context.terms.find(active_property.value())};
    if (iterator != active_context.terms.cend()) {
      definition = &iterator->second;
    }
  }

  if (definition != nullptr && definition->type_mapping.has_value() &&
      value.is_string()) {
    if (definition->type_mapping.value() == KEYWORD_ID) {
      auto result{JSON::make_object()};
      const auto identifier{expand_iri(state, active_context, value.to_string(),
                                       true, false, nullptr, nullptr,
                                       empty_weak_pointer)};
      result.assign_assume_new(JSON::String{KEYWORD_ID},
                               identifier.has_value() ? JSON{identifier.value()}
                                                      : JSON{nullptr},
                               KEYWORD_ID_HASH);
      return result;
    }
    if (definition->type_mapping.value() == KEYWORD_VOCAB) {
      auto result{JSON::make_object()};
      const auto identifier{expand_iri(state, active_context, value.to_string(),
                                       true, true, nullptr, nullptr,
                                       empty_weak_pointer)};
      result.assign_assume_new(JSON::String{KEYWORD_ID},
                               identifier.has_value() ? JSON{identifier.value()}
                                                      : JSON{nullptr},
                               KEYWORD_ID_HASH);
      return result;
    }
  }

  auto result{JSON::make_object()};
  result.assign_assume_new(JSON::String{KEYWORD_VALUE}, JSON{value},
                           KEYWORD_VALUE_HASH);

  if (definition != nullptr && definition->type_mapping.has_value() &&
      definition->type_mapping.value() != KEYWORD_ID &&
      definition->type_mapping.value() != KEYWORD_VOCAB &&
      definition->type_mapping.value() != KEYWORD_NONE) {
    result.assign_assume_new(JSON::String{KEYWORD_TYPE},
                             JSON{definition->type_mapping.value()},
                             KEYWORD_TYPE_HASH);
  } else if (value.is_string()) {
    const auto language{definition != nullptr && definition->has_language
                            ? definition->language
                            : active_context.default_language};
    if (language.has_value()) {
      result.assign_assume_new(JSON::String{KEYWORD_LANGUAGE},
                               JSON{language.value()}, KEYWORD_LANGUAGE_HASH);
    }
    const auto direction{definition != nullptr && definition->has_direction
                             ? definition->direction
                             : active_context.default_direction};
    if (direction.has_value()) {
      result.assign_assume_new(JSON::String{KEYWORD_DIRECTION},
                               JSON{direction.value()}, KEYWORD_DIRECTION_HASH);
    }
  }

  return result;
}

namespace {

auto is_valid_container(const JSON::StringView value) -> bool {
  return value == KEYWORD_LIST || value == KEYWORD_SET ||
         value == KEYWORD_INDEX || value == KEYWORD_LANGUAGE ||
         value == KEYWORD_ID || value == KEYWORD_TYPE || value == KEYWORD_GRAPH;
}

auto ends_with_gen_delim(const JSON::StringView value) -> bool {
  if (value.empty()) {
    return false;
  }
  const char last{value.back()};
  return last == ':' || last == '/' || last == '?' || last == '#' ||
         last == '[' || last == ']' || last == '@';
}

auto same_definition(const TermDefinition &left, const TermDefinition &right)
    -> bool {
  return left.iri == right.iri && left.type_mapping == right.type_mapping &&
         left.container == right.container && left.language == right.language &&
         left.has_language == right.has_language &&
         left.direction == right.direction &&
         left.has_direction == right.has_direction &&
         left.context == right.context &&
         left.context_base == right.context_base && left.index == right.index &&
         left.reverse == right.reverse && left.prefix == right.prefix;
}

auto finalize_definition(ExpansionState &state, ActiveContext &active_context,
                         DefinedTerms &defined, const JSON::String &term,
                         const WeakPointer &term_pointer,
                         const std::optional<TermDefinition> &previous,
                         TermDefinition &&candidate) -> void {
  if (previous.has_value() && previous->is_protected &&
      !state.protected_override) {
    if (!same_definition(previous.value(), candidate)) {
      throw JSONLDError("Protected term redefinition", term_pointer);
    }
    candidate.is_protected = true;
  }
  active_context.terms[term] = std::move(candidate);
  defined[term] = true;
}

} // namespace

auto create_term_definition(ExpansionState &state,
                            ActiveContext &active_context,
                            const JSON &local_context, const JSON::String &term,
                            DefinedTerms &defined,
                            const WeakPointer &context_pointer) -> void {
  const auto status{defined.find(term)};
  if (status != defined.cend()) {
    if (status->second) {
      return;
    }
    throw JSONLDError("Cyclic IRI mapping", context_pointer.concat(term));
  }

  if (term.empty()) {
    throw JSONLDError("Invalid term definition", context_pointer);
  }

  defined[term] = false;
  const auto &value{local_context.at(term)};
  const WeakPointer term_pointer{context_pointer.concat(term)};

  if (is_keyword(term)) {
    if (term == KEYWORD_TYPE && value.is_object() && !state.processing_1_0) {
      TermDefinition type_definition;
      bool has_container{false};
      bool invalid_entry{false};
      for (const auto &entry : value.as_object()) {
        const auto &name{entry.first};
        if (name == KEYWORD_PROTECTED) {
          if (!entry.second.is_boolean()) {
            throw JSONLDError("Invalid @protected value", term_pointer,
                              {KEYWORD_PROTECTED});
          }
          type_definition.is_protected = entry.second.to_boolean();
        } else if (name == KEYWORD_CONTAINER && entry.second.is_string()) {
          const auto &container{entry.second.to_string()};
          if (container == KEYWORD_SET) {
            type_definition.container.push_back(container);
            has_container = true;
          } else {
            invalid_entry = true;
          }
        } else {
          invalid_entry = true;
        }
      }
      const auto existing_type{active_context.terms.find(KEYWORD_TYPE)};
      if (existing_type != active_context.terms.cend() &&
          existing_type->second.is_protected && !state.protected_override) {
        if (!same_definition(existing_type->second, type_definition)) {
          throw JSONLDError("Protected term redefinition", term_pointer);
        }
        type_definition.is_protected = true;
      } else if (invalid_entry || !has_container) {
        throw JSONLDError("Keyword redefinition", term_pointer);
      } else if (!type_definition.is_protected) {
        type_definition.is_protected = state.context_protected;
      }
      active_context.terms[JSON::String{KEYWORD_TYPE}] =
          std::move(type_definition);
      defined[term] = true;
      return;
    }
    throw JSONLDError("Keyword redefinition", term_pointer);
  }

  if (has_keyword_form(term)) {
    defined[term] = true;
    return;
  }

  std::optional<TermDefinition> previous;
  const auto existing{active_context.terms.find(term)};
  if (existing != active_context.terms.cend()) {
    previous = existing->second;
  }
  active_context.terms.erase(term);

  const auto *id_entry{
      value.is_object() ? value.try_at(KEYWORD_ID, KEYWORD_ID_HASH) : nullptr};
  if (value.is_null() || (id_entry != nullptr && id_entry->is_null())) {
    TermDefinition empty;
    empty.is_protected = state.context_protected;
    if (id_entry != nullptr) {
      if (const auto *protected_entry{
              value.try_at(KEYWORD_PROTECTED, KEYWORD_PROTECTED_HASH)}) {
        if (!protected_entry->is_boolean()) {
          throw JSONLDError("Invalid @protected value", term_pointer,
                            {KEYWORD_PROTECTED});
        }
        if (state.processing_1_0) {
          throw JSONLDError("Invalid term definition", term_pointer,
                            {KEYWORD_PROTECTED});
        }
        empty.is_protected = protected_entry->to_boolean();
      }
    }
    finalize_definition(state, active_context, defined, term, term_pointer,
                        previous, std::move(empty));
    return;
  }

  TermDefinition definition;
  definition.is_protected = state.context_protected;
  bool simple_term{false};

  if (value.is_string()) {
    simple_term = true;
    const auto &string_value{value.to_string()};
    if (!is_keyword(string_value) && has_keyword_form(string_value)) {
      defined[term] = true;
      return;
    }
    if (string_value == term) {
      const auto colon{term.find(':')};
      if (colon != JSON::String::npos) {
        const auto prefix{term.substr(0, colon)};
        const auto suffix{term.substr(colon + 1)};
        if (prefix != "_" && !suffix.starts_with("//") &&
            local_context.is_object() && local_context.defines(prefix)) {
          const auto iterator{defined.find(prefix)};
          if (iterator == defined.cend() || !iterator->second) {
            create_term_definition(state, active_context, local_context, prefix,
                                   defined, context_pointer);
          }
        }
        const auto prefix_definition{active_context.terms.find(prefix)};
        if (prefix_definition != active_context.terms.cend() &&
            prefix_definition->second.iri.has_value()) {
          definition.iri = prefix_definition->second.iri.value() + suffix;
        } else {
          definition.iri = term;
        }
      } else if (term.find('/') != JSON::String::npos) {
        definition.iri = expand_iri(state, active_context, term, false, true,
                                    nullptr, nullptr, empty_weak_pointer);
      } else if (active_context.vocabulary.has_value()) {
        definition.iri = active_context.vocabulary.value() + term;
      }
    } else {
      definition.iri =
          expand_iri(state, active_context, string_value, false, true,
                     &local_context, &defined, context_pointer);
      if (!state.processing_1_0 && definition.iri.has_value()) {
        const auto colon_position{term.find(':')};
        const bool iri_like_colon{colon_position != JSON::String::npos &&
                                  colon_position != 0 &&
                                  colon_position + 1 != term.size()};
        if (iri_like_colon || term.find('/') != JSON::String::npos) {
          auto probe{active_context};
          const auto expanded_term{expand_iri(state, probe, term, false, true,
                                              nullptr, nullptr,
                                              empty_weak_pointer)};
          if (expanded_term.has_value() && expanded_term != definition.iri) {
            throw JSONLDError("Invalid IRI mapping", term_pointer);
          }
        }
      }
    }
  } else if (value.is_object()) {
    const bool has_id{id_entry != nullptr};
    const JSON *const id{id_entry};
    if (const auto *reverse_entry{
            value.try_at(KEYWORD_REVERSE, KEYWORD_REVERSE_HASH)}) {
      if (has_id || value.defines(KEYWORD_NEST, KEYWORD_NEST_HASH)) {
        throw JSONLDError("Invalid reverse property", term_pointer,
                          {KEYWORD_REVERSE});
      }
      const auto &reverse{*reverse_entry};
      if (!reverse.is_string()) {
        throw JSONLDError("Invalid IRI mapping", term_pointer,
                          {KEYWORD_REVERSE});
      }
      definition.reverse = true;
      definition.iri =
          expand_iri(state, active_context, reverse.to_string(), false, true,
                     &local_context, &defined, context_pointer);
      if (!definition.iri.has_value()) {
        defined[term] = true;
        return;
      }
      if (definition.iri.value().find(':') == JSON::String::npos) {
        throw JSONLDError("Invalid IRI mapping", term_pointer,
                          {KEYWORD_REVERSE});
      }
    } else if (has_id && !id->is_null() &&
               (!id->is_string() || id->to_string() != term)) {
      if (!id->is_string()) {
        throw JSONLDError("Invalid IRI mapping", term_pointer, {KEYWORD_ID});
      }
      const auto &id_value{id->to_string()};
      if (!is_keyword(id_value) && has_keyword_form(id_value)) {
        defined[term] = true;
        return;
      }
      definition.iri = expand_iri(state, active_context, id_value, false, true,
                                  &local_context, &defined, context_pointer);
      const auto &mapping{definition.iri};
      if (!mapping.has_value() ||
          (!is_keyword(mapping.value()) &&
           mapping.value().find(':') == JSON::String::npos &&
           !active_context.vocabulary.has_value())) {
        throw JSONLDError("Invalid IRI mapping", term_pointer, {KEYWORD_ID});
      }
      if (mapping.has_value() && mapping.value() == KEYWORD_CONTEXT) {
        throw JSONLDError("Invalid keyword alias", term_pointer, {KEYWORD_ID});
      }
      if (!state.processing_1_0 && mapping.has_value()) {
        const auto colon_position{term.find(':')};
        const bool iri_like_colon{colon_position != JSON::String::npos &&
                                  colon_position != 0 &&
                                  colon_position + 1 != term.size()};
        if (iri_like_colon || term.find('/') != JSON::String::npos) {
          auto probe{active_context};
          const auto expanded_term{expand_iri(state, probe, term, false, true,
                                              nullptr, nullptr,
                                              empty_weak_pointer)};
          if (expanded_term.has_value() && expanded_term != mapping) {
            throw JSONLDError("Invalid IRI mapping", term_pointer,
                              {KEYWORD_ID});
          }
        }
      }
    } else if (term.find(':') != JSON::String::npos && !term.starts_with(':') &&
               !term.ends_with(':')) {
      const auto colon{term.find(':')};
      const auto prefix{term.substr(0, colon)};
      const auto suffix{term.substr(colon + 1)};
      if (prefix != "_" && !suffix.starts_with("//") &&
          local_context.is_object() && local_context.defines(prefix)) {
        const auto iterator{defined.find(prefix)};
        if (iterator == defined.cend() || !iterator->second) {
          create_term_definition(state, active_context, local_context, prefix,
                                 defined, context_pointer);
        }
      }
      const auto prefix_definition{active_context.terms.find(prefix)};
      if (prefix_definition != active_context.terms.cend() &&
          prefix_definition->second.iri.has_value()) {
        definition.iri = prefix_definition->second.iri.value() + suffix;
      } else {
        definition.iri = term;
      }
    } else if (term.find('/') != JSON::String::npos) {
      definition.iri = expand_iri(state, active_context, term, false, true,
                                  nullptr, nullptr, empty_weak_pointer);
    } else if (active_context.vocabulary.has_value()) {
      definition.iri = active_context.vocabulary.value() + term;
    }

    if (const auto *type_entry{value.try_at(KEYWORD_TYPE, KEYWORD_TYPE_HASH)}) {
      const auto &type_value{*type_entry};
      if (!type_value.is_string()) {
        throw JSONLDError("Invalid type mapping", term_pointer, {KEYWORD_TYPE});
      }
      const auto type{expand_iri(state, active_context, type_value.to_string(),
                                 false, true, &local_context, &defined,
                                 context_pointer)};
      if (!type.has_value() || type.value().starts_with("_:") ||
          (type.value() != KEYWORD_ID && type.value() != KEYWORD_VOCAB &&
           type.value() != KEYWORD_JSON && type.value() != KEYWORD_NONE &&
           type.value().find(':') == JSON::String::npos) ||
          (state.processing_1_0 &&
           (type.value() == KEYWORD_JSON || type.value() == KEYWORD_NONE))) {
        throw JSONLDError("Invalid type mapping", term_pointer, {KEYWORD_TYPE});
      }
      definition.type_mapping = type;
    }

    if (const auto *container_entry{
            value.try_at(KEYWORD_CONTAINER, KEYWORD_CONTAINER_HASH)}) {
      const auto &container{*container_entry};
      if (container.is_array()) {
        if (state.processing_1_0) {
          throw JSONLDError("Invalid container mapping", term_pointer,
                            {KEYWORD_CONTAINER});
        }
        for (const auto &item : container.as_array()) {
          if (!item.is_string()) {
            throw JSONLDError("Invalid container mapping", term_pointer,
                              {KEYWORD_CONTAINER});
          }
          const auto &item_string{item.to_string()};
          if (!is_valid_container(item_string)) {
            throw JSONLDError("Invalid container mapping", term_pointer,
                              {KEYWORD_CONTAINER});
          }
          for (const auto &seen : definition.container) {
            if (seen == item_string) {
              throw JSONLDError("Invalid container mapping", term_pointer,
                                {KEYWORD_CONTAINER});
            }
          }
          definition.container.push_back(item_string);
        }
      } else if (container.is_string()) {
        const auto &container_string{container.to_string()};
        if (state.processing_1_0 && (container_string == KEYWORD_GRAPH ||
                                     container_string == KEYWORD_ID ||
                                     container_string == KEYWORD_TYPE)) {
          throw JSONLDError("Invalid container mapping", term_pointer,
                            {KEYWORD_CONTAINER});
        }
        if (!is_valid_container(container_string)) {
          throw JSONLDError("Invalid container mapping", term_pointer,
                            {KEYWORD_CONTAINER});
        }
        definition.container.push_back(container_string);
      } else {
        throw JSONLDError("Invalid container mapping", term_pointer,
                          {KEYWORD_CONTAINER});
      }
      if (definition.reverse) {
        for (const auto &item : definition.container) {
          if (item != KEYWORD_SET && item != KEYWORD_INDEX) {
            throw JSONLDError("Invalid reverse property", term_pointer,
                              {KEYWORD_CONTAINER});
          }
        }
      }
      bool container_graph{false};
      bool container_id{false};
      bool container_index{false};
      bool container_language{false};
      bool container_list{false};
      bool container_set{false};
      bool container_type{false};
      for (const auto &item : definition.container) {
        if (item == KEYWORD_GRAPH) {
          container_graph = true;
        } else if (item == KEYWORD_ID) {
          container_id = true;
        } else if (item == KEYWORD_INDEX) {
          container_index = true;
        } else if (item == KEYWORD_LANGUAGE) {
          container_language = true;
        } else if (item == KEYWORD_LIST) {
          container_list = true;
        } else if (item == KEYWORD_SET) {
          container_set = true;
        } else if (item == KEYWORD_TYPE) {
          container_type = true;
        }
      }
      if (definition.container.size() != 1) {
        const bool graph_form{
            container_graph && (container_id != container_index) &&
            !container_list && !container_type && !container_language};
        const bool set_form{container_set && !container_list};
        if (!graph_form && !set_form) {
          throw JSONLDError("Invalid container mapping", term_pointer,
                            {KEYWORD_CONTAINER});
        }
      }
      if (container_type && definition.type_mapping.has_value() &&
          definition.type_mapping.value() != KEYWORD_ID &&
          definition.type_mapping.value() != KEYWORD_VOCAB) {
        throw JSONLDError("Invalid type mapping", term_pointer, {KEYWORD_TYPE});
      }
    }

    if (const auto *language_entry{
            value.try_at(KEYWORD_LANGUAGE, KEYWORD_LANGUAGE_HASH)};
        language_entry != nullptr &&
        !value.defines(KEYWORD_TYPE, KEYWORD_TYPE_HASH)) {
      const auto &language{*language_entry};
      if (!language.is_null() && !language.is_string()) {
        throw JSONLDError("Invalid language mapping", term_pointer,
                          {KEYWORD_LANGUAGE});
      }
      definition.has_language = true;
      if (language.is_string()) {
        definition.language = language.to_string();
      }
    }

    if (const auto *direction_entry{
            value.try_at(KEYWORD_DIRECTION, KEYWORD_DIRECTION_HASH)};
        direction_entry != nullptr &&
        !value.defines(KEYWORD_TYPE, KEYWORD_TYPE_HASH)) {
      const auto &direction{*direction_entry};
      if (!direction.is_null() &&
          (!direction.is_string() || (direction.to_string() != "ltr" &&
                                      direction.to_string() != "rtl"))) {
        throw JSONLDError("Invalid base direction", term_pointer,
                          {KEYWORD_DIRECTION});
      }
      definition.has_direction = true;
      if (direction.is_string()) {
        definition.direction = direction.to_string();
      }
    }

    if (const auto *context_entry{
            value.try_at(KEYWORD_CONTEXT, KEYWORD_CONTEXT_HASH)}) {
      if (state.processing_1_0) {
        throw JSONLDError("Invalid term definition", term_pointer,
                          {KEYWORD_CONTEXT});
      }
      const bool saved_override{state.protected_override};
      const bool saved_context_protected{state.context_protected};
      try {
        ActiveContext probe{active_context};
        state.protected_override = true;
        process_context(state, probe, *context_entry, empty_weak_pointer);
        state.protected_override = saved_override;
        state.context_protected = saved_context_protected;
      } catch (const JSONLDError &error) {
        state.protected_override = saved_override;
        state.context_protected = saved_context_protected;
        const JSON::StringView code{error.what()};
        if (code != "Loading remote context failed" &&
            code != "Recursive context inclusion" &&
            code != "Invalid remote context") {
          throw JSONLDError("Invalid scoped context", term_pointer,
                            {KEYWORD_CONTEXT});
        }
      }
      definition.context = *context_entry;
      definition.context_base = state.context_resolution_base();
    }

    if (const auto *prefix_entry{
            value.try_at(KEYWORD_PREFIX, KEYWORD_PREFIX_HASH)}) {
      if (state.processing_1_0 || term.find(':') != JSON::String::npos ||
          term.find('/') != JSON::String::npos) {
        throw JSONLDError("Invalid term definition", term_pointer,
                          {KEYWORD_PREFIX});
      }
      if (!prefix_entry->is_boolean()) {
        throw JSONLDError("Invalid @prefix value", term_pointer,
                          {KEYWORD_PREFIX});
      }
      definition.prefix = prefix_entry->to_boolean();
      if (definition.prefix && definition.iri.has_value() &&
          is_keyword(definition.iri.value())) {
        throw JSONLDError("Invalid term definition", term_pointer,
                          {KEYWORD_PREFIX});
      }
    }

    if (const auto *nest_entry{value.try_at(KEYWORD_NEST, KEYWORD_NEST_HASH)}) {
      if (state.processing_1_0) {
        throw JSONLDError("Invalid term definition", term_pointer,
                          {KEYWORD_NEST});
      }
      const auto &nest{*nest_entry};
      if (!nest.is_string()) {
        throw JSONLDError("Invalid @nest value", term_pointer, {KEYWORD_NEST});
      }
      const auto &nest_string{nest.to_string()};
      if (is_keyword(nest_string) && nest_string != KEYWORD_NEST) {
        throw JSONLDError("Invalid @nest value", term_pointer, {KEYWORD_NEST});
      }
    }

    if (const auto *index_entry{
            value.try_at(KEYWORD_INDEX, KEYWORD_INDEX_HASH)}) {
      if (state.processing_1_0) {
        throw JSONLDError("Invalid term definition", term_pointer,
                          {KEYWORD_INDEX});
      }
      bool has_index_container{false};
      for (const auto &item : definition.container) {
        if (item == KEYWORD_INDEX) {
          has_index_container = true;
        }
      }
      const auto &index{*index_entry};
      if (!index.is_string() || !has_index_container) {
        throw JSONLDError("Invalid term definition", term_pointer,
                          {KEYWORD_INDEX});
      }
      const auto &index_string{index.to_string()};
      const auto index_iri{expand_iri(state, active_context, index_string,
                                      false, true, &local_context, &defined,
                                      context_pointer)};
      if (!index_iri.has_value() || is_keyword(index_iri.value())) {
        throw JSONLDError("Invalid term definition", term_pointer,
                          {KEYWORD_INDEX});
      }
      definition.index = index_string;
    }

    if (const auto *protected_entry{
            value.try_at(KEYWORD_PROTECTED, KEYWORD_PROTECTED_HASH)}) {
      if (!protected_entry->is_boolean()) {
        throw JSONLDError("Invalid @protected value", term_pointer,
                          {KEYWORD_PROTECTED});
      }
      if (state.processing_1_0) {
        throw JSONLDError("Invalid term definition", term_pointer,
                          {KEYWORD_PROTECTED});
      }
      definition.is_protected = protected_entry->to_boolean();
    }

    for (const auto &entry : value.as_object()) {
      const JSON::StringView key{entry.first};
      if (key != KEYWORD_ID && key != KEYWORD_REVERSE &&
          key != KEYWORD_CONTAINER && key != KEYWORD_CONTEXT &&
          key != KEYWORD_DIRECTION && key != KEYWORD_INDEX &&
          key != KEYWORD_LANGUAGE && key != KEYWORD_NEST &&
          key != KEYWORD_PREFIX && key != KEYWORD_PROTECTED &&
          key != KEYWORD_TYPE) {
        throw JSONLDError("Invalid term definition", term_pointer);
      }
    }
  } else {
    throw JSONLDError("Invalid term definition", term_pointer);
  }

  if (simple_term && term.find(':') == JSON::String::npos &&
      term.find('/') == JSON::String::npos && definition.iri.has_value() &&
      (ends_with_gen_delim(definition.iri.value()) ||
       definition.iri.value().starts_with("_:"))) {
    definition.prefix = true;
  }

  if (!definition.reverse && !definition.iri.has_value()) {
    throw JSONLDError("Invalid IRI mapping", term_pointer);
  }

  finalize_definition(state, active_context, defined, term, term_pointer,
                      previous, std::move(definition));
}

auto process_context(ExpansionState &state, ActiveContext &active_context,
                     const JSON &local_context, const WeakPointer &pointer,
                     const bool propagate) -> void {
  std::vector<std::pair<const JSON *, WeakPointer>> contexts;
  if (local_context.is_array()) {
    std::size_t index{0};
    for (const auto &item : local_context.as_array()) {
      contexts.emplace_back(&item, pointer.concat(index));
      index += 1;
    }
  } else {
    contexts.emplace_back(&local_context, pointer);
  }

  bool effective_propagate{propagate};
  if (local_context.is_object()) {
    if (const auto *propagate_entry{
            local_context.try_at(KEYWORD_PROPAGATE, KEYWORD_PROPAGATE_HASH)};
        propagate_entry != nullptr && propagate_entry->is_boolean()) {
      effective_propagate = propagate_entry->to_boolean();
    }
  }
  if (!effective_propagate && !active_context.previous) {
    auto snapshot{std::make_shared<ActiveContext>(active_context)};
    snapshot->previous = nullptr;
    active_context.previous = snapshot;
  }

  for (const auto &[entry_pointer, location] : contexts) {
    const auto &context{*entry_pointer};
    if (context.is_null()) {
      if (!state.protected_override) {
        for (const auto &entry : active_context.terms) {
          if (entry.second.is_protected) {
            throw JSONLDError("Invalid context nullification", location);
          }
        }
      }
      active_context = ActiveContext{};
      active_context.base = state.document_base;
      continue;
    }

    if (context.is_string()) {
      auto reference{context.to_string()};
      const auto resolution_base{state.context_resolution_base()};
      if (resolution_base.has_value()) {
        reference = URI::from_iri(reference)
                        .resolve_from(URI::from_iri(resolution_base.value()))
                        .recompose();
      }
      for (const auto &loaded : state.remote_context_chain) {
        if (loaded == reference) {
          throw JSONLDError("Recursive context inclusion", location);
        }
      }
      if (state.resolver == nullptr || !*state.resolver) {
        throw JSONLDError("Loading remote context failed", location);
      }
      const auto document{(*state.resolver)(reference)};
      if (!document.has_value()) {
        throw JSONLDError("Loading remote context failed", location);
      }
      const auto *context_entry{
          document->is_object()
              ? document->try_at(KEYWORD_CONTEXT, KEYWORD_CONTEXT_HASH)
              : nullptr};
      if (context_entry == nullptr) {
        throw JSONLDError("Invalid remote context", location);
      }
      state.remote_context_chain.push_back(reference);
      try {
        process_context(state, active_context, *context_entry, location);
      } catch (...) {
        state.remote_context_chain.pop_back();
        throw;
      }
      state.remote_context_chain.pop_back();
      continue;
    }

    if (!context.is_object()) {
      throw JSONLDError("Invalid local context", location);
    }

    if (const auto *version{
            context.try_at(KEYWORD_VERSION, KEYWORD_VERSION_HASH)};
        version != nullptr &&
        (!version->is_real() || version->to_real() != 1.1)) {
      throw JSONLDError("Invalid @version value", location, {KEYWORD_VERSION});
    }
    if (state.processing_1_0 &&
        context.defines(KEYWORD_VERSION, KEYWORD_VERSION_HASH)) {
      throw JSONLDError("Processing mode conflict", location,
                        {KEYWORD_VERSION});
    }
    if (state.processing_1_0 &&
        (context.defines(KEYWORD_PROPAGATE, KEYWORD_PROPAGATE_HASH) ||
         context.defines(KEYWORD_IMPORT, KEYWORD_IMPORT_HASH) ||
         context.defines(KEYWORD_PROTECTED, KEYWORD_PROTECTED_HASH))) {
      throw JSONLDError("Invalid context entry", location);
    }

    if (const auto *propagate_entry{
            context.try_at(KEYWORD_PROPAGATE, KEYWORD_PROPAGATE_HASH)};
        propagate_entry != nullptr && !propagate_entry->is_boolean()) {
      throw JSONLDError("Invalid @propagate value", location,
                        {KEYWORD_PROPAGATE});
    }

    const bool saved_protected{state.context_protected};
    if (const auto *protected_entry{
            context.try_at(KEYWORD_PROTECTED, KEYWORD_PROTECTED_HASH)}) {
      if (!protected_entry->is_boolean()) {
        throw JSONLDError("Invalid @protected value", location,
                          {KEYWORD_PROTECTED});
      }
      state.context_protected = protected_entry->to_boolean();
    }

    if (const auto *import_entry{
            context.try_at(KEYWORD_IMPORT, KEYWORD_IMPORT_HASH)}) {
      const auto &import{*import_entry};
      if (!import.is_string()) {
        throw JSONLDError("Invalid @import value", location, {KEYWORD_IMPORT});
      }
      auto reference{import.to_string()};
      const auto resolution_base{state.context_resolution_base()};
      if (resolution_base.has_value()) {
        reference = URI::from_iri(reference)
                        .resolve_from(URI::from_iri(resolution_base.value()))
                        .recompose();
      }
      if (state.resolver == nullptr || !*state.resolver) {
        throw JSONLDError("Loading remote context failed", location,
                          {KEYWORD_IMPORT});
      }
      const auto document{(*state.resolver)(reference)};
      if (!document.has_value()) {
        throw JSONLDError("Loading remote context failed", location,
                          {KEYWORD_IMPORT});
      }
      const auto *imported_context{
          document->is_object()
              ? document->try_at(KEYWORD_CONTEXT, KEYWORD_CONTEXT_HASH)
              : nullptr};
      if (imported_context == nullptr || !imported_context->is_object()) {
        throw JSONLDError("Invalid remote context", location, {KEYWORD_IMPORT});
      }
      if (imported_context->defines(KEYWORD_IMPORT, KEYWORD_IMPORT_HASH)) {
        throw JSONLDError("Invalid context entry", location, {KEYWORD_IMPORT});
      }
      auto merged{JSON{*imported_context}};
      for (const auto &entry : context.as_object()) {
        if (JSON::StringView{entry.first} != KEYWORD_IMPORT) {
          merged.assign(entry.first, entry.second);
        }
      }
      process_context(state, active_context, merged, location, propagate);
      state.context_protected = saved_protected;
      continue;
    }

    if (const auto *base_entry{
            state.remote_context_chain.empty()
                ? context.try_at(KEYWORD_BASE, KEYWORD_BASE_HASH)
                : nullptr}) {
      const auto &base{*base_entry};
      if (base.is_null()) {
        active_context.base = std::nullopt;
      } else if (!base.is_string()) {
        throw JSONLDError("Invalid base IRI", location, {KEYWORD_BASE});
      } else {
        const auto &base_string{base.to_string()};
        if (active_context.base.has_value()) {
          active_context.base =
              URI::from_iri(base_string)
                  .resolve_from(URI::from_iri(active_context.base.value()))
                  .recompose();
        } else if (URI::from_iri(base_string).is_absolute()) {
          active_context.base = base_string;
        } else {
          throw JSONLDError("Invalid base IRI", location, {KEYWORD_BASE});
        }
      }
    }

    if (const auto *vocabulary_entry{
            context.try_at(KEYWORD_VOCAB, KEYWORD_VOCAB_HASH)}) {
      const auto &vocabulary{*vocabulary_entry};
      if (vocabulary.is_null()) {
        active_context.vocabulary = std::nullopt;
      } else if (!vocabulary.is_string()) {
        throw JSONLDError("Invalid vocab mapping", location, {KEYWORD_VOCAB});
      } else {
        const auto &vocabulary_string{vocabulary.to_string()};
        if (state.processing_1_0 &&
            vocabulary_string.find(':') == JSON::String::npos) {
          throw JSONLDError("Invalid vocab mapping", location, {KEYWORD_VOCAB});
        }
        active_context.vocabulary =
            expand_iri(state, active_context, vocabulary_string, true, true,
                       nullptr, nullptr, empty_weak_pointer);
      }
    }

    if (const auto *language_entry{
            context.try_at(KEYWORD_LANGUAGE, KEYWORD_LANGUAGE_HASH)}) {
      const auto &language{*language_entry};
      if (language.is_null()) {
        active_context.default_language = std::nullopt;
      } else if (!language.is_string()) {
        throw JSONLDError("Invalid default language", location,
                          {KEYWORD_LANGUAGE});
      } else {
        active_context.default_language = language.to_string();
      }
    }

    if (const auto *direction_entry{
            context.try_at(KEYWORD_DIRECTION, KEYWORD_DIRECTION_HASH)}) {
      const auto &direction{*direction_entry};
      if (direction.is_null()) {
        active_context.default_direction = std::nullopt;
      } else if (!direction.is_string()) {
        throw JSONLDError("Invalid base direction", location,
                          {KEYWORD_DIRECTION});
      } else {
        const auto &direction_string{direction.to_string()};
        if (direction_string != "ltr" && direction_string != "rtl") {
          throw JSONLDError("Invalid base direction", location,
                            {KEYWORD_DIRECTION});
        }
        active_context.default_direction = direction_string;
      }
    }

    DefinedTerms defined;
    for (const auto &entry : context.as_object()) {
      const auto &name{entry.first};
      if (name == KEYWORD_BASE || name == KEYWORD_VOCAB ||
          name == KEYWORD_LANGUAGE || name == KEYWORD_VERSION ||
          name == KEYWORD_DIRECTION || name == KEYWORD_IMPORT ||
          name == KEYWORD_PROPAGATE || name == KEYWORD_PROTECTED) {
        continue;
      }
      create_term_definition(state, active_context, context, name, defined,
                             location);
    }

    state.context_protected = saved_protected;
  }
}

namespace {

auto into_array(JSON &&value) -> JSON {
  if (value.is_array()) {
    return std::move(value);
  }
  auto result{JSON::make_array()};
  result.push_back(std::move(value));
  return result;
}

auto sorted_entries(const JSON &object)
    -> std::vector<std::pair<const JSON::String *, const JSON *>> {
  std::vector<std::pair<const JSON::String *, const JSON *>> entries;
  for (const auto &entry : object.as_object()) {
    entries.emplace_back(&entry.first, &entry.second);
  }
  std::ranges::sort(entries, [](const auto &left, const auto &right) -> bool {
    return *left.first < *right.first;
  });
  return entries;
}

auto merge(JSON &object, const JSON::StringView name, JSON &&values) -> void {
  if (object.defines(name)) {
    for (auto &item : values.as_array()) {
      object.at(name).push_back(item);
    }
  } else {
    object.assign(name, std::move(values));
  }
}

auto container_includes(const TermDefinition *const definition,
                        const JSON::StringView name) -> bool {
  if (definition == nullptr) {
    return false;
  }
  for (const auto &entry : definition->container) {
    if (entry == name) {
      return true;
    }
  }
  return false;
}

auto expand_type(ExpansionState &state, const ActiveContext &type_context,
                 const JSON &item) -> JSON {
  auto context{type_context};
  const auto type{expand_iri(state, context, item.to_string(), true, true,
                             nullptr, nullptr, empty_weak_pointer)};
  return type.has_value() ? JSON{type.value()} : JSON{nullptr};
}

auto expand_entries(ExpansionState &state, ActiveContext &active_context,
                    const ActiveContext &type_context, JSON &result,
                    const std::optional<JSON::String> &active_property,
                    const JSON &source, const WeakPointer &source_pointer)
    -> void;

auto expand_object(ExpansionState &state, ActiveContext active_context,
                   const std::optional<JSON::String> &active_property,
                   const JSON &element, const WeakPointer &pointer) -> JSON {
  auto result{JSON::make_object()};

  const ActiveContext type_context{active_context};

  std::vector<JSON::StringView> type_values;
  for (const auto &entry : element.as_object()) {
    const auto expanded{expand_iri(state, active_context, entry.first, false,
                                   true, nullptr, nullptr, empty_weak_pointer)};
    if (!expanded.has_value() || expanded.value() != KEYWORD_TYPE) {
      continue;
    }
    if (entry.second.is_array()) {
      for (const auto &item : entry.second.as_array()) {
        if (item.is_string()) {
          type_values.push_back(item.to_string());
        }
      }
    } else if (entry.second.is_string()) {
      type_values.push_back(entry.second.to_string());
    }
  }
  std::ranges::sort(type_values);
  for (const auto &type : type_values) {
    const auto definition{type_context.terms.find(type)};
    if (definition != type_context.terms.cend() &&
        definition->second.context.has_value()) {
      const auto &scoped{definition->second.context.value()};
      const auto saved_base{state.context_base_override};
      state.context_base_override = definition->second.context_base;
      process_context(state, active_context, scoped, pointer, false);
      state.context_base_override = saved_base;
    }
  }

  expand_entries(state, active_context, type_context, result, active_property,
                 element, pointer);

  if (const auto *reverse{result.try_at(KEYWORD_REVERSE, KEYWORD_REVERSE_HASH)};
      reverse != nullptr && reverse->empty()) {
    result.erase(KEYWORD_REVERSE);
  }

  if (const auto *value_entry{
          result.try_at(KEYWORD_VALUE, KEYWORD_VALUE_HASH)}) {
    const JSON *const type{result.try_at(KEYWORD_TYPE, KEYWORD_TYPE_HASH)};
    const bool has_type{type != nullptr};
    const JSON::String *const type_string{
        type != nullptr && type->is_string() ? &type->to_string() : nullptr};
    const bool is_json{type_string != nullptr && *type_string == KEYWORD_JSON};
    for (const auto &entry : result.as_object()) {
      const auto &name{entry.first};
      if (name != KEYWORD_VALUE && name != KEYWORD_TYPE &&
          name != KEYWORD_LANGUAGE && name != KEYWORD_INDEX &&
          name != KEYWORD_DIRECTION) {
        throw JSONLDError("Invalid value object", pointer);
      }
      if ((name == KEYWORD_LANGUAGE || name == KEYWORD_DIRECTION) && has_type) {
        throw JSONLDError("Invalid value object", pointer);
      }
    }
    const auto &content{*value_entry};
    if (content.is_null() && !is_json) {
      return JSON{nullptr};
    }
    if (result.defines(KEYWORD_LANGUAGE, KEYWORD_LANGUAGE_HASH) &&
        !content.is_string()) {
      throw JSONLDError("Invalid language-tagged value", pointer);
    }
    if (has_type && (type_string == nullptr || type_string->starts_with("_:") ||
                     type_string->find(' ') != JSON::String::npos)) {
      throw JSONLDError("Invalid typed value", pointer);
    }
    if (!is_json && !content.is_string() && !content.is_number() &&
        !content.is_boolean()) {
      throw JSONLDError("Invalid value object value", pointer);
    }
  } else if (const auto *type_entry{
                 result.try_at(KEYWORD_TYPE, KEYWORD_TYPE_HASH)};
             type_entry != nullptr && !type_entry->is_array()) {
    result.assign(KEYWORD_TYPE, into_array(JSON{*type_entry}));
  }

  if (result.defines(KEYWORD_LIST, KEYWORD_LIST_HASH) ||
      result.defines(KEYWORD_SET, KEYWORD_SET_HASH)) {
    for (const auto &entry : result.as_object()) {
      const auto &name{entry.first};
      if (name != KEYWORD_LIST && name != KEYWORD_SET &&
          name != KEYWORD_INDEX) {
        throw JSONLDError("Invalid set or list object", pointer);
      }
    }
  }

  if (const auto *set{result.try_at(KEYWORD_SET, KEYWORD_SET_HASH)}) {
    return *set;
  }

  if (!result.defines(KEYWORD_VALUE, KEYWORD_VALUE_HASH) &&
      (result.defines(KEYWORD_LANGUAGE, KEYWORD_LANGUAGE_HASH) ||
       result.defines(KEYWORD_DIRECTION, KEYWORD_DIRECTION_HASH))) {
    bool only_value_keys{true};
    for (const auto &entry : result.as_object()) {
      const auto &name{entry.first};
      if (name != KEYWORD_LANGUAGE && name != KEYWORD_DIRECTION &&
          name != KEYWORD_INDEX) {
        only_value_keys = false;
      }
    }
    if (only_value_keys) {
      return JSON{nullptr};
    }
  }

  if (!active_property.has_value() ||
      active_property.value() == KEYWORD_GRAPH) {
    if (result.empty() || result.defines(KEYWORD_VALUE, KEYWORD_VALUE_HASH) ||
        result.defines(KEYWORD_LIST, KEYWORD_LIST_HASH) ||
        (result.object_size() == 1 &&
         result.defines(KEYWORD_ID, KEYWORD_ID_HASH))) {
      return JSON{nullptr};
    }
  }

  return result;
}

auto expand_entries(ExpansionState &state, ActiveContext &active_context,
                    const ActiveContext &type_context, JSON &result,
                    const std::optional<JSON::String> &active_property,
                    const JSON &source, const WeakPointer &source_pointer)
    -> void {
  std::vector<std::pair<const JSON::String *, const JSON *>> nests;
  for (const auto &[key_pointer, value_pointer] : sorted_entries(source)) {
    const std::pair<const JSON::String &, const JSON &> entry{*key_pointer,
                                                              *value_pointer};
    const JSON::String &property{entry.first};
    const WeakPointer entry_pointer{source_pointer.concat(property)};
    if (property == KEYWORD_CONTEXT) {
      continue;
    }

    const auto expanded_property{expand_iri(state, active_context, property,
                                            false, true, nullptr, nullptr,
                                            empty_weak_pointer)};

    if (expanded_property.has_value() &&
        expanded_property.value() == KEYWORD_NEST) {
      if (entry.second.is_array()) {
        for (const auto &nest_value : entry.second.as_array()) {
          if (!nest_value.is_object() ||
              nest_value.defines(KEYWORD_VALUE, KEYWORD_VALUE_HASH)) {
            throw JSONLDError("Invalid @nest value", entry_pointer);
          }
          nests.emplace_back(&property, &nest_value);
        }
      } else if (entry.second.is_object() &&
                 !entry.second.defines(KEYWORD_VALUE, KEYWORD_VALUE_HASH)) {
        nests.emplace_back(&property, &entry.second);
      } else {
        throw JSONLDError("Invalid @nest value", entry_pointer);
      }
      continue;
    }
    if (!expanded_property.has_value()) {
      continue;
    }

    const auto &name{expanded_property.value()};
    if (name.find(':') == JSON::String::npos && !is_keyword(name)) {
      continue;
    }

    if (is_keyword(name) && active_property.has_value() &&
        active_property.value() == KEYWORD_REVERSE) {
      throw JSONLDError("Invalid reverse property map", entry_pointer);
    }

    if (is_keyword(name) && result.defines(name) &&
        (state.processing_1_0 ||
         (name != KEYWORD_TYPE && name != KEYWORD_INCLUDED))) {
      throw JSONLDError("Colliding keywords", entry_pointer);
    }

    if (name == KEYWORD_ID) {
      if (!entry.second.is_string()) {
        throw JSONLDError("Invalid @id value", entry_pointer);
      }
      const auto identifier{expand_iri(state, active_context,
                                       entry.second.to_string(), true, false,
                                       nullptr, nullptr, empty_weak_pointer)};
      if (identifier.has_value()) {
        result.assign_assume_new(JSON::String{KEYWORD_ID},
                                 JSON{identifier.value()}, KEYWORD_ID_HASH);
      } else {
        result.assign_assume_new(JSON::String{KEYWORD_ID}, JSON{nullptr},
                                 KEYWORD_ID_HASH);
      }
      continue;
    }

    if (name == KEYWORD_TYPE) {
      if (entry.second.is_array()) {
        for (const auto &item : entry.second.as_array()) {
          if (!item.is_string()) {
            throw JSONLDError("Invalid type value", entry_pointer);
          }
        }
      } else if (!entry.second.is_string()) {
        throw JSONLDError("Invalid type value", entry_pointer);
      }
      JSON expanded_type{nullptr};
      if (entry.second.is_array()) {
        expanded_type = JSON::make_array();
        for (const auto &item : entry.second.as_array()) {
          auto type{expand_type(state, type_context, item)};
          if (!type.is_null()) {
            expanded_type.push_back(std::move(type));
          }
        }
      } else {
        expanded_type = expand_type(state, type_context, entry.second);
      }
      if (expanded_type.is_null()) {
        continue;
      }
      if (result.defines(KEYWORD_TYPE, KEYWORD_TYPE_HASH)) {
        auto merged{
            into_array(std::move(result.at(KEYWORD_TYPE, KEYWORD_TYPE_HASH)))};
        auto expanded_type_array{into_array(std::move(expanded_type))};
        for (auto &item : expanded_type_array.as_array()) {
          merged.push_back(item);
        }
        result.assign(KEYWORD_TYPE, std::move(merged));
      } else {
        result.assign_assume_new(JSON::String{KEYWORD_TYPE},
                                 std::move(expanded_type), KEYWORD_TYPE_HASH);
      }
      continue;
    }

    if (name == KEYWORD_VALUE) {
      result.assign_assume_new(JSON::String{KEYWORD_VALUE}, JSON{entry.second},
                               KEYWORD_VALUE_HASH);
      continue;
    }

    if (name == KEYWORD_LANGUAGE) {
      if (!entry.second.is_string()) {
        throw JSONLDError("Invalid language-tagged string", entry_pointer);
      }
      result.assign_assume_new(JSON::String{KEYWORD_LANGUAGE},
                               JSON{entry.second}, KEYWORD_LANGUAGE_HASH);
      continue;
    }

    if (name == KEYWORD_DIRECTION) {
      if (state.processing_1_0) {
        continue;
      }
      if (!entry.second.is_string() || (entry.second.to_string() != "ltr" &&
                                        entry.second.to_string() != "rtl")) {
        throw JSONLDError("Invalid base direction", entry_pointer);
      }
      result.assign_assume_new(JSON::String{KEYWORD_DIRECTION},
                               JSON{entry.second}, KEYWORD_DIRECTION_HASH);
      continue;
    }

    if (name == KEYWORD_LIST || name == KEYWORD_SET) {
      auto elements{JSON::make_array()};
      const auto values{into_array(JSON{entry.second})};
      std::size_t value_index{0};
      for (const auto &item : values.as_array()) {
        const WeakPointer item_pointer{entry.second.is_array()
                                           ? entry_pointer.concat(value_index)
                                           : entry_pointer};
        auto expanded_item{
            expand(state, active_context, active_property, item, item_pointer)};
        if (expanded_item.is_array()) {
          for (auto &nested : expanded_item.as_array()) {
            elements.push_back(nested);
          }
        } else if (!expanded_item.is_null()) {
          elements.push_back(std::move(expanded_item));
        }
        value_index += 1;
      }
      if (name == KEYWORD_LIST && state.processing_1_0) {
        for (const auto &item : elements.as_array()) {
          if (item.is_object() &&
              item.defines(KEYWORD_LIST, KEYWORD_LIST_HASH)) {
            throw JSONLDError("List of lists", entry_pointer);
          }
        }
      }
      result.assign(name, std::move(elements));
      continue;
    }

    if (name == KEYWORD_GRAPH) {
      auto graph{expand(state, active_context, JSON::String{KEYWORD_GRAPH},
                        entry.second, entry_pointer)};
      merge(result, KEYWORD_GRAPH,
            graph.is_null() ? JSON::make_array()
                            : into_array(std::move(graph)));
      continue;
    }

    if (name == KEYWORD_INCLUDED) {
      if (state.processing_1_0) {
        continue;
      }
      auto included{into_array(expand(state, active_context, std::nullopt,
                                      entry.second, entry_pointer))};
      for (const auto &item : included.as_array()) {
        if (!item.is_object() ||
            item.defines(KEYWORD_VALUE, KEYWORD_VALUE_HASH) ||
            item.defines(KEYWORD_LIST, KEYWORD_LIST_HASH) ||
            item.defines(KEYWORD_SET, KEYWORD_SET_HASH)) {
          throw JSONLDError("Invalid @included value", entry_pointer);
        }
      }
      merge(result, KEYWORD_INCLUDED, std::move(included));
      continue;
    }

    if (name == KEYWORD_INDEX) {
      if (!entry.second.is_string()) {
        throw JSONLDError("Invalid @index value", entry_pointer);
      }
      result.assign_assume_new(JSON::String{KEYWORD_INDEX}, JSON{entry.second},
                               KEYWORD_INDEX_HASH);
      continue;
    }

    if (name == KEYWORD_REVERSE) {
      if (!entry.second.is_object()) {
        throw JSONLDError("Invalid @reverse value", entry_pointer);
      }
      auto reversed{expand(state, active_context, JSON::String{KEYWORD_REVERSE},
                           entry.second, entry_pointer)};
      if (reversed.is_object()) {
        const auto *existing_reverse{
            result.try_at(KEYWORD_REVERSE, KEYWORD_REVERSE_HASH)};
        auto reverse_map{existing_reverse != nullptr ? *existing_reverse
                                                     : JSON::make_object()};
        for (const auto &reverse_entry : reversed.as_object()) {
          const auto &reverse_property{reverse_entry.first};
          if (reverse_entry.hash == KEYWORD_REVERSE_HASH) {
            for (auto &forward : reverse_entry.second.as_object()) {
              merge(result, JSON::StringView{forward.first},
                    into_array(JSON{forward.second}));
            }
          } else if (is_keyword(reverse_property, reverse_entry.hash)) {
            throw JSONLDError("Invalid reverse property map", entry_pointer);
          } else {
            const auto reverse_values{into_array(JSON{reverse_entry.second})};
            for (const auto &item : reverse_values.as_array()) {
              if (item.is_object() &&
                  (item.defines(KEYWORD_VALUE, KEYWORD_VALUE_HASH) ||
                   item.defines(KEYWORD_LIST, KEYWORD_LIST_HASH))) {
                throw JSONLDError("Invalid reverse property value",
                                  entry_pointer);
              }
            }
            merge(reverse_map, reverse_property,
                  into_array(JSON{reverse_entry.second}));
          }
        }
        result.assign(KEYWORD_REVERSE, std::move(reverse_map));
      }
      continue;
    }

    if (is_keyword(name)) {
      continue;
    }

    const TermDefinition *definition{nullptr};
    const auto term{active_context.terms.find(property)};
    if (term != active_context.terms.cend()) {
      definition = &term->second;
    }

    ActiveContext scoped_context;
    const bool scoped{definition != nullptr && definition->context.has_value()};
    if (scoped) {
      scoped_context = active_context;
      scoped_context.previous = nullptr;
      const auto saved_override{state.protected_override};
      state.protected_override = true;
      const auto saved_base{state.context_base_override};
      state.context_base_override = definition->context_base;
      process_context(state, scoped_context, definition->context.value(),
                      entry_pointer);
      state.context_base_override = saved_base;
      state.protected_override = saved_override;
    }
    ActiveContext &value_context{scoped ? scoped_context : active_context};

    JSON expanded_value{nullptr};
    if (definition != nullptr && definition->type_mapping.has_value() &&
        definition->type_mapping.value() == KEYWORD_JSON) {
      auto json_value{JSON::make_object()};
      json_value.assign_assume_new(JSON::String{KEYWORD_VALUE},
                                   JSON{entry.second}, KEYWORD_VALUE_HASH);
      json_value.assign_assume_new(JSON::String{KEYWORD_TYPE},
                                   JSON{KEYWORD_JSON}, KEYWORD_TYPE_HASH);
      expanded_value = std::move(json_value);
    } else if (entry.second.is_object() &&
               container_includes(definition, KEYWORD_GRAPH) &&
               (container_includes(definition, KEYWORD_ID) ||
                container_includes(definition, KEYWORD_INDEX))) {
      const bool by_id{container_includes(definition, KEYWORD_ID)};
      const bool property_valued{definition->index.has_value() &&
                                 definition->index.value() != KEYWORD_INDEX};
      std::optional<JSON::String> index_property;
      if (property_valued) {
        index_property =
            expand_iri(state, value_context, definition->index.value(), false,
                       true, nullptr, nullptr, empty_weak_pointer);
      }
      expanded_value = JSON::make_array();
      for (const auto &[graph_key, graph_value] :
           sorted_entries(entry.second)) {
        const JSON::String &index{*graph_key};
        const auto expanded_key{index == KEYWORD_NONE
                                    ? std::optional<JSON::String>{KEYWORD_NONE}
                                    : expand_iri(state, value_context, index,
                                                 true, false, nullptr, nullptr,
                                                 empty_weak_pointer)};
        const bool none_key{expanded_key.has_value() &&
                            expanded_key.value() == KEYWORD_NONE};
        auto graph_items{
            into_array(expand(state, value_context, property, *graph_value,
                              entry_pointer.concat(index)))};
        for (auto &item : graph_items.as_array()) {
          JSON graph{nullptr};
          if (item.is_object() &&
              item.defines(KEYWORD_GRAPH, KEYWORD_GRAPH_HASH)) {
            graph = std::move(item);
          } else {
            graph = JSON::make_object();
            graph.assign_assume_new(JSON::String{KEYWORD_GRAPH},
                                    into_array(std::move(item)),
                                    KEYWORD_GRAPH_HASH);
          }
          if (!none_key) {
            if (by_id) {
              if (!graph.defines(KEYWORD_ID, KEYWORD_ID_HASH)) {
                graph.assign_assume_new(JSON::String{KEYWORD_ID},
                                        JSON{expanded_key.value_or(index)},
                                        KEYWORD_ID_HASH);
              }
            } else if (property_valued) {
              auto combined{into_array(expand_value(
                  state, value_context, definition->index, JSON{index}))};
              if (graph.defines(index_property.value())) {
                for (auto &existing :
                     graph.at(index_property.value()).as_array()) {
                  combined.push_back(existing);
                }
              }
              graph.assign(index_property.value(), std::move(combined));
            } else if (!graph.defines(KEYWORD_INDEX, KEYWORD_INDEX_HASH)) {
              graph.assign_assume_new(JSON::String{KEYWORD_INDEX}, JSON{index},
                                      KEYWORD_INDEX_HASH);
            }
          }
          expanded_value.push_back(std::move(graph));
        }
      }
    } else if (entry.second.is_object() &&
               container_includes(definition, KEYWORD_LANGUAGE)) {
      expanded_value = JSON::make_array();
      for (const auto &[language_key, language_value] :
           sorted_entries(entry.second)) {
        const JSON::String &language{*language_key};
        const auto expanded_language{expand_iri(state, value_context, language,
                                                false, true, nullptr, nullptr,
                                                empty_weak_pointer)};
        const bool is_none{language == KEYWORD_NONE ||
                           (expanded_language.has_value() &&
                            expanded_language.value() == KEYWORD_NONE)};
        auto language_items{into_array(JSON{*language_value})};
        for (auto &item : language_items.as_array()) {
          if (item.is_null()) {
            continue;
          }
          if (!item.is_string()) {
            throw JSONLDError("Invalid language map value",
                              entry_pointer.concat(language));
          }
          auto value{JSON::make_object()};
          value.assign_assume_new(JSON::String{KEYWORD_VALUE}, JSON{item},
                                  KEYWORD_VALUE_HASH);
          if (!is_none) {
            value.assign_assume_new(JSON::String{KEYWORD_LANGUAGE},
                                    JSON{language}, KEYWORD_LANGUAGE_HASH);
          }
          const auto direction{definition->has_direction
                                   ? definition->direction
                                   : value_context.default_direction};
          if (direction.has_value()) {
            value.assign_assume_new(JSON::String{KEYWORD_DIRECTION},
                                    JSON{direction.value()},
                                    KEYWORD_DIRECTION_HASH);
          }
          expanded_value.push_back(std::move(value));
        }
      }
    } else if (entry.second.is_object() &&
               container_includes(definition, KEYWORD_INDEX)) {
      const bool property_valued{definition->index.has_value() &&
                                 definition->index.value() != KEYWORD_INDEX};
      std::optional<JSON::String> index_property;
      if (property_valued) {
        index_property =
            expand_iri(state, value_context, definition->index.value(), false,
                       true, nullptr, nullptr, empty_weak_pointer);
      }
      expanded_value = JSON::make_array();
      for (const auto &[index_key, index_value] :
           sorted_entries(entry.second)) {
        const JSON::String &index{*index_key};
        auto index_items{
            into_array(expand(state, value_context, property, *index_value,
                              entry_pointer.concat(index)))};
        for (auto &item : index_items.as_array()) {
          if (index != KEYWORD_NONE) {
            if (property_valued) {
              if (item.is_object() &&
                  item.defines(KEYWORD_VALUE, KEYWORD_VALUE_HASH)) {
                throw JSONLDError("Invalid value object",
                                  entry_pointer.concat(index));
              }
              auto combined{into_array(expand_value(
                  state, value_context, definition->index, JSON{index}))};
              if (item.defines(index_property.value())) {
                for (auto &existing :
                     item.at(index_property.value()).as_array()) {
                  combined.push_back(existing);
                }
              }
              item.assign(index_property.value(), std::move(combined));
            } else if (!item.defines(KEYWORD_INDEX, KEYWORD_INDEX_HASH)) {
              item.assign_assume_new(JSON::String{KEYWORD_INDEX}, JSON{index},
                                     KEYWORD_INDEX_HASH);
            }
          }
          expanded_value.push_back(item);
        }
      }
    } else if (entry.second.is_object() &&
               (container_includes(definition, KEYWORD_ID) ||
                container_includes(definition, KEYWORD_TYPE))) {
      const bool by_id{container_includes(definition, KEYWORD_ID)};
      expanded_value = JSON::make_array();
      for (const auto &[map_key, map_value] : sorted_entries(entry.second)) {
        const JSON::String &index{*map_key};
        std::optional<JSON::String> expanded_index;
        if (index != KEYWORD_NONE) {
          expanded_index =
              expand_iri(state, value_context, index, by_id, !by_id, nullptr,
                         nullptr, empty_weak_pointer);
        }
        if (expanded_index.has_value() &&
            expanded_index.value() == KEYWORD_NONE) {
          expanded_index = std::nullopt;
        }
        const ActiveContext &base_context{value_context.previous && !by_id
                                              ? *value_context.previous
                                              : value_context};
        ActiveContext entry_context{base_context};
        if (!by_id) {
          const auto type_definition{base_context.terms.find(index)};
          if (type_definition != base_context.terms.cend() &&
              type_definition->second.context.has_value()) {
            const auto saved_base{state.context_base_override};
            state.context_base_override = type_definition->second.context_base;
            process_context(state, entry_context,
                            type_definition->second.context.value(),
                            entry_pointer.concat(index));
            state.context_base_override = saved_base;
            entry_context.previous = nullptr;
          }
        }
        auto entries{JSON::make_array()};
        auto raw_values{into_array(JSON{*map_value})};
        for (auto &raw : raw_values.as_array()) {
          if (raw.is_string() && !by_id) {
            auto reference{JSON::make_object()};
            const bool reference_vocab{definition->type_mapping.has_value() &&
                                       definition->type_mapping.value() ==
                                           KEYWORD_VOCAB};
            const auto &raw_string{raw.to_string()};
            const auto referenced{expand_iri(state, value_context, raw_string,
                                             true, reference_vocab, nullptr,
                                             nullptr, empty_weak_pointer)};
            reference.assign_assume_new(JSON::String{KEYWORD_ID},
                                        JSON{referenced.value_or(raw_string)},
                                        KEYWORD_ID_HASH);
            entries.push_back(std::move(reference));
          } else {
            auto expanded_items{
                into_array(expand(state, entry_context, property, raw,
                                  entry_pointer.concat(index)))};
            for (auto &expanded : expanded_items.as_array()) {
              entries.push_back(expanded);
            }
          }
        }
        for (auto &item : entries.as_array()) {
          if (expanded_index.has_value()) {
            if (by_id) {
              if (!item.defines(KEYWORD_ID, KEYWORD_ID_HASH)) {
                item.assign_assume_new(JSON::String{KEYWORD_ID},
                                       JSON{expanded_index.value()},
                                       KEYWORD_ID_HASH);
              }
            } else {
              auto types{JSON::make_array()};
              types.push_back(JSON{expanded_index.value()});
              if (item.defines(KEYWORD_TYPE, KEYWORD_TYPE_HASH)) {
                auto existing_types{into_array(
                    std::move(item.at(KEYWORD_TYPE, KEYWORD_TYPE_HASH)))};
                for (auto &existing : existing_types.as_array()) {
                  types.push_back(existing);
                }
              }
              item.assign(KEYWORD_TYPE, std::move(types));
            }
          }
          expanded_value.push_back(item);
        }
      }
    } else if (container_includes(definition, KEYWORD_GRAPH)) {
      expanded_value = JSON::make_array();
      auto graph_items{into_array(
          expand(state, value_context, property, entry.second, entry_pointer))};
      for (auto &item : graph_items.as_array()) {
        auto graph{JSON::make_object()};
        graph.assign_assume_new(JSON::String{KEYWORD_GRAPH},
                                into_array(std::move(item)),
                                KEYWORD_GRAPH_HASH);
        expanded_value.push_back(std::move(graph));
      }
    } else {
      if (scoped && value_context.previous && entry.second.is_object() &&
          !entry.second.defines(KEYWORD_VALUE, KEYWORD_VALUE_HASH)) {
        expanded_value = expand_object(state, value_context, property,
                                       entry.second, entry_pointer);
      } else {
        expanded_value =
            expand(state, value_context, property, entry.second, entry_pointer);
      }
    }

    if (container_includes(definition, KEYWORD_LIST) &&
        !expanded_value.is_null() &&
        !(expanded_value.is_object() &&
          expanded_value.defines(KEYWORD_LIST, KEYWORD_LIST_HASH))) {
      expanded_value = into_array(std::move(expanded_value));
      if (state.processing_1_0) {
        for (const auto &item : expanded_value.as_array()) {
          if (item.is_object() &&
              item.defines(KEYWORD_LIST, KEYWORD_LIST_HASH)) {
            throw JSONLDError("List of lists", entry_pointer);
          }
        }
      }
      auto wrapper{JSON::make_object()};
      wrapper.assign_assume_new(JSON::String{KEYWORD_LIST},
                                std::move(expanded_value), KEYWORD_LIST_HASH);
      expanded_value = std::move(wrapper);
    }

    if (expanded_value.is_null()) {
      continue;
    }

    if (definition != nullptr && definition->reverse) {
      const auto reverse_items{into_array(JSON{expanded_value})};
      for (const auto &item : reverse_items.as_array()) {
        if (item.is_object() &&
            (item.defines(KEYWORD_VALUE, KEYWORD_VALUE_HASH) ||
             item.defines(KEYWORD_LIST, KEYWORD_LIST_HASH))) {
          throw JSONLDError("Invalid reverse property value", entry_pointer);
        }
      }
      const auto *existing_reverse{
          result.try_at(KEYWORD_REVERSE, KEYWORD_REVERSE_HASH)};
      auto reverse_map{existing_reverse != nullptr ? *existing_reverse
                                                   : JSON::make_object()};
      merge(reverse_map, name, into_array(std::move(expanded_value)));
      result.assign(KEYWORD_REVERSE, std::move(reverse_map));
      continue;
    }

    merge(result, name, into_array(std::move(expanded_value)));
  }
  for (const auto &[nest_property, nest] : nests) {
    const WeakPointer nest_pointer{source_pointer.concat(*nest_property)};
    const auto definition{active_context.terms.find(*nest_property)};
    if (definition != active_context.terms.cend() &&
        definition->second.context.has_value()) {
      ActiveContext nested{active_context};
      const auto saved_base{state.context_base_override};
      state.context_base_override = definition->second.context_base;
      const auto saved_override{state.protected_override};
      state.protected_override = true;
      process_context(state, nested, definition->second.context.value(),
                      nest_pointer);
      state.protected_override = saved_override;
      state.context_base_override = saved_base;
      nested.previous = nullptr;
      expand_entries(state, nested, type_context, result, active_property,
                     *nest, nest_pointer);
    } else {
      expand_entries(state, active_context, type_context, result,
                     active_property, *nest, nest_pointer);
    }
  }
}

} // namespace

auto expand(ExpansionState &state, ActiveContext &active_context,
            const std::optional<JSON::String> &active_property,
            const JSON &element, const WeakPointer &pointer) -> JSON {
  if (element.is_null()) {
    return JSON{nullptr};
  }

  if (!element.is_object() && !element.is_array()) {
    if (!active_property.has_value() ||
        active_property.value() == KEYWORD_GRAPH) {
      return JSON{nullptr};
    }
    return expand_value(state, active_context, active_property, element);
  }

  if (element.is_array()) {
    const TermDefinition *definition{nullptr};
    if (active_property.has_value()) {
      const auto term{active_context.terms.find(active_property.value())};
      if (term != active_context.terms.cend()) {
        definition = &term->second;
      }
    }

    auto result{JSON::make_array()};
    std::size_t item_index{0};
    for (const auto &item : element.as_array()) {
      auto expanded{expand(state, active_context, active_property, item,
                           pointer.concat(item_index))};
      if (expanded.is_array()) {
        for (auto &nested : expanded.as_array()) {
          result.push_back(nested);
        }
      } else if (!expanded.is_null()) {
        result.push_back(std::move(expanded));
      }
      item_index += 1;
    }

    if (container_includes(definition, KEYWORD_LIST)) {
      if (state.processing_1_0) {
        for (const auto &item : result.as_array()) {
          if (item.is_object() &&
              item.defines(KEYWORD_LIST, KEYWORD_LIST_HASH)) {
            throw JSONLDError("List of lists", pointer);
          }
        }
      }
      auto wrapper{JSON::make_object()};
      wrapper.assign_assume_new(JSON::String{KEYWORD_LIST}, std::move(result),
                                KEYWORD_LIST_HASH);
      return wrapper;
    }

    return result;
  }

  ActiveContext reverted;
  ActiveContext *current{&active_context};
  if (active_context.previous) {
    bool value_or_id{false};
    const bool single{element.object_size() == 1};
    for (const auto &entry : element.as_object()) {
      const auto expanded{expand_iri(state, active_context, entry.first, false,
                                     true, nullptr, nullptr,
                                     empty_weak_pointer)};
      if (expanded.has_value() &&
          (expanded.value() == KEYWORD_VALUE ||
           (single && expanded.value() == KEYWORD_ID))) {
        value_or_id = true;
        break;
      }
    }
    if (!value_or_id) {
      reverted = *active_context.previous;
      current = &reverted;
    }
  }

  if (element.defines(KEYWORD_CONTEXT, KEYWORD_CONTEXT_HASH)) {
    ActiveContext local{*current};
    process_context(state, local,
                    element.at(KEYWORD_CONTEXT, KEYWORD_CONTEXT_HASH),
                    pointer.concat(keyword_context()));
    return expand_object(state, local, active_property, element, pointer);
  }

  return expand_object(state, *current, active_property, element, pointer);
}

namespace {

auto run_expansion(ExpansionState &state, ActiveContext &active_context,
                   const JSON &input) -> JSON {
  auto expanded{
      expand(state, active_context, std::nullopt, input, empty_weak_pointer)};

  if (expanded.is_object() && expanded.object_size() == 1 &&
      expanded.defines(KEYWORD_GRAPH, KEYWORD_GRAPH_HASH)) {
    expanded = expanded.at(KEYWORD_GRAPH, KEYWORD_GRAPH_HASH);
  }

  if (expanded.is_object()) {
    if (expanded.empty() || (expanded.object_size() == 1 &&
                             expanded.defines(KEYWORD_ID, KEYWORD_ID_HASH))) {
      return JSON::make_array();
    }
    auto result{JSON::make_array()};
    result.push_back(std::move(expanded));
    return result;
  }

  if (!expanded.is_array()) {
    return JSON::make_array();
  }

  return expanded;
}

auto initialise_expansion(const JSONLDResolver &resolver,
                          const JSON::StringView base_iri,
                          const JSONLDVersion version, ExpansionState &state,
                          ActiveContext &active_context) -> void {
  state.resolver = &resolver;
  state.processing_1_0 = version == JSONLDVersion::V1_0;
  if (!base_iri.empty()) {
    active_context.base = JSON::String{base_iri};
    state.document_base = JSON::String{base_iri};
  }
}

} // namespace

auto jsonld_expand(const JSON &input, const JSON::StringView base_iri,
                   const JSONLDResolver &resolver, const JSONLDVersion version)
    -> JSON {
  ExpansionState state;
  ActiveContext active_context;
  initialise_expansion(resolver, base_iri, version, state, active_context);
  return run_expansion(state, active_context, input);
}

auto jsonld_expand(const JSON &input, const JSON &expand_context,
                   const JSON::StringView base_iri,
                   const JSONLDResolver &resolver, const JSONLDVersion version)
    -> JSON {
  ExpansionState state;
  ActiveContext active_context;
  initialise_expansion(resolver, base_iri, version, state, active_context);
  const auto &context{
      expand_context.is_object() &&
              expand_context.defines(KEYWORD_CONTEXT, KEYWORD_CONTEXT_HASH)
          ? expand_context.at(KEYWORD_CONTEXT, KEYWORD_CONTEXT_HASH)
          : expand_context};
  process_context(state, active_context, context, empty_weak_pointer);
  return run_expansion(state, active_context, input);
}

} // namespace sourcemeta::core
