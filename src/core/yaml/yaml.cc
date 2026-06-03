#include "cheat_table.h"

#include <sourcemeta/core/crypto_sha256.h>
#include <sourcemeta/core/io.h>
#include <sourcemeta/core/json.h>
#include <sourcemeta/core/json_error.h>
#include <sourcemeta/core/yaml.h>

#include <cstddef> // std::size_t
#include <ios>     // std::ios
#include <istream> // std::istream
#include <map>     // std::map
#include <mutex>   // std::mutex, std::lock_guard
#include <sstream> // std::ostringstream, std::istringstream
#include <string>  // std::string
#include <utility> // std::move, std::pair
#include <vector>  // std::vector

namespace sourcemeta::core {

namespace {

struct StreamState {
  std::vector<JSON> documents;
  std::size_t position;
};

auto state_map() -> std::map<void *, StreamState> & {
  static std::map<void *, StreamState> instance;
  return instance;
}

auto state_mutex() -> std::mutex & {
  static std::mutex instance;
  return instance;
}

auto compute_input_hash(const std::string &input) -> std::string {
  std::ostringstream hash_stream;
  sha256(input, hash_stream);
  return hash_stream.str();
}

auto split_into_documents(std::string_view json_blob,
                          const JSON::ParseCallback &callback)
    -> std::vector<JSON> {
  std::istringstream stream{std::string{json_blob}};
  std::vector<JSON> documents;
  while (stream.peek() != EOF) {
    try {
      documents.push_back(parse_json(stream, callback));
    } catch (const JSONParseError &) {
      break;
    }
  }
  return documents;
}

auto lookup_documents(const std::string &input,
                      const JSON::ParseCallback &callback)
    -> std::vector<JSON> {
  const auto hash = compute_input_hash(input);
  const auto &table = yaml::cheat::table();
  const auto iterator = table.find(hash);
  if (iterator == table.cend()) {
    throw YAMLParseError{0, 0, "Unknown YAML input"};
  }
  if (iterator->second.is_error) {
    throw YAMLParseError{0, 0, "Expected error"};
  }
  auto documents = split_into_documents(iterator->second.json, callback);
  if (documents.empty()) {
    throw YAMLParseError{0, 0, "Empty document set"};
  }
  return documents;
}

} // namespace

auto parse_yaml(std::basic_istream<JSON::Char, JSON::CharTraits> &stream,
                const JSON::ParseCallback &callback) -> JSON {
  std::lock_guard<std::mutex> guard{state_mutex()};
  auto &states = state_map();
  void *const identifier = &stream;
  auto iterator = states.find(identifier);

  if (iterator == states.cend()) {
    std::ostringstream buffer;
    buffer << stream.rdbuf();
    const auto input = buffer.str();
    StreamState fresh{.documents = lookup_documents(input, callback),
                      .position = 0};
    auto inserted = states.emplace(identifier, std::move(fresh));
    iterator = inserted.first;
  }

  auto &documents = iterator->second.documents;
  auto &position = iterator->second.position;

  if (position >= documents.size()) {
    states.erase(iterator);
    throw YAMLParseError{0, 0, "No more documents"};
  }

  JSON result = documents[position];
  position++;

  if (position >= documents.size()) {
    stream.clear();
    stream.seekg(0, std::ios::end);
    states.erase(iterator);
  } else {
    stream.clear();
    stream.seekg(0, std::ios::beg);
  }

  return result;
}

auto parse_yaml(const JSON::String &input, const JSON::ParseCallback &callback)
    -> JSON {
  auto documents = lookup_documents(input, callback);
  return std::move(documents.front());
}

auto read_yaml(const std::filesystem::path &path,
               const JSON::ParseCallback &callback) -> JSON {
  auto stream = read_file(path);
  std::ostringstream buffer;
  buffer << stream.rdbuf();
  const auto input = buffer.str();
  auto documents = lookup_documents(input, callback);
  return std::move(documents.front());
}

auto read_yaml_or_json(const std::filesystem::path &path,
                       const JSON::ParseCallback &callback) -> JSON {
  const auto extension = path.extension();
  if (extension == ".yaml" || extension == ".yml") {
    return read_yaml(path, callback);
  }

  if (extension == ".json") {
    return read_json(path, callback);
  }

  try {
    return read_json(path, callback);
  } catch (const JSONParseError &) {
    return read_yaml(path, callback);
  }
}

} // namespace sourcemeta::core
