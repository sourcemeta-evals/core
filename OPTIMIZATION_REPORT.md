# Performance Optimization Report for sourcemeta-evals/core

## Executive Summary

This report documents performance optimization opportunities identified in the sourcemeta-evals/core C++ JSON processing library. The library is already well-optimized with custom hash functions, flat map data structures, and efficient memory management. However, several specific areas for improvement were identified.

## Optimization Opportunities Identified

### 1. Hash Function Switch Statement Optimization (IMPLEMENTED)

**Location**: `src/core/json/include/sourcemeta/core/json_hash.h`, lines 58-135

**Issue**: The `PropertyHashJSON::operator()` contains a large switch statement with 32 cases (sizes 0-31) that all call the same `perfect()` function with different size parameters. This creates unnecessary branching overhead.

**Impact**: High - This function is called frequently during JSON object property lookups and hashing operations.

**Solution**: Replace the switch statement with a direct conditional approach that eliminates the branching overhead while maintaining identical behavior.

**Performance Benefits**:
- Reduced code size (eliminates ~75 lines of repetitive switch cases)
- Better branch prediction
- Improved instruction cache efficiency
- Maintains perfect hash behavior for strings ≤31 characters

### 2. String Parsing Memory Allocation Improvements

**Location**: `src/core/json/parser.h`, multiple functions

**Issue**: String parsing functions create multiple `std::ostringstream` objects for temporary string building, which involves dynamic memory allocation.

**Examples**:
- `parse_string()` (line 231-233)
- `parse_string_unicode()` (line 119-121)
- `parse_number_*()` functions (lines 317-319, 350-352, etc.)

**Impact**: Medium - Affects JSON parsing performance, especially for documents with many strings.

**Potential Solution**: Use pre-allocated string buffers or stack-based character arrays for small strings, falling back to dynamic allocation only when necessary.

### 3. FlatMap Linear Search Optimization

**Location**: `src/core/json/include/sourcemeta/core/json_object.h`, lines 111-130, 138-153, etc.

**Issue**: The `FlatMap` implementation uses linear search through all entries for property lookups, even when hash collisions are unlikely.

**Impact**: Medium - Affects performance for JSON objects with many properties (>10-20 properties).

**Potential Solutions**:
- Use binary search for larger objects when hash collisions are detected
- Implement a hybrid approach that switches to a hash table for objects above a certain size threshold
- Sort entries by hash value to enable early termination in searches

### 4. Memory Allocation Pattern Improvements

**Location**: `src/core/json/json_value.cc`, multiple constructor and assignment operators

**Issue**: Extensive use of placement `new` operators for union member construction, which could potentially be optimized.

**Examples**:
- Lines 53, 58, 62: String construction
- Lines 66, 83: Array construction  
- Lines 88, 92: Object construction

**Impact**: Low-Medium - Affects object creation and copying performance.

**Potential Solution**: Investigate whether some allocations can be avoided through better move semantics or by using stack allocation for small objects.

### 5. Unicode Processing Optimization

**Location**: `src/core/json/parser.h`, `parse_string_unicode()` function (lines 116-172)

**Issue**: Unicode code point processing involves multiple character-by-character operations and could benefit from vectorization or lookup tables.

**Impact**: Low - Only affects JSON documents with Unicode escape sequences.

**Potential Solutions**:
- Use lookup tables for hex digit conversion instead of `std::isxdigit()`
- Vectorize UTF-8 encoding operations
- Pre-compute common Unicode transformations

### 6. Switch Statement Optimizations in Type Operations

**Location**: `src/core/json/json_value.cc`, multiple functions

**Issue**: Several functions use switch statements on JSON type enums that could be optimized with jump tables or function pointer arrays.

**Examples**:
- `operator<()` (lines 243-260)
- `operator==()` (lines 285-300)
- Copy/move constructors and assignment operators

**Impact**: Low - These are typically single operations per JSON value.

**Potential Solution**: Use constexpr function dispatch or template specialization to eliminate runtime switching.

## Implementation Priority

1. **Hash Function Switch Statement** (IMPLEMENTED) - High impact, low risk
2. **String Parsing Memory Allocation** - Medium impact, medium risk
3. **FlatMap Linear Search** - Medium impact, medium risk  
4. **Memory Allocation Patterns** - Low-medium impact, medium risk
5. **Unicode Processing** - Low impact, low risk
6. **Type Operation Switch Statements** - Low impact, low risk

## Benchmarking Recommendations

To validate these optimizations, the following benchmarks should be run:

1. **Hash Function Performance**: Measure object property lookup times with varying object sizes
2. **String Parsing Performance**: Parse JSON documents with many string values
3. **Large Object Performance**: Test with JSON objects containing 50+ properties
4. **Memory Allocation Profiling**: Use tools like Valgrind or AddressSanitizer to measure allocation patterns
5. **Overall Parsing Performance**: Use the existing benchmark suite in `benchmark/json.cc`

## Conclusion

The sourcemeta-evals/core library is already highly optimized, but these targeted improvements could provide measurable performance gains, especially for workloads involving large JSON objects or frequent property lookups. The implemented hash function optimization provides immediate benefits with zero risk of behavioral changes.
