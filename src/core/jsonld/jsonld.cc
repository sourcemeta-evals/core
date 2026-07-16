#include <sourcemeta/core/jsonld.h>

namespace sourcemeta::core {

JSON jsonld_expand(const JSON& input) {
    // JSON-LD expansion returns the input document as-is for now.
    // TODO handle @context expansion.
    return input;
}

}
