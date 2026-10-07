// Run via verify/repro/probe.sh: `verify/repro/probe.sh [--v1.0] '<json document>'` (prints expansion, or error code + pointer)
#include <sourcemeta/core/json.h>
#include <sourcemeta/core/jsonld.h>
#include <sourcemeta/core/jsonpointer.h>

#include <cstring>
#include <iostream>

auto main(int argc, char **argv) -> int {
  auto version{sourcemeta::core::JSONLDVersion::V1_1};
  int index{1};
  if (argc > 2 && std::strcmp(argv[1], "--v1.0") == 0) {
    version = sourcemeta::core::JSONLDVersion::V1_0;
    index = 2;
  }
  const auto input{sourcemeta::core::parse_json(argv[index])};
  // Echo the parsed input so the preserved key insertion order is visible
  std::cout << "INPUT ";
  sourcemeta::core::stringify(input, std::cout);
  std::cout << "\n";
  try {
    const auto result{sourcemeta::core::jsonld_expand(input, "", {}, version)};
    std::cout << "OK ";
    sourcemeta::core::stringify(result, std::cout);
    std::cout << "\n";
  } catch (const sourcemeta::core::JSONLDError &error) {
    std::cout << "ERROR \"" << error.what() << "\" pointer=\"";
    sourcemeta::core::stringify(error.pointer(), std::cout);
    std::cout << "\"\n";
  }
  return 0;
}
