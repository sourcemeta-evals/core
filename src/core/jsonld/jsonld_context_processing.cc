#include "jsonld_algorithms.h"
#include "jsonld_keywords.h"

#include <sourcemeta/core/uri.h>

#include <algorithm>        // std::ranges::sort
#include <cstddef>          // std::size_t
#include <initializer_list> // std::initializer_list
#include <memory>           // std::make_shared
#include <optional>         // std::optional
#include <utility>          // std::move, std::pair
#include <vector>           // std::vector

namespace {

// The processor-defined limit on the remote context chain, which bounds
// resolver-controlled recursion over distinct context IRIs
constexpr std::size_t REMOTE_CONTEXT_LIMIT{32};

// Invoke the resolver callback, translating any failure it raises into the
// public loading error at the given input location
auto resolve_remote_document(
    const sourcemeta::core::ExpansionState &state,
    const sourcemeta::core::JSON::String &reference,
    const sourcemeta::core::WeakPointer &location,
    const std::initializer_list<sourcemeta::core::JSON::StringView> children)
    -> std::optional<sourcemeta::core::JSON> {
  try {
    return (*state.resolver)(reference);
  } catch (...) {
    // A JSONLDError raised by the callback is translated too: its code and
    // pointer concern the resolver's own domain, not the input document
    throw sourcemeta::core::JSONLDError("Loading remote context failed",
                                        location, children);
  }
}

// The input location of a context keyword entry: the keyword itself when the
// input supplied or overrode it, or the @import entry that merged it in
auto keyword_error_token(const sourcemeta::core::ExpansionState &state,
                         const sourcemeta::core::JSON::StringView keyword)
    -> sourcemeta::core::JSON::StringView {
  return state.imported_keys.contains(keyword)
             ? sourcemeta::core::KEYWORD_IMPORT
             : keyword;
}

} // namespace

namespace sourcemeta::core {

// Context Processing (JSON-LD 1.1 API Section 5.1)
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

  // The @propagate flag is read once from a top-level map context, before the
  // contexts are processed (JSON-LD 1.1 API Section 5.1 steps 2 and 3). A
  // non-boolean value is reported per entry by the loop below.
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
      // Nullifying the context resets to the initial context, whose base is
      // the document base. A non-propagated nullification keeps the saved
      // context so that nested nodes can revert to it (JSON-LD 1.1 API
      // Section 5.1 step 5.1.2)
      ActiveContext fresh;
      fresh.base = state.document_base;
      if (!effective_propagate) {
        fresh.previous = active_context.previous;
      }
      active_context = std::move(fresh);
      continue;
    }

    if (context.is_string()) {
      auto reference{context.to_string()};
      const auto resolution_base{state.context_resolution_base()};
      // A reference that cannot be parsed, resolved, or made absolute cannot
      // name a document at all (JSON-LD 1.1 API Section 5.1 step 5.2.1), and
      // the parser's own exception never escapes the public error contract
      bool resolvable{true};
      try {
        if (resolution_base.has_value()) {
          reference = URI::from_iri(reference)
                          .resolve_from(URI::from_iri(resolution_base.value()))
                          .recompose();
        }
        resolvable = URI::from_iri(reference).is_absolute();
      } catch (...) {
        resolvable = false;
      }
      if (!resolvable) {
        throw JSONLDError("Loading document failed", location);
      }
      bool already_loaded{false};
      for (const auto &loaded : state.remote_context_chain) {
        if (loaded == reference) {
          already_loaded = true;
          break;
        }
      }
      if (already_loaded) {
        // When validating a scoped context at definition time, a reference
        // already in the active chain is skipped rather than reprocessed, so
        // legitimately recursive scoped contexts stay valid
        if (!state.validate_scoped_context) {
          continue;
        }
        // JSON-LD 1.1 replaced the recursive context inclusion error with
        // context overflow, with re-inclusion in the active chain acting as
        // the processor-defined limit
        if (state.processing_1_0) {
          throw JSONLDError("Recursive context inclusion", location);
        }
        throw JSONLDError("Context overflow", location);
      }
      // A previously dereferenced context is never dereferenced again within
      // one expansion, and only its @context entry is retained (JSON-LD 1.1
      // API Section 5.1 step 5.2.4)
      const auto cached{state.remote_documents.find(reference)};
      const JSON *loaded_context{nullptr};
      if (cached != state.remote_documents.cend()) {
        loaded_context = &cached->second;
      } else {
        if (state.resolver == nullptr || !*state.resolver) {
          throw JSONLDError("Loading remote context failed", location);
        }
        const auto document{
            resolve_remote_document(state, reference, location, {})};
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
        loaded_context =
            &state.remote_documents.emplace(reference, *context_entry)
                 .first->second;
      }
      if (state.remote_context_chain.size() >= REMOTE_CONTEXT_LIMIT) {
        throw JSONLDError("Context overflow", location);
      }
      state.remote_context_chain.push_back(reference);
      try {
        // A loaded remote context is processed with the default propagation.
        process_context(state, active_context, *loaded_context, location);
      } catch (const JSONLDError &error) {
        state.remote_context_chain.pop_back();
        // The offending entries live in the remote document, so the error is
        // reported at the input location of the reference that loaded it
        throw JSONLDError(error.what(), location);
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

    // The version number 1.1 is accepted in both numeric representations the
    // JSON library supports
    if (const auto *version{
            context.try_at(KEYWORD_VERSION, KEYWORD_VERSION_HASH)};
        version != nullptr &&
        !((version->is_real() && version->to_real() == 1.1) ||
          (version->is_decimal() && version->to_decimal() == Decimal{"1.1"}))) {
      throw JSONLDError("Invalid @version value", location,
                        {keyword_error_token(state, KEYWORD_VERSION)});
    }
    if (state.processing_1_0 &&
        context.defines(KEYWORD_VERSION, KEYWORD_VERSION_HASH)) {
      throw JSONLDError("Processing mode conflict", location,
                        {KEYWORD_VERSION});
    }
    if (state.processing_1_0) {
      if (context.defines(KEYWORD_DIRECTION, KEYWORD_DIRECTION_HASH)) {
        throw JSONLDError("Invalid context entry", location,
                          {KEYWORD_DIRECTION});
      }
      if (context.defines(KEYWORD_PROPAGATE, KEYWORD_PROPAGATE_HASH)) {
        throw JSONLDError("Invalid context entry", location,
                          {KEYWORD_PROPAGATE});
      }
      if (context.defines(KEYWORD_IMPORT, KEYWORD_IMPORT_HASH)) {
        throw JSONLDError("Invalid context entry", location, {KEYWORD_IMPORT});
      }
      if (context.defines(KEYWORD_PROTECTED, KEYWORD_PROTECTED_HASH)) {
        throw JSONLDError("Invalid context entry", location,
                          {KEYWORD_PROTECTED});
      }
    }

    if (const auto *propagate_entry{
            context.try_at(KEYWORD_PROPAGATE, KEYWORD_PROPAGATE_HASH)};
        propagate_entry != nullptr && !propagate_entry->is_boolean()) {
      throw JSONLDError("Invalid @propagate value", location,
                        {keyword_error_token(state, KEYWORD_PROPAGATE)});
    }

    // @protected applies to imported terms too, so it is set before @import.
    const bool saved_protected{state.context_protected};
    if (const auto *protected_entry{
            context.try_at(KEYWORD_PROTECTED, KEYWORD_PROTECTED_HASH)}) {
      if (!protected_entry->is_boolean()) {
        throw JSONLDError("Invalid @protected value", location,
                          {keyword_error_token(state, KEYWORD_PROTECTED)});
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
      // The resolver contract only admits absolute IRIs, so a reference that
      // cannot be parsed, resolved, or made absolute is a loading failure
      bool resolvable{true};
      try {
        if (resolution_base.has_value()) {
          reference = URI::from_iri(reference)
                          .resolve_from(URI::from_iri(resolution_base.value()))
                          .recompose();
        }
        resolvable = URI::from_iri(reference).is_absolute();
      } catch (...) {
        resolvable = false;
      }
      if (!resolvable) {
        throw JSONLDError("Loading remote context failed", location,
                          {KEYWORD_IMPORT});
      }
      // An import shares the per-expansion remote-document cache with direct
      // context references, so one absolute IRI is dereferenced at most once
      const auto cached{state.remote_documents.find(reference)};
      const JSON *imported_context{nullptr};
      if (cached != state.remote_documents.cend()) {
        imported_context = &cached->second;
      } else {
        if (state.resolver == nullptr || !*state.resolver) {
          throw JSONLDError("Loading remote context failed", location,
                            {KEYWORD_IMPORT});
        }
        const auto document{resolve_remote_document(state, reference, location,
                                                    {KEYWORD_IMPORT})};
        if (!document.has_value()) {
          throw JSONLDError("Loading remote context failed", location,
                            {KEYWORD_IMPORT});
        }
        const auto *context_entry{
            document->is_object()
                ? document->try_at(KEYWORD_CONTEXT, KEYWORD_CONTEXT_HASH)
                : nullptr};
        if (context_entry == nullptr) {
          throw JSONLDError("Invalid remote context", location,
                            {KEYWORD_IMPORT});
        }
        imported_context =
            &state.remote_documents.emplace(reference, *context_entry)
                 .first->second;
      }
      if (!imported_context->is_object()) {
        throw JSONLDError("Invalid remote context", location, {KEYWORD_IMPORT});
      }
      if (imported_context->defines(KEYWORD_IMPORT, KEYWORD_IMPORT_HASH)) {
        throw JSONLDError("Invalid context entry", location, {KEYWORD_IMPORT});
      }
      // Merge the imported entries with the current ones, the current ones
      // overriding, and process the result as a single context.
      auto merged{JSON{*imported_context}};
      // A @base defined inside a remotely-loaded document is ignored, and the
      // merged context is processed outside the remote chain, so the imported
      // entry is dropped before merging
      merged.erase(KEYWORD_BASE);
      // The keys that the import contributes carry remote origin and are not
      // present in the input document, unlike the local ones overriding them
      auto saved_imported{std::move(state.imported_keys)};
      state.imported_keys.clear();
      for (const auto &imported_entry : merged.as_object()) {
        if (!context.defines(imported_entry.first)) {
          state.imported_keys.insert(imported_entry.first);
        }
      }
      for (const auto &entry : context.as_object()) {
        if (JSON::StringView{entry.first} != KEYWORD_IMPORT) {
          merged.assign(entry.first, entry.second);
        }
      }
      try {
        process_context(state, active_context, merged, location, propagate);
      } catch (...) {
        state.imported_keys = std::move(saved_imported);
        throw;
      }
      state.imported_keys = std::move(saved_imported);
      state.context_protected = saved_protected;
      continue;
    }

    if (const auto *base_entry{
            state.remote_context_chain.empty() && !state.remote_base_override
                ? context.try_at(KEYWORD_BASE, KEYWORD_BASE_HASH)
                : nullptr}) {
      const auto &base{*base_entry};
      if (base.is_null()) {
        active_context.base = std::nullopt;
      } else if (!base.is_string()) {
        throw JSONLDError("Invalid base IRI", location, {KEYWORD_BASE});
      } else {
        const auto &base_string{base.to_string()};
        // A string the URI parser rejects is an invalid base, not a parser
        // exception escaping the public error contract
        try {
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
        } catch (const JSONLDError &) {
          throw;
        } catch (...) {
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
        throw JSONLDError("Invalid vocab mapping", location,
                          {keyword_error_token(state, KEYWORD_VOCAB)});
      } else {
        const auto &vocabulary_string{vocabulary.to_string()};
        // In 1.0, @vocab must be an absolute IRI or blank node identifier.
        if (state.processing_1_0 &&
            vocabulary_string.find(':') == JSON::String::npos) {
          throw JSONLDError("Invalid vocab mapping", location,
                            {keyword_error_token(state, KEYWORD_VOCAB)});
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
                          {keyword_error_token(state, KEYWORD_LANGUAGE)});
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
                          {keyword_error_token(state, KEYWORD_DIRECTION)});
      } else {
        const auto &direction_string{direction.to_string()};
        if (direction_string != "ltr" && direction_string != "rtl") {
          throw JSONLDError("Invalid base direction", location,
                            {keyword_error_token(state, KEYWORD_DIRECTION)});
        }
        active_context.default_direction = direction_string;
      }
    }

    DefinedTerms defined;
    // Term definitions are created in lexicographical key order, so the first
    // error among several independently invalid terms is deterministic
    std::vector<const JSON::String *> term_names;
    for (const auto &entry : context.as_object()) {
      const auto &name{entry.first};
      if (name == KEYWORD_BASE || name == KEYWORD_VOCAB ||
          name == KEYWORD_LANGUAGE || name == KEYWORD_VERSION ||
          name == KEYWORD_DIRECTION || name == KEYWORD_IMPORT ||
          name == KEYWORD_PROPAGATE || name == KEYWORD_PROTECTED) {
        continue;
      }
      term_names.push_back(&name);
    }
    std::ranges::sort(term_names,
                      [](const auto *left, const auto *right) -> bool {
                        return *left < *right;
                      });
    for (const auto *name_pointer : term_names) {
      const auto &name{*name_pointer};
      create_term_definition(state, active_context, context, name, defined,
                             location, location.concat(name));
    }

    state.context_protected = saved_protected;
  }
}

} // namespace sourcemeta::core
