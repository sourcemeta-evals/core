#include <sourcemeta/core/jsonschema.h>

#include <sourcemeta/core/uri.h>

#include "helpers.h"

#include <algorithm>     // std::ranges::any_of, std::ranges::none_of
#include <array>         // std::array
#include <cassert>       // assert
#include <cstddef>       // std::size_t
#include <cstdint>       // std::uint64_t
#include <deque>         // std::deque
#include <functional>    // std::cref
#include <map>           // std::map
#include <optional>      // std::optional
#include <string>        // std::string
#include <string_view>   // std::string_view
#include <tuple>         // std::tuple
#include <unordered_map> // std::unordered_map
#include <unordered_set> // std::unordered_set
#include <utility>       // std::move, std::pair
#include <variant>       // std::holds_alternative, std::get
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

// Whether a schema spells a name under either of the keywords that identify
// one. Which of the two a dialect reserves is a question a frame answers, and
// the one it does not reserve is payload beside it, so asking both is what
// rules an entry out without guessing which it would have been
auto says_the_name(const JSON &candidate, const JSON::String &identifier)
    -> bool {
  static constexpr std::array<SchemaKeyword, 2> KEYWORDS{
      {{.name = "$id"sv, .hash = JSONSCHEMA_HASH_ID},
       {.name = "id"sv, .hash = JSONSCHEMA_HASH_LEGACY_ID}}};
  const auto canonical{canonical_uri(identifier)};
  return std::ranges::any_of(
      KEYWORDS, [&canonical, &candidate](const auto keyword) -> bool {
        const auto *declared{candidate.try_at(keyword.name, keyword.hash)};
        return declared != nullptr && declared->is_string() &&
               canonical_uri(declared->to_string()) == canonical &&
               URI{declared->to_string()}.is_absolute();
      });
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

// What something newly placed under a position in the document reads under,
// which is what the position reads under except where a mark is what said so.
// Walked rather than recursed, as how far a mark reaches is a question of how
// deep the document the caller handed over goes
auto dialect_within(const SchemaFrame &frame, const JSON &document,
                    const WeakPointer &position,
                    const std::string_view default_dialect) -> JSON::String {
  const auto *current{&position};
  while (true) {
    const auto location{frame.traverse(*current)};
    if (!location.has_value()) {
      // Nothing framed reaches here, which leaves what the caller said. That
      // came from the caller rather than from a frame, so it is held against
      // a remote's dialect in the one form both can be compared in
      return canonical_uri(default_dialect);
    }

    const auto &reported{location.value().get().dialect};
    const auto &value{get(document, *current)};
    const auto *const marked{
        value.is_object() ? value.try_at(DIALECT_OVERRIDE_KEYWORD,
                                         JSONSCHEMA_HASH_DIALECT_OVERRIDE)
                          : nullptr};
    // Whether the mark is what said so, which a reference overriding whatever
    // sits beside it is enough to decide otherwise. Both are spellings
    // something wrote, so what each of them names is what settles it.
    // Everywhere else framing has already read past a declaration that says
    // nothing where it stands, so what it reported is what anything put here
    // reads under
    if (marked == nullptr || !marked->is_string() ||
        canonical_uri(marked->to_string()) != canonical_uri(reported)) {
      return JSON::String{reported};
    }

    // A mark speaks for the place that carries it and says nothing about what
    // sits below it, so its contents read under whatever the place would have
    // read under with no mark on it. Saying anything of itself takes a place
    // that answers to a name, or the top of what was framed, since framing
    // reads straight past a declaration anywhere else, and so does whatever
    // reads the document afterwards
    const auto declared{declared_dialect(value, "")};
    const auto &enclosing{location.value().get().parent};
    if (!declared.empty() &&
        (!enclosing.has_value() ||
         frame.traverse(*current, SchemaFrame::LocationType::Resource)
             .has_value())) {
      return JSON::String{declared};
    }

    // Nothing stands above the top of what was framed to say that, so a mark
    // there speaks for what it holds too
    if (!enclosing.has_value()) {
      return JSON::String{reported};
    }

    current = &enclosing.value();
  }
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

// What a schema says of itself, said the one way, so that two copies of one
// resource can be held against each other on what they mean rather than on how
// each was written. A name spelled against a base, one carrying the empty
// fragment the specifications say names the same resource, and a dialect
// spelled either way all read as something else otherwise, at any depth. Which
// places say anything of themselves is the frame's answer, since a value a
// schema carries can spell the same members and be none of it. A copy read
// under a dialect whose meta-schema keeps no core vocabulary is left as
// written, which is the stricter answer rather than a wrong one
auto say_what_it_says_of_itself(JSON &schema, const SchemaWalker &walker,
                                const SchemaResolver &resolver,
                                const std::string_view dialect,
                                const std::string_view identifier,
                                std::uint64_t &remaining) -> void {
  if (!schema.is_object()) {
    return;
  }

  struct Place {
    Pointer position;
    JSON::String base;
    JSON::String dialect;
    SchemaBaseDialect base_dialect;
    bool names_itself;
  };

  std::vector<Place> places;
  std::vector<std::pair<Pointer, JSON::String>> reaches;
  try {
    const SchemaFrame frame{SchemaFrame::Mode::References,
                            schema,
                            walker,
                            resolver,
                            dialect,
                            identifier,
                            SchemaFrame::IdentifierMode::Additional,
                            {EMPTY_WEAK_POINTER},
                            "",
                            remaining};
    charge(remaining, frame);
    frame.for_each_subschema([&places](const auto &location) -> void {
      places.push_back({.position = to_pointer(location.pointer),
                        .base = JSON::String{location.base},
                        .dialect = canonical_uri(location.dialect),
                        .base_dialect = location.base_dialect,
                        .names_itself = location.type ==
                                        SchemaFrame::LocationType::Resource});
    });
    // What a reference reaches is settled against whatever base is in force
    // where it is written, so one copy spelling it against that base and
    // another spelling it in full reach the same schema
    frame.for_each_reference([&reaches](const auto, const auto &pointer,
                                        const auto &reference) -> void {
      reaches.emplace_back(to_pointer(pointer),
                           JSON::String{reference.destination});
    });
  } catch (const SchemaError &) {
    // Each copy is settled on its own, so this one is held as it was read
    // against whatever the other came to. Anything short of the two agreeing
    // value for value is a disagreement, which errs toward two schemas of one
    // name rather than toward taking two for one
    return;
  }

  for (const auto &place : places) {
    auto &target{get(schema, place.position)};
    if (!target.is_object()) {
      continue;
    }

    // A mark says what a schema is read under as much as a declaration does,
    // and the declaration written below says it the one way, so the mark has
    // nothing left to add. One side declaring where the other inherits says
    // nothing either, which is why this is written wherever the frame reached
    target.erase(DIALECT_OVERRIDE_KEYWORD, JSONSCHEMA_HASH_DIALECT_OVERRIDE);
    target.assign("$schema", JSON{place.dialect});

    if (!place.names_itself) {
      continue;
    }

    // Which keyword carries a name is the dialect's answer, and the one it
    // does not reserve is payload wherever it turns up, so saying that the one
    // way would be rewriting what the schema is about. At the top the name is
    // settled already, where a copy may spell it against whatever base it came
    // from and the walk above has nothing to resolve against
    const auto carries{id_keyword(place.base_dialect)};
    if (target.defines(carries.name, carries.hash)) {
      target.assign(JSON::String{carries.name},
                    JSON{place.position.empty() ? canonical_uri(identifier)
                                                : place.base});
    }
  }

  for (const auto &[position, destination] : reaches) {
    set(schema, position, JSON{destination});
  }
}

// What one bundling reads from first to last: where it puts what it brings in
// and what that place is read under, how it reads and fetches schemas, and
// what the caller named for a document that declares nothing. Holding these
// together is what keeps the dialects among them from being handed over in one
// another's place
struct Reading {
  const Pointer &container;
  std::string_view container_dialect;
  const SchemaWalker &walker;
  const SchemaResolver &resolver;
  // What the document carried where a reader looks for a meta-schema when it
  // was handed over, answering before whatever is outside it. Asking the
  // document as it stands would answer with what bundling has since put
  // there, which the document does not carry, and would let the order the
  // references were gone over decide what a schema is read under
  const SchemaResolver &carried;
  std::string_view default_dialect;
};

// What a subschema reads as where it sits, held against what it would read
// under once it is embedded. Which of the two is which is the whole of the
// question marking answers, so neither of them goes over on its own
struct Marking {
  std::string_view reads_as;
  std::string_view lands_under;
};

// One of the two copies a name reaches, along with what it is read under and
// what reads it. A copy is taken out of the document holding it to be read on
// its own, which no longer reaches a dialect that document carries, so what
// answers for that document travels with the copy rather than being shared
// between the two: each may have come out of a different one
struct Copy {
  const JSON &schema;
  std::string_view dialect;
  const SchemaResolver &reads_with;
};

// Whether two copies of one resource say the same thing, which is not the same
// question as whether either was written the way the other was
auto says_the_same(const Reading &reading, const Copy &held,
                   const Copy &arriving, const JSON::String &identifier,
                   std::uint64_t &remaining) -> bool {
  auto expected{held.schema};
  say_what_it_says_of_itself(expected, reading.walker, held.reads_with,
                             held.dialect, identifier, remaining);
  auto candidate{arriving.schema};
  say_what_it_says_of_itself(candidate, reading.walker, arriving.reads_with,
                             arriving.dialect, identifier, remaining);
  return expected == candidate;
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

// Where something bundling put in came from, and where it went. A document
// fetched whole arrived as itself, so nothing of it sits above where a
// reference inside it was written, while a resource lifted out of one sat
// somewhere within the document that brought it
struct Embedding {
  Pointer source;
  Pointer destination;
};

// A reference held over for want of anything answering to the name it asks by
struct Asked {
  JSON::String identifier;
  JSON::String destination;
  Pointer origin;
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
  std::vector<Embedding> embeddings;
  // Also recorded while embedding: every place the document did not hold
  // until bundling put something there. What a reference names within the
  // caller's own document is a question about the document as the caller
  // wrote it, which the document itself is what answers, so what has gone
  // into it since has to be told from what it held all along
  std::vector<Pointer> introduced;
  // Elevation lifts a resource that a remote carried out of the copy that
  // lands, so a place named inside one is no longer where the reference that
  // named it said. Such a resource answers to the name it declared, which is
  // what says where it went, and which is not always the key it sat under
  std::unordered_map<JSON::String, std::vector<Elevated>> elevations;
  // What a reference asked for where nothing answered to the name it asked by.
  // A document bundling fetched for some other reference can carry such a
  // name, and what it carries is only known once it has been walked, which
  // happens after the reference that asked. So the asking is remembered rather
  // than refused, and settled once everything is in
  std::vector<Asked> unresolved;
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
// Whether a place says what it reads under in the keyword its dialect reserves
// for it, and goes on saying it wherever it ends up, which is what naming
// itself buys: a declaration anywhere else is read by nobody once it sits
// inside another document
auto says_what_it_reads_under(const SchemaFrame &frame, const JSON &document,
                              const WeakPointer &position) -> bool {
  return frame.traverse(position, SchemaFrame::LocationType::Resource)
             .has_value() &&
         !declared_dialect(get(document, position), "").empty();
}

// What a place inside a fetched schema reads under once it is embedded. A mark
// says nothing for what sits below it, so what carries any further is a
// declaration kept by something that names itself, and where nothing above it
// says anything there is only what the place it all goes to says
auto inherited_once_embedded(const SchemaFrame &frame, const JSON &document,
                             const SchemaFrame::Location &location,
                             const std::string_view lands_under)
    -> JSON::String {
  auto enclosing{location.parent};
  while (enclosing.has_value()) {
    if (says_what_it_reads_under(frame, document, enclosing.value())) {
      return JSON::String{
          declared_dialect(get(document, enclosing.value()), "")};
    }

    const auto above{frame.traverse(enclosing.value())};
    if (!above.has_value()) {
      break;
    }

    enclosing = above.value().get().parent;
  }

  return JSON::String{lands_under};
}

// Whether two dialects put the same reading in force, which is not the same
// question as whether they are spelled alike, as one dialect can answer to
// more than one name. A mark stands in for a reading rather than for a name,
// so what each puts in force is what it has to go by
auto read_the_same_way(const SchemaVocabularies &one,
                       const SchemaVocabularies &other) -> bool {
  if (one.size() != other.size()) {
    return false;
  }

  auto same{true};
  one.for_each(
      [&other, &same](const auto &vocabulary, const auto required) -> void {
        if (!same) {
          return;
        }

        const auto status{
            std::holds_alternative<SchemaVocabularies::Known>(vocabulary)
                ? other.get(std::get<SchemaVocabularies::Known>(vocabulary))
                : other.get(std::get<JSON::String>(vocabulary))};
        same = status.has_value() && status.value() == required;
      });

  return same;
}

// What a dialect puts in force, worked out from the name alone. A name nothing
// can say a base for, and one that cannot be read for what it declares, each
// leave nothing to work it out from, so both are left to be compared the only
// other way there is
auto reading_named_by(const std::string_view dialect,
                      const SchemaResolver &resolver)
    -> std::optional<SchemaVocabularies> {
  try {
    // Asked of a schema that declares nothing, so that what comes back is what
    // the name alone puts in force rather than anything a document said
    const auto base{
        base_dialect(JSON::make_object(), resolver, dialect, false)};
    if (!base.has_value()) {
      return std::nullopt;
    }

    return vocabularies(resolver, base.value(), dialect);
  } catch (const SchemaResolutionError &) {
    return std::nullopt;
  } catch (const SchemaUnknownBaseDialectError &) {
    return std::nullopt;
  } catch (const SchemaKeywordError &) {
    return std::nullopt;
  } catch (const SchemaError &) {
    return std::nullopt;
  }
}

auto mark_dialect_of_every_subschema(const Reading &reading, JSON &schema,
                                     const Marking &marking,
                                     std::uint64_t &remaining) -> void {
  const SchemaFrame frame{SchemaFrame::Mode::Locations,
                          schema,
                          reading.walker,
                          reading.carried,
                          marking.reads_as,
                          "",
                          SchemaFrame::IdentifierMode::Additional,
                          {EMPTY_WEAK_POINTER},
                          "",
                          remaining};
  charge(remaining, frame);

  // What the document carries answers for a dialect before anything outside it
  // does, which is what framing went by, so reading a name any other way would
  // hold what the frame said against something it never said
  const SchemaResolver as_the_document_does{
      resolver_with_embedded(schema, reading.carried)};

  std::vector<std::pair<Pointer, JSON::String>> targets;
  frame.for_each_location(
      [&targets, lands_under = marking.lands_under, &frame,
       &carried = reading.carried, &as_the_document_does,
       &document = std::as_const(schema)](const auto, const auto &,
                                          const auto &location) -> void {
        if (location.type != SchemaFrame::LocationType::Resource &&
            location.type != SchemaFrame::LocationType::Subschema) {
          return;
        }

        // One that names itself and says what it reads under keeps saying it
        // wherever it goes, so there is nothing for a mark to carry and
        // nothing to take that declaration's place
        if (says_what_it_reads_under(frame, document, location.pointer)) {
          return;
        }

        // Marking stands in for a declaration that this dialect has no keyword
        // for, so what it has to say is said where being embedded would leave
        // a subschema reading as something other than what it reads as here.
        // One that would read the same way either side needs nothing said for
        // it, whether that is because what it goes into reads that way or
        // because something above it goes on saying so
        const auto inherited{
            inherited_once_embedded(frame, document, location, lands_under)};
        if (canonical_uri(location.dialect) == canonical_uri(inherited)) {
          return;
        }

        // Spelled differently, which says nothing on its own about what either
        // one puts in force, so what they come to is what settles it. What this
        // place reads under the frame has worked out already
        const auto lands_as{reading_named_by(inherited, as_the_document_does)};
        if (lands_as.has_value() &&
            read_the_same_way(frame.vocabularies(location, carried),
                              lands_as.value())) {
          return;
        }

        targets.push_back(
            {to_pointer(location.pointer), JSON::String{location.dialect}});
      });

  for (const auto &[target, reads_under] : targets) {
    auto &subschema{get(schema, target)};
    // A boolean carries no keyword whose meaning could have differed, so
    // leaving it to inherit costs nothing
    if (subschema.is_object()) {
      // A mark says what this is read under, so a declaration beside it is
      // read by nobody, and where this lands a declaration is not something
      // a subschema is allowed to carry at all unless it names itself. What a
      // subschema was already marked with is what whoever wrote it said of it,
      // which this has nothing to add to
      subschema.erase("$schema"sv, JSONSCHEMA_HASH_SCHEMA);
      subschema.assign(JSON::String{DIALECT_OVERRIDE_KEYWORD},
                       JSON{canonical_uri(reads_under)});
    }
  }
}

// What a document bundling went and got is named by: the name it was asked
// for under, and the reference that asked
struct Fetch {
  JSON::String name;
  Pointer origin;
};

// A resource on its way into the document, kept so that one arriving before
// it does can still be held against it. One bundling went and got also keeps
// the name it was fetched by and the reference that sent for it, as those are
// what a refusal the fetched document brings on has to name, however many
// later references meet the same document and whatever they call it. One that
// was already inside something was fetched by nothing
struct Fetched {
  JSON schema;
  JSON::String dialect;
};

struct Pending {
  const JSON *schema;
  JSON::String dialect;
  std::optional<Fetch> fetch;
};

// Where the document holds a resource, and what reading it there reads it
// under, as one that declares nothing of its own reads under whatever holds it
struct Placement {
  Pointer where;
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
  // Where the document ended up holding each name it was given. Nothing reads
  // a resource by that place, but what a reference names within one is looked
  // for there once it is no longer waiting to go in
  std::unordered_map<JSON::String, Placement> placements;
  // What a later pass will place. A name is answered from here first, as the
  // document does not hold it yet
  std::unordered_map<JSON::String, Pending> pending;
  // What came back under each name, as it came back, and what it was read
  // under when it did. What a reference names within a resource is a question
  // about what was fetched, and the document stops saying that once a name of
  // its own and marks have gone in, so this outlives the wait rather than
  // going when the resource is placed. The two travel together, as reading
  // what came back under anything else frames it without the vocabulary a
  // name of its own needs
  std::unordered_map<JSON::String, Fetched> fetched;
  // Which of these names came back as a boolean. One holds nothing at all, so
  // a reference naming a place within it names nothing, and writing it out as
  // the object saying the same thing does not put anything there to name
  std::unordered_set<JSON::String> says_nothing_within;
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
            const JSON &schema, const std::string_view dialect,
            std::optional<Fetch> fetch) -> void {
  auto canonical{canonical_uri(name)};
  const auto held{known.fetched.insert_or_assign(
      canonical, Fetched{.schema = schema, .dialect = JSON::String{dialect}})};
  known.pending.insert_or_assign(std::move(canonical),
                                 Pending{.schema = &held.first->second.schema,
                                         .dialect = JSON::String{dialect},
                                         .fetch = std::move(fetch)});
}

auto placed(KnownResources &known, const std::string_view name) -> void {
  known.pending.erase(canonical_uri(name));
}

auto landed(KnownResources &known, const std::string_view name,
            const Pointer &where, const std::string_view dialect) -> void {
  known.placements.insert_or_assign(
      canonical_uri(name),
      Placement{.where = where, .dialect = JSON::String{dialect}});
}

// A resource read on its own is read without the document that holds it, so a
// dialect the document carries for itself goes looking outside it and is not
// found. What the document answers to a name is what it holds under that name,
// wherever that is, including what a later pass has yet to put in
auto answers_within(const JSON &root, const KnownResources &known,
                    const SchemaResolver &resolver) -> SchemaResolver {
  return [&root, &known,
          &resolver](const std::string_view target) -> SchemaResolverResult {
    const auto canonical{canonical_uri(target)};
    const auto placement{known.placements.find(canonical)};
    if (placement != known.placements.cend()) {
      const auto *held{try_get(root, placement->second.where)};
      if (held != nullptr) {
        return *held;
      }
    }

    const auto waiting{known.pending.find(canonical)};
    if (waiting != known.pending.cend()) {
      return *(waiting->second.schema);
    }

    return resolver(target);
  };
}

// What a schema carries where a reader looks for a meta-schema, taken as the
// caller handed it over. Which of those entries is a meta-schema of anything
// is not asked here, as asking would mean reading what each one holds before
// anything has framed it, and a lookup settles it entry by entry anyway
auto metaschemas_carried_by(const JSON &schema) -> JSON {
  auto result{JSON::make_object()};
  if (!holds_embedded_candidates(schema)) {
    return result;
  }

  for (const auto container : EMBEDDED_METASCHEMA_CONTAINERS) {
    const auto *entries{schema.try_at(container.name, container.hash)};
    if ((entries == nullptr) || !entries->is_object()) {
      continue;
    }

    result.assign(JSON::String{container.name}, *entries);
  }

  return result;
}

// What each schema holding a place carries, nearest first, and apart from
// those what the schema the caller framed carries, as the two are asked in
// different ways. They stay a chain rather than one pool, as a reader asks
// them one at a time: what the nearest holds answers first whichever container
// a reader would look in for it, two of them can hold an entry under one key,
// and an entry answering to the name asked for that leads nowhere leaves the
// next one out to be asked. Each is kept as a copy of its own, as bundling
// goes on to write into the document
struct CarriedMetaschemas {
  std::vector<JSON> held;
  std::deque<PinnedScope> scopes;
  JSON framed{JSON::make_object()};
};

// Which of the schemas the caller framed holds a given place, as naming a path
// is what puts everything outside it out of scope
auto framed_schema_holding(const SchemaFrame::Paths &paths,
                           const WeakPointer &position) -> const WeakPointer * {
  for (const auto &path : paths) {
    if (position.starts_with(path)) {
      return &path;
    }
  }

  return nullptr;
}

// A meta-schema a schema carries speaks for that schema and for whatever sits
// within it, which is how a dialect is settled where something lands: the
// schema the caller framed answers for everything inside it, and below that
// only a schema that answers to a name of its own encloses anything in its own
// right. So what is carried where something lands is what each schema holding
// that place carries, nearest first, up to the one the caller framed. Where
// the caller framed the schemas it wanted bundled inside a document of its
// own, a place outside every one of them sits within no schema at all, and
// carries nothing for anything to read
auto metaschemas_carried_at(const SchemaFrame &frame, const JSON &document,
                            const SchemaFrame::Paths &paths,
                            const WeakPointer &position) -> CarriedMetaschemas {
  CarriedMetaschemas result;
  const auto *framed{framed_schema_holding(paths, position)};
  if (framed == nullptr) {
    return result;
  }

  auto current{position};
  while (current.size() > framed->size()) {
    const auto &schema{get(document, current)};
    if (holds_embedded_candidates(schema) &&
        frame.traverse(current, SchemaFrame::LocationType::Resource)
            .has_value()) {
      result.held.push_back(metaschemas_carried_by(schema));
    }

    current.pop_back();
  }

  result.framed = metaschemas_carried_by(get(document, *framed));
  // Built from the outside in, as each link says which one encloses it
  for (auto held{result.held.crbegin()}; held != result.held.crend(); ++held) {
    const auto *enclosing{result.scopes.empty() ? nullptr
                                                : &result.scopes.front()};
    result.scopes.push_front({.holder = &*held, .enclosing = enclosing});
  }

  return result;
}

// What the schema the caller framed carries answers before whatever is outside
// the document, whichever dialect is asked for, as that schema pins the
// meta-schema it is itself described by. An entry it holds under the name a
// schema reads by may be no meta-schema at all, saying nothing of the dialect
// it is written for, and nothing outside can stand in for one, as a reader of
// the document would come upon that same entry. Such a name answers to
// nothing, so that it is refused for being unresolvable rather than held
// against the schema that asked by it
auto resolver_with_framed(const JSON &framed, const SchemaResolver &resolver)
    -> SchemaResolver {
  return [&framed,
          &resolver](const std::string_view target) -> SchemaResolverResult {
    try {
      const auto *embedded{metaschema_try_embedded(framed, target, resolver)};
      if (embedded != nullptr) {
        return *embedded;
      }
    } catch (const SchemaUnknownBaseDialectError &) {
      return std::nullopt;
    }

    return resolver(target);
  };
}

// What a schema within the document carries answers before whatever encloses
// it, nearest first, and a name none of them answers for is left to the schema
// the caller framed and to whatever is outside the document after that, while
// an entry one of them holds that says nothing of the dialect it is written
// for leaves that name answering to nothing at all
auto resolver_within_carried(const CarriedMetaschemas &carried,
                             const SchemaResolver &framed) -> SchemaResolver {
  return [&carried,
          &framed](const std::string_view target) -> SchemaResolverResult {
    const auto *innermost{carried.scopes.empty() ? nullptr
                                                 : &carried.scopes.front()};
    for (const auto *scope{innermost}; scope != nullptr;
         scope = scope->enclosing) {
      try {
        const auto *embedded{
            metaschema_try_embedded(*scope->holder, target, framed, innermost)};
        if (embedded != nullptr) {
          return *embedded;
        }
      } catch (const SchemaUnknownBaseDialectError &) {
        return std::nullopt;
      }
    }

    return framed(target);
  };
}

// Whether the dialect in force where a container sits reserves that place for
// schema definitions. What the schemas holding the place carry is what settles
// the dialect there, as a document can hold the meta-schema that says so, and
// asking what is outside the document on its own goes looking for a name the
// document itself answers to
auto container_is_reserved(const SchemaFrame &frame, const JSON &document,
                           const SchemaFrame::Paths &paths,
                           const SchemaWalker &walker,
                           const SchemaResolver &resolver,
                           const Pointer &container,
                           const SchemaFrame::Location &parent) -> bool {
  const auto carried{metaschemas_carried_at(
      frame, document, paths, to_weak_pointer(container.initial()))};
  const SchemaResolver as_the_framed_schema_carries{
      resolver_with_framed(carried.framed, resolver)};
  const SchemaResolver as_the_document_carries{
      resolver_within_carried(carried, as_the_framed_schema_carries)};
  return walker(container.back().to_property(),
                frame.vocabularies(parent, as_the_document_carries))
             .type == SchemaKeywordType::LocationMembers;
}

// Every name the document the caller handed over answers to, and where each
// of those sits within it, so that what a fetched schema names within one is
// held against what is there. The top of a document answers to the base it
// was retrieved from as well as to a name it declares, which framing says
// without calling a retrieval address a resource, as such an address names a
// document rather than saying where what sits within it is addressed from
auto record_own_names(const SchemaFrame &frame, KnownResources &known) -> void {
  frame.for_each_location([&known](const auto, const auto &uri,
                                   const auto &location) -> void {
    const auto names_the_top{location.pointer.empty() &&
                             location.type ==
                                 SchemaFrame::LocationType::Subschema};
    if (uri.empty() || (location.type != SchemaFrame::LocationType::Resource &&
                        !names_the_top)) {
      return;
    }

    record(known, uri, JSON::String{uri});
    landed(known, uri, to_pointer(location.pointer), location.dialect);
  });
}

// Every meta-schema that anything within this schema pins. A schema that
// looks for one of these has to go on finding it, so collecting only what the
// top of the document declares would miss a resource further in that pins its
// own. Which places those are is the walk's answer rather than this
// function's, as a declaration says what reads a schema only where a schema
// is what it sits in, and a value a schema carries can look exactly like one
auto collect_pinned_dialects(
    const JSON &schema, const SchemaWalker &walker,
    const SchemaResolver &resolver, const std::string_view dialect,
    std::unordered_map<JSON::String, std::vector<SchemaBaseDialect>> &result,
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

  // Below the document itself, what a schema is read under is something it
  // says for itself only where it answers to a name, inheritance being what
  // settles it everywhere else
  frame.for_each_subschema([&result](const auto &location) -> void {
    if (location.type == SchemaFrame::LocationType::Resource ||
        location.pointer.empty()) {
      result[canonical_uri(location.dialect)].push_back(location.base_dialect);
    }
  });
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
                  const SchemaBundleOptions::Callback &callback,
                  std::vector<Pointer> &introduced) -> JSON::String {
  // A step of the container the document has nothing at is one bundling makes,
  // and a place bundling made is as much nowhere a reference could have named
  // as the entry that goes in under it
  auto missing{container};
  while (!missing.empty() && try_get(root, missing) == nullptr) {
    introduced.push_back(missing);
    missing = missing.initial();
  }

  auto *current{&root};
  for (const auto &token : container) {
    if (token.is_property()) {
      current->assign_if_missing(token.to_property(), JSON::make_object());
      current = &current->at(token.to_property());
    } else if (current->is_array() && token.to_index() < current->size()) {
      // Where a resource that needs a copy of its own sits under an applicator
      // holding a list of schemas, the way in to it runs through a step that
      // is a number. Such a step is one bundling worked out to reach a place
      // that is already there, rather than one it was handed, and what the
      // caller hands over is held to naming properties before any of this
      current = &current->at(token.to_index());
    } else {
      throw SchemaContainerError(container, NOT_AN_OBJECT);
    }
  }

  if (!current->is_object()) {
    throw SchemaContainerError(container, NOT_AN_OBJECT);
  }

  std::string key{identifier};
  // Ensure we get a definitions entry that does not exist
  while (current->defines(key)) {
    key += "/x";
  }

  current->assign(key, std::move(target));
  introduced.push_back(container.concat(key));

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

// Whether what a reference named inside a remote is somewhere elevation moved
// it to. Where that went is settled once the document is finished, and the
// pass that settles it refuses a reference that named nothing there, so asking
// here would hold the reference against a place the remote no longer keeps
auto elevation_moved_what_it_named(const Pointer &named,
                                   const std::vector<Elevated> &elevated)
    -> bool {
  return std::ranges::any_of(elevated, [&named](const auto &moved) -> bool {
    return named.starts_with(moved.at);
  });
}

// Where a reference was written, which is the document it arrived in rather
// than the one it ends up in. A refusal names it there, and this pass reads the
// finished document, so what went in is what says which of those a place
// belongs to. The innermost is the answer, and a reference the caller wrote
// sits inside none of them
// Where the caller wrote a reference that bundling moved to make room for a
// container. A root that is nothing but a reference cannot hold one beside it,
// since the reference overrides whatever sits there, so the reference goes
// under a branch of its own. That is bundling making room rather than anything
// the caller said, and a refusal names the place the caller wrote. Only the
// one reference the root held can be under there, so this reaches nothing else
auto where_the_caller_wrote(const Pointer &position, const Pointer &wrapper)
    -> Pointer {
  if (wrapper.empty() || position.size() != wrapper.size() + 1 ||
      !position.starts_with(wrapper)) {
    return position;
  }

  return position.resolve_from(wrapper);
}

auto where_it_was_written(const Pointer &position,
                          const std::vector<Embedding> &embeddings,
                          const Pointer &wrapper) -> Pointer {
  const Embedding *innermost{nullptr};
  for (const auto &embedding : embeddings) {
    if (position.starts_with(embedding.destination) &&
        (innermost == nullptr ||
         embedding.destination.size() > innermost->destination.size())) {
      innermost = &embedding;
    }
  }

  // Inside nothing that went in, so this is a place of the root the caller
  // handed over, which bundling may have made to hold a branch
  if (innermost == nullptr) {
    return where_the_caller_wrote(position, wrapper);
  }

  // Where it sat in whatever brought it, and then where it sits in that
  auto written{innermost->source};
  written.push_back(position.resolve_from(innermost->destination));
  return written;
}

auto settle_positional_references(const Reading &reading, JSON &schema,
                                  const KnownResources &known,
                                  PositionalReferences &positional,
                                  const Pointer &wrapper,
                                  std::uint64_t &remaining,
                                  const SchemaBundleOptions::Callback &callback)
    -> void {
  if (positional.unnamed.empty() && positional.elevations.empty()) {
    return;
  }

  const SchemaResolver as_the_document_does{
      answers_within(schema, known, reading.resolver)};

  // What the caller asked to be framed does not have to reach the
  // container, as a wrapper format keeps one outside every schema it
  // frames. So each landing is framed in its own right, which is also what the
  // walk does when it recurses into a remote
  SchemaFrame::Paths settling{known.paths};
  for (const auto &embedding : positional.embeddings) {
    const auto pointer{to_weak_pointer(embedding.destination)};
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
  // until a round places none. What bounds that is the copy saying the name of
  // every resource the schema it came from carried, rather than carrying one
  // itself: a copy answers to nothing and holds nothing another copy could be
  // made for. A resource under an applicator is carried that way too, which is
  // not something lifting out of the reserved location reaches. The map below
  // keeps one per resource and identifier, so a round that has placed them all
  // places none
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
                            reading.walker,
                            reading.resolver,
                            reading.default_dialect,
                            known.default_id,
                            SchemaFrame::IdentifierMode::Additional,
                            settling,
                            known.default_base,
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
            throw SchemaReferenceError(
                reference.destination,
                where_it_was_written(to_pointer(pointer), positional.embeddings,
                                     wrapper),
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
          throw SchemaReferenceError(reference.destination,
                                     where_it_was_written(to_pointer(pointer),
                                                          positional.embeddings,
                                                          wrapper),
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

        throw SchemaReferenceError(reference.destination,
                                   where_it_was_written(to_pointer(pointer),
                                                        positional.embeddings,
                                                        wrapper),
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
              reading.walker, frame.vocabularies(host_location.value().get(),
                                                 reading.resolver))};
          if (keyword.empty()) {
            throw SchemaReferenceError(
                reference.destination,
                where_it_was_written(to_pointer(pointer), positional.embeddings,
                                     wrapper),
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
        const auto inside{landing.concat(position)};
        const auto within{frame.traverse(to_weak_pointer(inside))};
        if (!resolved.has_value() || !within.has_value() ||
            !says_the_same(
                reading,
                {.schema =
                     get(schema, to_pointer(resolved.value().get().pointer)),
                 .dialect = resolved.value().get().dialect,
                 .reads_with = as_the_document_does},
                {.schema = get(schema, inside),
                 .dialect = within.value().get().dialect,
                 .reads_with = as_the_document_does},
                name, remaining)) {
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
            mark_dialect_of_every_subschema(
                reading, duplicate,
                {.reads_as = copy.dialect,
                 .lands_under =
                     dialect_within(frame, schema, to_weak_pointer(copy.host),
                                    reading.default_dialect)},
                remaining);
          }

          placed.emplace(seen,
                         embed_schema(schema, host_container, copy.identifier,
                                      std::move(duplicate), callback,
                                      positional.introduced));
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

// What the caller's document held somewhere when it was handed over. Bundling
// has been writing into it ever since, and a place bundling made is no part of
// what the schema holding it says, so reading it as it stands would hold a
// copy against however much had been fetched by the time the two were held
// together
auto as_it_was_handed_over(const JSON &root, const Pointer &where,
                           const SchemaBaseDialect base_dialect,
                           const std::vector<Pointer> &introduced) -> JSON {
  const auto carries{id_keyword(base_dialect)};
  auto result{get(root, where)};
  for (const auto &place : introduced) {
    if (place.size() <= where.size() || !place.starts_with(where)) {
      continue;
    }

    Pointer within;
    for (auto step{where.size()}; step < place.size(); ++step) {
      within.emplace_back(place.at(step));
    }

    // Writing out the name a document was handed is saying what it is called
    // rather than putting something in it, and claiming that name is the only
    // way a copy could be held against this one in the first place. Taking it
    // back out would leave the two disagreeing over the one thing they have to
    // share
    if (within.size() == 1 && within.back().is_property() &&
        within.back().to_property() == carries.name) {
      continue;
    }

    // A place bundling made within another is gone along with the one holding
    // it, which leaves nothing here to take out
    if (try_get(result, within) != nullptr) {
      remove(result, within);
    }
  }

  return result;
}

// What the document already holds under a name, wherever it sits. The
// container alone cannot answer that, as a caller may hold a resource
// anywhere, and holding the two against each other is what keeps a name
// standing for one schema rather than for two that disagree
auto hold_against_what_is_known(const Reading &reading, const JSON &root,
                                const Copy &arriving,
                                const KnownResources &known,
                                const std::vector<Pointer> &introduced,
                                const JSON::String &identifier,
                                std::uint64_t &remaining) -> bool {
  const SchemaResolver as_the_document_does{
      answers_within(root, known, reading.resolver)};

  // What came back under a name is what that name stands for, whether or not
  // it is in the document yet: one arriving before it goes in would otherwise
  // be dropped against nothing at all, and once it is in, whatever it carried
  // has been lifted out of the copy that landed, which says less than the name
  // was answered with
  const auto came_back{known.fetched.find(canonical_uri(identifier))};
  if (came_back != known.fetched.cend()) {
    if (!says_the_same(reading,
                       {.schema = came_back->second.schema,
                        .dialect = came_back->second.dialect,
                        .reads_with = as_the_document_does},
                       arriving, identifier, remaining)) {
      throw SchemaError(
          "Conflicting embedded resources with the same identifier");
    }

    return true;
  }

  // Nothing the caller framed reaches the place bundling puts things, by
  // design for a wrapper format, so what is already there is asked of the
  // container itself rather than of the document around it
  const auto *held_container{try_get(root, reading.container)};
  if (held_container != nullptr && held_container->is_object()) {
    for (const auto &entry : held_container->as_object()) {
      // Rule out what cannot match, and what framing would reject, before
      // paying for a frame. What a container calls an entry is no guide
      // to what that entry identifies, since a caller may hold one under a name
      // of its own choosing
      if (!entry.second.is_object()) {
        continue;
      }

      if (!says_the_name(entry.second, identifier)) {
        continue;
      }

      SchemaFrame entry_frame{SchemaFrame::Mode::Root,
                              entry.second,
                              reading.walker,
                              as_the_document_does,
                              reading.container_dialect,
                              "",
                              SchemaFrame::IdentifierMode::Additional,
                              {EMPTY_WEAK_POINTER},
                              "",
                              remaining};
      charge(remaining, entry_frame);
      if (canonical_uri(entry_frame.root()) != canonical_uri(identifier)) {
        continue;
      }

      // What sits here reads under whatever holds the container, which
      // is not what the one claiming its name arrived reading under. Taking the
      // incoming dialect for both is how two schemas that only look alike are
      // taken for one
      if (!says_the_same(reading,
                         {.schema = entry.second,
                          .dialect = reading.container_dialect,
                          .reads_with = as_the_document_does},
                         arriving, identifier, remaining)) {
        throw SchemaError(
            "Conflicting embedded resources with the same identifier");
      }

      return true;
    }
  }

  const SchemaFrame frame{SchemaFrame::Mode::Locations,
                          root,
                          reading.walker,
                          reading.resolver,
                          reading.default_dialect,
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
  const auto held{as_it_was_handed_over(root, to_pointer(held_location.pointer),
                                        held_location.base_dialect,
                                        introduced)};
  if (!says_the_same(reading,
                     {.schema = held,
                      .dialect = held_location.dialect,
                      .reads_with = as_the_document_does},
                     arriving, identifier, remaining)) {
    throw SchemaError(
        "Conflicting embedded resources with the same identifier");
  }

  return true;
}

// Whether an entry where a remote's dialect reserves definitions is one that
// elevation drops for agreeing with what its name already reaches. Everything
// such an entry carries leaves the document along with it, so none of it
// answers for a name of its own
auto removable_entry(const Reading &reading, const JSON &root,
                     const JSON &remote, const SchemaFrame &frame,
                     const Pointer &entry_position, const KnownResources &known,
                     const std::vector<Pointer> &introduced,
                     std::string_view remote_dialect_uri,
                     std::uint64_t &remaining) -> bool {
  const auto entry{frame.traverse(to_weak_pointer(entry_position),
                                  SchemaFrame::LocationType::Resource)};
  if (!entry.has_value()) {
    return false;
  }

  const JSON::String identifier{entry.value().get().base};
  const SchemaResolver as_the_remote_does{
      resolver_with_embedded(remote, reading.carried)};
  return is_known(known, identifier) &&
         hold_against_what_is_known(reading, root,
                                    {.schema = get(remote, entry_position),
                                     .dialect = remote_dialect_uri,
                                     .reads_with = as_the_remote_does},
                                    known, introduced, identifier, remaining);
}

auto elevate_embedded_resources(
    const Reading &reading, JSON &remote, JSON &root,
    const SchemaBaseDialect remote_dialect, std::string_view remote_identifier,
    KnownResources &known, PositionalReferences &positional,
    std::uint64_t &remaining, const SchemaBundleOptions::Callback &callback)
    -> void {
  // A remote can say what it is read under by a mark rather than by
  // `$schema`, which is the only way a dialect that reserves no keyword for
  // saying so can be carried, and what it says is where its definitions live
  const auto remote_dialect_uri{dialect(remote, reading.default_dialect, true)};
  const auto remote_vocabularies{vocabularies_with_embedded(
      remote, reading.carried, remote_dialect, remote_dialect_uri)};
  if (!remote.is_object()) {
    return;
  }

  // A schema finds a meta-schema it pins by looking for it among the locations
  // the specifications reserve, at the top of the document it is reading, and
  // which of those it reserves is its own dialect's answer. So what is pinned
  // can only be moved where every dialect reading under it would look, and
  // what reads under each pin goes with it
  std::unordered_map<JSON::String, std::vector<SchemaBaseDialect>> pinned;
  // What is pinned only bears on what might be lifted out, and nothing is
  // where the remote holds none of the places its dialect reserves for
  // definitions, so those are looked at before paying for a walk of it
  if (std::ranges::any_of(
          definitions_keywords(reading.walker, remote_vocabularies),
          [&remote](const auto candidate) -> bool {
            if (candidate.empty()) {
              return false;
            }

            const JSON::String keyword_string{candidate};
            return remote.defines(keyword_string) &&
                   remote.at(keyword_string).is_object() &&
                   !remote.at(keyword_string).empty();
          })) {
    collect_pinned_dialects(remote, reading.walker, reading.carried,
                            remote_dialect_uri, pinned, remaining);
  }

  // An entry can name a sibling as the dialect it is read under, and framing
  // one on its own reaches only what the caller handed over, so the entries
  // beside it are offered alongside that. Which one that is goes by what each
  // says it is rather than by the key it sits under, since the specifications
  // leave that key to whoever bundled and nothing reads an entry by it
  const SchemaResolver entry_resolver{
      [&remote, &resolver = reading.carried](
          const std::string_view identifier) -> SchemaResolverResult {
        const auto *sibling{
            embedded_metaschema_candidate(remote, identifier).first};
        if (sibling != nullptr) {
          return *sibling;
        }

        return resolver(identifier);
      }};

  // What a dialect reserves for definitions can be more than one place, where
  // the specifications say an older name it kept behaves as the one it
  // introduced, so what sits in either is a definition and each of them is
  // walked. A remote holding one identity in two of them is no document at
  // all, and framing refuses it before any of this
  for (const auto candidate :
       definitions_keywords(reading.walker, remote_vocabularies)) {
    if (candidate.empty()) {
      continue;
    }

    const JSON::String keyword_string{candidate};
    if (!remote.defines(keyword_string) ||
        !remote.at(keyword_string).is_object()) {
      continue;
    }

    auto &defs{remote.at(keyword_string)};

    std::vector<std::pair<JSON::String, bool>> to_extract;
    std::vector<Removed> to_remove;
    for (const auto &entry : defs.as_object()) {
      const auto &key{entry.first};
      const auto &value{entry.second};
      // Framing says what an entry identifies, and which keyword carries that
      // is the dialect's answer rather than this loop's, so the presence of
      // either is only a reason to ask. The one a dialect does not reserve is
      // payload wherever it turns up, so one of them holding what is no name
      // at all leaves the other to be looked at. Most entries identify nothing
      // at all, which is what rules them out before paying for a frame
      if (!value.is_object()) {
        continue;
      }

      const auto *declared_id{value.try_at("$id"sv, JSONSCHEMA_HASH_ID)};
      if (declared_id == nullptr || !declared_id->is_string()) {
        declared_id = value.try_at("id"sv, JSONSCHEMA_HASH_LEGACY_ID);
      }

      if (declared_id == nullptr || !declared_id->is_string()) {
        continue;
      }

      // The remote's dialect is what an entry that declares none inherits, so
      // hand it to the frame as the default rather than falling back after. A
      // name spelled relatively says whatever the resource around the entry
      // resolves it against, so the remote's own name goes with it
      SchemaFrame entry_frame{SchemaFrame::Mode::Root,
                              value,
                              reading.walker,
                              entry_resolver,
                              remote_dialect_uri,
                              "",
                              SchemaFrame::IdentifierMode::Additional,
                              {EMPTY_WEAK_POINTER},
                              remote_identifier,
                              remaining};
      charge(remaining, entry_frame);
      const auto &identifier{entry_frame.root()};
      if (identifier.empty() || !URI{identifier}.is_absolute()) {
        continue;
      }

      const JSON::String identifier_string{identifier};
      // What is looked for among the locations the specifications reserve is
      // looked for by the name it carries rather than by the key it sits under,
      // so what a key says is no reason to ask this later
      const auto reads_under{pinned.find(canonical_uri(identifier))};
      if (reads_under != pinned.cend() &&
          !std::ranges::all_of(reads_under->second,
                               [&container = reading.container, &value,
                                &identifier](const auto base) -> bool {
                                 return container.size() == 1 &&
                                        container.back().is_property() &&
                                        embedded_metaschema_link_valid(
                                            value, identifier,
                                            container.back().to_property(),
                                            base);
                               })) {
        // Staying put is the only way this one goes on being found, so the
        // document cannot also hold it elsewhere: one identity in two places is
        // not a document anything can read
        if (is_known(known, identifier_string)) {
          throw SchemaError("A meta-schema that has to stay within the schema "
                            "that pins it cannot be embedded elsewhere too");
        }

        // What stays behind is still in the document under the identity it
        // carries, so a reference to it from anywhere else is answered by what
        // is already here rather than by asking the resolver for
        // another copy. What it holds is what such a reference is held against,
        // as where the document that carries it ends up is not settled here
        record(known, identifier_string, identifier_string);
        expect(known, identifier_string, value, remote_dialect_uri,
               std::nullopt);
        continue;
      }

      // Only an entry whose key says what it identifies can be elevated, since
      // the key is where it would land. What a key says is no guide to what an
      // entry identifies, though, so one the document already holds under the
      // name this one identifies is still held against what is there. A name
      // spelled relatively resolves against whatever resource ends up around
      // it, so where such an entry already sits is the one place it says what
      // it says, however well the key agrees
      // Which keyword carries the name is the dialect's answer, and the other
      // spelling is payload wherever it turns up, so whether the name was
      // spelled against something is read from the one the frame went by
      const auto carries{
          id_keyword(entry_frame.root_location().value().get().base_dialect)};
      const auto *spelled{value.try_at(carries.name, carries.hash)};
      if (identifier != key || spelled == nullptr || !spelled->is_string() ||
          !URI{spelled->to_string()}.is_absolute()) {
        if (is_known(known, identifier_string)) {
          // Dropping one that nothing was held against is how a schema that
          // disagrees goes missing, so it stays where it is unless something
          // answered for it, and whatever answered for it holds the name
          // already
          if (hold_against_what_is_known(reading, root,
                                         {.schema = value,
                                          .dialect = remote_dialect_uri,
                                          .reads_with = entry_resolver},
                                         known, positional.introduced,
                                         identifier_string, remaining)) {
            to_remove.push_back({.key = key, .identifier = identifier_string});
            continue;
          }
        }

        record(known, identifier_string, identifier_string);
        // What stays goes in along with the document that carries it, so what
        // it holds is what a reference naming a place within it is held
        // against, as where the two of them end up is not settled here
        expect(known, identifier_string, value, remote_dialect_uri,
               std::nullopt);
        continue;
      }

      const auto defines_dialect{
          value.defines("$schema"sv, JSONSCHEMA_HASH_SCHEMA)};
      if (is_known(known, identifier_string)) {
        // What the document holds under this name can be anywhere, including
        // the container bundling puts things in, so there is one
        // question rather than one per place
        if (!hold_against_what_is_known(reading, root,
                                        {.schema = value,
                                         .dialect = remote_dialect_uri,
                                         .reads_with = entry_resolver},
                                        known, positional.introduced,
                                        identifier_string, remaining)) {
          continue;
        }

        to_remove.push_back({.key = key, .identifier = identifier_string});
      } else {
        to_extract.emplace_back(key, !defines_dialect &&
                                         dialect_defines(reading.walker,
                                                         remote_vocabularies,
                                                         "$schema"sv));
        record(known, identifier_string, identifier_string);
        expect(known, identifier_string, value, remote_dialect_uri,
               std::nullopt);
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

      const auto landing{embed_schema(root, reading.container, key,
                                      std::move(value), callback,
                                      positional.introduced)};
      placed(known, key);
      landed(known, key, reading.container.concat(landing),
             reading.container_dialect);
      positional.embeddings.push_back(
          {.source = Pointer{keyword_string, key},
           .destination = reading.container.concat(landing)});
    }

    for (const auto &removed : to_remove) {
      positional.elevations[JSON::String{remote_identifier}].push_back(
          {.at = Pointer{keyword_string, removed.key},
           .identifier = removed.identifier});
      defs.erase(removed.key);
    }

    if (defs.empty()) {
      remote.erase(keyword_string);
    }
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

// Whether a place a reference named is one bundling put there rather than one
// the resource came with. Where the resource is the caller's own document it is
// read where it sits, and bundling has been writing into the document since the
// caller wrote it, so where it sits is what tells the two apart. A resource
// held aside is as it came back, with nothing of bundling's in it
auto bundling_put_it_there(const std::optional<Pointer> &where_it_sits,
                           const Pointer &named,
                           const std::vector<Pointer> &introduced) -> bool {
  if (!where_it_sits.has_value() || introduced.empty()) {
    return false;
  }

  const auto named_within{where_it_sits.value().concat(named)};
  return std::ranges::any_of(introduced,
                             [&named_within](const auto &place) -> bool {
                               return named_within.starts_with(place);
                             });
}

// Whether what a reference named is something the resource it names holds. A
// pointer fragment names a place of the document, and the document can answer
// for that on its own without paying to frame it. An anchor is not a place of
// the document, and the drafts that spell identifiers as `id` let one look
// just like a pointer, so a miss still has to ask the frame. Only the anchors
// of this resource matter, rather than every pointer of it. Either way, what
// bundling has put into the document is nothing the reference could have named
auto names_something_within(
    const JSON &resource, const JSON::String &named, const SchemaWalker &walker,
    const SchemaResolver &resolver, const std::string_view default_dialect,
    const JSON::String &identifier, const std::optional<Pointer> &where_it_sits,
    const std::vector<Pointer> &introduced, std::uint64_t &remaining) -> bool {
  const auto within{fragment_to_pointer(URI{named})};
  if (within.has_value() && try_get(resource, within.value()) != nullptr &&
      !bundling_put_it_there(where_it_sits, within.value(), introduced)) {
    return true;
  }

  const SchemaFrame frame{SchemaFrame::Mode::Locations,
                          resource,
                          walker,
                          resolver,
                          default_dialect,
                          identifier,
                          SchemaFrame::IdentifierMode::Additional,
                          {EMPTY_WEAK_POINTER},
                          "",
                          remaining};
  charge(remaining, frame);
  const auto located{frame.traverse(named)};
  return located.has_value() &&
         !bundling_put_it_there(where_it_sits,
                                to_pointer(located.value().get().pointer),
                                introduced);
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

// Everything bundling went for is in by now, so a name still answered by
// nothing is answered by nothing at all, and what a reference asked for within
// one that is answered is asked of the resource answering to it. The resource
// is asked rather than the finished document, as a container the caller framed
// nothing around is reached by nothing that frames it
auto refuse_what_nothing_answers(const Reading &reading, const JSON &schema,
                                 const KnownResources &known,
                                 const PositionalReferences &positional,
                                 std::uint64_t &remaining) -> void {
  for (const auto &asked : positional.unresolved) {
    if (!is_recorded(known, asked.identifier)) {
      throw SchemaResolutionError(
          asked.identifier,
          "Could not resolve the reference to an external schema");
    }

    const URI destination{asked.destination};
    const auto fragment{destination.fragment()};
    if (!fragment.has_value() || fragment.value().empty()) {
      continue;
    }

    const auto &answering{answers_to(known, asked.identifier)};

    // Elevation lifts a resource out of the copy that landed, so a place named
    // inside one is no longer where the reference that named it said. Settling
    // has already put that reference onto where it went, which leaves nothing
    // here to ask after
    const auto elevated{positional.elevations.find(answering)};
    if (elevated != positional.elevations.cend()) {
      const auto named{fragment_to_pointer(destination)};
      if (named.has_value() &&
          elevation_moved_what_it_named(named.value(), elevated->second)) {
        continue;
      }
    }

    const auto canonical{canonical_uri(answering)};
    const auto waiting{known.pending.find(canonical)};
    const auto settled{known.placements.find(canonical)};
    const auto holds{waiting != known.pending.cend()};
    // The same question as the one above: what came back under this name is
    // what a reference names within it, placed or not
    const auto held{known.fetched.find(canonical)};
    const JSON *within{nullptr};
    std::string_view reads_under;
    std::optional<Pointer> where_it_sits;
    if (holds) {
      within = waiting->second.schema;
      reads_under = waiting->second.dialect;
    } else if (held != known.fetched.cend()) {
      within = &held->second.schema;
      reads_under = held->second.dialect;
    } else if (settled != known.placements.cend()) {
      within = try_get(schema, settled->second.where);
      reads_under = settled->second.dialect;
      where_it_sits = settled->second.where;
    }

    if (within == nullptr) {
      continue;
    }
    const SchemaResolver as_the_document_does{
        answers_within(schema, known, reading.resolver)};
    if (!names_something_within(*within, asked.destination, reading.walker,
                                as_the_document_does, reads_under, answering,
                                where_it_sits, positional.introduced,
                                remaining)) {
      throw SchemaReferenceError(asked.destination, asked.origin,
                                 "Could not resolve schema reference");
    }
  }
}

auto embed_references(const Reading &reading, JSON &root, JSON &subschema,
                      const SchemaBundleOptions::Mode mode,
                      std::string_view default_id, KnownResources &known,
                      PositionalReferences &positional, const Pointer &wrapper,
                      std::uint64_t &remaining,
                      const SchemaBundleOptions::Callback &callback,
                      const std::size_t depth = 0) -> void {
  // Only the root the caller handed over was made to hold a branch, so a
  // reference that arrived inside something fetched is named where it was
  // written already
  const auto &wrote{depth == 0 ? wrapper : EMPTY_POINTER};
  // Create a fresh frame for each schema we analyze to avoid key collisions
  // between different schemas that have references at the same pointer paths
  static const SchemaFrame::Paths NESTED_PATHS{EMPTY_WEAK_POINTER};
  const SchemaFrame frame{
      SchemaFrame::Mode::References, subschema, reading.walker, reading.carried,
      reading.default_dialect, default_id,
      SchemaFrame::IdentifierMode::Additional,
      // We only want to frame in "wrapper" mode for the top
      // level object, which is also the only one that the
      // base the caller retrieved it from applies to, as
      // every remote carries the identity it was resolved by
      depth == 0 ? known.paths : NESTED_PATHS,
      depth == 0 ? std::string_view{known.default_base} : std::string_view{},
      remaining};
  charge(remaining, frame);

  // A fetched document answers to every name it declares, wherever that name
  // is declared within it, and all of it goes in together. So each one is held
  // against what the document already answers to before any of it does. What
  // sits in the place the dialect reserves for definitions is left to the walk
  // that lifts those out, which can drop one that agrees rather than refuse it,
  // as nothing reads an entry there by where it sits
  if (depth > 0) {
    const auto &carrier{frame.root_location()};
    const auto carried_keywords{
        carrier.has_value()
            ? definitions_keywords(
                  reading.walker,
                  frame.vocabularies(carrier.value().get(), reading.carried))
            : DefinitionsKeywords{}};
    const auto carried_dialect{
        dialect(subschema, reading.default_dialect, true)};
    frame.for_each_resource([&](const auto &, const auto &location) -> void {
      const auto position{to_pointer(location.pointer)};
      const auto within_definitions{
          position.size() >= 2 && position.at(0).is_property() &&
          std::ranges::any_of(carried_keywords,
                              [&position](const auto reserved) -> bool {
                                return reserved == position.at(0).to_property();
                              })};
      if (position.empty() || (within_definitions && position.size() == 2)) {
        return;
      }

      // What an entry where this dialect reserves definitions carries leaves
      // the document along with the entry, so none of it answers for a name
      // either way. Saying it does would hold a later copy against one that is
      // on its way out, and dropping that copy too loses the schema entirely
      if (within_definitions &&
          removable_entry(reading, root, subschema, frame,
                          Pointer{JSON::String{position.at(0).to_property()},
                                  position.at(1)},
                          known, positional.introduced, carried_dialect,
                          remaining)) {
        return;
      }

      const JSON::String carried{location.base};
      if (!is_known(known, carried)) {
        record(known, carried, carried);
        // What carries this is still on its way in, so nothing holds it where
        // a later claimant could be held against it. Saying what it is
        // alongside its name is what leaves one arriving before it does
        // something to answer to
        expect(known, carried, get(subschema, position), location.dialect,
               std::nullopt);
        return;
      }

      // Where this one sits is what the schema says, so there is nothing
      // that can be taken out of it, and one name in two places is not a
      // document anything can read however well the two agree. Which of
      // them bundling met first is no reason to answer differently
      throw SchemaError(
          "Conflicting embedded resources with the same identifier");
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
      throw SchemaReferenceError(
          reference.destination,
          where_the_caller_wrote(to_pointer(pointer), wrote),
          "Could not resolve schema reference");
    }

    if (reference.base.empty()) {
      throw SchemaReferenceError(
          reference.destination,
          where_the_caller_wrote(to_pointer(pointer), wrote),
          "Could not resolve schema reference");
    }

    assert(!reference.base.empty());
    const JSON::String identifier{reference.base};

    if (is_recorded(known, identifier)) {
      // What a reference names within a resource is the resource's question
      // however many references name it, and whichever one brought it in is
      // done by now, so what it is waits among what has yet to be placed
      // A name the document was only fetched by is not the name whatever came
      // back answers to, and what it is was recorded under the latter, so that
      // is what this asks after
      const auto &answering{answers_to(known, identifier)};
      const auto pending{known.pending.find(canonical_uri(answering))};
      const auto placement{known.placements.find(canonical_uri(answering))};
      const auto elevated{positional.elevations.find(answering)};
      auto moved_away{false};
      if (elevated != positional.elevations.cend() &&
          reference.fragment.has_value() &&
          !reference.fragment.value().empty()) {
        const auto named{fragment_to_pointer(URI{reference.destination})};
        moved_away = named.has_value() && elevation_moved_what_it_named(
                                              named.value(), elevated->second);
      }

      if (reference.fragment.has_value() &&
          !reference.fragment.value().empty() && !moved_away &&
          (pending != known.pending.cend() ||
           placement != known.placements.cend())) {
        // What it was read under is what it was recorded with, as a resource
        // that declares nothing of its own reads under whatever fetched it
        // rather than under what the caller happened to pass in. Asking it
        // under anything else frames it without the vocabulary an anchor of
        // its own needs
        const auto waiting{pending != known.pending.cend()};
        // What a reference names within a resource is a question about what
        // came back under that name, which stays the question once the
        // resource is placed and the document has had a name of its own and
        // its marks written into it
        const auto held{known.fetched.find(canonical_uri(answering))};
        const JSON *within{nullptr};
        std::string_view reads_under;
        std::optional<Pointer> where_it_sits;
        if (waiting) {
          within = pending->second.schema;
          reads_under = pending->second.dialect;
        } else if (held != known.fetched.cend()) {
          within = &held->second.schema;
          reads_under = held->second.dialect;
        } else {
          within = try_get(root, placement->second.where);
          reads_under = placement->second.dialect;
          where_it_sits = placement->second.where;
        }
        const SchemaResolver as_the_document_does{
            answers_within(root, known, reading.resolver)};
        // A name the document was only fetched by is not what this reference
        // says once it is settled, as what came back answers to a name of its
        // own and the reference is restated onto that further down. What it
        // names is asked after as it will read, since a resource keeps its
        // anchors under the name it declares rather than under whatever was
        // asked for
        const auto settled{
            answering == identifier
                ? JSON::String{reference.destination}
                : rebase_reference(answering, reference.fragment)};
        bool reaches{within == nullptr};
        try {
          reaches =
              reaches ||
              (!known.says_nothing_within.contains(canonical_uri(answering)) &&
               names_something_within(*within, settled, reading.walker,
                                      as_the_document_does, reads_under,
                                      answering, where_it_sits,
                                      positional.introduced, remaining));
        } catch (const SchemaDialectImpossibleError &) {
          // What a fetched document brings on is the fault of the reference
          // that sent for it, which is not this one, where this one found it
          // fetched already. Where nothing sent for it this is the only
          // reference there is to name
          if (waiting && pending->second.fetch.has_value()) {
            throw impossible_dialect_of_fetched(
                pending->second.fetch.value().name,
                pending->second.fetch.value().origin);
          }

          throw impossible_dialect_of_fetched(
              identifier, where_the_caller_wrote(to_pointer(pointer), wrote));
        }

        if (!reaches) {
          throw SchemaReferenceError(
              reference.destination,
              where_the_caller_wrote(to_pointer(pointer), wrote),
              "Could not resolve schema reference");
        }
      }

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

    auto resolved{reading.resolver(identifier)};
    if (!resolved.has_value()) {
      if (frame.traverse(identifier).has_value()) {
        throw SchemaReferenceError(
            reference.destination,
            where_the_caller_wrote(to_pointer(pointer), wrote),
            "Could not resolve schema reference");
      }

      // Nothing outside answers to this, and what is already on its way in
      // may carry it, which is not known until each of those has been walked.
      // How it is spelled is settled here all the same, as a relative
      // spelling reads against wherever the schema ends up
      if (spells_another_document_relatively(reference)) {
        ref_rewrites.emplace_back(to_pointer(pointer),
                                  JSON::String{reference.destination});
      }

      positional.unresolved.push_back(
          {.identifier = JSON::String{identifier},
           .destination = JSON::String{reference.destination},
           .origin = where_the_caller_wrote(to_pointer(pointer), wrote)});
      return;
    }

    // Bundling rewrites the schema before embedding it, so it needs a copy
    // it owns rather than whatever the resolver chose to hand back
    auto remote{std::move(resolved).to_owned()};
    if (!remote.is_object() && !remote.is_boolean()) {
      throw SchemaReferenceError(
          identifier, where_the_caller_wrote(to_pointer(pointer), wrote),
          "The JSON document is not a valid JSON "
          "Schema");
    }

    std::optional<SchemaFrame> remote_root_frame;
    try {
      remote_root_frame.emplace(
          SchemaFrame::Mode::Root, remote, reading.walker, reading.carried,
          reading.default_dialect, "", SchemaFrame::IdentifierMode::Additional,
          SchemaFrame::Paths{EMPTY_WEAK_POINTER}, "", remaining);
      charge(remaining, remote_root_frame.value());
    } catch (const SchemaUnknownBaseDialectError &) {
      throw SchemaReferenceError(
          identifier, where_the_caller_wrote(to_pointer(pointer), wrote),
          "The JSON document is not a valid JSON "
          "Schema");
    }

    const auto &remote_root{remote_root_frame->root_location().value().get()};
    // Working these out can cost a meta-schema to resolve, so it happens once
    const auto &remote_vocabularies{
        remote_root_frame->vocabularies(remote_root, reading.carried)};

    const auto remote_base_dialect{remote_root.base_dialect};
    auto remote_id = remote_root_frame->root();

    // Whether what the reference names within the remote carries a name of its
    // own, which settles how the reference reads without settling where the
    // remote goes
    bool names_a_resource{false};

    // If the reference has a fragment, verify it exists in the remote
    // schema
    if (reference.fragment.has_value()) {
      const auto fragment_pointer{
          fragment_to_pointer(URI{reference.destination})};
      const auto *named_value{fragment_pointer.has_value()
                                  ? try_get(remote, fragment_pointer.value())
                                  : nullptr};
      bool exists{false};
      try {
        exists = names_something_within(
            remote, reference.destination, reading.walker, reading.carried,
            reading.default_dialect, identifier, std::nullopt,
            positional.introduced, remaining);
      } catch (const SchemaDialectImpossibleError &) {
        throw impossible_dialect_of_fetched(
            identifier, where_the_caller_wrote(to_pointer(pointer), wrote));
      }

      if (!exists) {
        throw SchemaReferenceError(
            reference.destination,
            where_the_caller_wrote(to_pointer(pointer), wrote),
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
                                    reading.walker,
                                    reading.carried,
                                    reading.default_dialect,
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
          throw impossible_dialect_of_fetched(
              identifier, where_the_caller_wrote(to_pointer(pointer), wrote));
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
        reading.walker, remote_vocabularies, remote_base_dialect)};

    // What the remote is read under is what framing made of it, which takes in
    // a mark that a previous bundling left on it as well as what it declares
    // and what it was resolved with. Asking the document again would miss the
    // mark, as no keyword of the schema carries it
    const JSON::String remote_dialect_uri{remote_root.dialect};

    // Carrying a name and saying what it is read under are things a dialect
    // reserves a keyword for one at a time, so which it can do is two
    // questions. Writing a declaration its dialect gives no meaning would be
    // saying something the schema never said, and nothing could read it back
    const auto dialect_allows_declaring{
        dialect_defines(reading.walker, remote_vocabularies, "$schema"sv)};

    // A boolean says exactly what an object says, which JSON Schema 2020-12
    // Section 4.3.2 spells out for each of the two, and only the object can
    // carry a name. Where the dialect has no name to give, the place the
    // boolean lands is what a reference reads instead, so writing it out would
    // add nothing. Where the one that refuses everything takes a keyword the
    // dialect does not reserve, writing it out would say something else, which
    // is no better than leaving it as it is
    if (remote.is_boolean()) {
      known.says_nothing_within.emplace(canonical_uri(effective_id));
    }

    if (remote.is_boolean() && dialect_allows_naming &&
        (remote.to_boolean() ||
         dialect_defines(reading.walker, remote_vocabularies, "not"sv))) {
      const auto permissive{remote.to_boolean()};
      remote = JSON::make_object();
      if (!permissive) {
        remote.assign("not", JSON::make_object());
      }
    }

    // What a reference names within this remote is a question about what was
    // fetched, not about what bundling goes on to write into it. Holding it
    // here, before a name, a declaration or a mark is written, is what keeps a
    // reference from reaching a place only bundling put there
    expect(known, effective_id, remote, remote_dialect_uri,
           Fetch{.name = identifier,
                 .origin = where_the_caller_wrote(to_pointer(pointer), wrote)});

    // Whether what went in can answer to the name it was resolved by, which
    // takes a dialect that reserves a keyword for naming and something that
    // can carry one. A boolean carries nothing, and it is only written out as
    // something that can where the dialect it is read under reserves a way to
    // refuse everything, so what is left of it is reached by where it lands
    // however freely that dialect names what can be named
    const auto answers_to_its_name{dialect_allows_naming && remote.is_object()};

    if (remote.is_object()) {
      if (answers_to_its_name) {
        schema_reidentify(remote, effective_id, remote_base_dialect);
      }

      if (dialect_allows_declaring) {
        // Otherwise the embedded resource would be re-interpreted under the
        // dialect of the schema it gets embedded into, which can differ from
        // the default dialect that the remote was resolved with
        if (!remote.defines("$schema"sv, JSONSCHEMA_HASH_SCHEMA)) {
          remote.assign("$schema", JSON{remote_dialect_uri});
        }
      } else if (canonical_uri(remote_dialect_uri) !=
                     canonical_uri(reading.container_dialect) ||
                 (!declared_dialect(remote, "").empty() &&
                  canonical_uri(declared_dialect(remote, "")) !=
                      canonical_uri(remote_dialect_uri))) {
        // Marking is what carries a dialect that no keyword of the schema can
        // declare. Where whatever holds the container already reads
        // under the dialect the remote was read under, inheritance says the
        // same thing for free, unless what the remote declares is not what the
        // remote itself reads under: that declaration speaks for what the
        // remote holds, and where it lands nobody reads it
        try {
          mark_dialect_of_every_subschema(
              reading, remote,
              {.reads_as = remote_dialect_uri,
               .lands_under = reading.container_dialect},
              remaining);
        } catch (const SchemaDialectImpossibleError &) {
          throw impossible_dialect_of_fetched(
              identifier, where_the_caller_wrote(to_pointer(pointer), wrote));
        }
      }
    }

    // Whether what went in can answer to the name it was resolved by is a
    // question of the remote, not of the reference that happened to bring it
    // in. A reference that names a resource the remote carries is settled by
    // that name, which says nothing about the remote around it, and a later
    // reference to the remote itself still has to be settled by place
    if (!answers_to_its_name) {
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
    } else if (!answers_to_its_name ||
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
    deferred.push_back(
        {.schema = std::move(remote),
         .identifier = std::move(effective_id),
         .fetched_by = identifier,
         .base_dialect = remote_base_dialect,
         .origin = where_the_caller_wrote(to_pointer(pointer), wrote)});
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
      embed_references(reading, root, entry.schema, mode, entry.identifier,
                       known, positional, wrapper, remaining, callback,
                       depth + 1);
      elevate_embedded_resources(reading, entry.schema, root,
                                 entry.base_dialect, entry.identifier, known,
                                 positional, remaining, callback);
    } catch (const SchemaDialectImpossibleError &) {
      throw impossible_dialect_of_fetched(entry.fetched_by, entry.origin);
    }

    placed(known, entry.identifier);
    const auto key{embed_schema(root, reading.container, entry.identifier,
                                std::move(entry.schema), callback,
                                positional.introduced)};
    auto landing{reading.container.concat(key)};
    landed(known, entry.identifier, landing, reading.container_dialect);
    if (positional.unnamed.contains(entry.identifier)) {
      positional.landings.emplace(entry.identifier, landing);
    }
    positional.embeddings.push_back(
        {.source = {}, .destination = std::move(landing)});
  }
}

// Where a reference overrides whatever sits beside it, a container beside one
// is read by nobody, so a root that is nothing but a reference becomes a branch
// that can hold both. A root that holds more than the reference has nothing
// that can be done with it. Says whether the document now reads differently
auto prepare_root_reference(JSON &schema, const SchemaBaseDialect base_dialect,
                            Pointer wrapper, std::vector<Pointer> &introduced)
    -> Pointer {
  if (!ref_overrides_adjacent_keywords(base_dialect) || !schema.is_object() ||
      !schema.defines("$ref"sv, JSONSCHEMA_HASH_REF)) {
    return wrapper;
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
  const auto *const branch{is_draft3 ? "extends" : "allOf"};
  schema.rename("$ref", branch);
  // The branch is bundling's own, and so is the place the reference moved to,
  // neither of which the caller could have named
  introduced.push_back(Pointer{JSON::String{branch}});
  return {branch, 0};
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
                       .placements = {},
                       .pending = {},
                       .fetched = {},
                       .says_nothing_within = {},
                       .paths = paths,
                       .default_id = JSON::String{default_id},
                       .default_base = JSON::String{default_base}};
  PositionalReferences positional;
  // Where bundling put a reference the root held alone, so that a refusal can
  // still name the place the caller wrote it
  Pointer wrapper;
  std::optional<SchemaFrame> initial_frame;
  initial_frame.emplace(SchemaFrame::Mode::Locations, schema, walker, resolver,
                        default_dialect, default_id,
                        SchemaFrame::IdentifierMode::Additional, paths,
                        default_base, remaining);
  charge(remaining, initial_frame.value());
  // Nothing reads a resource by where it sits, and the caller's own document
  // is as much somewhere a reference can name nothing as anything bundling
  // puts in
  record_own_names(initial_frame.value(), known);
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
    if (framed_schema_holding(paths, container) != nullptr) {
      const auto parent_pointer{default_container.value().initial()};
      const auto parent{
          initial_frame->traverse(to_weak_pointer(parent_pointer))};
      if (!parent.has_value() ||
          (parent.value().get().type != SchemaFrame::LocationType::Resource &&
           parent.value().get().type != SchemaFrame::LocationType::Subschema) ||
          !container_is_reserved(initial_frame.value(), schema, paths, walker,
                                 resolver, default_container.value(),
                                 parent.value().get())) {
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
    if (initial_frame->root_location().has_value()) {
      wrapper = prepare_root_reference(
          schema, initial_frame->root_location().value().get().base_dialect,
          std::move(wrapper), positional.introduced);
    }

    if (!wrapper.empty()) {
      initial_frame.emplace(SchemaFrame::Mode::Locations, schema, walker,
                            resolver, default_dialect, default_id,
                            SchemaFrame::IdentifierMode::Additional, paths,
                            default_base, remaining);
      charge(remaining, initial_frame.value());
      // A root that was nothing but a reference could not hold a name it was
      // handed until it was made to hold a branch, so what the document
      // answers to is asked after again
      record_own_names(initial_frame.value(), known);
    }

    check_container(schema, default_container.value());

    const auto container_parent_pointer{default_container.value().initial()};
    const auto container_dialect{dialect_within(
        initial_frame.value(), schema,
        to_weak_pointer(container_parent_pointer), default_dialect)};
    // What carries the place the container sits in is read from the document
    // as it was handed over, before anything bundling brings in has gone into
    // it
    const auto carried{
        metaschemas_carried_at(initial_frame.value(), schema, paths,
                               to_weak_pointer(container_parent_pointer))};
    const SchemaResolver as_the_framed_schema_carries{
        resolver_with_framed(carried.framed, resolver)};
    const SchemaResolver as_the_document_carries{
        resolver_within_carried(carried, as_the_framed_schema_carries)};
    const Reading reading{.container = default_container.value(),
                          .container_dialect = container_dialect,
                          .walker = walker,
                          .resolver = resolver,
                          .carried = as_the_document_carries,
                          .default_dialect = default_dialect};
    embed_references(reading, schema, schema, mode, default_id, known,
                     positional, wrapper, remaining, callback);
    settle_positional_references(reading, schema, known, positional, wrapper,
                                 remaining, callback);
    refuse_what_nothing_answers(reading, schema, known, positional, remaining);
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
        wrapper =
            prepare_root_reference(schema, root_location.base_dialect,
                                   std::move(wrapper), positional.introduced);
      }

      // A document that declares no name of its own is handed the one it was
      // retrieved by, which goes in where the dialect keeps a name. That is
      // somewhere the caller never wrote, unless what it wrote there was a
      // name that speaks for nothing outside the document
      const auto name_keyword{
          schema_identifier_keyword(root_location.base_dialect)};
      const auto held_a_name{schema.defines(name_keyword)};
      schema_reidentify(schema, default_id, walker, resolver, default_dialect);
      if (!held_a_name) {
        positional.introduced.push_back(Pointer{JSON::String{name_keyword}});
      }
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

  // Asked while this frame still says what the document is, as what the root
  // is made to hold further down leaves the two of them speaking of different
  // documents. What a root is read under is no different for having been made
  // to hold a branch either way
  const auto container_dialect{dialect_within(
      schema_root_frame.value(), schema, EMPTY_WEAK_POINTER, default_dialect)};

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

  wrapper = prepare_root_reference(schema, schema_base_dialect,
                                   std::move(wrapper), positional.introduced);
  if (!wrapper.empty() && (!default_id.empty() || !default_base.empty())) {
    // A root that was nothing but a reference could keep neither a name it
    // declares nor one it was handed until it was made to hold a branch, and
    // where the dialect reserves no keyword for one, the only place that name
    // can be read off is the frame. So what the document answers to is asked
    // after again once the branch is in and whatever name goes with it has
    // been put where it belongs. Only a name the caller gave could have gone
    // missing that way, as a root with nothing but a reference in it declares
    // none of its own, and asking after nothing costs a frame
    initial_frame.emplace(SchemaFrame::Mode::Locations, schema, walker,
                          resolver, default_dialect, default_id,
                          SchemaFrame::IdentifierMode::Additional, paths,
                          default_base, remaining);
    charge(remaining, initial_frame.value());
    record_own_names(initial_frame.value(), known);
  }

  const Pointer container{JSON::String{container_keyword}};
  // A schema that is not an object reserves nowhere for anything, and nothing
  // can reference its way out of one either, so bundling one is vacuous rather
  // than impossible. Only a container the caller named is held against a root
  // that cannot hold it
  if (schema.is_object()) {
    check_container(schema, container);
  }
  // The container goes at the top of the document, so what carries it is the
  // document itself, read as it was handed over, before anything bundling
  // brings in has gone into it
  const auto carried{metaschemas_carried_at(initial_frame.value(), schema,
                                            paths, EMPTY_WEAK_POINTER)};
  const SchemaResolver as_the_framed_schema_carries{
      resolver_with_framed(carried.framed, resolver)};
  const SchemaResolver as_the_document_carries{
      resolver_within_carried(carried, as_the_framed_schema_carries)};
  const Reading reading{.container = container,
                        .container_dialect = container_dialect,
                        .walker = walker,
                        .resolver = resolver,
                        .carried = as_the_document_carries,
                        .default_dialect = default_dialect};
  embed_references(reading, schema, schema, mode, default_id, known, positional,
                   wrapper, remaining, callback);
  settle_positional_references(reading, schema, known, positional, wrapper,
                               remaining, callback);
  refuse_what_nothing_answers(reading, schema, known, positional, remaining);
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
