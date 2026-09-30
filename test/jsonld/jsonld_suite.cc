#include <gtest/gtest.h>

#include <sourcemeta/core/json.h>
#include <sourcemeta/core/jsonld.h>
#include <sourcemeta/core/text.h>

#include <cstddef>     // std::size_t
#include <filesystem>  // std::filesystem
#include <optional>    // std::optional
#include <string>      // std::string
#include <string_view> // std::string_view
#include <utility>     // std::move
#include <vector>      // std::vector

namespace {

// JSON-LD 1.1 API §3.1.5: property arrays (and the top-level expansion result)
// are order-insensitive multisets, except for the value of the `@list` keyword
// which preserves order, and except for JSON literals (`@type: @json`) whose
// values pass through expansion verbatim with plain JSON semantics where
// array order is significant. This helper implements that comparison so the
// suite runner does not reject spec-legit implementations whose emission
// order differs from the reference expected output, while still rejecting
// implementations that reorder the contents of a JSON literal.
auto jsonld_deep_equal(const sourcemeta::core::JSON &left,
                       const sourcemeta::core::JSON &right, bool ordered)
    -> bool {
  if (left.is_array() && right.is_array()) {
    if (left.size() != right.size()) {
      return false;
    }
    if (ordered) {
      for (std::size_t index{0}; index < left.size(); index += 1) {
        if (!jsonld_deep_equal(left.at(index), right.at(index), false)) {
          return false;
        }
      }
      return true;
    }
    std::vector<bool> matched(right.size(), false);
    for (const auto &left_item : left.as_array()) {
      bool found{false};
      for (std::size_t index{0}; index < right.size(); index += 1) {
        if (matched[index]) {
          continue;
        }
        if (jsonld_deep_equal(left_item, right.at(index), false)) {
          matched[index] = true;
          found = true;
          break;
        }
      }
      if (!found) {
        return false;
      }
    }
    return true;
  }
  if (left.is_object() && right.is_object()) {
    if (left.size() != right.size()) {
      return false;
    }
    const bool is_json_literal{left.defines("@type") &&
                               left.at("@type").is_string() &&
                               left.at("@type").to_string() == "@json"};
    for (const auto &entry : left.as_object()) {
      if (!right.defines(entry.first)) {
        return false;
      }
      if (is_json_literal && entry.first == "@value") {
        if (entry.second != right.at(entry.first)) {
          return false;
        }
        continue;
      }
      const bool is_list{entry.first == "@list"};
      if (!jsonld_deep_equal(entry.second, right.at(entry.first), is_list)) {
        return false;
      }
    }
    return true;
  }
  return left == right;
}

struct JSONLDExpandCase {
  std::filesystem::path suite_root;
  sourcemeta::core::JSON::String base_prefix;
  std::filesystem::path input;
  std::filesystem::path expect;
  sourcemeta::core::JSON::String error_code;
  sourcemeta::core::JSON::String base_iri;
  sourcemeta::core::JSONLDVersion version;
  bool negative;
  std::optional<std::filesystem::path> expand_context;
};

class JSONLDExpandTest : public testing::Test {
public:
  explicit JSONLDExpandTest(JSONLDExpandCase test_case)
      : test_case_{std::move(test_case)} {}

  auto TestBody() -> void override {
    const auto &test_case{this->test_case_};
    const sourcemeta::core::JSONLDResolver resolver =
        [&test_case](const sourcemeta::core::JSON::StringView identifier)
        -> std::optional<sourcemeta::core::JSON> {
      if (!identifier.starts_with(test_case.base_prefix)) {
        return std::nullopt;
      }
      // Dereferencing a context IRI ignores any fragment or query component.
      const auto suffix{identifier.substr(test_case.base_prefix.size())};
      const auto path{test_case.suite_root /
                      suffix.substr(0, suffix.find_first_of("#?"))};
      if (!std::filesystem::exists(path)) {
        return std::nullopt;
      }
      return sourcemeta::core::read_json(path);
    };

    const auto input{sourcemeta::core::read_json(test_case.input)};

    if (test_case.negative) {
      try {
        if (test_case.expand_context.has_value()) {
          const auto context{
              sourcemeta::core::read_json(test_case.expand_context.value())};
          static_cast<void>(sourcemeta::core::jsonld_expand(
              input, context, test_case.base_iri, resolver, test_case.version));
        } else {
          static_cast<void>(sourcemeta::core::jsonld_expand(
              input, test_case.base_iri, resolver, test_case.version));
        }
        FAIL() << "Expected error code: " << test_case.error_code;
      } catch (const sourcemeta::core::JSONLDError &error) {
        // The implementation capitalises the first letter of every error code,
        // whereas the upstream suite expresses them in lower case.
        std::string actual_code{error.what()};
        if (!actual_code.empty()) {
          actual_code.front() =
              sourcemeta::core::to_lowercase(actual_code.front());
        }
        EXPECT_EQ(static_cast<std::string_view>(test_case.error_code),
                  actual_code);
      }
    } else {
      const auto expected{sourcemeta::core::read_json(test_case.expect)};
      const auto actual{
          test_case.expand_context.has_value()
              ? sourcemeta::core::jsonld_expand(
                    input,
                    sourcemeta::core::read_json(
                        test_case.expand_context.value()),
                    test_case.base_iri, resolver, test_case.version)
              : sourcemeta::core::jsonld_expand(input, test_case.base_iri,
                                                resolver, test_case.version)};
      EXPECT_TRUE(jsonld_deep_equal(actual, expected, false))
          << "Expanded output did not match expected under JSON-LD 1.1 "
             "unordered semantics";
    }
  }

private:
  JSONLDExpandCase test_case_;
};

auto sanitize(const std::string_view identifier) -> std::string {
  std::string result;
  for (const auto character : identifier) {
    if (character == '#') {
      continue;
    }
    result += sourcemeta::core::is_alphanum(character) ? character : '_';
  }
  return result;
}

auto register_case(const sourcemeta::core::JSON &entry,
                   const std::filesystem::path &suite_root,
                   const sourcemeta::core::JSON::String &base_prefix) -> void {
  bool negative{false};
  for (const auto &type : entry.at("@type").as_array()) {
    if (type.to_string() == "jld:NegativeEvaluationTest") {
      negative = true;
    }
  }

  const auto &input_relative{entry.at("input").to_string()};

  JSONLDExpandCase test_case;
  test_case.suite_root = suite_root;
  test_case.base_prefix = base_prefix;
  test_case.input = suite_root / input_relative;
  test_case.base_iri = base_prefix + input_relative;
  test_case.version = sourcemeta::core::JSONLDVersion::V1_1;
  test_case.negative = negative;

  if (entry.defines("option")) {
    const auto &option{entry.at("option")};
    if (option.defines("base")) {
      test_case.base_iri = option.at("base").to_string();
    }
    if ((option.defines("specVersion") &&
         option.at("specVersion").to_string() == "json-ld-1.0") ||
        (option.defines("processingMode") &&
         option.at("processingMode").to_string() == "json-ld-1.0")) {
      test_case.version = sourcemeta::core::JSONLDVersion::V1_0;
    }
    if (option.defines("expandContext")) {
      test_case.expand_context =
          suite_root / option.at("expandContext").to_string();
    }
  }

  if (negative) {
    test_case.error_code = entry.at("expectErrorCode").to_string();
  } else {
    test_case.expect = suite_root / entry.at("expect").to_string();
  }

  testing::RegisterTest(
      "JSONLD_expand", sanitize(entry.at("@id").to_string()).c_str(), nullptr,
      nullptr, __FILE__, __LINE__, [test_case]() -> testing::Test * {
        return new JSONLDExpandTest(test_case);
      });
}

} // namespace

auto main(int argc, char **argv) -> int {
  testing::InitGoogleTest(&argc, argv);

  const std::filesystem::path suite_root{JSONLD_SUITE_PATH};
  const auto manifest{
      sourcemeta::core::read_json(suite_root / "expand-manifest.jsonld")};
  const auto &base_prefix{manifest.at("baseIri").to_string()};

  for (const auto &entry : manifest.at("sequence").as_array()) {
    register_case(entry, suite_root, base_prefix);
  }

  return RUN_ALL_TESTS();
}
