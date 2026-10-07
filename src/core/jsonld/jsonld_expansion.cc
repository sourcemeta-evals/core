#include "jsonld_algorithms.h"
#include "jsonld_keywords.h"

#include <sourcemeta/core/uri.h>

#include <algorithm> // std::ranges::sort
#include <cstddef>   // std::size_t
#include <optional>  // std::optional
#include <utility>   // std::move, std::pair
#include <vector>    // std::vector

namespace sourcemeta::core {

namespace {

auto into_array(JSON &&value) -> JSON {
  if (value.is_array()) {
    return std::move(value);
  }
  auto result{JSON::make_array()};
  result.push_back(std::move(value));
  return result;
}

// The entries of an object in sorted key order, which is the order expansion
// uses so that values merged from several keys are deterministic. The keys and
// values are referenced from the object (which must outlive the result), never
// copied.
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

// Append the values, which must be an array, into the array stored at the given
// key, creating it if absent.
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

// Expand a single @type value against the context that preceded type-scoped
// processing.
auto expand_type(ExpansionState &state, const ActiveContext &type_context,
                 const JSON &item) -> JSON {
  auto context{type_context};
  const auto type{expand_iri(state, context, item.to_string(), true, true,
                             nullptr, nullptr, empty_weak_pointer)};
  return type.has_value() ? JSON{type.value()} : JSON{nullptr};
}

// The input-spelled locations of the members that expanded to @value and
// @type, so the value-object post-processing checks can point at the
// offending key rather than at the enclosing element.
struct ValueMemberPointers {
  std::optional<WeakPointer> value;
  std::optional<WeakPointer> type;
};

// Process a deferred scoped context, carrying the definition's remote origin.
// A locally defined scoped context lives in the input, so it is processed
// against its definition location and errors keep their specific pointers,
// while remote, imported, and external origins report at the stored defining
// reference
auto process_deferred_scoped_context(ExpansionState &state,
                                     ActiveContext &context,
                                     const TermDefinition &definition,
                                     const bool propagate) -> void {
  const auto saved_base{state.context_base_override};
  const auto saved_remote{state.remote_base_override};
  state.context_base_override = definition.context_base;
  state.remote_base_override = definition.context_remote;
  const bool local{!definition.context_remote &&
                   !definition.context_location.empty()};
  const auto location{to_weak_pointer(definition.context_location)};
  try {
    process_context(state, context, definition.context.value(), location,
                    propagate);
  } catch (const JSONLDError &error) {
    state.remote_base_override = saved_remote;
    state.context_base_override = saved_base;
    if (local) {
      throw;
    }
    throw JSONLDError(error.what(), definition.context_location);
  }
  state.remote_base_override = saved_remote;
  state.context_base_override = saved_base;
}

// Apply a property-scoped context to the given context copy, overriding
// protected terms as the specification requires
auto apply_scoped_context(ExpansionState &state, ActiveContext &context,
                          const TermDefinition &definition) -> void {
  const auto saved_override{state.protected_override};
  state.protected_override = true;
  try {
    process_deferred_scoped_context(state, context, definition, true);
  } catch (...) {
    state.protected_override = saved_override;
    throw;
  }
  state.protected_override = saved_override;
}

// Whether any key of the given nested object expands to @value, which the
// @nest validation forbids, including through keyword aliases
auto nest_defines_value(ExpansionState &state, ActiveContext &active_context,
                        const JSON &nest_value) -> bool {
  for (const auto &nested_entry : nest_value.as_object()) {
    const auto expanded{expand_iri(state, active_context, nested_entry.first,
                                   false, true, nullptr, nullptr,
                                   empty_weak_pointer)};
    if (expanded.has_value() && expanded.value() == KEYWORD_VALUE) {
      return true;
    }
  }
  return false;
}

// Expand the direct (and deferred @nest) entries of a map into the result,
// mutating it in place. Mutually recursive with expand_object.
auto expand_entries(ExpansionState &state, ActiveContext &active_context,
                    const ActiveContext &type_context, JSON &result,
                    ValueMemberPointers &value_members,
                    const std::optional<JSON::String> &active_property,
                    const JSON &source, const WeakPointer &source_pointer)
    -> void;

// Expand a map element: the node-object (and value-object) branch of the
// Expansion algorithm, factored out of expand() below.
auto expand_object(ExpansionState &state, ActiveContext active_context,
                   const std::optional<JSON::String> &active_property,
                   const JSON &element, const WeakPointer &pointer) -> JSON {
  auto result{JSON::make_object()};

  // @type values are expanded against the context before type-scoped contexts
  // are applied.
  const ActiveContext type_context{active_context};

  // Type-scoped contexts (JSON-LD 1.1 API Section 5.1.2 step 11): the
  // type-bearing entries are visited in lexicographical key order and the
  // values are sorted within each entry, so scoped-context precedence follows
  // the input keys. The values are referenced from the input element, never
  // copied.
  std::vector<std::pair<const JSON::String *, std::vector<JSON::StringView>>>
      type_entries;
  for (const auto &entry : element.as_object()) {
    const auto expanded{expand_iri(state, active_context, entry.first, false,
                                   true, nullptr, nullptr, empty_weak_pointer)};
    if (!expanded.has_value() || expanded.value() != KEYWORD_TYPE) {
      continue;
    }
    std::vector<JSON::StringView> values;
    if (entry.second.is_array()) {
      for (const auto &item : entry.second.as_array()) {
        if (item.is_string()) {
          values.push_back(item.to_string());
        }
      }
    } else if (entry.second.is_string()) {
      values.push_back(entry.second.to_string());
    }
    type_entries.emplace_back(&entry.first, std::move(values));
  }
  std::ranges::sort(type_entries,
                    [](const auto &left, const auto &right) -> bool {
                      return *left.first < *right.first;
                    });
  std::vector<JSON::StringView> type_values;
  for (auto &type_entry : type_entries) {
    std::ranges::sort(type_entry.second);
    for (const auto &value : type_entry.second) {
      type_values.push_back(value);
    }
  }
  for (const auto &type : type_values) {
    // Each type-scoped context is resolved against the context that preceded
    // type-scoped processing, so one type's context cannot hide another's.
    const auto definition{type_context.terms.find(type)};
    if (definition != type_context.terms.cend() &&
        definition->second.context.has_value()) {
      process_deferred_scoped_context(state, active_context, definition->second,
                                      false);
    }
  }

  ValueMemberPointers value_members;
  expand_entries(state, active_context, type_context, result, value_members,
                 active_property, element, pointer);

  // An empty reverse map carries no information.
  if (const auto *reverse{result.try_at(KEYWORD_REVERSE, KEYWORD_REVERSE_HASH)};
      reverse != nullptr && reverse->empty()) {
    result.erase(KEYWORD_REVERSE);
  }

  // Post-processing (JSON-LD 1.1 API Section 5.1.2)
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
      throw JSONLDError("Invalid language-tagged value",
                        value_members.value.has_value()
                            ? value_members.value.value()
                            : pointer);
    }
    // The datatype of a typed value must be an absolute IRI, except for the
    // @json datatype
    bool type_is_absolute{true};
    if (has_type && type_string != nullptr && !is_json) {
      try {
        type_is_absolute = URI::from_iri(*type_string).is_absolute();
      } catch (...) {
        type_is_absolute = false;
      }
    }
    if (has_type &&
        (type_string == nullptr || type_string->starts_with("_:") ||
         type_string->find(' ') != JSON::String::npos || !type_is_absolute)) {
      throw JSONLDError("Invalid typed value", value_members.type.has_value()
                                                   ? value_members.type.value()
                                                   : pointer);
    }
    if (!is_json && !content.is_string() && !content.is_number() &&
        !content.is_boolean()) {
      throw JSONLDError("Invalid value object value",
                        value_members.value.has_value()
                            ? value_members.value.value()
                            : pointer);
    }
  } else if (const auto *type_entry{
                 result.try_at(KEYWORD_TYPE, KEYWORD_TYPE_HASH)};
             type_entry != nullptr && !type_entry->is_array()) {
    // Node objects always carry @type as an array.
    result.assign(KEYWORD_TYPE, into_array(JSON{*type_entry}));
  }

  // A set or list object may only carry an @index entry besides, and this is
  // validated before any value is dropped. Carrying both collection keywords
  // at once is equally invalid.
  if (result.defines(KEYWORD_LIST, KEYWORD_LIST_HASH) ||
      result.defines(KEYWORD_SET, KEYWORD_SET_HASH)) {
    if (result.defines(KEYWORD_LIST, KEYWORD_LIST_HASH) &&
        result.defines(KEYWORD_SET, KEYWORD_SET_HASH)) {
      throw JSONLDError("Invalid set or list object", pointer);
    }
    for (const auto &entry : result.as_object()) {
      const auto &name{entry.first};
      if (name != KEYWORD_LIST && name != KEYWORD_SET &&
          name != KEYWORD_INDEX) {
        throw JSONLDError("Invalid set or list object", pointer);
      }
    }
  }

  // A bare @set collapses to its array.
  if (const auto *set{result.try_at(KEYWORD_SET, KEYWORD_SET_HASH)}) {
    return *set;
  }

  // Drop an incomplete value object that has a language or direction but no
  // value.
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

  // Drop free-floating values when not under a property.
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
                    ValueMemberPointers &value_members,
                    const std::optional<JSON::String> &active_property,
                    const JSON &source, const WeakPointer &source_pointer)
    -> void {
  // @nest entries are deferred and processed after the direct ones. The
  // property is referenced from the source object, never copied. The index
  // locates the nested object within an array-valued @nest entry so errors
  // inside it identify the exact input element.
  struct NestEntry {
    const JSON::String *property;
    const JSON *value;
    std::optional<std::size_t> index;
  };
  std::vector<NestEntry> nests;
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
      // A keyword inside a reverse property map is invalid before any
      // deferral (JSON-LD 1.1 API Section 5.1.2 step 13.4.1)
      if (active_property.has_value() &&
          active_property.value() == KEYWORD_REVERSE) {
        throw JSONLDError("Invalid reverse property map", entry_pointer);
      }
      if (entry.second.is_array()) {
        std::size_t nest_index{0};
        for (const auto &nest_value : entry.second.as_array()) {
          if (!nest_value.is_object() ||
              nest_defines_value(state, active_context, nest_value)) {
            throw JSONLDError("Invalid @nest value", entry_pointer);
          }
          nests.emplace_back(&property, &nest_value, nest_index);
          nest_index += 1;
        }
      } else if (entry.second.is_object() &&
                 !nest_defines_value(state, active_context, entry.second)) {
        nests.emplace_back(&property, &entry.second, std::nullopt);
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

    // The @type and @included exemption from colliding keywords does not apply
    // in json-ld-1.0.
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
      value_members.type = entry_pointer;
      if (entry.second.is_array()) {
        for (const auto &item : entry.second.as_array()) {
          if (!item.is_string()) {
            throw JSONLDError("Invalid type value", entry_pointer);
          }
        }
      } else if (!entry.second.is_string()) {
        throw JSONLDError("Invalid type value", entry_pointer);
      }
      // Expand each value, preserving whether the input was a string or an
      // array. The node-object post-processing later turns a lone string into
      // an array, but a value object keeps its @type as a string.
      JSON expanded_type{nullptr};
      if (entry.second.is_array()) {
        expanded_type = JSON::make_array();
        for (const auto &item : entry.second.as_array()) {
          // A value that does not expand to an IRI is omitted, so @type stays
          // an array of strings.
          auto type{expand_type(state, type_context, item)};
          if (!type.is_null()) {
            expanded_type.push_back(std::move(type));
          }
        }
      } else {
        expanded_type = expand_type(state, type_context, entry.second);
      }
      // A lone @type value that does not expand to an IRI carries nothing.
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
      value_members.value = entry_pointer;
      if (state.processing_1_0) {
        // In 1.0, a value whose input type is @json is rejected while the
        // @value entry is processed, before any null preservation or datatype
        // validation (JSON-LD 1.1 API Section 5.1.2 step 13.4.7)
        for (const auto &sibling : source.as_object()) {
          const auto sibling_property{
              expand_iri(state, active_context, sibling.first, false, true,
                         nullptr, nullptr, empty_weak_pointer)};
          if (!sibling_property.has_value() ||
              sibling_property.value() != KEYWORD_TYPE) {
            continue;
          }
          const JSON *last_type{nullptr};
          if (sibling.second.is_array()) {
            for (const auto &item : sibling.second.as_array()) {
              last_type = &item;
            }
          } else {
            last_type = &sibling.second;
          }
          if (last_type != nullptr && last_type->is_string()) {
            const auto input_type{
                expand_iri(state, active_context, last_type->to_string(), false,
                           true, nullptr, nullptr, empty_weak_pointer)};
            if (input_type.has_value() && input_type.value() == KEYWORD_JSON) {
              throw JSONLDError("Invalid value object value", entry_pointer);
            }
          }
          break;
        }
      }
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
      // A free-floating list is removed before its items are expanded
      // (JSON-LD 1.1 API Section 5.1.2), so errors inside it never surface
      if (name == KEYWORD_LIST && (!active_property.has_value() ||
                                   active_property.value() == KEYWORD_GRAPH)) {
        continue;
      }

      // A null @set value keeps its null expansion, so the bare-set collapse
      // turns the whole element into null and the property disappears
      if (name == KEYWORD_SET && entry.second.is_null()) {
        result.assign(name, JSON{nullptr});
        continue;
      }

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
      // @graph expands to an array of node objects, so a value that expands to
      // null contributes no element rather than a null one.
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
      // Included content is expanded under the @included active property so
      // identifier-only node references survive free-floating cleanup, while
      // invalid members still fail the node-object validation required by the
      // official suite
      auto included{into_array(expand(state, active_context,
                                      JSON::String{KEYWORD_INCLUDED},
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
            throw JSONLDError("Invalid reverse property map", entry_pointer,
                              {reverse_property});
          } else {
            const auto reverse_values{into_array(JSON{reverse_entry.second})};
            for (const auto &item : reverse_values.as_array()) {
              if (item.is_object() &&
                  (item.defines(KEYWORD_VALUE, KEYWORD_VALUE_HASH) ||
                   item.defines(KEYWORD_LIST, KEYWORD_LIST_HASH))) {
                // The expanded map merges aliases, including ones the reverse
                // map defines through its own local context, so the offending
                // input entry is located by re-expanding each input entry
                // against the context effective inside the map, preserving the
                // input's own spelling
                ActiveContext reverse_context{active_context};
                if (entry.second.defines(KEYWORD_CONTEXT,
                                         KEYWORD_CONTEXT_HASH)) {
                  process_context(
                      state, reverse_context,
                      entry.second.at(KEYWORD_CONTEXT, KEYWORD_CONTEXT_HASH),
                      entry_pointer.concat(keyword_context()));
                }
                for (const auto &input_entry : entry.second.as_object()) {
                  const auto input_property{expand_iri(
                      state, reverse_context, input_entry.first, false, true,
                      nullptr, nullptr, empty_weak_pointer)};
                  if (!input_property.has_value() ||
                      input_property.value() != reverse_property) {
                    continue;
                  }
                  const WeakPointer input_pointer{
                      entry_pointer.concat(input_entry.first)};
                  const auto probe{into_array(
                      expand(state, reverse_context, input_entry.first,
                             input_entry.second, input_pointer))};
                  for (const auto &probe_item : probe.as_array()) {
                    if (probe_item.is_object() &&
                        (probe_item.defines(KEYWORD_VALUE,
                                            KEYWORD_VALUE_HASH) ||
                         probe_item.defines(KEYWORD_LIST, KEYWORD_LIST_HASH))) {
                      throw JSONLDError("Invalid reverse property value",
                                        input_pointer);
                    }
                  }
                }
                throw JSONLDError("Invalid reverse property value",
                                  entry_pointer, {reverse_property});
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
      // Keywords with no property-level handler (such as @vocab or @none
      // reached through an alias) carry no expanded value, so the Expansion
      // algorithm adds nothing to the result for them.
      continue;
    }

    const TermDefinition *definition{nullptr};
    const auto term{active_context.terms.find(property)};
    if (term != active_context.terms.cend()) {
      definition = &term->second;
    }

    JSON expanded_value{nullptr};
    if (definition != nullptr && definition->type_mapping.has_value() &&
        definition->type_mapping.value() == KEYWORD_JSON) {
      // A term coerced to @json keeps its value verbatim.
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
            expand_iri(state, active_context, definition->index.value(), false,
                       true, nullptr, nullptr, empty_weak_pointer);
      }
      expanded_value = JSON::make_array();
      for (const auto &[graph_key, graph_value] :
           sorted_entries(entry.second)) {
        const JSON::String &index{*graph_key};
        const auto expanded_key{index == KEYWORD_NONE
                                    ? std::optional<JSON::String>{KEYWORD_NONE}
                                    : expand_iri(state, active_context, index,
                                                 true, false, nullptr, nullptr,
                                                 empty_weak_pointer)};
        const bool none_key{expanded_key.has_value() &&
                            expanded_key.value() == KEYWORD_NONE};
        auto graph_items{
            into_array(expand(state, active_context, property, *graph_value,
                              entry_pointer.concat(index)))};
        for (auto &item : graph_items.as_array()) {
          // A null map value expands to nothing
          if (item.is_null()) {
            continue;
          }
          // Wrap the item in a graph object, unless it is already one.
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
                  state, active_context, definition->index, JSON{index}))};
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
      // The property's scoped context supplies the direction default for the
      // language map values (a disclosed task behaviour), while the keys keep
      // the element-level context
      auto direction_default{active_context.default_direction};
      if (definition->context.has_value()) {
        ActiveContext scoped{active_context};
        scoped.previous = nullptr;
        apply_scoped_context(state, scoped, *definition);
        direction_default = scoped.default_direction;
      }
      expanded_value = JSON::make_array();
      for (const auto &[language_key, language_value] :
           sorted_entries(entry.second)) {
        const JSON::String &language{*language_key};
        const auto expanded_language{expand_iri(state, active_context, language,
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
          const auto direction{definition->has_direction ? definition->direction
                                                         : direction_default};
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
            expand_iri(state, active_context, definition->index.value(), false,
                       true, nullptr, nullptr, empty_weak_pointer);
      }
      expanded_value = JSON::make_array();
      for (const auto &[index_key, index_value] :
           sorted_entries(entry.second)) {
        const JSON::String &index{*index_key};
        // The key may be an alias of @none, which suppresses index metadata
        // like the literal keyword (JSON-LD 1.1 API Section 5.1.2)
        const auto expanded_index{expand_iri(state, active_context, index,
                                             false, true, nullptr, nullptr,
                                             empty_weak_pointer)};
        const bool index_is_none{index == KEYWORD_NONE ||
                                 (expanded_index.has_value() &&
                                  expanded_index.value() == KEYWORD_NONE)};
        auto index_items{
            into_array(expand(state, active_context, property, *index_value,
                              entry_pointer.concat(index)))};
        for (auto &item : index_items.as_array()) {
          // A null map value expands to nothing
          if (item.is_null()) {
            continue;
          }
          if (!index_is_none) {
            if (property_valued) {
              if (item.is_object() &&
                  item.defines(KEYWORD_VALUE, KEYWORD_VALUE_HASH)) {
                throw JSONLDError("Invalid value object",
                                  entry_pointer.concat(index));
              }
              // The index value is prepended to any existing values.
              auto combined{into_array(expand_value(
                  state, active_context, definition->index, JSON{index}))};
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
              expand_iri(state, active_context, index, by_id, !by_id, nullptr,
                         nullptr, empty_weak_pointer);
        }
        // The key may be an alias of @none, which carries no identifier.
        if (expanded_index.has_value() &&
            expanded_index.value() == KEYWORD_NONE) {
          expanded_index = std::nullopt;
        }
        // A type map key may carry a type-scoped context for its values.
        // Type-scoped contexts do not propagate, so the values are resolved
        // against the context that preceded the containing type-scoped
        // context, with only this key's context layered on top.
        const ActiveContext &base_context{active_context.previous && !by_id
                                              ? *active_context.previous
                                              : active_context};
        ActiveContext entry_context{base_context};
        if (!by_id) {
          // Resolve the type term against the context the copy was made from,
          // which outlives the copy being mutated below.
          const auto type_definition{base_context.terms.find(index)};
          if (type_definition != base_context.terms.cend() &&
              type_definition->second.context.has_value()) {
            process_deferred_scoped_context(state, entry_context,
                                            type_definition->second, true);
            entry_context.previous = nullptr;
          }
        }
        // String values in a type map are node references.
        auto entries{JSON::make_array()};
        const bool value_array{map_value->is_array()};
        auto raw_values{into_array(JSON{*map_value})};
        std::size_t raw_index{0};
        for (auto &raw : raw_values.as_array()) {
          // An array-valued map entry carries the item position so errors
          // identify the exact input element
          const WeakPointer map_value_pointer{entry_pointer.concat(index)};
          const WeakPointer raw_pointer{
              value_array ? map_value_pointer.concat(raw_index)
                          : map_value_pointer};
          if (raw.is_string() && !by_id) {
            auto reference{JSON::make_object()};
            const bool reference_vocab{definition->type_mapping.has_value() &&
                                       definition->type_mapping.value() ==
                                           KEYWORD_VOCAB};
            const auto &raw_string{raw.to_string()};
            const auto referenced{expand_iri(state, active_context, raw_string,
                                             true, reference_vocab, nullptr,
                                             nullptr, empty_weak_pointer)};
            reference.assign_assume_new(JSON::String{KEYWORD_ID},
                                        JSON{referenced.value_or(raw_string)},
                                        KEYWORD_ID_HASH);
            entries.push_back(std::move(reference));
          } else {
            auto expanded_items{into_array(
                expand(state, entry_context, property, raw, raw_pointer))};
            for (auto &expanded : expanded_items.as_array()) {
              // A null map value expands to nothing
              if (expanded.is_null()) {
                continue;
              }
              entries.push_back(expanded);
            }
          }
          raw_index += 1;
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
      auto graph_items{into_array(expand(state, active_context, property,
                                         entry.second, entry_pointer))};
      for (auto &item : graph_items.as_array()) {
        // A null expansion result contributes no graph entry
        if (item.is_null()) {
          continue;
        }
        auto graph{JSON::make_object()};
        graph.assign_assume_new(JSON::String{KEYWORD_GRAPH},
                                into_array(std::move(item)),
                                KEYWORD_GRAPH_HASH);
        expanded_value.push_back(std::move(graph));
      }
    } else {
      expanded_value =
          expand(state, active_context, property, entry.second, entry_pointer);
    }

    // A @list container wraps the expanded value, including a @json-coerced
    // one.
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
  for (const auto &[nest_property, nest, nest_index] : nests) {
    // A @nest alias term may carry a property-scoped context for the nested
    // entries.
    const WeakPointer nest_property_pointer{
        source_pointer.concat(*nest_property)};
    const WeakPointer nest_pointer{
        nest_index.has_value()
            ? nest_property_pointer.concat(nest_index.value())
            : nest_property_pointer};
    const auto definition{active_context.terms.find(*nest_property)};
    if (definition != active_context.terms.cend() &&
        definition->second.context.has_value()) {
      // Process the scoped context into a copy so the term that owns it is not
      // freed while it is being read.
      ActiveContext nested{active_context};
      apply_scoped_context(state, nested, definition->second);
      nested.previous = nullptr;
      expand_entries(state, nested, type_context, result, value_members,
                     active_property, *nest, nest_pointer);
    } else {
      expand_entries(state, active_context, type_context, result, value_members,
                     active_property, *nest, nest_pointer);
    }
  }
}

} // namespace

// Expansion (JSON-LD 1.1 API Section 5.1.2)
auto expand(ExpansionState &state, ActiveContext &active_context,
            const std::optional<JSON::String> &active_property,
            const JSON &element, const WeakPointer &pointer) -> JSON {
  if (element.is_null()) {
    return JSON{nullptr};
  }

  // Property-scoped context (JSON-LD 1.1 API Section 5.1.2 steps 3, 4.2 and
  // 8): the active property's term definition may carry a local context that
  // applies to each of its values
  const TermDefinition *scoped_definition{nullptr};
  if (active_property.has_value()) {
    const auto scoped_term{active_context.terms.find(active_property.value())};
    if (scoped_term != active_context.terms.cend() &&
        scoped_term->second.context.has_value()) {
      scoped_definition = &scoped_term->second;
    }
  }

  if (!element.is_object() && !element.is_array()) {
    if (!active_property.has_value() ||
        active_property.value() == KEYWORD_GRAPH) {
      return JSON{nullptr};
    }
    if (scoped_definition != nullptr) {
      ActiveContext scoped{active_context};
      scoped.previous = nullptr;
      apply_scoped_context(state, scoped, *scoped_definition);
      return expand_value(state, scoped, active_property, element);
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

  // Revert a non-propagating (type-scoped) context when descending into a node
  // object that is neither a value object nor an @id-only reference.
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

  // The property-scoped context applies after the revert, so a
  // non-propagating one covers the immediate node while nested nodes regain
  // the previous context
  ActiveContext scoped;
  if (scoped_definition != nullptr) {
    scoped = *current;
    // A property-scoped context propagates by default, so it does not inherit
    // an enclosing type-scoped revert. It may, however, set its own revert
    // when it specifies @propagate: false.
    scoped.previous = nullptr;
    apply_scoped_context(state, scoped, *scoped_definition);
    current = &scoped;
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

} // namespace sourcemeta::core
