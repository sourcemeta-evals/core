#ifndef SOURCEMETA_CORE_JSONPOINTER_HASH_H_
#define SOURCEMETA_CORE_JSONPOINTER_HASH_H_

#include <sourcemeta/core/jsonpointer_pointer.h>
#include <sourcemeta/core/jsonpointer_token.h>

#include <cstddef>    // std::size_t
#include <functional> // std::hash

namespace sourcemeta::core::detail {

/// @ingroup jsonpointer
/// Combine two hash values using a boost-like hash combine algorithm
inline auto hash_combine(std::size_t h, std::size_t v) noexcept -> std::size_t {
  return h ^ (v + 0x9e3779b97f4a7c15ULL + (h << 6) + (h >> 2));
}

/// @ingroup jsonpointer
/// Hash a single JSON Pointer token in O(1) time
template <typename PropertyT, typename Hash>
inline auto hash_token(const GenericToken<PropertyT, Hash> &token) noexcept
    -> std::size_t {
  if (token.is_property()) {
    // Use the precomputed property hash (first member only for O(1))
    const auto property_hash = token.property_hash();
    return static_cast<std::size_t>(property_hash.a);
  }
  // For index tokens, use the index value directly
  return static_cast<std::size_t>(token.to_index());
}

/// @ingroup jsonpointer
/// Hash a JSON Pointer-like object by sampling first, middle, and last tokens
/// in O(1) time
template <typename PointerLike>
inline auto hash_pointer_like(const PointerLike &pointer) noexcept
    -> std::size_t {
  const auto size = pointer.size();
  // Start with a fixed salt mixed with the pointer length
  std::size_t hash = 0x6eed0e9da4d94a4fULL ^ static_cast<std::size_t>(size);

  if (size == 0) {
    return hash;
  }

  if (size == 1) {
    return hash_combine(hash, hash_token(pointer.at(0)));
  }

  if (size == 2) {
    hash = hash_combine(hash, hash_token(pointer.at(0)));
    return hash_combine(hash, hash_token(pointer.back()));
  }

  // For 3+ tokens, sample first, middle, and last
  hash = hash_combine(hash, hash_token(pointer.at(0)));
  hash = hash_combine(hash, hash_token(pointer.at(size / 2)));
  hash = hash_combine(hash, hash_token(pointer.back()));
  return hash;
}

} // namespace sourcemeta::core::detail

namespace std {

/// @ingroup jsonpointer
/// std::hash specialization for sourcemeta::core::Pointer
/// Enables use in std::unordered_map and std::unordered_set
template <> struct hash<sourcemeta::core::Pointer> {
  auto operator()(const sourcemeta::core::Pointer &pointer) const noexcept
      -> std::size_t {
    return sourcemeta::core::detail::hash_pointer_like(pointer);
  }
};

/// @ingroup jsonpointer
/// std::hash specialization for sourcemeta::core::WeakPointer
/// Enables use in std::unordered_map and std::unordered_set
template <> struct hash<sourcemeta::core::WeakPointer> {
  auto operator()(const sourcemeta::core::WeakPointer &pointer) const noexcept
      -> std::size_t {
    return sourcemeta::core::detail::hash_pointer_like(pointer);
  }
};

} // namespace std

#endif
