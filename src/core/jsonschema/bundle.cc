#include <sourcemeta/core/jsonschema.h>

#include <sourcemeta/core/uri.h>

#include "helpers.h"

#include <algorithm>     // std::ranges::any_of, std::ranges::none_of
#include <cassert>       // assert
#include <cstddef>       // std::size_t
#include <cstdint>       // std::uint64_t
#include <functional>    // std::cref
#include <map>           // std::map
#include <optional>      // std::optional
#include <string>        // std::string
#include <string_view>   // std::string_view
#include <tuple>         // std::tuple
#include <unordered_map> // std::unordered_map
#include <unordered_set> // std::unordered_set
#include <utility>       // std::move, std::pair
#include <vector>        // std::vector

namespace sourcemeta::core {

namespace {

auto is_skippable_metaschema_reference(const SchemaBundleOptions::Mode mode,
                                       const WeakPointer &pointer,
                                       const std::string &destination) -> bool {
  assert(!pointer.empty());
  assert(pointer.back().is_property());
  if (pointer.back().to_property() != "$schema") {
    return false;
  }

  return mode == SchemaBundleOptions::Mode::References ||
         schema_is_official(destination);
}

// RFC 3986, section 4.4 calls a reference that leads back to the document
// holding it a same-document reference, whose "most frequent examples [...] are
// relative references that are empty or include only the number sign ('#')
// separator followed by a fragment identifier". Those stay as spelled, as they
// need no base URI beyond the one already in effect. Any other relative
// reference names another document, so bundling restates it as the URI it
// resolves to, which is what lets it keep naming its target once the bundled
// document travels somewhere else
//
// A dynamic reference needs no exception here. JSON Schema 2020-12, section
// 8.2.3.2 has it "resolved against the current URI base" like any other before
// the dynamic scope is consulted, so the absolute form of that resolution is
// what the document should spell, and framing only reports the anchor it ends
// up at in place of that resolution when the two name the same place anyway.
// JSON Schema 2019-09, section 8.2.4.2.1 defines the behavior of
// `$recursiveRef` "only for the value `#`", which the rule below already spares
auto spells_another_document_relatively(const SchemaFrame::Reference &reference)
    -> bool {
  if (reference.original == reference.destination) {
    return false;
  }

  const URI original{reference.original};
  return original.is_relative() && !original.is_fragment_only() &&
         !original.empty();
}

// The absolute form of a reference whose target answers to an identifier other
// than the URI it was resolved by
auto rebase_reference(const JSON::String &base,
                      const std::optional<std::string_view> &fragment)
    -> JSON::String {
  URI result{base};
  if (fragment.has_value()) {
    result.fragment(fragment.value());
  }

  return result.recompose();
}

// The dialect a schema declares, falling back to the given default
auto declared_dialect(const JSON &schema,
                      const std::string_view default_dialect)
    -> std::string_view {
  if (!schema.is_object()) {
    return default_dialect;
  }

  const auto *dialect{schema.try_at("$schema")};
  return (dialect != nullptr && dialect->is_string()) ? dialect->to_string()
                                                      : default_dialect;
}

// Every frame that bundling constructs spends from the same limit, as how
// many frames it ends up needing is a function of what the resolver hands
// back rather than of the schema the caller passed in. A frame that ran past
// what was left of the limit threw rather than returned, so what it holds is
// always within it
auto charge(std::uint64_t &remaining, const SchemaFrame &frame) -> void {
  assert(frame.location_count() <= remaining);
  remaining -= frame.location_count();
}

// A schema of a dialect that cannot name itself answers to nothing once it is
// embedded, so every reference to one has to spell out where it went. Neither
// half is known where the reference is found: the embedding settles the
// position, and elevation can move a schema again afterwards. So the walk only
// records which identifiers went in unnamed and where each landed, and the
// references themselves are settled against the finished document
// Where a resource a remote carried ended up, and the name it answers to,
// which is not always the key it sat under
struct Elevated {
  Pointer at;
  JSON::String identifier;
};

// An entry the walk takes out of a remote, by the key it sits under and the
// name it answers to
struct Removed {
  JSON::String key;
  JSON::String identifier;
};

struct PositionalReferences {
  // Recorded while walking, as that is where the dialect of a remote is known
  std::unordered_set<JSON::String> unnamed;
  // Recorded while embedding, as that is where the position is settled. Only
  // the unnamed ones are kept, since a schema that carries its own name is
  // reached by that name wherever it ends up
  std::unordered_map<JSON::String, Pointer> landings;
  // Also recorded while embedding, and for every remote rather than only the
  // unnamed ones. Settling reads the finished document through a frame, and a
  // container that sits outside every schema the caller framed is reached by
  // nothing else, so a remote that carries its own name has to be framed here
  // too for the references it brought with it to be seen at all
  std::vector<Pointer> embeddings;
  // Elevation lifts a resource that a remote carried out of the copy that
  // lands, so a place named inside one is no longer where the reference that
  // named it said. Such a resource answers to the name it declared, which is
  // what says where it went, and which is not always the key it sat under
  std::unordered_map<JSON::String, std::vector<Elevated>> elevations;
};

// Reading the finished document is what makes this independent of how the
// bundling got there. A reference to an unnamed schema is left unresolved by
// construction, as the identity it named is gone, so framing hands back every
// one of them wherever they ended up
// A dialect that reserves no property name for declaring itself leaves an
// embedded schema no way to say what it is, and what it inherits from wherever
// it lands would be wrong. Marking every subschema is what carries it, since
// the keyword that names a dialect speaks for the one subschema it sits on.
// Only the frame knows where those subschemas are, and taking its word for it
// is what keeps a value that merely looks like a schema from being marked
auto mark_dialect_of_every_subschema(JSON &schema, const SchemaWalker &walker,
                                     const SchemaResolver &resolver,
                                     const std::string_view dialect,
                                     std::uint64_t &remaining) -> void {
  const SchemaFrame frame{SchemaFrame::Mode::Locations,
                          schema,
                          walker,
                          resolver,
                          dialect,
                          "",
                          SchemaFrame::IdentifierMode::Additional,
                          {EMPTY_WEAK_POINTER},
                          "",
                          remaining};
  charge(remaining, frame);

  std::vector<Pointer> targets;
  frame.for_each_location([&targets, dialect](const auto, const auto &,
                                              const auto &location) -> void {
    if (location.type != SchemaFrame::LocationType::Resource &&
        location.type != SchemaFrame::LocationType::Subschema) {
      return;
    }

    // Marking stands in for a declaration that this dialect has no keyword
    // for, so it has nothing to say where a subschema already reads as
    // something else. Going by the dialect in force rather than the one asked
    // for is what leaves such a subschema, and everything under it, as
    // whoever wrote it meant
    if (location.dialect != dialect) {
      return;
    }

    targets.push_back(to_pointer(location.pointer));
  });

  for (const auto &target : targets) {
    auto &subschema{get(schema, target)};
    // A boolean carries no keyword whose meaning could have differed, so
    // leaving it to inherit costs nothing
    if (subschema.is_object()) {
      subschema.assign(JSON::String{DIALECT_OVERRIDE_KEYWORD},
                       JSON{JSON::String{dialect}});
    }
  }
}

// The key this assigns is the only record of where the schema landed, which a
// reference that names a place rather than an identifier has to be told
auto canonical_uri(const std::string_view uri) -> JSON::String {
  try {
    return URI::canonicalize(uri);
  } catch (const URIParseError &) {
    return JSON::String{uri};
  }
}

// A resource on its way into the document, kept so that one arriving before
// it does can still be held against it
struct Pending {
  JSON schema;
  JSON::String dialect;
};

struct KnownResources {
  // What the document already answers to, and what the name it answers to is
  // spelled as where a reference has to say it. A name is held canonically, as
  // the same name spelled two ways is one name, while what a reference is
  // rewritten to stays the spelling the document itself uses
  std::unordered_map<JSON::String, JSON::String> names;
  // Which of those names the document says of itself, which a name it was
  // only fetched by never becomes however many times it is asked for. Reading
  // one off the other would let a name the document goes on answering to
  // elsewhere be claimed as an identity without anything noticing
  std::unordered_set<JSON::String> declared;
  // What a later pass will place. A name is answered from here first, as the
  // document does not hold it yet
  std::unordered_map<JSON::String, Pending> pending;
  // What it takes to read the caller's document the same way it was read when
  // these names were collected. Reading it any other way looks for a name
  // somewhere it was never recorded from
  SchemaFrame::Paths paths;
  JSON::String default_id;
  JSON::String default_base;
};

// Whether anything was recorded against this name at all, which includes a
// name something was only fetched by, since a reference that says one is
// answered by whatever came back
auto is_recorded(const KnownResources &known, const std::string_view name)
    -> bool {
  return known.names.contains(canonical_uri(name));
}

// Whether the document holds this name itself. Where something was fetched
// from says nothing about what it is, as what came back carries its own name,
// so two resources are not one just because one of them was retrieved from
// where the other's name points
auto is_known(const KnownResources &known, const std::string_view name)
    -> bool {
  return known.declared.contains(canonical_uri(name));
}

auto answers_to(const KnownResources &known, const std::string_view name)
    -> const JSON::String & {
  return known.names.at(canonical_uri(name));
}

auto record(KnownResources &known, const std::string_view name,
            const JSON::String &identity) -> void {
  auto canonical{canonical_uri(name)};
  if (canonical == canonical_uri(identity)) {
    known.declared.insert(canonical);
  }

  // What a name was first answered by is what every reference that says it
  // means, as that is what bundling went and got
  known.names.emplace(std::move(canonical), identity);
}

auto expect(KnownResources &known, const std::string_view name,
            const JSON &schema, const std::string_view dialect) -> void {
  known.pending.insert_or_assign(
      canonical_uri(name),
      Pending{.schema = schema, .dialect = JSON::String{dialect}});
}

auto placed(KnownResources &known, const std::string_view name) -> void {
  known.pending.erase(canonical_uri(name));
}

// Every meta-schema that anything within this schema pins. A schema that
// looks for one of these has to go on finding it, so collecting only what the
// top of the document declares would miss a resource further in that pins its
// own
auto collect_pinned_dialects(const JSON &schema,
                             std::unordered_set<JSON::String> &result) -> void {
  if (schema.is_object()) {
    const auto *declared{schema.try_at("$schema"sv, JSONSCHEMA_HASH_SCHEMA)};
    if ((declared != nullptr) && declared->is_string()) {
      result.insert(canonical_uri(declared->to_string()));
    }

    for (const auto &entry : schema.as_object()) {
      collect_pinned_dialects(entry.second, result);
    }
  } else if (schema.is_array()) {
    for (const auto &item : schema.as_array()) {
      collect_pinned_dialects(item, result);
    }
  }
}

constexpr auto NOT_AN_OBJECT{
    "Could not bundle to a container that is not an object"};

// A container is reached by naming properties, so a step that is a number is
// not a way in. Asking this before anything else is what makes the answer the
// same whether or not the document holds what comes before it, and whether or
// not the dialect reserves where it leads
auto check_container_steps(const Pointer &container) -> void {
  if (std::ranges::any_of(container, [](const auto &token) -> bool {
        return !token.is_property();
      })) {
    throw SchemaContainerError(container, NOT_AN_OBJECT);
  }
}

// Whether the container can hold what bundling puts there is a question of the
// document rather than of what bundling ends up embedding, so it is asked
// whether or not anything does. Embedding makes the objects it needs along the
// way, so a property that nothing holds yet is no reason to refuse, while a
// place that something else holds is one
auto check_container(const JSON &root, const Pointer &container) -> void {
  const auto *current{&root};
  for (const auto &token : container) {
    if (token.is_property()) {
      if (!current->is_object()) {
        throw SchemaContainerError(container, NOT_AN_OBJECT);
      }

      const auto *next{current->try_at(token.to_property())};
      if (next == nullptr) {
        return;
      }

      current = next;
    } else {
      throw SchemaContainerError(container, NOT_AN_OBJECT);
    }
  }

  if (!current->is_object()) {
    throw SchemaContainerError(container, NOT_AN_OBJECT);
  }
}

auto embed_schema(JSON &root, const Pointer &container,
                  const std::string_view identifier, JSON &&target,
                  const SchemaBundleOptions::Callback &callback)
    -> JSON::String {
  auto *current{&root};
  for (const auto &token : container) {
    if (token.is_property()) {
      current->assign_if_missing(token.to_property(), JSON::make_object());
      current = &current->at(token.to_property());
    } else {
      throw SchemaContainerError(container,
                                 "Could not bundle to a container that is not "
                                 "an object");
    }
  }

  if (!current->is_object()) {
    throw SchemaContainerError(container,
                               "Could not bundle to a container that is not "
                               "an object");
  }

  std::string key{identifier};
  // Ensure we get a definitions entry that does not exist
  while (current->defines(key)) {
    key += "/x";
  }

  current->assign(key, std::move(target));

  if (callback) {
    auto location{to_weak_pointer(container)};
    location.push_back(std::cref(key));
    callback(identifier, location);
  }

  return key;
}

// A resource that carries its own name cannot name a place of the document it
// sits in, so what it reaches instead is a copy of its own
struct PositionalCopy {
  Pointer host;
  JSON::String keyword;
  JSON::String identifier;
  std::optional<Pointer> tail;
  Pointer reference;
  // What the copy is read under where it came from, which the place it goes
  // to may not say for it
  JSON::String dialect;
  bool carries_its_dialect;
};

// Elevation lifts a resource out of the copy that lands, so a place named
// inside one is no longer where the reference that named it said. Saying the
// name of what moved reaches the same schema, but only where what the
// reference named is still inside it: one that named nothing to begin with
// names nothing here either
enum class Moved : std::uint8_t { Nothing, Rewritten, Nowhere };

auto rewrite_into_what_elevation_moved(
    const JSON &schema, const SchemaFrame &frame, const Pointer &tail,
    const std::vector<Elevated> &elevated, const Pointer &position,
    std::map<Pointer, JSON::String> &rewrites) -> Moved {
  auto covered{Moved::Nothing};
  for (const auto &moved : elevated) {
    if (!tail.starts_with(moved.at)) {
      continue;
    }

    // Whatever it names, the reference named it inside this one, so leaving it
    // where it is would leave a reference to nowhere behind
    covered = Moved::Nowhere;

    const auto &identifier{moved.identifier};
    const auto suffix{tail.resolve_from(moved.at)};
    const auto landed{frame.traverse(identifier)};
    if (!landed.has_value()) {
      continue;
    }

    auto within_resource{to_pointer(landed.value().get().pointer)};
    within_resource.push_back(Pointer{suffix});
    if (try_get(schema, within_resource) == nullptr) {
      continue;
    }

    rewrites.insert_or_assign(
        position,
        suffix.empty()
            ? identifier
            : JSON::String{to_uri(suffix, URI{identifier}).recompose()});
    return Moved::Rewritten;
  }

  return covered;
}

auto settle_positional_references(
    JSON &schema, const SchemaWalker &walker, const SchemaResolver &resolver,
    std::string_view default_dialect, std::string_view default_id,
    const SchemaFrame::Paths &paths, std::string_view default_base,
    const PositionalReferences &positional, std::uint64_t &remaining,
    const SchemaBundleOptions::Callback &callback) -> void {
  if (positional.unnamed.empty() && positional.elevations.empty()) {
    return;
  }

  // What the caller asked to be framed does not have to reach the container,
  // as a wrapper format keeps one outside every schema it frames. So each
  // landing is framed in its own right, which is also what the walk does when
  // it recurses into a remote
  SchemaFrame::Paths settling{paths};
  for (const auto &embedding : positional.embeddings) {
    const auto pointer{to_weak_pointer(embedding)};
    // Framing takes paths that do not sit inside one another, so a landing
    // that something already covers is left to the path that covers it. What
    // has been gathered so far is what each one is held against, as one
    // landing can sit within another once elevation has moved things around
    if (std::ranges::none_of(settling, [&pointer](const auto &path) -> bool {
          return pointer.starts_with(path) || path.starts_with(pointer);
        })) {
      settling.push_back(pointer);
    }
  }

  // A copy carries whatever references the schema it came from had, and what
  // those have to say depends on where the copy went rather than on where it
  // came from. So settling reads the document again after it has placed any,
  // until a round places none. One copy per resource and identifier is what
  // bounds this: elevation has already lifted every identified resource out of
  // what lands, so a copy carries no identifier and adds no resource that
  // could host copies of its own
  std::map<std::pair<Pointer, JSON::String>, JSON::String> placed;
  // Held back until every round is done, so that each copy is taken from a
  // schema that still spells its references the way it was resolved by. A
  // rewrite names a place by property rather than by index, so placing a copy
  // never moves one
  std::map<Pointer, JSON::String> rewrites;
  auto settled{false};
  while (!settled) {
    settled = true;
    const SchemaFrame frame{SchemaFrame::Mode::References,
                            schema,
                            walker,
                            resolver,
                            default_dialect,
                            default_id,
                            SchemaFrame::IdentifierMode::Additional,
                            settling,
                            default_base,
                            remaining};
    charge(remaining, frame);

    std::vector<PositionalCopy> copies;
    frame.for_each_unresolved_reference([&](const auto &pointer,
                                            const auto &reference) -> void {
      const JSON::String base{reference.base};
      const auto landing{positional.landings.find(base)};
      const auto elevated{positional.elevations.find(base)};
      if (landing == positional.landings.cend()) {
        // A remote that carries its own name is reached by that name wherever
        // it goes, so nothing recorded where it landed. What it carried can
        // still have been moved out from under a reference that named a place
        // inside it
        if (elevated != positional.elevations.cend() &&
            reference.fragment.has_value() &&
            !reference.fragment.value().empty()) {
          const auto moved{fragment_to_pointer(URI{reference.destination})};
          if (moved.has_value() &&
              rewrite_into_what_elevation_moved(
                  schema, frame, moved.value(), elevated->second,
                  to_pointer(pointer), rewrites) == Moved::Nowhere) {
            throw SchemaReferenceError(reference.destination,
                                       to_pointer(pointer),
                                       "Could not resolve schema reference");
          }
        }

        return;
      }

      std::optional<Pointer> tail;
      if (reference.fragment.has_value() &&
          !reference.fragment.value().empty()) {
        tail = fragment_to_pointer(URI{reference.destination});
        if (!tail.has_value()) {
          // Whatever the fragment names, it is not a place of the schema, and a
          // dialect that reserves no keyword for a name reserves none for an
          // anchor either, so there is nothing for it to have named
          throw SchemaReferenceError(reference.destination, to_pointer(pointer),
                                     "Could not resolve schema reference");
        }
      }

      auto within_document{landing->second};
      if (tail.has_value()) {
        within_document.push_back(Pointer{tail.value()});
      }

      if (try_get(schema, within_document) == nullptr) {
        // What the reference named may be inside a resource that elevation
        // lifted out of the copy that landed. That resource kept the name it
        // declared, which is the key it sat under, so saying that name and
        // whatever the reference named within it reaches the same schema from
        // wherever the reference is written
        if (tail.has_value() && elevated != positional.elevations.cend() &&
            rewrite_into_what_elevation_moved(
                schema, frame, tail.value(), elevated->second,
                to_pointer(pointer), rewrites) == Moved::Rewritten) {
          return;
        }

        throw SchemaReferenceError(reference.destination, to_pointer(pointer),
                                   "Could not resolve schema reference");
      }

      // What the reference reaches can be a resource that the remote carried,
      // which keeps the name it declared wherever it ends up. Saying that name
      // reads the same from every scope and survives the resource being moved
      // again, where saying the place does neither. What makes it that
      // resource rather than a subschema of it is the name leading back to the
      // same place
      const auto reached{frame.traverse(to_weak_pointer(within_document))};
      if (reached.has_value() && !reached.value().get().base.empty()) {
        const auto named{frame.traverse(reached.value().get().base)};
        if (named.has_value() &&
            to_pointer(named.value().get().pointer) == within_document) {
          rewrites.insert_or_assign(to_pointer(pointer),
                                    JSON::String{reached.value().get().base});
          return;
        }
      }

      // A pointer names a place of whichever resource is in force where it is
      // written, so what names the landing is where it sits within the resource
      // that holds it rather than within the document
      const auto landed{frame.traverse(to_weak_pointer(landing->second))};
      assert(landed.has_value());
      const auto &landed_location{landed.value().get()};
      const auto landed_resource{frame.traverse(landed_location.base)};
      auto target{landed_resource.has_value()
                      ? to_pointer(landed_location.pointer)
                            .resolve_from(to_pointer(
                                landed_resource.value().get().pointer))
                      : to_pointer(landed_location.pointer)};
      if (tail.has_value()) {
        target.push_back(Pointer{tail.value()});
      }

      // Written on its own that pointer says the same thing only from a scope
      // that resolves against the same base. From anywhere else it takes the
      // name of the resource in front of it, and where that resource answers to
      // no name there is nothing to put there
      const auto enclosing{frame.traverse(pointer.initial())};
      auto value{to_uri(target)};
      if (enclosing.has_value() &&
          enclosing.value().get().base != landed_location.base) {
        if (landed_location.base.empty()) {
          // The landing can only be named on its own from the scope the
          // document itself answers to, and a resource that carries its own
          // name is not that scope. What such a resource needs is its own copy,
          // in whichever location its own dialect reserves for one, which is
          // then a place of that resource and travels with it
          const auto &enclosing_location{enclosing.value().get()};
          // A pointer written here names a place of the resource in force,
          // which is the one that needs the copy. That is not always the
          // subschema holding the reference, as a copy placed in a resource
          // is a subschema of it and carries references of its own
          const auto host_location{frame.traverse(enclosing_location.base)};
          assert(host_location.has_value());
          const auto host{to_pointer(host_location.value().get().pointer)};
          const auto keyword{definitions_keyword(
              walker,
              frame.vocabularies(host_location.value().get(), resolver))};
          if (keyword.empty()) {
            throw SchemaReferenceError(
                reference.destination, to_pointer(pointer),
                "Could not reach a schema that this dialect embeds without an "
                "identifier from a resource that reserves nowhere to put one");
          }

          copies.push_back(
              {.host = host,
               .keyword = JSON::String{keyword},
               .identifier = base,
               .tail = tail,
               .reference = to_pointer(pointer),
               .dialect = JSON::String{landed_location.dialect},
               .carries_its_dialect = host_location.value().get().dialect !=
                                      landed_location.dialect});
          return;
        }

        URI absolute{JSON::String{landed_location.base}};
        absolute.fragment(value.fragment().value());
        value = std::move(absolute);
      }

      rewrites.insert_or_assign(to_pointer(pointer),
                                JSON::String{value.recompose()});
    });

    // One copy per resource that needs one, however many of its references name
    // the same schema
    for (const auto &copy : copies) {
      auto host_container{copy.host};
      host_container.push_back(copy.keyword);
      const std::pair<Pointer, JSON::String> seen{host_container,
                                                  copy.identifier};
      const auto &landing{positional.landings.at(copy.identifier)};

      // A resource the landing carries answers to the name it declared, and
      // one identity in two places is not a document anything can read. So
      // the copy says that name where the resource sat, which reaches the
      // one that stayed, and a resource that names its own anonymous parent
      // stops the copying there rather than inside itself
      std::map<Pointer, JSON::String> carried;
      frame.for_each_resource(
          [&carried, &landing](const auto &, const auto &location) -> void {
            const auto position{to_pointer(location.pointer)};
            if (position.size() <= landing.size() ||
                !position.starts_with(landing)) {
              return;
            }

            carried.insert_or_assign(position.resolve_from(landing),
                                     JSON::String{location.base});
          });

      // What the landing already holds is what each of those names reaches.
      // Holding the two against each other is what keeps a name standing in
      // for the same schema rather than for another one that answers to it,
      // and it is asked wherever one of them is said, as a copy is not what
      // brings two schemas of one name about
      for (const auto &[position, name] : carried) {
        const auto resolved{frame.traverse(name)};
        if (!resolved.has_value() ||
            get(schema, to_pointer(resolved.value().get().pointer)) !=
                get(schema, landing.concat(position))) {
          throw SchemaError(
              "Conflicting embedded resources with the same identifier");
        }
      }

      // What the reference named can sit inside a resource that answers to a
      // name of its own, which reaches the resource that stayed from anywhere
      // at all. Saying that name is both shorter and the whole of what this
      // reference needs, so nothing has to be copied on its behalf. The name
      // the copy would say is the outermost one, so that is the one to say
      // here too
      std::optional<JSON::String> spelled;
      if (copy.tail.has_value()) {
        for (const auto &[position, name] : carried) {
          if (!copy.tail.value().starts_with(position)) {
            continue;
          }

          const auto suffix{copy.tail.value().resolve_from(position)};
          spelled = suffix.empty()
                        ? name
                        : JSON::String{to_uri(suffix, URI{name}).recompose()};
          break;
        }
      }

      if (!spelled.has_value()) {
        if (!placed.contains(seen)) {
          auto duplicate{JSON{get(schema, landing)}};

          // A resource that another carried resource holds is reached through
          // the name of the one that holds it, so saying the outermost name is
          // saying all of them
          std::optional<Pointer> said;
          for (const auto &[position, name] : carried) {
            if (said.has_value() && position.starts_with(said.value())) {
              continue;
            }

            auto named{JSON::make_object()};
            named.assign("$ref", JSON{name});
            set(duplicate, position, std::move(named));
            said = position;
          }

          // Where it came from said what it was by sitting where it sat, and
          // the place it goes to says something else, so the copy has to say it
          if (copy.carries_its_dialect) {
            mark_dialect_of_every_subschema(duplicate, walker, resolver,
                                            copy.dialect, remaining);
          }

          placed.emplace(seen,
                         embed_schema(schema, host_container, copy.identifier,
                                      std::move(duplicate), callback));
          settled = false;
        }

        Pointer target{copy.keyword};
        target.push_back(JSON::String{placed.at(seen)});
        if (copy.tail.has_value()) {
          target.push_back(Pointer{copy.tail.value()});
        }

        spelled = JSON::String{to_uri(target).recompose()};
      }

      rewrites.insert_or_assign(copy.reference, std::move(spelled).value());
    }
  }

  for (const auto &[pointer, value] : rewrites) {
    set(schema, pointer, JSON{value});
  }
}

// A dialect spelled two ways is one dialect, so each side of a comparison says
// the canonical spelling of whatever it is read under. What a frame answers is
// canonical already, where what a document declares for itself has been
// through nothing of the sort
auto say_what_it_is_read_under(JSON &schema, const std::string_view dialect)
    -> void {
  if (!schema.is_object()) {
    return;
  }

  schema.assign("$schema",
                JSON{canonical_uri(declared_dialect(schema, dialect))});
  // Both sides are already known to claim the same name, and one of them may
  // spell it relatively where the other was resolved, so how each spells it
  // says nothing about whether they are the same schema
}

// What the document already holds under a name, wherever it sits. The
// container alone cannot answer that, as a caller may hold a resource
// anywhere, and holding the two against each other is what keeps a name
// standing for one schema rather than for two that disagree
auto hold_against_what_is_known(const JSON &root, const Pointer &container,
                                const JSON &value, const KnownResources &known,
                                const SchemaWalker &walker,
                                const SchemaResolver &resolver,
                                std::string_view default_dialect,
                                std::string_view remote_dialect_uri,
                                const JSON::String &identifier,
                                std::uint64_t &remaining) -> bool {
  // What a later pass will place is not in the document yet, so a candidate
  // arriving first would otherwise be dropped against nothing at all
  const auto pending{known.pending.find(canonical_uri(identifier))};
  if (pending != known.pending.cend()) {
    auto expected{pending->second.schema};
    say_what_it_is_read_under(expected, pending->second.dialect);
    auto candidate{value};
    say_what_it_is_read_under(candidate, remote_dialect_uri);
    if (expected != candidate) {
      throw SchemaError(
          "Conflicting embedded resources with the same identifier");
    }

    return true;
  }

  // Nothing the caller framed reaches the place bundling puts things, by
  // design for a wrapper format, so what is already there is asked of the
  // container itself rather than of the document around it
  const auto *held_container{try_get(root, container)};
  if (held_container != nullptr && held_container->is_object()) {
    for (const auto &entry : held_container->as_object()) {
      // Rule out what cannot match, and what framing would reject, before
      // paying for a frame. What a container calls an entry is no guide to
      // what that entry identifies, since a caller may hold one under a name
      // of its own choosing
      if (!entry.second.is_object()) {
        continue;
      }

      const auto *declared{entry.second.try_at("$id"sv, JSONSCHEMA_HASH_ID)};
      if (declared == nullptr) {
        declared = entry.second.try_at("id"sv, JSONSCHEMA_HASH_LEGACY_ID);
      }

      if (declared == nullptr || !declared->is_string() ||
          canonical_uri(declared->to_string()) != canonical_uri(identifier) ||
          !URI{declared->to_string()}.is_absolute()) {
        continue;
      }

      SchemaFrame entry_frame{SchemaFrame::Mode::Root,
                              entry.second,
                              walker,
                              resolver,
                              remote_dialect_uri,
                              "",
                              SchemaFrame::IdentifierMode::Additional,
                              {EMPTY_WEAK_POINTER},
                              "",
                              remaining};
      charge(remaining, entry_frame);
      if (canonical_uri(entry_frame.root()) != canonical_uri(identifier)) {
        continue;
      }

      auto expected{entry.second};
      say_what_it_is_read_under(expected, remote_dialect_uri);
      auto candidate{value};
      say_what_it_is_read_under(candidate, remote_dialect_uri);
      if (expected != candidate) {
        throw SchemaError(
            "Conflicting embedded resources with the same identifier");
      }

      return true;
    }
  }

  const SchemaFrame frame{SchemaFrame::Mode::Locations,
                          root,
                          walker,
                          resolver,
                          default_dialect,
                          known.default_id,
                          SchemaFrame::IdentifierMode::Additional,
                          known.paths,
                          known.default_base,
                          remaining};
  charge(remaining, frame);
  // A name is looked for as it is written and as it canonically is, since
  // what the document declares has been through neither
  const auto canonical{canonical_uri(identifier)};
  const auto location{frame.traverse(identifier).has_value()
                          ? frame.traverse(identifier)
                          : frame.traverse(canonical)};
  if (!location.has_value()) {
    return false;
  }

  // One of the two can declare the dialect it is read under while the other
  // inherits it, and either can spell it its own way, none of which says
  // anything about whether they are the same schema
  const auto &held_location{location.value().get()};
  auto held{get(root, to_pointer(held_location.pointer))};
  say_what_it_is_read_under(held, held_location.dialect);
  auto candidate{value};
  say_what_it_is_read_under(candidate, remote_dialect_uri);

  if (held != candidate) {
    throw SchemaError(
        "Conflicting embedded resources with the same identifier");
  }

  return true;
}

auto elevate_embedded_resources(
    JSON &remote, JSON &root, const Pointer &container,
    const SchemaBaseDialect remote_dialect, const SchemaWalker &walker,
    const SchemaResolver &resolver, std::string_view default_dialect,
    std::string_view remote_identifier, KnownResources &known,
    PositionalReferences &positional, std::uint64_t &remaining,
    const SchemaBundleOptions::Callback &callback) -> void {
  // A remote can say what it is read under by a mark rather than by
  // `$schema`, which is the only way a dialect that reserves no keyword for
  // saying so can be carried, and what it says is where its definitions live
  const auto remote_dialect_uri{dialect(remote, default_dialect, true)};
  const auto keyword{definitions_keyword(
      walker, vocabularies_with_embedded(remote, resolver, remote_dialect,
                                         remote_dialect_uri))};
  const JSON::String keyword_string{keyword};
  if (keyword.empty() || !remote.is_object() ||
      !remote.defines(keyword_string) ||
      !remote.at(keyword_string).is_object()) {
    return;
  }

  auto &defs{remote.at(keyword_string)};

  // A schema finds a meta-schema it pins by looking for it among the locations
  // the specifications reserve, at the top of the document it is reading. A
  // dialect whose own reserved location is neither of those takes its embedded
  // resources somewhere that search does not go, so what is pinned has to stay
  // where whatever pinned it can still find it
  const auto container_is_searched{
      container.size() == 1 && container.back().is_property() &&
      (container.back().to_property() == "$defs" ||
       container.back().to_property() == "definitions")};
  std::unordered_set<JSON::String> pinned;
  if (!container_is_searched) {
    collect_pinned_dialects(remote, pinned);
  }

  // An entry can name a sibling as the dialect it is read under, and framing
  // one on its own reaches only what the caller handed over, so the entries
  // beside it are offered alongside that. What a key says an entry is only
  // counts where the entry says the same, which is what framing below settles
  const SchemaResolver entry_resolver{
      [&defs,
       &resolver](const std::string_view identifier) -> SchemaResolverResult {
        const auto *sibling{defs.try_at(identifier)};
        if (sibling != nullptr && sibling->is_object() &&
            sibling->defines("$id"sv, JSONSCHEMA_HASH_ID) &&
            sibling->at("$id"sv, JSONSCHEMA_HASH_ID).is_string() &&
            canonical_uri(
                sibling->at("$id"sv, JSONSCHEMA_HASH_ID).to_string()) ==
                canonical_uri(identifier)) {
          return *sibling;
        }

        return resolver(identifier);
      }};

  std::vector<std::pair<JSON::String, bool>> to_extract;
  std::vector<Removed> to_remove;
  for (const auto &entry : defs.as_object()) {
    const auto &key{entry.first};
    const auto &value{entry.second};
    // Framing says what an entry identifies, and which keyword carries that is
    // the dialect's answer rather than this loop's, so the presence of either
    // is only a reason to ask. Most entries identify nothing at all, which is
    // what rules them out before paying for a frame
    if (!value.is_object()) {
      continue;
    }

    const auto *declared_id{value.try_at("$id"sv, JSONSCHEMA_HASH_ID)};
    if (declared_id == nullptr) {
      declared_id = value.try_at("id"sv, JSONSCHEMA_HASH_LEGACY_ID);
    }

    if (declared_id == nullptr || !declared_id->is_string()) {
      continue;
    }

    // The remote's dialect is what an entry that declares none inherits, so
    // hand it to the frame as the default rather than falling back after
    SchemaFrame entry_frame{SchemaFrame::Mode::Root,
                            value,
                            walker,
                            entry_resolver,
                            remote_dialect_uri,
                            "",
                            SchemaFrame::IdentifierMode::Additional,
                            {EMPTY_WEAK_POINTER},
                            "",
                            remaining};
    charge(remaining, entry_frame);
    const auto &identifier{entry_frame.root()};
    if (identifier.empty() || !URI{identifier}.is_absolute()) {
      continue;
    }

    const JSON::String identifier_string{identifier};
    // Only an entry whose key says what it identifies can be elevated, since
    // the key is where it would land. What a key says is no guide to what an
    // entry identifies, though, so one the document already holds under the
    // name this one identifies is still held against what is there
    if (identifier != key) {
      if (is_known(known, identifier_string)) {
        // Dropping one that nothing was held against is how a schema that
        // disagrees goes missing, so it stays where it is unless something
        // answered for it
        if (hold_against_what_is_known(root, container, value, known, walker,
                                       resolver, default_dialect,
                                       remote_dialect_uri, identifier_string,
                                       remaining)) {
          to_remove.push_back({.key = key, .identifier = identifier_string});
        }
      }

      record(known, identifier_string, identifier_string);
      continue;
    }

    if (pinned.contains(canonical_uri(identifier))) {
      // Staying put is the only way this one goes on being found, so the
      // document cannot also hold it elsewhere: one identity in two places is
      // not a document anything can read
      if (is_known(known, identifier_string)) {
        throw SchemaError("A meta-schema that has to stay within the schema "
                          "that pins it cannot be embedded elsewhere too");
      }

      // What stays behind is still in the document under the identity it
      // carries, so a reference to it from anywhere else is answered by what
      // is already here rather than by asking the resolver for another copy
      record(known, identifier_string, identifier_string);
      continue;
    }

    const auto defines_dialect{
        value.defines("$schema"sv, JSONSCHEMA_HASH_SCHEMA)};
    if (is_known(known, identifier_string)) {
      // What the document holds under this name can be anywhere, including the
      // container bundling puts things in, so there is one question rather
      // than one per place
      if (!hold_against_what_is_known(
              root, container, value, known, walker, resolver, default_dialect,
              remote_dialect_uri, identifier_string, remaining)) {
        continue;
      }

      to_remove.push_back({.key = key, .identifier = identifier_string});
    } else {
      to_extract.emplace_back(key, !defines_dialect);
      record(known, identifier_string, identifier_string);
      expect(known, identifier_string, value, remote_dialect_uri);
    }
  }

  for (const auto &[key, needs_dialect] : to_extract) {
    positional.elevations[JSON::String{remote_identifier}].push_back(
        {.at = Pointer{keyword_string, key}, .identifier = key});
    auto value{std::move(defs.at(key))};
    defs.erase(key);
    // Otherwise the elevated resource would be re-interpreted under the
    // dialect of the schema it gets embedded into, which can differ from
    // the dialect it inherited from the remote it was elevated out of
    if (needs_dialect) {
      value.assign("$schema",
                   JSON{declared_dialect(value, remote_dialect_uri)});
    }

    const auto landing{
        embed_schema(root, container, key, std::move(value), callback)};
    placed(known, key);
    positional.embeddings.push_back(container.concat(landing));
  }

  for (const auto &removed : to_remove) {
    positional.elevations[JSON::String{remote_identifier}].push_back(
        {.at = Pointer{keyword_string, removed.key},
         .identifier = removed.identifier});
    defs.erase(removed.key);
  }

  if (defs.empty()) {
    remote.erase(JSON::String{keyword});
  }
}

// A schema that names a dialect which gives the keyword that named it no
// meaning cannot be taken at its word, and framing refuses it wherever it
// meets one, naming a place of whatever document it was walking. Where that
// document is one bundling fetched, the caller holds neither it nor that
// place, so what names the schema is the identifier it was fetched by and the
// reference that went looking for it
auto impossible_dialect_of_fetched(const JSON::String &identifier,
                                   const Pointer &origin)
    -> SchemaReferenceError {
  return {identifier, origin,
          "The referenced schema declares a dialect that does not define the "
          "keyword that declares it"};
}

// What bundling fetched and has yet to put anywhere, alongside the name it
// answers to, the name it was fetched by, and the reference that went looking
// for it. The last two are what a refusal the fetched document brings on has
// to name, and the first two part ways wherever the document declares a name
// of its own
struct FetchedRemote {
  JSON schema;
  JSON::String identifier;
  JSON::String fetched_by;
  SchemaBaseDialect base_dialect;
  Pointer origin;
};

auto embed_references(
    JSON &root, const Pointer &container, JSON &subschema,
    const SchemaWalker &walker, const SchemaResolver &resolver,
    const SchemaBundleOptions::Mode mode, std::string_view default_dialect,
    std::string_view default_id, const SchemaFrame::Paths &paths,
    std::string_view default_base, KnownResources &known,
    PositionalReferences &positional, std::string_view container_dialect,
    std::uint64_t &remaining, const SchemaBundleOptions::Callback &callback,
    const std::size_t depth = 0) -> void {
  // Create a fresh frame for each schema we analyze to avoid key collisions
  // between different schemas that have references at the same pointer paths
  static const SchemaFrame::Paths NESTED_PATHS{EMPTY_WEAK_POINTER};
  const SchemaFrame frame{
      SchemaFrame::Mode::References, subschema, walker, resolver,
      default_dialect, default_id, SchemaFrame::IdentifierMode::Additional,
      // We only want to frame in "wrapper" mode for the top
      // level object, which is also the only one that the
      // base the caller retrieved it from applies to, as
      // every remote carries the identity it was resolved by
      depth == 0 ? paths : NESTED_PATHS,
      depth == 0 ? default_base : std::string_view{}, remaining};
  charge(remaining, frame);

  // A fetched document answers to every name it declares, wherever that name
  // is declared within it, and all of it goes in together. So each one is held
  // against what the document already answers to before any of it does. What
  // sits in the place the dialect reserves for definitions is left to the walk
  // that lifts those out, which can drop one that agrees rather than refuse it,
  // as nothing reads an entry there by where it sits
  if (depth > 0) {
    const auto &carrier{frame.root_location()};
    const JSON::String carried_keyword{
        carrier.has_value()
            ? definitions_keyword(
                  walker, frame.vocabularies(carrier.value().get(), resolver))
            : std::string_view{}};
    frame.for_each_resource([&](const auto &, const auto &location) -> void {
      const auto position{to_pointer(location.pointer)};
      if (position.empty() ||
          (position.size() == 2 && position.at(0).is_property() &&
           position.at(0).to_property() == carried_keyword)) {
        return;
      }

      const JSON::String carried{location.base};
      // Where this one sits is what the schema says, so there is nothing
      // that can be taken out of it, and one name in two places is not a
      // document anything can read however well the two agree. Which of
      // them bundling met first is no reason to answer differently
      if (is_known(known, carried)) {
        throw SchemaError(
            "Conflicting embedded resources with the same identifier");
      }

      record(known, carried, carried);
    });
  }

  std::vector<FetchedRemote> deferred;
  std::vector<std::pair<Pointer, JSON::String>> ref_rewrites;

  frame.for_each_unresolved_reference([&](const auto &pointer,
                                          const auto &reference) -> void {
    // We don't want to bundle official schemas, as we can expect
    // virtually all implementations to understand them out of the box.
    // Depending on the bundling strategy, we may skip meta-schemas entirely
    if (is_skippable_metaschema_reference(mode, pointer,
                                          reference.destination)) {
      return;
    }

    // If we can't find the destination but there is a base and we can
    // find base, then we are facing an unresolved fragment
    if (!reference.base.empty() && frame.traverse(reference.base).has_value()) {
      throw SchemaReferenceError(reference.destination, to_pointer(pointer),
                                 "Could not resolve schema reference");
    }

    if (reference.base.empty()) {
      throw SchemaReferenceError(reference.destination, to_pointer(pointer),
                                 "Could not resolve schema reference");
    }

    assert(!reference.base.empty());
    const JSON::String identifier{reference.base};

    if (is_recorded(known, identifier)) {
      // A target that went in without a name answers to nothing, so this
      // reference is settled against the finished document rather than here.
      // Spelling it out in full is what lets that pass recognise it, since a
      // relative spelling reads against wherever the schema ends up
      if (positional.unnamed.contains(identifier)) {
        ref_rewrites.emplace_back(to_pointer(pointer),
                                  JSON::String{reference.destination});
        return;
      }

      const auto &mapped_id{answers_to(known, identifier)};
      if (mapped_id != identifier) {
        ref_rewrites.emplace_back(
            to_pointer(pointer),
            rebase_reference(mapped_id, reference.fragment));
      } else if (spells_another_document_relatively(reference)) {
        ref_rewrites.emplace_back(to_pointer(pointer),
                                  JSON::String{reference.destination});
      }

      return;
    }

    auto resolved{resolver(identifier)};
    if (!resolved.has_value()) {
      if (frame.traverse(identifier).has_value()) {
        throw SchemaReferenceError(reference.destination, to_pointer(pointer),
                                   "Could not resolve schema reference");
      }

      throw SchemaResolutionError(
          identifier, "Could not resolve the reference to an external schema");
    }

    // Bundling rewrites the schema before embedding it, so it needs a copy
    // it owns rather than whatever the resolver chose to hand back
    auto remote{std::move(resolved).to_owned()};
    if (!remote.is_object() && !remote.is_boolean()) {
      throw SchemaReferenceError(identifier, to_pointer(pointer),
                                 "The JSON document is not a valid JSON "
                                 "Schema");
    }

    std::optional<SchemaFrame> remote_root_frame;
    try {
      remote_root_frame.emplace(
          SchemaFrame::Mode::Root, remote, walker, resolver, default_dialect,
          "", SchemaFrame::IdentifierMode::Additional,
          SchemaFrame::Paths{EMPTY_WEAK_POINTER}, "", remaining);
      charge(remaining, remote_root_frame.value());
    } catch (const SchemaUnknownBaseDialectError &) {
      throw SchemaReferenceError(identifier, to_pointer(pointer),
                                 "The JSON document is not a valid JSON "
                                 "Schema");
    }

    const auto &remote_root{remote_root_frame->root_location().value().get()};
    // Working these out can cost a meta-schema to resolve, so it happens once
    const auto &remote_vocabularies{
        remote_root_frame->vocabularies(remote_root, resolver)};

    const auto remote_base_dialect{remote_root.base_dialect};
    auto remote_id = remote_root_frame->root();

    // Whether what the reference names within the remote carries a name of its
    // own, which settles how the reference reads without settling where the
    // remote goes
    bool names_a_resource{false};

    // If the reference has a fragment, verify it exists in the remote
    // schema
    if (reference.fragment.has_value()) {
      // A pointer fragment names a place of the document, and the document can
      // answer for that on its own without paying to frame it
      const auto fragment_pointer{
          fragment_to_pointer(URI{reference.destination})};
      const auto *named_value{fragment_pointer.has_value()
                                  ? try_get(remote, fragment_pointer.value())
                                  : nullptr};
      bool exists{named_value != nullptr};

      // An anchor is not a place of the document, and the drafts that spell
      // identifiers as `id` let one look just like a pointer, so a miss above
      // still has to ask the frame. Only the anchors of the remote matter
      // here, rather than every pointer of it
      if (!exists) {
        try {
          const SchemaFrame remote_frame{
              SchemaFrame::Mode::Locations,
              remote,
              walker,
              resolver,
              default_dialect,
              identifier,
              SchemaFrame::IdentifierMode::Additional,
              {EMPTY_WEAK_POINTER},
              "",
              remaining};
          charge(remaining, remote_frame);
          exists = remote_frame.traverse(reference.destination).has_value();
        } catch (const SchemaDialectImpossibleError &) {
          throw impossible_dialect_of_fetched(identifier, to_pointer(pointer));
        }
      }

      if (!exists) {
        throw SchemaReferenceError(reference.destination, to_pointer(pointer),
                                   "Could not resolve schema reference");
      }

      // Where what it named carries a name of its own, that name is what
      // reaches it from anywhere, and goes on doing so wherever the bundling
      // puts it. Saying where it sits instead would only hold while it sat
      // there, which is not something a reference into a remote can rely on.
      // Nothing that declares no identifier can be answering to one, and
      // ruling that out costs a property lookup where asking the frame costs
      // a walk of the whole remote
      if (named_value != nullptr && named_value->is_object() &&
          (named_value->defines("$id"sv, JSONSCHEMA_HASH_ID) ||
           named_value->defines("id"sv, JSONSCHEMA_HASH_LEGACY_ID))) {
        try {
          const SchemaFrame located{SchemaFrame::Mode::Locations,
                                    remote,
                                    walker,
                                    resolver,
                                    default_dialect,
                                    identifier,
                                    SchemaFrame::IdentifierMode::Additional,
                                    {EMPTY_WEAK_POINTER},
                                    "",
                                    remaining};
          charge(remaining, located);
          const auto named{
              located.traverse(to_weak_pointer(fragment_pointer.value()))};
          if (named.has_value() &&
              named.value().get().type == SchemaFrame::LocationType::Resource &&
              to_pointer(named.value().get().pointer) ==
                  fragment_pointer.value() &&
              named.value().get().base != identifier) {
            ref_rewrites.emplace_back(to_pointer(pointer),
                                      JSON::String{named.value().get().base});
            names_a_resource = true;
          }
        } catch (const SchemaDialectImpossibleError &) {
          throw impossible_dialect_of_fetched(identifier, to_pointer(pointer));
        }
      }
    }

    JSON::String effective_id{remote_id.empty() ? JSON::String{identifier}
                                                : JSON::String{remote_id}};

    // What was fetched under one name can answer to another that the document
    // already holds, and one identity in two places is not a document anything
    // can read. Bundling does not keep what it put in, so it cannot tell two
    // documents fetched under different names apart, and saying so is the only
    // answer that never drops a disagreement between them
    if (is_known(known, effective_id)) {
      throw SchemaError(
          "Conflicting embedded resources with the same identifier");
    }

    // Whether the dialect of the remote has a way to name it at all, which
    // decides between giving it a name and pointing at wherever it lands
    const auto dialect_allows_naming{dialect_defines_identifier(
        walker, remote_vocabularies, remote_base_dialect)};

    // What the remote is read under is what framing made of it, which takes in
    // a mark that a previous bundling left on it as well as what it declares
    // and what it was resolved with. Asking the document again would miss the
    // mark, as no keyword of the schema carries it
    const JSON::String remote_dialect_uri{remote_root.dialect};

    if (remote.is_object()) {
      if (dialect_allows_naming) {
        // Otherwise the embedded resource would be re-interpreted under the
        // dialect of the schema it gets embedded into, which can differ from
        // the default dialect that the remote was resolved with
        if (!remote.defines("$schema"sv, JSONSCHEMA_HASH_SCHEMA)) {
          remote.assign("$schema", JSON{remote_dialect_uri});
        }

        schema_reidentify(remote, effective_id, remote_base_dialect);
      } else if (remote_dialect_uri != container_dialect) {
        // Marking is what carries a dialect that no keyword of the schema can
        // declare. Where whatever holds the container already reads under the
        // dialect the remote was read under, inheritance says the same thing
        // for free
        try {
          mark_dialect_of_every_subschema(remote, walker, resolver,
                                          remote_dialect_uri, remaining);
        } catch (const SchemaDialectImpossibleError &) {
          throw impossible_dialect_of_fetched(identifier, to_pointer(pointer));
        }
      }
    }

    // Whether what went in can answer to the name it was resolved by is a
    // question of the remote, not of the reference that happened to bring it
    // in. A reference that names a resource the remote carries is settled by
    // that name, which says nothing about the remote around it, and a later
    // reference to the remote itself still has to be settled by place
    if (!dialect_allows_naming) {
      assert(effective_id == identifier);
      positional.unnamed.insert(identifier);
    }

    if (names_a_resource) {
      // Settled above, and by a name rather than by a place, so none of what
      // follows has anything to add to it
    } else if (effective_id != identifier) {
      // A remote that answers to a name of its own keeps it, so the reference
      // is rebased onto that rather than onto the one it was resolved by. Only
      // a dialect that can declare a name reaches here, as one that cannot
      // leaves the two the same
      ref_rewrites.emplace_back(
          to_pointer(pointer),
          rebase_reference(effective_id, reference.fragment));
    } else if (!dialect_allows_naming ||
               spells_another_document_relatively(reference)) {
      // Either the remote cannot declare the name it was resolved by, so what
      // this reference points at is settled against the finished document
      // instead, or it keeps that name while the reference spells it as a
      // place of another document. Spelling it out in full is what the later
      // pass reads, since a relative spelling reads against wherever the
      // schema ends up
      ref_rewrites.emplace_back(to_pointer(pointer),
                                JSON::String{reference.destination});
    }

    record(known, identifier, effective_id);
    record(known, effective_id, effective_id);
    expect(known, effective_id, remote, remote_dialect_uri);
    deferred.push_back({.schema = std::move(remote),
                        .identifier = std::move(effective_id),
                        .fetched_by = identifier,
                        .base_dialect = remote_base_dialect,
                        .origin = to_pointer(pointer)});
  });

  // Whatever the walk above reaches is on its way into the document, so its
  // spelling is settled there rather than here. What is left is a reference
  // whose target the document already holds, which still has to name it in a
  // way that does not depend on where the document came from
  frame.for_each_reference(
      [&](const auto, const auto &pointer, const auto &reference) -> void {
        if (!frame.traverse(reference.destination).has_value()) {
          return;
        }

        // A reference that resolved against the identity its own document was
        // read under reads as settled here, and is not. Spelling it out in
        // full is what keeps what it meant, as the embedding takes that
        // identity away and a fragment left on its own would then name a place
        // of whatever document the schema lands in. Where it points is settled
        // once everything has landed
        if (positional.unnamed.contains(JSON::String{reference.base})) {
          ref_rewrites.emplace_back(to_pointer(pointer),
                                    JSON::String{reference.destination});
          return;
        }

        if (spells_another_document_relatively(reference)) {
          ref_rewrites.emplace_back(to_pointer(pointer),
                                    JSON::String{reference.destination});
        }
      });

  for (auto &[rewrite_pointer, rewrite_value] : ref_rewrites) {
    set(subschema, rewrite_pointer, JSON{rewrite_value});
  }

  for (auto &entry : deferred) {
    // Whatever the walks below meet, they meet inside a document that was
    // fetched rather than given
    try {
      embed_references(root, container, entry.schema, walker, resolver, mode,
                       default_dialect, entry.identifier, paths, default_base,
                       known, positional, container_dialect, remaining,
                       callback, depth + 1);
      elevate_embedded_resources(entry.schema, root, container,
                                 entry.base_dialect, walker, resolver,
                                 default_dialect, entry.identifier, known,
                                 positional, remaining, callback);
    } catch (const SchemaDialectImpossibleError &) {
      throw impossible_dialect_of_fetched(entry.fetched_by, entry.origin);
    }

    placed(known, entry.identifier);
    const auto key{embed_schema(root, container, entry.identifier,
                                std::move(entry.schema), callback)};
    auto landing{container.concat(key)};
    if (positional.unnamed.contains(entry.identifier)) {
      positional.landings.emplace(entry.identifier, landing);
    }
    positional.embeddings.push_back(std::move(landing));
  }
}

// Where a reference overrides whatever sits beside it, a container beside one
// is read by nobody, so a root that is nothing but a reference becomes a branch
// that can hold both. A root that holds more than the reference has nothing
// that can be done with it. Says whether the document now reads differently
auto prepare_root_reference(JSON &schema, const SchemaBaseDialect base_dialect)
    -> bool {
  if (!ref_overrides_adjacent_keywords(base_dialect) || !schema.is_object() ||
      !schema.defines("$ref"sv, JSONSCHEMA_HASH_REF)) {
    return false;
  }

  if (schema.size() != 1) {
    throw SchemaError(
        "Cannot bundle a JSON Schema Draft 7 or older with a top-level "
        "`$ref` (which overrides sibling keywords) without introducing "
        "undefined behavior");
  }

  const auto is_draft3{base_dialect == SchemaBaseDialect::JSON_SCHEMA_DRAFT_3 ||
                       base_dialect ==
                           SchemaBaseDialect::JSON_SCHEMA_DRAFT_3_HYPER};
  auto branches{JSON::make_array()};
  branches.push_back(schema);
  schema.at("$ref"sv, JSONSCHEMA_HASH_REF).into(std::move(branches));
  schema.rename("$ref", is_draft3 ? "extends" : "allOf");
  return true;
}

auto bundle_internal(JSON &schema, const SchemaWalker &walker,
                     const SchemaResolver &resolver,
                     const SchemaBundleOptions::Mode mode,
                     std::string_view default_dialect,
                     std::string_view default_id,
                     const std::optional<Pointer> &default_container,
                     const SchemaFrame::Paths &paths,
                     std::string_view default_base, std::uint64_t &remaining,
                     const SchemaBundleOptions::Callback &callback) -> void {
  // Pre-scan the schema to find any already-embedded schemas and mark them
  // as bundled to avoid re-embedding them. This includes the root schema itself
  // and any schemas already embedded within it
  KnownResources known{.names = {},
                       .declared = {},
                       .pending = {},
                       .paths = paths,
                       .default_id = JSON::String{default_id},
                       .default_base = JSON::String{default_base}};
  PositionalReferences positional;
  std::optional<SchemaFrame> initial_frame;
  initial_frame.emplace(SchemaFrame::Mode::Locations, schema, walker, resolver,
                        default_dialect, default_id,
                        SchemaFrame::IdentifierMode::Additional, paths,
                        default_base, remaining);
  charge(remaining, initial_frame.value());
  initial_frame->for_each_resource_uri([&known](const auto &uri) -> void {
    record(known, uri, JSON::String{uri});
  });
  if (default_container.has_value()) {
    // This is undefined behavior
    assert(!default_container.value().empty());
    check_container_steps(default_container.value());
    // Whatever bundling embeds has to land somewhere that framing reaches
    // again, or a later pass cannot see it and embeds a second copy. So a
    // container has to be a keyword that the dialect reserves for schema
    // definitions, declared on a schema that the given paths cover. A wrapper
    // format keeps its container outside every schema it frames, where JSON
    // Schema has nothing to say about where things may go
    const auto container{to_weak_pointer(default_container.value())};
    if (std::ranges::any_of(paths, [&container](const auto &path) -> bool {
          return container.starts_with(path);
        })) {
      const auto parent_pointer{default_container.value().initial()};
      const auto parent{
          initial_frame->traverse(to_weak_pointer(parent_pointer))};
      if (!parent.has_value() ||
          (parent.value().get().type != SchemaFrame::LocationType::Resource &&
           parent.value().get().type != SchemaFrame::LocationType::Subschema) ||
          walker(default_container.value().back().to_property(),
                 initial_frame->vocabularies(parent.value().get(), resolver))
                  .type != SchemaKeywordType::LocationMembers) {
        throw SchemaContainerError(
            default_container.value(),
            "Could not bundle to a container that the dialect does not "
            "reserve for schema definitions");
      }

      // Where a reference overrides what sits beside it, a container beside
      // one is read by nobody. A root that is nothing but a reference is made
      // to hold both further down, as there is nowhere else a container could
      // go, and anywhere deeper than the place a reference sits goes unframed
      // and is answered above
      const auto *parent_value{try_get(schema, parent_pointer)};
      if (!parent_pointer.empty() && parent_value != nullptr &&
          parent_value->is_object() &&
          parent_value->defines("$ref"sv, JSONSCHEMA_HASH_REF) &&
          ref_overrides_adjacent_keywords(parent.value().get().base_dialect)) {
        throw SchemaContainerError(
            default_container.value(),
            "Could not bundle to a container beside a reference that "
            "overrides it");
      }
    }

    // A container the caller named goes in beside whatever is already at the
    // root, which a reference there would override, so the root is made to
    // hold both. What the dialect chooses for itself gets the same treatment
    // further down, and asking after the container has been judged is what
    // keeps a container nobody reserves answered as such
    if (initial_frame->root_location().has_value() &&
        prepare_root_reference(
            schema,
            initial_frame->root_location().value().get().base_dialect)) {
      initial_frame.emplace(SchemaFrame::Mode::Locations, schema, walker,
                            resolver, default_dialect, default_id,
                            SchemaFrame::IdentifierMode::Additional, paths,
                            default_base, remaining);
      charge(remaining, initial_frame.value());
    }

    check_container(schema, default_container.value());

    // What the container holds reads under the dialect its resource is read
    // under, which is not the one a local override gives the subschema the
    // container sits in, as such an override says nothing about what sits
    // below it
    const auto container_parent{initial_frame->traverse(
        to_weak_pointer(default_container.value().initial()))};
    const auto container_resource{
        container_parent.has_value()
            ? initial_frame->traverse(container_parent.value().get().base)
            : std::nullopt};
    // Where no resource holds the container, what it holds reads under
    // whatever the caller said the document is. That came from the caller
    // rather than from a frame, so it is held against a remote's dialect in
    // the one form both can be compared in
    const JSON::String container_dialect{
        container_resource.has_value()
            ? JSON::String{container_resource.value().get().dialect}
            : canonical_uri(default_dialect)};
    embed_references(schema, default_container.value(), schema, walker,
                     resolver, mode, default_dialect, default_id, paths,
                     default_base, known, positional, container_dialect,
                     remaining, callback);
    settle_positional_references(schema, walker, resolver, default_dialect,
                                 default_id, paths, default_base, positional,
                                 remaining, callback);
    return;
  }

  // If the schema identifier is implicit, add it to the top-level of the
  // bundled schema. Otherwise, potential relative references based on this
  // implicit base URI will likely not resolve unless end users happen to
  // know that this implicit base URI is. Note that boolean schemas cannot
  // declare identifiers, so we leave those untouched
  if (!default_id.empty() && schema.is_object()) {
    // Deliberately framed without a default identifier, so that the root
    // comes back empty exactly when the schema declares none of its own
    SchemaFrame declared_frame{SchemaFrame::Mode::Root,
                               schema,
                               walker,
                               resolver,
                               default_dialect,
                               "",
                               SchemaFrame::IdentifierMode::Additional,
                               {EMPTY_WEAK_POINTER},
                               "",
                               remaining};
    charge(remaining, declared_frame);
    // A dialect that reserves no keyword for an identifier cannot be handed
    // one, and the references that the base would have settled are spelled
    // out by the bundling itself rather than left to resolve against it
    const auto &root_location{declared_frame.root_location().value().get()};
    if (declared_frame.root().empty() &&
        dialect_defines_identifier(
            walker, declared_frame.vocabularies(root_location, resolver),
            root_location.base_dialect)) {
      // A root that is nothing but a reference has nowhere to keep a name it
      // is handed, as the reference overrides whatever sits beside it, so it
      // is made to hold both before the name goes in. A root that holds
      // anything else beside its reference is answered further down, where
      // saying so does not depend on the caller having named it
      if (schema.is_object() && schema.size() == 1) {
        prepare_root_reference(schema, root_location.base_dialect);
      }

      schema_reidentify(schema, default_id, walker, resolver, default_dialect);
    }
  }

  std::optional<SchemaFrame> schema_root_frame;
  try {
    schema_root_frame.emplace(
        SchemaFrame::Mode::Root, schema, walker, resolver, default_dialect,
        default_id, SchemaFrame::IdentifierMode::Additional,
        SchemaFrame::Paths{EMPTY_WEAK_POINTER}, "", remaining);
    charge(remaining, schema_root_frame.value());
  } catch (const SchemaUnknownBaseDialectError &) {
    throw SchemaError("Could not determine how to perform bundling in this "
                      "dialect");
  }

  const auto schema_base_dialect{
      schema_root_frame->root_location().value().get().base_dialect};

  const auto container_keyword{definitions_keyword(
      walker, schema_root_frame->vocabularies(
                  schema_root_frame->root_location().value().get(), resolver))};
  if (container_keyword.empty()) {
    SchemaFrame frame{SchemaFrame::Mode::References,
                      schema,
                      walker,
                      resolver,
                      default_dialect,
                      default_id,
                      SchemaFrame::IdentifierMode::Additional,
                      {EMPTY_WEAK_POINTER},
                      default_base,
                      remaining};
    charge(remaining, frame);
    // This dialect reserves no location for schema definitions, so there is
    // nowhere to put a meta-schema even where the document names one it does
    // not carry. Leaving that as it stands is as far as bundling can get
    // rather than a failure on its part, whatever the mode asked for, whereas
    // a reference to a schema that is merely absent is still something it was
    // asked to resolve and could not
    if (frame.standalone_ignoring_metaschemas()) {
      return;
    }

    throw SchemaError("Could not determine how to perform bundling in this "
                      "dialect");
  }

  prepare_root_reference(schema, schema_base_dialect);

  const Pointer container{JSON::String{container_keyword}};
  // A schema that is not an object reserves nowhere for anything, and nothing
  // can reference its way out of one either, so bundling one is vacuous rather
  // than impossible. Only a container the caller named is held against a root
  // that cannot hold it
  if (schema.is_object()) {
    check_container(schema, container);
  }
  embed_references(schema, container, schema, walker, resolver, mode,
                   default_dialect, default_id, paths, default_base, known,
                   positional,
                   schema_root_frame->root_location().value().get().dialect,
                   remaining, callback);
  settle_positional_references(schema, walker, resolver, default_dialect,
                               default_id, paths, default_base, positional,
                               remaining, callback);
}

} // namespace

auto schema_bundle(JSON &schema, const SchemaWalker &walker,
                   const SchemaResolver &resolver,
                   std::string_view default_dialect,
                   std::string_view default_id,
                   const SchemaBundleOptions &options) -> void {
  auto remaining{options.max_locations};
  try {
    bundle_internal(schema, walker, resolver, options.mode, default_dialect,
                    default_id, options.default_container, options.paths,
                    options.default_base, remaining, options.callback);
  } catch (const SchemaFrameLimitError &) {
    // Every frame spends from what is left rather than from the whole, so the
    // one that ran out reports what it was handed. The caller set the limit
    // for the operation, so that is what the operation reports back
    throw SchemaFrameLimitError{options.max_locations};
  }
}

auto schema_bundle(const JSON &schema, const SchemaWalker &walker,
                   const SchemaResolver &resolver,
                   std::string_view default_dialect,
                   std::string_view default_id,
                   const SchemaBundleOptions &options) -> JSON {
  JSON copy = schema;
  schema_bundle(copy, walker, resolver, default_dialect, default_id, options);
  return copy;
}

} // namespace sourcemeta::core
