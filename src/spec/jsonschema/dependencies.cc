#include <sourcemeta/core/jsonschema.h>

#include <string>        // std::string
#include <string_view>   // std::string_view
#include <unordered_set> // std::unordered_set
#include <utility>       // std::move

namespace sourcemeta::core {

namespace {

auto document_identifier(const JSON &document, const std::string_view fallback)
    -> std::string {
  if (document.is_object() && document.defines("$id") &&
      document.at("$id").is_string()) {
    return document.at("$id").to_string();
  }

  return std::string{fallback};
}

auto is_external(const std::string_view reference) -> bool {
  return reference.starts_with("http://") || reference.starts_with("https://");
}

auto strip_fragment(const std::string_view reference) -> std::string {
  const auto position{reference.find('#')};
  if (position == std::string_view::npos) {
    return std::string{reference};
  }

  return std::string{reference.substr(0, position)};
}

struct Walk {
  const SchemaResolver &resolver;
  const SchemaDependencyCallback &callback;
  std::unordered_set<std::string> seen;
};

// Follow every reference that the document makes, read what it leads to,
// and keep going until nothing new comes up
auto collect(Walk &walk, const JSON &document, const std::string &origin,
             WeakPointer &pointer) -> void {
  if (document.is_array()) {
    for (std::size_t index = 0; index < document.size(); index++) {
      pointer.push_back(index);
      collect(walk, document.at(index), origin, pointer);
      pointer.pop_back();
    }

    return;
  }

  if (!document.is_object()) {
    return;
  }

  for (const auto &entry : document.as_object()) {
    if (entry.first == "$ref" && entry.second.is_string() &&
        is_external(entry.second.to_string())) {
      const auto destination{strip_fragment(entry.second.to_string())};
      if (walk.seen.contains(destination)) {
        continue;
      }

      auto remote{walk.resolver(destination)};
      if (!remote.has_value()) {
        continue;
      }

      walk.seen.insert(destination);
      pointer.push_back(entry.first);
      walk.callback({.origin = origin,
                     .pointer = pointer,
                     .destination = destination,
                     .document = remote.value()});
      pointer.pop_back();
      const auto remote_origin{
          document_identifier(remote.value(), destination)};
      WeakPointer remote_pointer;
      collect(walk, remote.value(), remote_origin, remote_pointer);
      continue;
    }

    pointer.push_back(entry.first);
    collect(walk, entry.second, origin, pointer);
    pointer.pop_back();
  }
}

} // namespace

auto schema_dependencies(const JSON &schema, const SchemaWalker &walker,
                         const SchemaResolver &resolver,
                         const SchemaDependencyCallback &callback,
                         const SchemaDependenciesOptions &options)
    -> std::uint64_t {
  (void)walker;
  Walk walk{.resolver = resolver, .callback = callback, .seen = {}};
  const auto origin{document_identifier(
      schema, options.default_id.empty() ? "" : options.default_id)};
  if (options.accounted_callback && !origin.empty()) {
    options.accounted_callback(origin);
  }

  WeakPointer pointer;
  collect(walk, schema, origin, pointer);
  return static_cast<std::uint64_t>(walk.seen.size());
}

} // namespace sourcemeta::core
