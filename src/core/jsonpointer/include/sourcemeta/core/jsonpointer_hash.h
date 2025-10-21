#ifndef SOURCEMETA_CORE_JSONPOINTER_HASH_H_
#define SOURCEMETA_CORE_JSONPOINTER_HASH_H_

#include <cstddef>    // std::size_t
#include <functional> // std::hash

namespace sourcemeta::core::detail {

/// @ingroup jsonpointer
/// Internal helper to compute hash for GenericPointer types
/// This implements O(1) hashing by sampling first, middle, and last tokens
template <typename PointerT>
inline auto hash_pointer(const PointerT &pointer) noexcept -> std::size_t {
  // Helper to fold property hash to size_t
  auto fold_property_hash = [](const auto &hash) noexcept -> std::size_t {
#if defined(__SIZEOF_INT128__)
    // Fold 128-bit hash.a to size_t by XORing upper and lower 64 bits
    return static_cast<std::size_t>(hash.a) ^
           static_cast<std::size_t>(hash.a >> 64);
#else
    // On platforms without 128-bit integers, hash.a is already 64-bit
    return static_cast<std::size_t>(hash.a);
#endif
  };

  // Helper to get hash contribution from a single token
  auto token_hash = [&](const auto &token) noexcept -> std::size_t {
    if (token.is_property()) {
      // For property tokens, use the pre-computed property hash
      // We use only the first member (.a) for simplicity and O(1) performance
      return fold_property_hash(token.property_hash());
    } else {
      // For index tokens, use the index itself as the hash
      return static_cast<std::size_t>(token.to_index());
    }
  };

  const auto size = pointer.size();

  // FNV-like hash combination with constant-time mixing
  std::size_t hash = 0xcbf29ce484222325ULL;
  auto mix = [&](std::size_t value) {
    hash ^= value + 0x9e3779b97f4a7c15ULL + (hash << 6) + (hash >> 2);
  };

  // Always mix in the size to differentiate pointers of different lengths
  mix(static_cast<std::size_t>(size));

  // Sample first, middle, and last tokens for O(1) performance
  if (size >= 1) {
    mix(token_hash(pointer.at(0)));
  }
  if (size >= 3) {
    mix(token_hash(pointer.at(size / 2)));
  }
  if (size >= 2) {
    mix(token_hash(pointer.back()));
  }

  return hash;
}

} // namespace sourcemeta::core::detail

namespace std {

/// @ingroup jsonpointer
/// std::hash specialization for sourcemeta::core::Pointer
/// Enables use with std::unordered_map and std::unordered_set
template <> struct hash<sourcemeta::core::Pointer> {
  auto operator()(const sourcemeta::core::Pointer &pointer) const noexcept
      -> std::size_t {
    return sourcemeta::core::detail::hash_pointer(pointer);
  }
};

/// @ingroup jsonpointer
/// std::hash specialization for sourcemeta::core::WeakPointer
/// Enables use with std::unordered_map and std::unordered_set
template <> struct hash<sourcemeta::core::WeakPointer> {
  auto operator()(const sourcemeta::core::WeakPointer &pointer) const noexcept
      -> std::size_t {
    return sourcemeta::core::detail::hash_pointer(pointer);
  }
};

} // namespace std

#endif
