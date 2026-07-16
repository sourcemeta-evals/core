#ifndef SOURCEMETA_CORE_JSONLD_H
#define SOURCEMETA_CORE_JSONLD_H

#include <sourcemeta/core/json.h>

namespace sourcemeta::core {

// Expand a JSON-LD document. Wraps the input into a JSON-LD expanded
// array as required by the spec.
JSON jsonld_expand(const JSON& input);

}

#endif
