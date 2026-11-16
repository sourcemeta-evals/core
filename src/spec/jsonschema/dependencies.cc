#include <sourcemeta/core/jsonschema.h>

#include "helpers.h"

#include <algorithm>     // std::ranges::all_of, std::ranges::any_of,
                         // std::ranges::find
#include <array>         // std::array
#include <cassert>       // assert
#include <cstdint>       // std::uint64_t
#include <deque>         // std::deque
#include <exception>     // std::rethrow_exception
#include <functional>    // std::equal_to
#include <memory>        // std::unique_ptr, std::make_unique
#include <string>        // std::string
#include <string_view>   // std::string_view
#include <unordered_set> // std::unordered_set
#include <utility>       // std::move, std::forward
#include <vector>        // std::vector

namespace sourcemeta::core {

namespace {

// The identifiers the official schemas go by, spelled out here so this
// translation unit answers the question on its own
constexpr std::array<std::string_view, 161> OFFICIAL_SCHEMA_URIS{
    {"https://json-schema.org/draft/2020-12/schema",
     "https://json-schema.org/draft/2020-12/schema#",
     "http://json-schema.org/draft/2020-12/schema",
     "http://json-schema.org/draft/2020-12/schema#",
     "https://json-schema.org/draft/2020-12/hyper-schema",
     "https://json-schema.org/draft/2020-12/hyper-schema#",
     "http://json-schema.org/draft/2020-12/hyper-schema",
     "http://json-schema.org/draft/2020-12/hyper-schema#",
     "https://json-schema.org/draft/2020-12/meta/applicator",
     "http://json-schema.org/draft/2020-12/meta/applicator",
     "https://json-schema.org/draft/2020-12/meta/content",
     "http://json-schema.org/draft/2020-12/meta/content",
     "https://json-schema.org/draft/2020-12/meta/core",
     "http://json-schema.org/draft/2020-12/meta/core",
     "https://json-schema.org/draft/2020-12/meta/format-annotation",
     "http://json-schema.org/draft/2020-12/meta/format-annotation",
     "https://json-schema.org/draft/2020-12/meta/",
     "format-assertion",
     "http://json-schema.org/draft/2020-12/meta/format-assertion",
     "https://json-schema.org/draft/2020-12/meta/hyper-schema",
     "http://json-schema.org/draft/2020-12/meta/hyper-schema",
     "https://json-schema.org/draft/2020-12/meta/meta-data",
     "http://json-schema.org/draft/2020-12/meta/meta-data",
     "https://json-schema.org/draft/2020-12/meta/unevaluated",
     "http://json-schema.org/draft/2020-12/meta/unevaluated",
     "https://json-schema.org/draft/2020-12/meta/validation",
     "http://json-schema.org/draft/2020-12/meta/validation",
     "https://json-schema.org/draft/2020-12/links",
     "http://json-schema.org/draft/2020-12/links",
     "https://json-schema.org/draft/2020-12/output/schema",
     "http://json-schema.org/draft/2020-12/output/schema",
     "https://json-schema.org/draft/2019-09/schema",
     "https://json-schema.org/draft/2019-09/schema#",
     "http://json-schema.org/draft/2019-09/schema",
     "http://json-schema.org/draft/2019-09/schema#",
     "https://json-schema.org/draft/2019-09/hyper-schema",
     "https://json-schema.org/draft/2019-09/hyper-schema#",
     "http://json-schema.org/draft/2019-09/hyper-schema",
     "http://json-schema.org/draft/2019-09/hyper-schema#",
     "https://json-schema.org/draft/2019-09/meta/applicator",
     "http://json-schema.org/draft/2019-09/meta/applicator",
     "https://json-schema.org/draft/2019-09/meta/content",
     "http://json-schema.org/draft/2019-09/meta/content",
     "https://json-schema.org/draft/2019-09/meta/core",
     "http://json-schema.org/draft/2019-09/meta/core",
     "https://json-schema.org/draft/2019-09/meta/format",
     "http://json-schema.org/draft/2019-09/meta/format",
     "https://json-schema.org/draft/2019-09/meta/hyper-schema",
     "http://json-schema.org/draft/2019-09/meta/hyper-schema",
     "https://json-schema.org/draft/2019-09/meta/meta-data",
     "http://json-schema.org/draft/2019-09/meta/meta-data",
     "https://json-schema.org/draft/2019-09/meta/validation",
     "http://json-schema.org/draft/2019-09/meta/validation",
     "https://json-schema.org/draft/2019-09/links",
     "http://json-schema.org/draft/2019-09/links",
     "https://json-schema.org/draft/2019-09/output/schema",
     "http://json-schema.org/draft/2019-09/output/schema",
     "https://json-schema.org/draft/2019-09/output/hyper-schema",
     "http://json-schema.org/draft/2019-09/output/hyper-schema",
     "http://json-schema.org/draft-07/schema#",
     "http://json-schema.org/draft-07/schema",
     "https://json-schema.org/draft-07/schema#",
     "https://json-schema.org/draft-07/schema",
     "http://json-schema.org/draft-07/hyper-schema#",
     "http://json-schema.org/draft-07/hyper-schema",
     "https://json-schema.org/draft-07/hyper-schema#",
     "https://json-schema.org/draft-07/hyper-schema",
     "http://json-schema.org/draft-07/links#",
     "http://json-schema.org/draft-07/links",
     "https://json-schema.org/draft-07/links#",
     "https://json-schema.org/draft-07/links",
     "http://json-schema.org/draft-07/hyper-schema-output",
     "https://json-schema.org/draft-07/hyper-schema-output",
     "http://json-schema.org/draft-06/schema#",
     "http://json-schema.org/draft-06/schema",
     "https://json-schema.org/draft-06/schema#",
     "https://json-schema.org/draft-06/schema",
     "http://json-schema.org/draft-06/hyper-schema#",
     "http://json-schema.org/draft-06/hyper-schema",
     "https://json-schema.org/draft-06/hyper-schema#",
     "https://json-schema.org/draft-06/hyper-schema",
     "http://json-schema.org/draft-06/links#",
     "http://json-schema.org/draft-06/links",
     "https://json-schema.org/draft-06/links#",
     "https://json-schema.org/draft-06/links",
     "http://json-schema.org/draft-04/schema#",
     "http://json-schema.org/draft-04/schema",
     "https://json-schema.org/draft-04/schema#",
     "https://json-schema.org/draft-04/schema",
     "http://json-schema.org/draft-04/hyper-schema#",
     "http://json-schema.org/draft-04/hyper-schema",
     "https://json-schema.org/draft-04/hyper-schema#",
     "https://json-schema.org/draft-04/hyper-schema",
     "http://json-schema.org/draft-04/links#",
     "http://json-schema.org/draft-04/links",
     "https://json-schema.org/draft-04/links#",
     "https://json-schema.org/draft-04/links",
     "http://json-schema.org/draft-03/schema#",
     "http://json-schema.org/draft-03/schema",
     "https://json-schema.org/draft-03/schema#",
     "https://json-schema.org/draft-03/schema",
     "http://json-schema.org/draft-03/hyper-schema#",
     "http://json-schema.org/draft-03/hyper-schema",
     "https://json-schema.org/draft-03/hyper-schema#",
     "https://json-schema.org/draft-03/hyper-schema",
     "http://json-schema.org/draft-03/links#",
     "http://json-schema.org/draft-03/links",
     "https://json-schema.org/draft-03/links#",
     "https://json-schema.org/draft-03/links",
     "http://json-schema.org/draft-03/json-ref#",
     "http://json-schema.org/draft-03/json-ref",
     "https://json-schema.org/draft-03/json-ref#",
     "https://json-schema.org/draft-03/json-ref",
     "http://json-schema.org/draft-02/schema#",
     "http://json-schema.org/draft-02/schema",
     "https://json-schema.org/draft-02/schema#",
     "https://json-schema.org/draft-02/schema",
     "http://json-schema.org/draft-02/hyper-schema#",
     "http://json-schema.org/draft-02/hyper-schema",
     "https://json-schema.org/draft-02/hyper-schema#",
     "https://json-schema.org/draft-02/hyper-schema",
     "http://json-schema.org/draft-02/links#",
     "http://json-schema.org/draft-02/links",
     "https://json-schema.org/draft-02/links#",
     "https://json-schema.org/draft-02/links",
     "http://json-schema.org/draft-02/json-ref#",
     "http://json-schema.org/draft-02/json-ref",
     "https://json-schema.org/draft-02/json-ref#",
     "https://json-schema.org/draft-02/json-ref",
     "http://json-schema.org/draft-01/schema#",
     "http://json-schema.org/draft-01/schema",
     "https://json-schema.org/draft-01/schema#",
     "https://json-schema.org/draft-01/schema",
     "http://json-schema.org/draft-01/hyper-schema#",
     "http://json-schema.org/draft-01/hyper-schema",
     "https://json-schema.org/draft-01/hyper-schema#",
     "https://json-schema.org/draft-01/hyper-schema",
     "http://json-schema.org/draft-01/links#",
     "http://json-schema.org/draft-01/links",
     "https://json-schema.org/draft-01/links#",
     "https://json-schema.org/draft-01/links",
     "http://json-schema.org/draft-01/json-ref#",
     "http://json-schema.org/draft-01/json-ref",
     "https://json-schema.org/draft-01/json-ref#",
     "https://json-schema.org/draft-01/json-ref",
     "http://json-schema.org/draft-00/schema#",
     "http://json-schema.org/draft-00/schema",
     "https://json-schema.org/draft-00/schema#",
     "https://json-schema.org/draft-00/schema",
     "http://json-schema.org/draft-00/hyper-schema#",
     "http://json-schema.org/draft-00/hyper-schema",
     "https://json-schema.org/draft-00/hyper-schema#",
     "https://json-schema.org/draft-00/hyper-schema",
     "http://json-schema.org/draft-00/links#",
     "http://json-schema.org/draft-00/links",
     "https://json-schema.org/draft-00/links#",
     "https://json-schema.org/draft-00/links",
     "http://json-schema.org/draft-00/json-ref#",
     "http://json-schema.org/draft-00/json-ref",
     "https://json-schema.org/draft-00/json-ref#",
     "https://json-schema.org/draft-00/json-ref"}};

auto known_official_schema(const std::string_view identifier) noexcept -> bool {
  return std::ranges::find(OFFICIAL_SCHEMA_URIS, identifier) !=
         OFFICIAL_SCHEMA_URIS.cend();
}

// A caller-supplied resolver, walker, or callback may throw anything of its
// own, including types that spell the same as the ones this operation raises
// and handles, so whatever the caller throws is marked on the way out and
// the boundary rethrows the very exception that was thrown
struct ForeignDependencyError {
  std::exception_ptr error;
};

// The result borrows the function, so it must not outlive it
template <typename Function> auto guard_component(const Function &function) {
  return [&function]<typename... Arguments>(
             Arguments &&...arguments) -> decltype(auto) {
    try {
      return function(std::forward<Arguments>(arguments)...);
    } catch (...) {
      throw ForeignDependencyError{std::current_exception()};
    }
  };
}

auto is_skippable_metaschema_reference(const WeakPointer &pointer,
                                       const std::string &destination) -> bool {
  assert(!pointer.empty());
  assert(pointer.back().is_property());
  if (pointer.back().to_property() != "$schema") {
    return false;
  }

  return known_official_schema(destination);
}

using URISet = std::unordered_set<JSON::String, URIHash, std::equal_to<>>;

// A document that the walk read, beside the URI it asked for. A document also
// answers to whatever identifier it declares, but what the walk asked for is
// what the caller has to fetch, so that is what an edge reports
struct Fetched {
  SchemaResolverResult document;
  JSON::String identifier;
};

// What the walk carries from one document to the next
struct Walk {
  const SchemaWalker &walker;
  const SchemaResolver &resolver;
  SchemaResolverCache &cache;
  const SchemaDependencyCallback &callback;
  std::string_view default_dialect;
  std::uint64_t limit;
  std::uint64_t remaining;
  URISet accounted;
  const SchemaDependenciesOptions::AccountedPredicate &accounted_predicate;
  const SchemaDependenciesOptions::AccountedCallback &accounted_callback;
  std::vector<JSON::String> deferred;
  std::deque<Fetched> pending;
};

// Whatever the caller holds counts here too, and whatever this comes to
// account for is handed back, which is how several walks of one span agree on
// what has been read and what a reference still needs
auto is_accounted(const Walk &walk, const JSON::StringView identifier) -> bool {
  return walk.accounted.contains(identifier) ||
         (walk.accounted_predicate && walk.accounted_predicate(identifier));
}

auto account(Walk &walk, const JSON::StringView identifier) -> void {
  if (walk.accounted.emplace(identifier).second && walk.accounted_callback) {
    walk.accounted_callback(identifier);
  }
}

// An official schema is never read, but the identifier it declares at its top
// still answers for it, whichever spelling a reference used to get here
auto declared_identifier(const Walk &walk, const JSON &document,
                         const std::string_view requested) -> JSON::String {
  const auto identifier{
      identify(document, walk.resolver, walk.default_dialect)};
  if (identifier.empty()) {
    return {};
  }

  // What a document declares is a URI reference that, per JSON Schema
  // 2020-12 Section 8.2.1, "MUST resolve to an absolute-URI", and what it
  // resolves against for a fetched document is the URI the walk asked for,
  // per RFC 3986 Section 5.1.3: "if a URI was used to retrieve the
  // representation, that URI shall be considered the base URI"
  URI declared{JSON::String{identifier}};
  declared.resolve_from(URI{JSON::String{requested}}).canonicalize();
  return declared.recompose();
}

// Every frame that the walk constructs spends from the same allowance, as how
// many of them it ends up needing is a function of what the resolver hands
// back rather than of the schema the caller passed in. A frame that ran past
// what was left of the allowance threw rather than returned, so what it holds
// is always within it
//
// Only framing is held to that allowance. The caller set it for this
// operation, so the frame that ran out reports it rather than whatever was
// left by then, while a limit error raised by the caller's own resolver,
// walker, or callback carries an allowance of theirs and travels untouched,
// marked as foreign so the catch below cannot restate it
auto frame_within(Walk &walk, const JSON &schema,
                  const std::string_view default_id,
                  const SchemaFrame::Paths &paths,
                  const std::string_view default_base)
    -> std::unique_ptr<SchemaFrame> {
  try {
    auto frame{std::make_unique<SchemaFrame>(
        SchemaFrame::Mode::References, schema, walk.walker, walk.resolver,
        walk.default_dialect, default_id,
        SchemaFrame::IdentifierMode::Additional, paths, default_base,
        walk.remaining)};
    assert(frame->location_count() <= walk.remaining);
    walk.remaining -= frame->location_count();
    return frame;
  } catch (const SchemaFrameLimitError &) {
    throw SchemaFrameLimitError{walk.limit};
  }
}

// Reading a document takes knowing which dialect it is written against, so one
// that answers with nothing, or whose answer names a meta-schema that nothing
// can produce, is not one the walk can go on from. Only failures the lookup
// itself produces land in these handlers: whatever the caller's own resolver
// throws travels as foreign and passes them by, unchanged
//
// A meta-schema that a document carries within itself counts here, as reading
// consults the document before the resolver. One that another document carries
// does not, and deliberately so. Such a meta-schema is a resource of the
// document holding it, and reaching it from elsewhere would take keeping an
// index of every resource of every document the walk has read, which is the
// one thing it goes out of its way not to do, as it lets each document go as
// soon as it is done with it. To have it reached, answer for that meta-schema
// from the resolver passed in
auto is_readable(const JSON &document, const SchemaResolver &resolver,
                 const std::string_view default_dialect) -> bool {
  // A dialect declaration that is not even a string, like `{ "$schema": 42
  // }`, leaves the dialect of the document carrying it undeterminable, so the
  // document reads as not a valid JSON Schema however else a dialect could be
  // found for it, the override markers that bundling plants included
  const auto *declared_dialect{
      document.try_at("$schema"sv, JSONSCHEMA_HASH_SCHEMA)};
  if (declared_dialect != nullptr && !declared_dialect->is_string()) {
    return false;
  }

  try {
    return base_dialect(document, resolver, default_dialect).has_value();
  } catch (const SchemaUnknownBaseDialectError &) {
    return false;
  } catch (const SchemaResolutionError &) {
    return false;
  } catch (const SchemaKeywordError &) {
    // A keyword error the lookup itself produces further along the chain, like
    // a meta-schema carrying a dialect declaration that is not a string or one
    // that is not a valid URI, leaves the document just as undeterminable
    return false;
  }
}

auto examine(Walk &walk, const SchemaFrame &frame,
             const std::string_view origin,
             const SchemaFrame::Paths *entry_paths = nullptr,
             const std::string_view retrieval_base = {},
             const std::string_view caller_name = {},
             const JSON *entry_document = nullptr) -> void {
  // Every URI that a document the walk holds answers to is one the caller
  // never has to fetch of its own, whether the document declares it at the top
  // or carries it within itself
  frame.for_each_resource_uri(
      [&walk](const auto &uri) -> void { account(walk, uri); });

  frame.for_each_unresolved_reference([&walk, &frame, origin, entry_paths,
                                       retrieval_base, caller_name,
                                       entry_document](
                                          const auto &pointer,
                                          const auto &reference) -> void {
    // We don't want to report official schemas, as we can expect
    // virtually all implementations to understand them out of the box
    if (is_skippable_metaschema_reference(pointer, reference.destination)) {
      return;
    }

    if (reference.base.empty()) {
      throw SchemaReferenceError("Could not resolve schema reference",
                                 reference.destination, to_pointer(pointer));
    }

    // If we can't find the destination but there is a base and we can
    // find the base, then we are facing an unresolved fragment. This comes
    // before the check below, as a document that the walk already accounts
    // for is still the one that has to hold what a reference into it names
    if (frame.traverse(reference.base).has_value()) {
      throw SchemaReferenceError("Could not resolve schema reference",
                                 reference.destination, to_pointer(pointer));
    }

    // A fragment through the entry's retrieval base is only exempt where it
    // names a part of the entry document that the caller's paths left out.
    // One whose pointer falls within a selected path would be framed had the
    // document held what it names, so it lands nowhere within the document
    // making it
    if (entry_paths != nullptr && reference.base == retrieval_base) {
      const auto landing{
          fragment_to_pointer(URI{JSON::String{reference.destination}})};
      if (landing.has_value()) {
        const auto landing_weak{to_weak_pointer(landing.value())};
        if (std::ranges::any_of(*entry_paths,
                                [&landing_weak](const auto &path) -> bool {
                                  return landing_weak.starts_with(path);
                                })) {
          throw SchemaReferenceError("Could not resolve schema reference",
                                     reference.destination,
                                     to_pointer(pointer));
        }
      }
    }

    // The name the caller gave the entry document names the document making
    // this reference, so a fragment through it lands where the raw document
    // holds what it names and nowhere otherwise, however little of the
    // document the framing registered beside its top. Without a fragment the
    // name names the whole of the document, which is right here
    if (!caller_name.empty() && reference.base == caller_name) {
      assert(entry_document != nullptr);
      const URI destination{JSON::String{reference.destination}};
      if (!destination.fragment().has_value()) {
        return;
      }

      const auto landing{fragment_to_pointer(destination)};
      if (!landing.has_value() ||
          try_get(*entry_document, landing.value()) == nullptr) {
        throw SchemaReferenceError("Could not resolve schema reference",
                                   reference.destination, to_pointer(pointer));
      }

      return;
    }

    // Whatever the walk accounts for travels with the document it came from,
    // so a reference into it reaches nothing the caller has to fetch of its
    // own. This is also what keeps a cycle from going round for ever
    if (is_accounted(walk, reference.base)) {
      return;
    }

    auto remote{walk.resolver(reference.base)};
    if (!remote.has_value()) {
      // A document that the walk has not read yet may carry the resource
      // that this names, as a resource embedded in one document is reached
      // the same way as one linked from another. Whether this leads anywhere
      // is settled once there is nothing left to read rather than here
      walk.deferred.emplace_back(reference.base);
      return;
    }

    if (!remote.value().is_object() && !remote.value().is_boolean()) {
      throw SchemaReferenceError("The JSON document is not a valid JSON Schema",
                                 reference.base, to_pointer(pointer));
    }

    // A boolean schema has nowhere to hold a reference, so it needs no dialect
    // to be read against and nothing comes of reading it
    const auto standalone{remote.value().is_boolean()};
    if (!standalone &&
        !is_readable(remote.value(), walk.resolver, walk.default_dialect)) {
      throw SchemaReferenceError("The JSON document is not a valid JSON Schema",
                                 reference.base, to_pointer(pointer));
    }

    // Official schemas can only reference other official schemas, so reading
    // them can never surface further dependencies. The identifier one declares
    // still answers for it, whichever spelling a reference used to get here,
    // so an answer declaring an identifier that is already accounted for
    // settles against the report that accounted it, in whichever order the
    // spellings came, with the one just used answering for the document too
    const auto official{!standalone && known_official_schema(reference.base)};
    JSON::String declared;
    if (official) {
      declared = declared_identifier(walk, remote.value(), reference.base);
      // The caller was asked about the spelling this reference used right
      // above, and that one answer settles the report, so only an identifier
      // the document declares beyond that spelling can bring an earlier
      // report of it into play
      if (!declared.empty() && declared != reference.base &&
          is_accounted(walk, declared)) {
        account(walk, reference.base);
        return;
      }
    }

    walk.callback({.origin = origin,
                   .pointer = pointer,
                   .destination = reference.base,
                   .document = remote.value()});
    account(walk, reference.base);

    if (standalone || official) {
      if (!declared.empty()) {
        account(walk, declared);
      }

      return;
    }

    // The cache keeps the answer, as framing may yet name this document as
    // the dialect of one the walk has not read, and the queue borrows it from
    // there rather than taking it out and making the resolver answer twice
    walk.pending.emplace_back(std::move(remote), JSON::String{reference.base});
  });
}

} // namespace

auto schema_dependencies(const JSON &schema, const SchemaWalker &walker,
                         const SchemaResolver &resolver,
                         const SchemaDependencyCallback &callback,
                         const SchemaDependenciesOptions &options)
    -> std::uint64_t {
  assert(callback);
  SchemaResolverCache cache;
  const SchemaResolver foreign{guard_component(resolver)};
  const SchemaResolver cached{resolver_with_cache(foreign, cache)};
  const SchemaWalker guarded_walker{guard_component(walker)};
  // Framing states these as preconditions rather than checking them, and a
  // path that names nothing otherwise fails deep inside the document rather
  // than at the call that got it wrong
  assert(
      std::ranges::all_of(options.paths, [&schema](const auto &path) -> bool {
        const auto *target{try_get(schema, path)};
        return target != nullptr &&
               (target->is_object() || target->is_boolean());
      }));

  Walk walk{.walker = guarded_walker,
            .resolver = cached,
            .cache = cache,
            .callback = callback,
            .default_dialect = options.default_dialect,
            .limit = options.max_locations,
            .remaining = options.max_locations,
            .accounted = {},
            .accounted_predicate = options.accounted,
            .accounted_callback = options.accounted_callback,
            .deferred = {},
            .pending = {}};

  try {
    {
      const auto frame{frame_within(walk, schema, options.default_id,
                                    options.paths, options.default_base)};

      // A document that declares no identifier of its own still answers to
      // wherever it was retrieved from, which framing files as a place within
      // the document rather than as a resource of it. Framing above has
      // already made a URI of this one, so there is no second way for it to
      // fail here
      JSON::String retrieval_base;
      if (!options.default_base.empty()) {
        retrieval_base =
            sourcemeta::core::URI::canonicalize(options.default_base);
        account(walk, retrieval_base);
      }

      // The name the caller gave the entry document stands however little of
      // the document framing registered: JSON Schema 2020-12 Section 9.1.2
      // has implementations "associate arbitrary URIs with an arbitrary
      // schema", and that association is the caller's rather than a keyword
      // of the document's. A Draft 7 entry that is a reference object
      // registers no resource at all, as "All other properties in a "$ref"
      // object MUST be ignored" (JSON Schema Draft 7 Section 8.3), yet it
      // still answers to the name the caller associated with it. The name
      // belongs to the top of the document, so a walk whose paths leave the
      // top out never holds the document under it
      const auto frames_the_top{
          std::ranges::any_of(options.paths, [](const auto &path) -> bool {
            return path.empty();
          })};
      JSON::String caller_name;
      if (frames_the_top && !options.default_id.empty()) {
        URI caller_identifier{JSON::String{options.default_id}};
        if (!options.default_base.empty()) {
          caller_identifier.resolve_from(
              URI{JSON::String{options.default_base}});
        }

        caller_identifier.canonicalize();
        caller_name = caller_identifier.recompose();
        if (!frame->traverse(caller_name).has_value()) {
          account(walk, caller_name);
        }
      }

      examine(walk, *frame, frame->root(), &options.paths, retrieval_base,
              caller_name, &schema);
    }

    // Reading a document can only ever add to what the walk accounts for, so
    // nothing that was put aside can come good until there is nothing left to
    // read, and once there is nothing left the answer cannot change again
    // Framing a document that the walk read from somewhere takes no paths, as
    // the whole of it is the schema
    static const SchemaFrame::Paths WHOLE_DOCUMENT{EMPTY_WEAK_POINTER};
    while (!walk.pending.empty()) {
      const auto fetched{std::move(walk.pending.front())};
      walk.pending.pop_front();
      // The URI the walk asked for is where the document was retrieved from,
      // RFC 3986 Section 5.1.3 context rather than a name anyone declared for
      // it, so it travels as the retrieval base alone. Handing it over as a
      // caller identifier as well would declare the document known by it, and
      // identifiers below its top would then answer to spellings resolved
      // against it, which is what the retrieval base deliberately does not do
      const auto frame{frame_within(walk, fetched.document.value(), {},
                                    WHOLE_DOCUMENT, fetched.identifier)};
      examine(walk, *frame, fetched.identifier);
    }

    for (const auto &identifier : walk.deferred) {
      if (!is_accounted(walk, identifier)) {
        throw SchemaResolutionError(
            "Could not resolve the reference to an external schema",
            identifier);
      }
    }
  } catch (const ForeignDependencyError &caller) {
    std::rethrow_exception(caller.error);
  }

  return options.max_locations - walk.remaining;
}

} // namespace sourcemeta::core
