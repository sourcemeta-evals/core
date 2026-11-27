#include "jsonld_algorithms.h"
#include "jsonld_keywords.h"

#include <sourcemeta/core/uri.h>

#include <cassert>  // assert
#include <optional> // std::optional

namespace sourcemeta::core {

// IRI Expansion (JSON-LD 1.1 API Section 5.2)
auto expand_iri(ExpansionState &state, ActiveContext &active_context,
                const JSON::String &val, const bool document_relative,
                const bool vocabulary, const JSON *const local_context,
                DefinedTerms *const defined, const WeakPointer &context_pointer)
    -> std::optional<JSON::String> {
  // A caller either supplies the local context together with the terms it has
  // already defined or supplies neither, and a context whose terms are being
  // defined has already been read as a map
  assert((local_context == nullptr) == (defined == nullptr));
  assert(local_context == nullptr || local_context->is_object());

  if (is_keyword(val)) {
    return val;
  }

  if (has_keyword_form(val)) {
    return std::nullopt;
  }

  if (local_context != nullptr && local_context->defines(val)) {
    const auto iterator{defined->find(val)};
    if (iterator == defined->cend() || !iterator->second) {
      create_term_definition(state, active_context, *local_context, val,
                             *defined, context_pointer);
    }
  }

  const auto term{active_context.terms.find(val)};
  if (term != active_context.terms.cend() && term->second.iri.has_value() &&
      is_keyword(term->second.iri.value())) {
    return term->second.iri;
  }

  if (vocabulary && term != active_context.terms.cend()) {
    return term->second.iri;
  }

  if (val.find(':', 1) != JSON::String::npos) {
    // The term has the form of a compact IRI, so split at the first colon.
    const auto colon{val.find(':')};
    const auto prefix{val.substr(0, colon)};
    const auto suffix{val.substr(colon + 1)};
    if (prefix == "_" || suffix.starts_with("//")) {
      return val;
    }

    if (local_context != nullptr && local_context->defines(prefix)) {
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

    // The val is only already an IRI when its prefix is a valid scheme.
    // Otherwise it is resolved against the vocabulary or document base below.
    if (URI::is_scheme(prefix)) {
      return val;
    }
  }

  if (vocabulary && active_context.vocabulary.has_value()) {
    return active_context.vocabulary.value() + val;
  }

  if (document_relative && active_context.base.has_value()) {
    // The base is read outside what follows, as a caller that supplies one the
    // parser cannot read is answered with that rather than with an identifier
    // left unresolved
    const auto base{URI::from_iri(active_context.base.value())};
    // Expansion builds identifiers without validating them, so a val the
    // parser cannot read is carried through as it stands rather than
    // surfacing as an exception of another module's kind (JSON-LD 1.1 API
    // Section 5.2 step 5)
    try {
      return URI::from_iri(val).resolve_from(base).recompose();
    } catch (const URIParseError &) {
      return val;
    }
  }

  return val;
}

} // namespace sourcemeta::core
