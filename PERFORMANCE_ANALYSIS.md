# Performance Analysis Report - Sourcemeta Core Library

## Executive Summary

This report documents performance optimization opportunities identified in the sourcemeta-evals/core C++ JSON processing library. The analysis focused on critical performance paths including JSON parsing, hashing, object property lookups, and array operations.

## Identified Performance Issues

### 1. PropertyHashJSON Switch Statement Inefficiency (HIGH IMPACT)

**Location**: `src/core/json/include/sourcemeta/core/json_hash.h:60-137`

**Issue**: The `PropertyHashJSON::operator()` method uses a 32-case switch statement where all cases (1-31) perform identical operations calling `this->perfect(value, size)`. This creates unnecessary code bloat and potential branch misprediction overhead.

**Current Implementation**:
```cpp
switch (size) {
  case 0: return {};
  case 1: return this->perfect(value, 1);
  case 2: return this->perfect(value, 2);
  // ... 29 more identical cases
  case 31: return this->perfect(value, 31);
  default: // fallback hash logic
}
```

**Impact**: PropertyHashJSON is used extensively in JSON object operations, making this a critical performance path. The switch statement adds unnecessary complexity for what should be a simple conditional check.

**Proposed Fix**: Replace with simple conditional logic:
```cpp
if (size == 0) {
  return {};
} else if (size <= 31) {
  return this->perfect(value, size);
} else {
  // fallback hash logic
}
```

### 2. JSON fast_hash Algorithm Using std::accumulate (MEDIUM IMPACT)

**Location**: `src/core/json/json_value.cc:622-636`

**Issue**: The `fast_hash()` method for arrays and objects uses `std::accumulate` with lambda functions, which may not be optimally compiled and creates function call overhead.

**Current Implementation**:
```cpp
case Type::Array:
  return std::accumulate(
      this->as_array().cbegin(), this->as_array().cend(),
      static_cast<std::uint64_t>(6),
      [](const std::uint64_t accumulator, const JSON &item) {
        return accumulator + 1 + item.fast_hash();
      });
```

**Impact**: Hash computation is frequently used in object property lookups and equality comparisons.

**Proposed Fix**: Replace with manual loop for better performance and hash quality.

### 3. FlatMap Linear Search Operations (MEDIUM IMPACT)

**Location**: `src/core/json/include/sourcemeta/core/json_object.h:145-196`

**Issue**: The FlatMap implementation uses linear search through all entries for operations like `find()`, `try_at()`, and `contains()`. For larger objects, this becomes O(n) instead of the expected O(1) hash table performance.

**Current Implementation**:
```cpp
for (size_type index = 0; index < this->data.size(); index++) {
  if (this->data[index].hash == key_hash && this->data[index].first == key) {
    // found match
  }
}
```

**Impact**: Object property access performance degrades linearly with object size.

**Proposed Fix**: Consider implementing a hybrid approach with hash buckets for larger objects.

### 4. Array Uniqueness Check O(n²) Complexity (MEDIUM IMPACT)

**Location**: `src/core/json/json_value.cc:738-764`

**Issue**: The `unique()` method uses nested loops to check for duplicate elements, resulting in O(n²) time complexity.

**Current Implementation**:
```cpp
for (std::size_t index = 0; index < size; index++) {
  for (std::size_t subindex = index + 1; subindex < size; subindex++) {
    if (cache[index] == cache[subindex] && items[index] == items[subindex]) {
      return false;
    }
  }
}
```

**Impact**: Performance degrades quadratically with array size for uniqueness checks.

**Proposed Fix**: Use hash-based approach or sorting to achieve O(n log n) complexity.

### 5. String Parsing Memory Allocation Inefficiencies (LOW IMPACT)

**Location**: `src/core/json/parser.h:227-289`

**Issue**: String parsing uses `std::basic_ostringstream` which may perform multiple memory allocations during string construction.

**Impact**: Frequent small allocations during JSON parsing can impact performance.

**Proposed Fix**: Pre-allocate string buffers or use more efficient string building techniques.

## Benchmarking Context

The repository includes comprehensive benchmarks in the `benchmark/` directory:
- `json.cc`: Tests JSON parsing, object uniqueness, string equality, and hashing
- `jsonschema.cc`: Tests schema analysis and bundling operations  
- `jsonpointer.cc`: Tests JSON pointer traversal operations

These benchmarks provide a solid foundation for measuring the impact of optimizations.

## Recommendation

The PropertyHashJSON optimization should be implemented first due to its:
1. High impact on critical performance paths
2. Simple, low-risk implementation
3. Easy verification through existing benchmarks
4. Significant code size reduction (32 cases → 2 conditions)

## Implementation Priority

1. **HIGH**: PropertyHashJSON switch statement optimization
2. **MEDIUM**: JSON fast_hash algorithm improvement
3. **MEDIUM**: FlatMap linear search optimization
4. **MEDIUM**: Array uniqueness check optimization
5. **LOW**: String parsing memory allocation optimization

This analysis provides a roadmap for systematic performance improvements to the sourcemeta-evals/core library.
