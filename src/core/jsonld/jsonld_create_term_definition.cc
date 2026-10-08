#include "jsonld_algorithms.h"
#include "jsonld_keywords.h"

#include <sourcemeta/core/uri.h>

#include <algorithm> // std::ranges::sort
#include <optional>  // std::optional
#include <utility>   // std::move

namespace sourcemeta::core {

namespace {

// Whether the given value is a valid @container value.
auto is_valid_container(const JSON::StringView value) -> bool {
  return value == KEYWORD_LIST || value == KEYWORD_SET ||
         value == KEYWORD_INDEX || value == KEYWORD_LANGUAGE ||
         value == KEYWORD_ID || value == KEYWORD_TYPE || value == KEYWORD_GRAPH;
}

// Whether the given value ends with a URI generic delimiter (RFC 3986).
auto ends_with_gen_delim(const JSON::StringView value) -> bool {
  return !value.empty() && URI::is_gen_delim(value.back());
}

// Whether two definitions are equivalent ignoring their protected status, which
// is what a protected-term redefinition check compares.
auto same_definition(const TermDefinition &left, const TermDefinition &right)
    -> bool {
  return left.iri == right.iri && left.type_mapping == right.type_mapping &&
         left.container == right.container && left.language == right.language &&
         left.has_language == right.has_language &&
         left.direction == right.direction &&
         left.has_direction == right.has_direction &&
         left.context == right.context &&
         left.context_base == right.context_base && left.index == right.index &&
         left.nest == right.nest && left.reverse == right.reverse &&
         left.prefix == right.prefix;
}

// Store a freshly-built term definition, enforcing protected-term redefinition.
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
    // An equivalent redefinition keeps the previous definition, including its
    // origin metadata, so deferred errors keep their original defining
    // location
    active_context.terms[term] = previous.value();
    defined[term] = true;
    return;
  }
  active_context.terms[term] = std::move(candidate);
  defined[term] = true;
}

} // namespace

namespace {

// Create Term Definition (JSON-LD 1.1 API Section 5.1.1)
auto create_term_definition_internal(
    ExpansionState &state, ActiveContext &active_context,
    const JSON &local_context, const JSON::String &term, DefinedTerms &defined,
    const WeakPointer &context_pointer, const WeakPointer &reference_pointer)
    -> void {
  const auto status{defined.find(term)};
  if (status != defined.cend()) {
    if (status->second) {
      return;
    }
    throw JSONLDError("Cyclic IRI mapping", reference_pointer);
  }

  if (term.empty()) {
    throw JSONLDError("Invalid term definition", context_pointer.concat(term));
  }

  defined[term] = false;
  const auto &value{local_context.at(term)};
  const WeakPointer term_pointer{context_pointer.concat(term)};
  // Owned keyword spellings so weak pointers can reference keyword entries
  static const JSON::String TOKEN_ID{KEYWORD_ID};
  static const JSON::String TOKEN_REVERSE{KEYWORD_REVERSE};
  static const JSON::String TOKEN_TYPE{KEYWORD_TYPE};
  static const JSON::String TOKEN_INDEX{KEYWORD_INDEX};
  static const JSON::String TOKEN_CONTEXT{KEYWORD_CONTEXT};
  static const JSON::String TOKEN_IMPORT{KEYWORD_IMPORT};

  if (is_keyword(term)) {
    if (term == KEYWORD_TYPE && value.is_object() && !state.processing_1_0) {
      TermDefinition type_definition;
      bool has_container{false};
      bool has_protected{false};
      bool invalid_entry{false};
      for (const auto &entry : value.as_object()) {
        const auto &name{entry.first};
        if (name == KEYWORD_PROTECTED) {
          if (!entry.second.is_boolean()) {
            throw JSONLDError("Invalid @protected value", term_pointer,
                              {KEYWORD_PROTECTED});
          }
          type_definition.is_protected = entry.second.to_boolean();
          has_protected = true;
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
      // A redefinition of a protected @type is rejected before the shape of
      // the new definition is validated.
      const auto existing_type{active_context.terms.find(KEYWORD_TYPE)};
      if (existing_type != active_context.terms.cend() &&
          existing_type->second.is_protected && !state.protected_override) {
        if (!same_definition(existing_type->second, type_definition)) {
          throw JSONLDError("Protected term redefinition", term_pointer);
        }
        type_definition.is_protected = true;
      } else if (invalid_entry || (!has_container && !has_protected)) {
        // Either or both of a set container and a protected flag are valid
        // (JSON-LD 1.1 API Section 5.1.1 step 4)
        throw JSONLDError("Keyword redefinition", term_pointer);
      } else if (!has_protected) {
        // An explicit @protected flag, including false, wins over the
        // context-wide default
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
  if (value.is_null()) {
    TermDefinition empty;
    empty.is_protected = state.context_protected;
    finalize_definition(state, active_context, defined, term, term_pointer,
                        previous, std::move(empty));
    return;
  }
  // An explicit null mapping retains the term while removing it from IRI
  // expansion, and its remaining entries are still validated (JSON-LD 1.1 API
  // Section 5.1.1 step 14.1)
  const bool explicit_null_id{id_entry != nullptr && id_entry->is_null()};

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
      // A self-referential simple term resolves through the term itself.
      const auto colon{term.find(':')};
      if (colon != JSON::String::npos) {
        const auto prefix{term.substr(0, colon)};
        const auto suffix{term.substr(colon + 1)};
        if (prefix != "_" && !suffix.starts_with("//") &&
            local_context.is_object() && local_context.defines(prefix)) {
          const auto iterator{defined.find(prefix)};
          if (iterator == defined.cend() || !iterator->second) {
            create_term_definition(state, active_context, local_context, prefix,
                                   defined, context_pointer, term_pointer);
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
                     &local_context, &defined, context_pointer, term_pointer);
      // In 1.1, an IRI-like term must expand to its IRI mapping. The check
      // passes the local context so that prefix terms it depends on are
      // created on demand regardless of creation order, and the term itself is
      // marked as defined first so the probe cannot re-enter it
      if (!state.processing_1_0 && definition.iri.has_value()) {
        const auto colon_position{term.find(':')};
        const bool iri_like_colon{colon_position != JSON::String::npos &&
                                  colon_position != 0 &&
                                  colon_position + 1 != term.size()};
        if (iri_like_colon || term.find('/') != JSON::String::npos) {
          defined[term] = true;
          const auto expanded_term{expand_iri(
              state, active_context, term, false, true, &local_context,
              &defined, context_pointer, term_pointer)};
          if (expanded_term.has_value() && expanded_term != definition.iri) {
            throw JSONLDError("Invalid IRI mapping", term_pointer);
          }
        }
      }
      // A string definition stands for a map whose @id entry has that value,
      // so it is subject to the same mapping validation as the explicit form
      if (!definition.iri.has_value() ||
          (!is_keyword(definition.iri.value()) &&
           definition.iri.value().find(':') == JSON::String::npos &&
           !active_context.vocabulary.has_value())) {
        throw JSONLDError("Invalid IRI mapping", term_pointer);
      }
      if (definition.iri.value() == KEYWORD_CONTEXT) {
        throw JSONLDError("Invalid keyword alias", term_pointer);
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
      // A keyword-shaped reverse value is ignored, including a defined
      // keyword: unlike the @id entry, the @reverse rule carries no keyword
      // exclusion (JSON-LD 1.1 API Section 5.1.1 step 13.3)
      if (has_keyword_form(reverse.to_string())) {
        defined[term] = true;
        return;
      }
      definition.reverse = true;
      definition.iri =
          expand_iri(state, active_context, reverse.to_string(), false, true,
                     &local_context, &defined, context_pointer,
                     term_pointer.concat(TOKEN_REVERSE));
      if (!definition.iri.has_value()) {
        // A reverse value with the form of a keyword is ignored.
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
                                  &local_context, &defined, context_pointer,
                                  term_pointer.concat(TOKEN_ID));
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
      // In 1.1, a term that itself has the form of an IRI (a colon other than
      // at the edges, or a slash) must expand to its IRI mapping. The check
      // passes the local context so that prefix terms it depends on are
      // created on demand regardless of creation order, and the term itself is
      // marked as defined first so the probe cannot re-enter it
      if (!state.processing_1_0 && mapping.has_value()) {
        const auto colon_position{term.find(':')};
        const bool iri_like_colon{colon_position != JSON::String::npos &&
                                  colon_position != 0 &&
                                  colon_position + 1 != term.size()};
        if (iri_like_colon || term.find('/') != JSON::String::npos) {
          defined[term] = true;
          const auto expanded_term{expand_iri(
              state, active_context, term, false, true, &local_context,
              &defined, context_pointer, term_pointer)};
          if (expanded_term.has_value() && expanded_term != mapping) {
            throw JSONLDError("Invalid IRI mapping", term_pointer,
                              {KEYWORD_ID});
          }
        }
      }
    } else if (explicit_null_id) {
      // The term keeps no IRI mapping, and no fallback derivation applies
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
                                 defined, context_pointer, term_pointer);
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
                                 context_pointer,
                                 term_pointer.concat(TOKEN_TYPE))};
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
      if (definition.reverse) {
        // A reverse term only supports a single set or index container, or an
        // explicit null (JSON-LD 1.1 API Section 5.1.1 step 13.5)
        if (!container.is_null()) {
          if (!container.is_string() ||
              (container.to_string() != KEYWORD_SET &&
               container.to_string() != KEYWORD_INDEX)) {
            throw JSONLDError("Invalid reverse property", term_pointer,
                              {KEYWORD_CONTAINER});
          }
          definition.container.push_back(container.to_string());
        }
      } else if (container.is_array()) {
        // Array containers are a 1.1 feature.
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
          // A keyword may not appear more than once in the container array.
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
        // In 1.0, the @graph, @id and @type containers are not permitted.
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
      // Valid multi-keyword combinations are order-insensitive, so the stored
      // mapping is normalised for protected-term definition comparisons
      std::ranges::sort(definition.container);
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
      // Valid array combinations (JSON-LD 1.1 API Section 5.1.1 step 19.1): a
      // single keyword, or @graph with exactly one of @id or @index optionally
      // with @set, or @set combined with any one of @index, @graph, @id,
      // @type, or @language. A reverse container is already fully validated,
      // including its explicit null form, which stores no keyword at all
      if (!definition.reverse && definition.container.size() != 1) {
        const bool graph_form{
            container_graph && (container_id != container_index) &&
            !container_list && !container_type && !container_language};
        const bool set_form{container_set && definition.container.size() == 2 &&
                            !container_list};
        if (!graph_form && !set_form) {
          throw JSONLDError("Invalid container mapping", term_pointer,
                            {KEYWORD_CONTAINER});
        }
      }
      // A type-map container defaults to identifier coercion, and may only
      // coerce its keys to identifiers (JSON-LD 1.1 API Section 5.1.1)
      if (container_type) {
        if (!definition.type_mapping.has_value()) {
          definition.type_mapping = JSON::String{KEYWORD_ID};
        } else if (definition.type_mapping.value() != KEYWORD_ID &&
                   definition.type_mapping.value() != KEYWORD_VOCAB) {
          throw JSONLDError("Invalid type mapping", term_pointer,
                            {KEYWORD_TYPE});
        }
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
      const bool imported{state.imported_keys.contains(term)};
      const bool context_remote{!state.remote_context_chain.empty() ||
                                state.remote_base_override || imported};
      // Validate the scoped context eagerly, mirroring its use-time remote
      // origin, so that errors surface even when the term is never used. Any
      // failure, including a loading one, is an invalid scoped context
      // (JSON-LD 1.1 API Section 5.1.1 step 21)
      const bool saved_override{state.protected_override};
      const bool saved_context_protected{state.context_protected};
      const bool saved_remote_base{state.remote_base_override};
      const bool saved_validate{state.validate_scoped_context};
      try {
        // The error raised here is always discarded below, so its location does
        // not matter.
        ActiveContext probe{active_context};
        state.protected_override = true;
        state.remote_base_override = context_remote;
        state.validate_scoped_context = false;
        process_context(state, probe, *context_entry, empty_weak_pointer);
        state.validate_scoped_context = saved_validate;
        state.remote_base_override = saved_remote_base;
        state.protected_override = saved_override;
        state.context_protected = saved_context_protected;
      } catch (const JSONLDError &) {
        state.validate_scoped_context = saved_validate;
        state.remote_base_override = saved_remote_base;
        state.protected_override = saved_override;
        state.context_protected = saved_context_protected;
        throw JSONLDError("Invalid scoped context", term_pointer,
                          {KEYWORD_CONTEXT});
      }
      definition.context = *context_entry;
      definition.context_base = state.context_resolution_base();
      definition.context_remote = context_remote;
      // Remote definitions report at the input reference that loaded the
      // defining context, and external expansion-context definitions report
      // at the document root, as the scoped entry itself is not in the input
      definition.context_location =
          state.external_context ? to_pointer(empty_weak_pointer)
          : imported       ? to_pointer(context_pointer.concat(TOKEN_IMPORT))
          : context_remote ? to_pointer(context_pointer)
                           : to_pointer(term_pointer.concat(TOKEN_CONTEXT));
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
      definition.nest = nest_string;
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
      const auto index_iri{expand_iri(
          state, active_context, index_string, false, true, &local_context,
          &defined, context_pointer, term_pointer.concat(TOKEN_INDEX))};
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

    // A term definition may not contain any entry other than the keywords
    // recognised above.
    for (const auto &entry : value.as_object()) {
      const JSON::StringView key{entry.first};
      if (key != KEYWORD_ID && key != KEYWORD_REVERSE &&
          key != KEYWORD_CONTAINER && key != KEYWORD_CONTEXT &&
          key != KEYWORD_DIRECTION && key != KEYWORD_INDEX &&
          key != KEYWORD_LANGUAGE && key != KEYWORD_NEST &&
          key != KEYWORD_PREFIX && key != KEYWORD_PROTECTED &&
          key != KEYWORD_TYPE) {
        throw JSONLDError("Invalid term definition", term_pointer, {key});
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

  if (!definition.reverse && !explicit_null_id && !definition.iri.has_value()) {
    throw JSONLDError("Invalid IRI mapping", term_pointer);
  }

  finalize_definition(state, active_context, defined, term, term_pointer,
                      previous, std::move(definition));
}

} // namespace

// An imported term is not present in the input document, so every error its
// definition raises, including through recursive dependency creation, reports
// at the @import entry that merged it in
auto create_term_definition(ExpansionState &state,
                            ActiveContext &active_context,
                            const JSON &local_context, const JSON::String &term,
                            DefinedTerms &defined,
                            const WeakPointer &context_pointer,
                            const WeakPointer &reference_pointer) -> void {
  if (state.imported_keys.contains(term)) {
    static const JSON::String TOKEN_IMPORT_ENTRY{KEYWORD_IMPORT};
    try {
      create_term_definition_internal(state, active_context, local_context,
                                      term, defined, context_pointer,
                                      reference_pointer);
    } catch (const JSONLDError &error) {
      throw JSONLDError(error.what(),
                        context_pointer.concat(TOKEN_IMPORT_ENTRY));
    }
    return;
  }
  create_term_definition_internal(state, active_context, local_context, term,
                                  defined, context_pointer, reference_pointer);
}

} // namespace sourcemeta::core
