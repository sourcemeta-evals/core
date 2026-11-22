#include <sourcemeta/core/json.h>
#include <sourcemeta/core/jsonpointer.h>

#include "jsonld_algorithms.h"
#include "jsonld_keywords.h"

#include <algorithm> // std::find, std::sort
#include <cassert>   // assert
#include <map>       // std::map
#include <optional>  // std::optional, std::nullopt
#include <utility>   // std::move, std::pair
#include <vector>    // std::vector

namespace sourcemeta::core {

namespace {

auto definition_for(const ActiveContext &active_context,
                    const std::optional<JSON::String> &active_property)
    -> const TermDefinition * {
  if (!active_property.has_value()) {
    return nullptr;
  }
  const auto iterator{active_context.terms.find(active_property.value())};
  return iterator == active_context.terms.cend() ? nullptr : &iterator->second;
}

auto container_includes(const TermDefinition *const definition,
                        const JSON::StringView keyword) -> bool {
  return definition != nullptr &&
         std::ranges::find(definition->container, keyword) !=
             definition->container.cend();
}

// Append a compacted value under a key, growing it into an array as needed and
// forcing an array when the term requires one.
auto add_value(JSON &result, const JSON::String &key, JSON &&value,
               const bool as_array) -> void {
  if (!result.defines(key)) {
    if (as_array) {
      auto array{JSON::make_array()};
      array.push_back(std::move(value));
      result.assign_assume_new(key, std::move(array));
    } else {
      result.assign_assume_new(key, std::move(value));
    }
    return;
  }
  auto &existing{result.at(key)};
  if (!existing.is_array()) {
    auto array{JSON::make_array()};
    array.push_back(existing);
    existing = std::move(array);
  }
  existing.push_back(std::move(value));
}

// The object a property is written into: the result itself, or a nesting
// container when the term carries an @nest mapping.
auto nest_target(JSON &result, const TermDefinition *const definition,
                 const ActiveContext &active_context,
                 const JSON &inverse_context) -> JSON & {
  if (definition == nullptr || !definition->nest.has_value()) {
    return result;
  }
  JSON::String key{definition->nest.value()};
  if (key == KEYWORD_NEST) {
    key = compact_iri(active_context, inverse_context,
                      JSON::String{KEYWORD_NEST}, nullptr, true, false);
  } else {
    // A non-@nest nest value must reference a term that expands to @nest.
    const auto iterator{active_context.terms.find(key)};
    if (iterator == active_context.terms.cend() ||
        !iterator->second.iri.has_value() ||
        iterator->second.iri.value() != KEYWORD_NEST) {
      throw JSONLDError("Invalid @nest value", EMPTY_WEAK_POINTER);
    }
  }
  if (!result.defines(key)) {
    result.assign_assume_new(key, JSON::make_object());
  }
  // The nest target is a context-defined term that expands to @nest, so it can
  // never collide with an ordinary compacted property and is always an object.
  assert(result.at(key).is_object());
  return result.at(key);
}

} // namespace

auto compact(ExpansionState &state, const ActiveContext &active_context,
             const JSON &inverse_context,
             const std::optional<JSON::String> &active_property,
             const JSON &element, const bool compact_arrays) -> JSON {
  const NestingDepthScope scope{state.depth};
  if (state.depth > ExpansionState::MAXIMUM_DEPTH) {
    throw JSONLDError("Maximum nesting depth exceeded", EMPTY_WEAK_POINTER);
  }

  // Compaction walks an expanded document, where a scalar only ever stands as
  // what a value object holds, which never reaches here on its own
  assert(element.is_object() || element.is_array());

  const auto *definition{definition_for(active_context, active_property)};

  if (element.is_array()) {
    auto result{JSON::make_array()};
    for (const auto &item : element.as_array()) {
      // The step that drops a null item (JSON-LD 1.1 API Section 6.1.2: "If
      // compacted item is not null, then append it to result") has nothing to
      // act on here. The only null an expanded item compacts to is the one a
      // JSON literal holds, which is data rather than absence, and dropping it
      // would silently lose the literal
      result.push_back(compact(state, active_context, inverse_context,
                               active_property, item, compact_arrays));
    }
    // A lone item stands on its own only when the active property is neither
    // the graph nor the set keyword ("and active property is not @graph or
    // @set, and compactArrays is true", JSON-LD 1.1 API Section 6.1.2 step
    // 3.3). The set keyword does arrive here: a set object carrying a list
    // hands that list down under the set keyword itself, and the graph
    // keyword arrives from the graph entry of a node, while a graph object
    // consumed whole by the graph steps compacts its graph under the term
    // itself, which is how three suite entries see their lone node stand on
    // its own (suite entries #t0090, #t0092 and #t0094)
    const bool force_array{!compact_arrays ||
                           (active_property.has_value() &&
                            (active_property.value() == KEYWORD_SET ||
                             active_property.value() == KEYWORD_GRAPH)) ||
                           container_includes(definition, KEYWORD_SET) ||
                           container_includes(definition, KEYWORD_LIST)};
    if (result.size() == 1 && !force_array) {
      return result.front();
    }
    return result;
  }

  // A non-propagating (type-scoped) context does not apply when processing a
  // new node object (JSON-LD 1.1 API Section 6.1.2 step 3.1): revert it unless
  // the element is a value object or a sole @id reference.
  ActiveContext reverted;
  JSON reverted_inverse{JSON::make_object()};
  const bool did_revert{static_cast<bool>(active_context.previous) &&
                        !element.defines(KEYWORD_VALUE, KEYWORD_VALUE_HASH) &&
                        (element.object_size() != 1 ||
                         !element.defines(KEYWORD_ID, KEYWORD_ID_HASH))};
  if (did_revert) {
    reverted = *active_context.previous;
    reverted_inverse = create_inverse_context(reverted);
  }
  const ActiveContext &base{did_revert ? reverted : active_context};
  const JSON &base_inverse{did_revert ? reverted_inverse : inverse_context};
  definition = definition_for(base, active_property);

  // A property-scoped context applies while compacting this position. Its term
  // definition is taken from the active context as it was on entry so that
  // reverting a non-propagating type-scoped context does not discard the local
  // context of the term being compacted (Section 6.1.2 step 3.2).
  const auto *scoped_definition{
      definition_for(active_context, active_property)};
  ActiveContext scoped{base};
  JSON scoped_inverse{base_inverse};
  bool scoped_changed{false};
  if (scoped_definition != nullptr && scoped_definition->context.has_value()) {
    // A property-scoped context may legally override protected terms.
    const bool saved_override{state.protected_override};
    const bool saved_context_protected{state.context_protected};
    state.protected_override = true;
    process_context(state, scoped, scoped_definition->context.value(),
                    EMPTY_WEAK_POINTER);
    state.protected_override = saved_override;
    state.context_protected = saved_context_protected;
    scoped_inverse = create_inverse_context(scoped);
    scoped_changed = true;
    definition = definition_for(scoped, active_property);
  }
  const ActiveContext &context{scoped_changed ? scoped : base};
  const JSON &inverse{scoped_changed ? scoped_inverse : base_inverse};

  // A value object is fully handled by value compaction.
  if (element.defines(KEYWORD_VALUE, KEYWORD_VALUE_HASH)) {
    return compact_value(context, inverse, active_property, element);
  }
  // A node reference compacts to a scalar when the term so dictates, otherwise
  // it is treated as a node below.
  if (element.defines(KEYWORD_ID, KEYWORD_ID_HASH)) {
    // Value compaction answers with a compacted name, with what the value
    // held, or with the map it was handed, never with an array
    auto compacted{compact_value(context, inverse, active_property, element)};
    assert(!compacted.is_array());
    if (!compacted.is_object()) {
      return compacted;
    }
  }

  // A list object whose property coerces @list compacts to a bare array, which
  // also yields nested arrays for a list of lists (Section 6.1.2 step 3.3).
  if (element.defines(KEYWORD_LIST, KEYWORD_LIST_HASH) &&
      container_includes(definition_for(context, active_property),
                         KEYWORD_LIST)) {
    return compact(state, context, inverse, active_property,
                   element.at(KEYWORD_LIST, KEYWORD_LIST_HASH), compact_arrays);
  }

  const bool inside_reverse{active_property.has_value() &&
                            active_property.value() == KEYWORD_REVERSE};
  auto result{JSON::make_object()};
  // Where each keyed graph container map stands, by nest name and compacted
  // property: as the whole entry when nothing else claimed the name, and at
  // a recorded place of the shared array otherwise, as graph objects
  // matching no map case land beside the map on the same entry
  std::map<std::pair<JSON::String, JSON::String>,
           std::optional<JSON::Array::size_type>>
      graph_map_slots;

  // A type-scoped context from the node's types applies to the remaining
  // properties.
  ActiveContext typed{context};
  JSON typed_inverse{inverse};
  bool typed_changed{false};
  if (element.defines(KEYWORD_TYPE, KEYWORD_TYPE_HASH)) {
    // The type-scoped context is keyed by the compacted type term, applied in
    // lexicographical order of those terms.
    std::vector<JSON::String> types;
    for (const auto &item :
         element.at(KEYWORD_TYPE, KEYWORD_TYPE_HASH).as_array()) {
      types.push_back(compact_iri(context, inverse, item.to_string(), nullptr,
                                  true, false));
    }
    std::ranges::sort(types);
    for (const auto &type : types) {
      const auto iterator{context.terms.find(type)};
      if (iterator != context.terms.cend() &&
          iterator->second.context.has_value()) {
        process_context(state, typed, iterator->second.context.value(),
                        EMPTY_WEAK_POINTER, false);
        typed_changed = true;
      }
    }
    if (typed_changed) {
      typed_inverse = create_inverse_context(typed);
    }
  }
  const ActiveContext &node_context{typed_changed ? typed : context};
  const JSON &node_inverse{typed_changed ? typed_inverse : inverse};

  for (const auto &entry : element.as_object()) {
    const auto &expanded_property{entry.first};
    const auto &expanded_value{entry.second};

    if (entry.key_equals(KEYWORD_ID, KEYWORD_ID_HASH)) {
      const auto alias{compact_iri(node_context, node_inverse,
                                   JSON::String{KEYWORD_ID}, nullptr, true,
                                   false)};
      result.assign_assume_new(
          alias,
          JSON{compact_iri(node_context, node_inverse,
                           expanded_value.to_string(), nullptr, false, false)});
      continue;
    }

    if (entry.key_equals(KEYWORD_TYPE, KEYWORD_TYPE_HASH)) {
      const auto alias{compact_iri(node_context, node_inverse,
                                   JSON::String{KEYWORD_TYPE}, nullptr, true,
                                   false)};
      // Type values are compacted against the context before type-scoping.
      auto compacted{JSON::make_array()};
      for (const auto &item : expanded_value.as_array()) {
        compacted.push_back(JSON{compact_iri(context, inverse, item.to_string(),
                                             nullptr, true, false)});
      }
      const auto *type_definition{definition_for(node_context, alias)};
      // A @set container forces @type to an array only in JSON-LD 1.1.
      const bool type_as_array{
          !state.processing_1_0 &&
          container_includes(type_definition, KEYWORD_SET)};
      if (compacted.size() == 1 && compact_arrays && !type_as_array) {
        result.assign_assume_new(alias, JSON{compacted.front()});
      } else {
        result.assign_assume_new(alias, std::move(compacted));
      }
      continue;
    }

    if (entry.key_equals(KEYWORD_REVERSE, KEYWORD_REVERSE_HASH)) {
      auto compacted{compact(state, node_context, node_inverse,
                             JSON::String{KEYWORD_REVERSE}, expanded_value,
                             compact_arrays)};
      auto reverse_map{JSON::make_object()};
      for (const auto &reverse : compacted.as_object()) {
        const auto reverse_definition{node_context.terms.find(reverse.first)};
        if (reverse_definition != node_context.terms.cend() &&
            reverse_definition->second.reverse) {
          // The compacted value already reflects the term's @set container.
          add_value(result, reverse.first, JSON{reverse.second}, false);
        } else {
          reverse_map.assign(reverse.first, reverse.second);
        }
      }
      if (!reverse_map.empty()) {
        const auto alias{compact_iri(node_context, node_inverse,
                                     JSON::String{KEYWORD_REVERSE}, nullptr,
                                     true, false)};
        result.assign_assume_new(alias, std::move(reverse_map));
      }
      continue;
    }

    if (entry.key_equals(KEYWORD_INDEX, KEYWORD_INDEX_HASH) &&
        container_includes(definition, KEYWORD_INDEX)) {
      continue;
    }

    // The algorithm names four keywords together here, of which only the
    // index arrives: anything carrying a value was handed to value compaction
    // further up, and a language or a direction belongs to a value object, so
    // a node carrying one is not expanded form and the entry point holds the
    // input to that (JSON-LD 1.1 API Section 6.1.2)
    assert(!entry.key_equals(KEYWORD_VALUE, KEYWORD_VALUE_HASH) &&
           !entry.key_equals(KEYWORD_LANGUAGE, KEYWORD_LANGUAGE_HASH) &&
           !entry.key_equals(KEYWORD_DIRECTION, KEYWORD_DIRECTION_HASH));
    if (entry.key_equals(KEYWORD_INDEX, KEYWORD_INDEX_HASH)) {
      const auto alias{compact_iri(node_context, node_inverse,
                                   expanded_property, nullptr, true, false)};
      result.assign_assume_new(alias, JSON{expanded_value});
      continue;
    }

    // A node's named graph compacts to an array of compacted node objects.
    if (entry.key_equals(KEYWORD_GRAPH, KEYWORD_GRAPH_HASH)) {
      const auto alias{compact_iri(node_context, node_inverse,
                                   JSON::String{KEYWORD_GRAPH}, nullptr, true,
                                   false)};
      auto compacted{compact(state, node_context, node_inverse,
                             JSON::String{KEYWORD_GRAPH}, expanded_value,
                             compact_arrays)};
      // A graph entry keeps its array whatever else the node carries: what a
      // graph entry holds in expanded form is an array, and the graph keyword
      // as the active property keeps it one through the recursion ("and
      // active property is not @graph or @set", JSON-LD 1.1 API Section
      // 6.1.2 step 3.3)
      assert(compacted.is_array());
      result.assign_assume_new(alias, std::move(compacted));
      continue;
    }

    // As above, a property holds an array
    assert(expanded_value.is_array());
    if (expanded_value.empty()) {
      const auto item_property{compact_iri(node_context, node_inverse,
                                           expanded_property, &expanded_value,
                                           true, inside_reverse)};
      // An empty array stays empty, landing in the nest its term names and
      // joining whatever already stands under the same compacted name, as two
      // properties can settle on one name through a legacy mapping or a
      // reverse alias (JSON-LD 1.1 API Section 6.1.2 step 12.7.4)
      const auto *empty_definition{definition_for(node_context, item_property)};
      JSON &empty_target{
          nest_target(result, empty_definition, node_context, node_inverse)};
      if (!empty_target.defines(item_property)) {
        empty_target.assign_assume_new(item_property, JSON::make_array());
      } else if (auto &standing{empty_target.at(item_property)};
                 !standing.is_array()) {
        auto array{JSON::make_array()};
        array.push_back(standing);
        standing = std::move(array);
      }
      continue;
    }

    for (const auto &item : expanded_value.as_array()) {
      // What a property holds in expanded form is maps, which the entry point
      // holds the input to before any of this runs
      assert(item.is_object());
      const auto item_property{compact_iri(node_context, node_inverse,
                                           expanded_property, &item, true,
                                           inside_reverse)};
      const auto *item_definition{definition_for(node_context, item_property)};
      const bool is_list{item.defines(KEYWORD_LIST, KEYWORD_LIST_HASH)};
      const bool is_graph_object{
          item.defines(KEYWORD_GRAPH, KEYWORD_GRAPH_HASH)};
      JSON &target{
          nest_target(result, item_definition, node_context, node_inverse)};

      // @graph container: a simple graph object (without @id) is mapped by @id
      // or @index or inlined; a graph object carrying an @id that is not keyed
      // by @id is wrapped back into a graph object (Section 6.1.2).
      if (container_includes(item_definition, KEYWORD_GRAPH) &&
          is_graph_object) {
        const bool graph_as_array{
            container_includes(item_definition, KEYWORD_SET) ||
            !compact_arrays};
        const bool by_id{container_includes(item_definition, KEYWORD_ID)};
        const bool by_index{container_includes(item_definition, KEYWORD_INDEX)};
        const bool simple_graph{!item.defines(KEYWORD_ID, KEYWORD_ID_HASH)};
        auto compacted{compact(state, node_context, node_inverse, item_property,
                               item.at(KEYWORD_GRAPH, KEYWORD_GRAPH_HASH),
                               compact_arrays)};
        const auto none_key{compact_iri(node_context, node_inverse,
                                        JSON::String{KEYWORD_NONE}, nullptr,
                                        true, false)};
        if (by_id || (by_index && simple_graph)) {
          // The step reads the map out of the entry, "initializing it to a
          // new empty map, if necessary" (JSON-LD 1.1 API Section 6.1.2), yet
          // a graph object matching no map case lands on the same entry as a
          // wrapped value, so the keyed writes follow the map this step
          // initialized through the shapes the entry passes rather than
          // writing into whatever stands under the name
          JSON *map_object{nullptr};
          // A term with no definition names no graph container, so a
          // definition always stands behind this branch
          assert(item_definition != nullptr);
          const std::pair<JSON::String, JSON::String> slot_key{
              item_definition->nest.has_value() ? item_definition->nest.value()
                                                : JSON::String{},
              item_property};
          if (!target.defines(item_property)) {
            target.assign_assume_new(item_property, JSON::make_object());
            graph_map_slots.insert_or_assign(slot_key, std::nullopt);
            map_object = &target.at(item_property);
          } else {
            auto &existing{target.at(item_property)};
            const auto slot{graph_map_slots.find(slot_key)};
            if (slot == graph_map_slots.cend()) {
              // Other values claimed the name first, so the map joins them at
              // the back of the array they share
              if (!existing.is_array()) {
                auto array{JSON::make_array()};
                array.push_back(std::move(existing));
                existing = std::move(array);
              }
              existing.push_back(JSON::make_object());
              graph_map_slots.insert_or_assign(slot_key, existing.size() - 1);
              map_object = &existing.at(existing.size() - 1);
            } else if (!slot->second.has_value()) {
              if (existing.is_array()) {
                // A value added beside a lone map wraps it into an array
                // first, so the map rides at the front of what they share
                slot->second = 0;
                map_object = &existing.at(0);
              } else {
                map_object = &existing;
              }
            } else {
              map_object = &existing.at(slot->second.value());
            }
          }
          JSON::String key{none_key};
          if (by_id && item.defines(KEYWORD_ID, KEYWORD_ID_HASH)) {
            key = compact_iri(node_context, node_inverse,
                              item.at(KEYWORD_ID, KEYWORD_ID_HASH).to_string(),
                              nullptr, false, false);
          } else if (by_index &&
                     item.defines(KEYWORD_INDEX, KEYWORD_INDEX_HASH)) {
            key = item.at(KEYWORD_INDEX, KEYWORD_INDEX_HASH).to_string();
          }
          // The graph value is already compacted to an array when the term has
          // an @set container or compactArrays is false, so it is added as-is.
          add_value(*map_object, key, std::move(compacted), false);
          continue;
        }
        if (simple_graph) {
          // A single node is inlined, several are wrapped in an @included
          // block.
          if (compacted.is_array() && compacted.size() != 1) {
            const auto included_alias{compact_iri(
                node_context, node_inverse, JSON::String{KEYWORD_INCLUDED},
                nullptr, true, false)};
            auto wrapper{JSON::make_object()};
            wrapper.assign_assume_new(included_alias, std::move(compacted));
            compacted = std::move(wrapper);
          } else if (compacted.is_array()) {
            auto single{compacted.front()};
            compacted = std::move(single);
          }
          add_value(target, item_property, std::move(compacted),
                    graph_as_array);
          continue;
        }
        // Wrap a graph object that retains its @id back into a graph object.
        const auto graph_alias{compact_iri(node_context, node_inverse,
                                           JSON::String{KEYWORD_GRAPH}, nullptr,
                                           true, false)};
        auto wrapper{JSON::make_object()};
        const auto id_alias{compact_iri(node_context, node_inverse,
                                        JSON::String{KEYWORD_ID}, nullptr, true,
                                        false)};
        wrapper.assign_assume_new(
            id_alias,
            JSON{compact_iri(node_context, node_inverse,
                             item.at(KEYWORD_ID, KEYWORD_ID_HASH).to_string(),
                             nullptr, false, false)});
        if (item.defines(KEYWORD_INDEX, KEYWORD_INDEX_HASH)) {
          const auto index_alias{compact_iri(node_context, node_inverse,
                                             JSON::String{KEYWORD_INDEX},
                                             nullptr, true, false)};
          wrapper.assign_assume_new(
              index_alias, JSON{item.at(KEYWORD_INDEX, KEYWORD_INDEX_HASH)});
        }
        wrapper.assign_assume_new(graph_alias, std::move(compacted));
        add_value(target, item_property, std::move(wrapper), graph_as_array);
        continue;
      }

      // Only a list object takes the list path: an ordinary value under a
      // term declaring a list container compacts like any other value
      // (JSON-LD 1.1 API Section 6.1.2, the item steps gate on the item being
      // a list object). Under a term whose container names no list, the list
      // compacts to a wrapper that is carried on into the container steps,
      // which key it like any other member, the way jsonld.js orders the
      // steps: an add inside the list step would stand beside the one the
      // container steps make, holding the member twice
      std::optional<JSON> list_wrapper;
      if (is_list) {
        const auto &list_value{item.at(KEYWORD_LIST, KEYWORD_LIST_HASH)};
        // What a list entry holds in expanded form is an array, which the
        // entry point holds the input to
        assert(list_value.is_array());
        // JSON-LD 1.0 forbids a list of lists.
        if (state.processing_1_0) {
          for (const auto &member : list_value.as_array()) {
            // As above, a list holds maps
            assert(member.is_object());
            if (member.defines(KEYWORD_LIST, KEYWORD_LIST_HASH)) {
              throw JSONLDError("Compaction to list of lists",
                                EMPTY_WEAK_POINTER);
            }
          }
        }
        auto compacted{compact(state, node_context, node_inverse, item_property,
                               list_value, compact_arrays)};
        if (!compacted.is_array()) {
          auto array{JSON::make_array()};
          array.push_back(std::move(compacted));
          compacted = std::move(array);
        }
        if (container_includes(item_definition, KEYWORD_LIST)) {
          // "Otherwise, set the value of the item active property entry in
          // nest result to compacted item" (JSON-LD 1.1 API Section 6.1.2
          // step 12.8.7.3): an assignment, so a later list carried by the
          // same term replaces what an earlier one left
          target.assign(item_property, std::move(compacted));
          continue;
        }
        const auto list_alias{compact_iri(node_context, node_inverse,
                                          JSON::String{KEYWORD_LIST}, nullptr,
                                          true, false)};
        auto list_object{JSON::make_object()};
        list_object.assign_assume_new(list_alias, std::move(compacted));
        if (item.defines(KEYWORD_INDEX, KEYWORD_INDEX_HASH)) {
          const auto index_alias{compact_iri(node_context, node_inverse,
                                             JSON::String{KEYWORD_INDEX},
                                             nullptr, true, false)};
          list_object.assign_assume_new(
              index_alias, JSON{item.at(KEYWORD_INDEX, KEYWORD_INDEX_HASH)});
        }
        list_wrapper.emplace(std::move(list_object));
      }

      // The items of a list keyword entry keep their array, the way the
      // per-item add asks for an array when the item active property is the
      // list keyword (JSON-LD 1.1 API Section 6.1.2)
      const bool as_array{container_includes(item_definition, KEYWORD_SET) ||
                          expanded_property == KEYWORD_LIST || !compact_arrays};

      const auto none_key{compact_iri(node_context, node_inverse,
                                      JSON::String{KEYWORD_NONE}, nullptr, true,
                                      false)};

      // A graph object proper holds nothing beyond its graph, its name and
      // its index, and the graph steps consume one whole before any map
      // container is weighed. Under a term whose container names no graph,
      // its contents compact under the term itself and are wrapped back, the
      // name and the index coming along ("Otherwise, container does not
      // include @graph ... Set compacted item to a new map containing the key
      // from IRI compacting @graph using the original compacted item as a
      // value", JSON-LD 1.1 API Section 6.1.2)
      if (is_graph_object) {
        bool graph_object_proper{true};
        for (const auto &member : item.as_object()) {
          if (!member.key_equals(KEYWORD_GRAPH, KEYWORD_GRAPH_HASH) &&
              !member.key_equals(KEYWORD_ID, KEYWORD_ID_HASH) &&
              !member.key_equals(KEYWORD_INDEX, KEYWORD_INDEX_HASH)) {
            graph_object_proper = false;
            break;
          }
        }
        if (graph_object_proper) {
          auto wrapper{JSON::make_object()};
          wrapper.assign_assume_new(
              compact_iri(node_context, node_inverse,
                          JSON::String{KEYWORD_GRAPH}, nullptr, true, false),
              compact(state, node_context, node_inverse, item_property,
                      item.at(KEYWORD_GRAPH, KEYWORD_GRAPH_HASH),
                      compact_arrays));
          if (item.defines(KEYWORD_ID, KEYWORD_ID_HASH)) {
            wrapper.assign_assume_new(
                compact_iri(node_context, node_inverse,
                            JSON::String{KEYWORD_ID}, nullptr, true, false),
                JSON{compact_iri(
                    node_context, node_inverse,
                    item.at(KEYWORD_ID, KEYWORD_ID_HASH).to_string(), nullptr,
                    false, false)});
          }
          if (item.defines(KEYWORD_INDEX, KEYWORD_INDEX_HASH)) {
            wrapper.assign_assume_new(
                compact_iri(node_context, node_inverse,
                            JSON::String{KEYWORD_INDEX}, nullptr, true, false),
                JSON{item.at(KEYWORD_INDEX, KEYWORD_INDEX_HASH)});
          }
          add_value(target, item_property, std::move(wrapper), as_array);
          continue;
        }
      }

      // @language map container: the map gathers every member, value objects
      // keyed by their language and reduced to their value, and anything
      // else standing under the none label, as a member carrying no value
      // carries no language to key by, a wrapped list among them. A graph
      // object proper never arrives: the graph steps come before the map
      // container ones and consume it whole (JSON-LD 1.1 API Section 6.1.2)
      if (container_includes(item_definition, KEYWORD_LANGUAGE)) {
        if (!target.defines(item_property)) {
          target.assign_assume_new(item_property, JSON::make_object());
        }
        if (item.defines(KEYWORD_VALUE, KEYWORD_VALUE_HASH)) {
          const JSON::String key{
              item.defines(KEYWORD_LANGUAGE, KEYWORD_LANGUAGE_HASH)
                  ? item.at(KEYWORD_LANGUAGE, KEYWORD_LANGUAGE_HASH).to_string()
                  : none_key};
          add_value(target.at(item_property), key,
                    JSON{item.at(KEYWORD_VALUE, KEYWORD_VALUE_HASH)}, as_array);
        } else {
          add_value(target.at(item_property), none_key,
                    list_wrapper.has_value()
                        ? std::move(list_wrapper.value())
                        : compact(state, node_context, node_inverse,
                                  item_property, item, compact_arrays),
                    as_array);
        }
        continue;
      }

      // @index map container: index by the item's @index, or for a
      // property-valued index by the value of that property in the compacted
      // item (Section 6.1.2).
      if (container_includes(item_definition, KEYWORD_INDEX)) {
        if (!target.defines(item_property)) {
          target.assign_assume_new(item_property, JSON::make_object());
        }
        // The container test above answers false for a definition that is
        // not there, so one stands here
        assert(item_definition != nullptr);
        const bool property_index{item_definition->index_iri.has_value()};
        auto compacted{list_wrapper.has_value()
                           ? std::move(list_wrapper.value())
                           : compact(state, node_context, node_inverse,
                                     item_property, item, compact_arrays)};
        std::optional<JSON::String> key;
        if (property_index) {
          // The index key is the expanded index mapping, compacted to match the
          // corresponding property of the compacted item (Section 6.1.2).
          const auto container_key{compact_iri(
              node_context, node_inverse, item_definition->index_iri.value(),
              nullptr, true, false)};
          if (compacted.is_object() && compacted.defines(container_key)) {
            auto &values{compacted.at(container_key)};
            if (values.is_array() && !values.empty() &&
                values.front().is_string()) {
              key = values.front().to_string();
              if (values.size() == 1) {
                compacted.erase(container_key);
              } else {
                values.erase(values.as_array().cbegin());
                // A lone remaining value collapses to a scalar only when arrays
                // are being compacted.
                if (compact_arrays && values.size() == 1) {
                  auto remaining{values.front()};
                  values = std::move(remaining);
                }
              }
            } else if (!values.is_array() && values.is_string()) {
              key = values.to_string();
              compacted.erase(container_key);
            } else if (values.is_array() && values.empty()) {
              // An index property with nothing left goes, the way the step
              // removes the entry when no values remain, and the member
              // stands under the none label (JSON-LD 1.1 API Section 6.1.2)
              compacted.erase(container_key);
            }
          }
        } else if (item.defines(KEYWORD_INDEX, KEYWORD_INDEX_HASH)) {
          key = item.at(KEYWORD_INDEX, KEYWORD_INDEX_HASH).to_string();
          if (compacted.is_object()) {
            compacted.erase(compact_iri(node_context, node_inverse,
                                        JSON::String{KEYWORD_INDEX}, nullptr,
                                        true, false));
          }
        }
        add_value(target.at(item_property), key.value_or(none_key),
                  std::move(compacted), as_array);
        continue;
      }

      // @id map container: the key is the identifier the member carries once
      // compacted, which is where the step reads it from, so a member that
      // compacted to anything other than a map carries no identifier to key
      // by and stands under the none label (Section 6.1.2)
      if (container_includes(item_definition, KEYWORD_ID)) {
        if (!target.defines(item_property)) {
          target.assign_assume_new(item_property, JSON::make_object());
        }
        auto compacted{list_wrapper.has_value()
                           ? std::move(list_wrapper.value())
                           : compact(state, node_context, node_inverse,
                                     item_property, item, compact_arrays)};
        const auto id_alias{compact_iri(node_context, node_inverse,
                                        JSON::String{KEYWORD_ID}, nullptr, true,
                                        false)};
        JSON::String key{none_key};
        // The key is read from the member's own identifier alone: an ordinary
        // property can compact onto the identifier alias, a vocabulary suffix
        // shaped like the keyword among them, and names nothing (JSON-LD 1.1
        // API Section 6.1.2)
        if (compacted.is_object() &&
            item.defines(KEYWORD_ID, KEYWORD_ID_HASH)) {
          // The alias is spelled with the context in force here, while the
          // member compacted under the scoped context its term carries, which
          // can retire that spelling, hand it to another property, or name
          // the identifier some other way. The step reads the entry it names
          // and nothing else, and a member whose key stayed unset lands under
          // the none label ("If map key is null, set it to the result of IRI
          // compacting @none", JSON-LD 1.1 API Section 6.1.2)
          const auto *identifier{compacted.try_at(id_alias)};
          if (identifier != nullptr && identifier->is_string()) {
            key = identifier->to_string();
            compacted.erase(id_alias);
          }
        }
        add_value(target.at(item_property), key, std::move(compacted),
                  as_array);
        continue;
      }

      // @type map container: the key is the first type the member carries
      // once compacted, which is where the step reads it from, so a
      // property-scoped context has its say over the spelling (JSON-LD 1.1
      // API Section 6.1.2)
      if (container_includes(item_definition, KEYWORD_TYPE)) {
        if (!target.defines(item_property)) {
          target.assign_assume_new(item_property, JSON::make_object());
        }
        auto compacted{list_wrapper.has_value()
                           ? std::move(list_wrapper.value())
                           : compact(state, node_context, node_inverse,
                                     item_property, item, compact_arrays)};
        JSON::String key{none_key};
        if (compacted.is_object()) {
          const auto type_alias{compact_iri(node_context, node_inverse,
                                            JSON::String{KEYWORD_TYPE}, nullptr,
                                            true, false)};
          // The alias is spelled with the context in force here, and what the
          // compacted member holds under it is read as the types only where
          // it looks like them, as an ordinary property can compact onto the
          // same spelling (JSON-LD 1.1 API Section 6.1.2)
          if (compacted.defines(type_alias)) {
            auto &types{compacted.at(type_alias)};
            if (types.is_string()) {
              key = types.to_string();
              compacted.erase(type_alias);
            } else if (types.is_array() && !types.empty() &&
                       types.front().is_string()) {
              key = types.front().to_string();
              if (types.size() == 1) {
                compacted.erase(type_alias);
              } else {
                types.erase(types.as_array().cbegin());
                // A lone remaining type collapses to a scalar only when arrays
                // are being compacted.
                if (compact_arrays && types.size() == 1) {
                  auto remaining{types.front()};
                  types = std::move(remaining);
                }
              }
            } else if (types.is_array() && types.empty()) {
              // A type entry with nothing left goes, the way the step removes
              // it when no values remain, and the member stands under the
              // none label (JSON-LD 1.1 API Section 6.1.2)
              compacted.erase(type_alias);
            }
          }
          const auto id_alias{compact_iri(node_context, node_inverse,
                                          JSON::String{KEYWORD_ID}, nullptr,
                                          true, false)};
          // A member left with only an @id reduces to that identifier by
          // compacting a bare reference under its own property, where value
          // compaction reads the type mapping a type container settled at
          // term creation and a property-scoped context has its say over the
          // name. The reduction asks for a key expanding to the id keyword,
          // which a property-scoped context can take away by redefining the
          // alias, so the item must carry the identifier itself (JSON-LD 1.1
          // API Section 6.1.2 step 12.9.8.4)
          if (compacted.object_size() == 1 && compacted.defines(id_alias) &&
              item.defines(KEYWORD_ID, KEYWORD_ID_HASH)) {
            auto reference{JSON::make_object()};
            reference.assign_assume_new(
                JSON::String{KEYWORD_ID},
                JSON{item.at(KEYWORD_ID, KEYWORD_ID_HASH)}, KEYWORD_ID_HASH);
            compacted = compact(state, node_context, node_inverse,
                                item_property, reference, compact_arrays);
          }
        }
        add_value(target.at(item_property), key, std::move(compacted),
                  as_array);
        continue;
      }

      auto compacted{list_wrapper.has_value()
                         ? std::move(list_wrapper.value())
                         : compact(state, node_context, node_inverse,
                                   item_property, item, compact_arrays)};
      // A compacted JSON literal may itself be an array, in which case its
      // members are added individually rather than nesting another array.
      if (compacted.is_array()) {
        // An empty compacted literal has no members to add one by one, yet
        // the array itself is the value, so it stands as the property when
        // nothing claimed the name before it
        if (compacted.empty()) {
          if (!target.defines(item_property)) {
            target.assign(item_property, std::move(compacted));
          }
          continue;
        }
        for (const auto &member : compacted.as_array()) {
          add_value(target, item_property, JSON{member}, as_array);
        }
      } else {
        add_value(target, item_property, std::move(compacted), as_array);
      }
    }
  }

  return result;
}

} // namespace sourcemeta::core
