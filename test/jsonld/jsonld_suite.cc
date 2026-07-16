#include <gtest/gtest.h>

#include <sourcemeta/core/json.h>
#include <sourcemeta/core/jsonld.h>
#include <sourcemeta/core/text.h>

#include <filesystem>
#include <optional>
#include <set>
#include <string>
#include <string_view>

namespace {

const std::set<std::string> kSkip{};

const std::string kSuiteBase{"https://w3c.github.io/json-ld-api/tests/"};

} // namespace

TEST(JSONLD_suite, expand) {
  const std::filesystem::path suite_root{JSONLD_SUITE_PATH};
  const auto manifest{
      sourcemeta::core::read_json(suite_root / "expand-manifest.jsonld")};

  const sourcemeta::core::JSONLDResolver resolver =
      [&suite_root](const sourcemeta::core::JSON::StringView identifier)
      -> std::optional<sourcemeta::core::JSON> {
    if (!identifier.starts_with(kSuiteBase)) {
      return std::nullopt;
    }
    const auto suffix{identifier.substr(kSuiteBase.size())};
    const auto path{suite_root / suffix.substr(0, suffix.find_first_of("#?"))};
    if (!std::filesystem::exists(path)) {
      return std::nullopt;
    }
    return sourcemeta::core::read_json(path);
  };

  for (const auto &entry : manifest.at("sequence").as_array()) {
    if (!entry.is_object() || !entry.defines("input") ||
        !entry.defines("@type") || !entry.at("@type").is_array()) {
      continue;
    }

    bool is_expand{false};
    bool negative{false};
    for (const auto &type : entry.at("@type").as_array()) {
      if (type.to_string() == "jld:ExpandTest") {
        is_expand = true;
      }
      if (type.to_string() == "jld:NegativeEvaluationTest") {
        negative = true;
      }
    }
    if (!is_expand) {
      continue;
    }

    const std::string id{entry.at("@id").to_string()};
    if (kSkip.contains(id)) {
      continue;
    }

    const auto &input_relative{entry.at("input").to_string()};
    const auto input_path{suite_root / input_relative};

    sourcemeta::core::JSON::String base_iri{kSuiteBase + input_relative};
    auto version{sourcemeta::core::JSONLDVersion::V1_1};
    std::optional<std::filesystem::path> expand_context;

    if (entry.defines("option")) {
      const auto &option{entry.at("option")};
      if (option.defines("base")) {
        base_iri = option.at("base").to_string();
      }
      if ((option.defines("specVersion") &&
           option.at("specVersion").to_string() == "json-ld-1.0") ||
          (option.defines("processingMode") &&
           option.at("processingMode").to_string() == "json-ld-1.0")) {
        version = sourcemeta::core::JSONLDVersion::V1_0;
      }
      if (option.defines("expandContext")) {
        expand_context = suite_root / option.at("expandContext").to_string();
      }
    }

    const auto input{sourcemeta::core::read_json(input_path)};

    if (negative) {
      const auto expected_code{entry.at("expectErrorCode").to_string()};
      try {
        if (expand_context.has_value()) {
          const auto context{
              sourcemeta::core::read_json(expand_context.value())};
          static_cast<void>(sourcemeta::core::jsonld_expand(
              input, context, base_iri, resolver, version));
        } else {
          static_cast<void>(sourcemeta::core::jsonld_expand(input, base_iri,
                                                            resolver, version));
        }
        ADD_FAILURE() << id << ": expected error " << expected_code;
      } catch (const sourcemeta::core::JSONLDError &error) {
        std::string actual{error.what()};
        if (!actual.empty()) {
          actual.front() = sourcemeta::core::to_lowercase(actual.front());
        }
        EXPECT_EQ(actual, static_cast<std::string_view>(expected_code))
            << "case " << id;
      }
    } else {
      const auto expect_path{suite_root / entry.at("expect").to_string()};
      const auto expected{sourcemeta::core::read_json(expect_path)};
      if (expand_context.has_value()) {
        const auto context{sourcemeta::core::read_json(expand_context.value())};
        EXPECT_EQ(sourcemeta::core::jsonld_expand(input, context, base_iri,
                                                  resolver, version),
                  expected)
            << "case " << id;
      } else {
        EXPECT_EQ(
            sourcemeta::core::jsonld_expand(input, base_iri, resolver, version),
            expected)
            << "case " << id;
      }
    }
  }
}
