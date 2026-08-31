#ifndef SOURCEMETA_CORE_NUMERIC_DECIMAL_H_
#define SOURCEMETA_CORE_NUMERIC_DECIMAL_H_

#ifndef SOURCEMETA_CORE_NUMERIC_EXPORT
#include <sourcemeta/core/numeric_export.h>
#endif

#include <cassert>  // assert
#include <concepts> // std::integral
#include <cstdint>  // std::int32_t, std::int64_t, std::uint32_t, std::uint64_t
#include <string>   // std::string
#include <string_view> // std::string_view
#include <type_traits> // std::is_signed_v

#if defined(__GNUC__) || defined(__clang__)
#define SOURCEMETA_NUMERIC_DECIMAL_FORCE_INLINE [[gnu::always_inline]]
#elif defined(_MSC_VER)
#define SOURCEMETA_NUMERIC_DECIMAL_FORCE_INLINE [[msvc::forceinline]]
#else
#define SOURCEMETA_NUMERIC_DECIMAL_FORCE_INLINE
#endif

namespace sourcemeta::core {

class SOURCEMETA_CORE_NUMERIC_EXPORT Decimal {
public:
  Decimal() noexcept;

  ~Decimal();

  Decimal(const Decimal &other);

  Decimal(Decimal &&other) noexcept;

  template <typename T>
    requires std::integral<T> && std::is_signed_v<T> &&
             (!std::same_as<T, std::int64_t>)
  Decimal(const T value) : Decimal{static_cast<std::int64_t>(value)} {}

  Decimal(std::int64_t value);

  template <typename T>
    requires std::integral<T> && std::is_unsigned_v<T> &&
             (!std::same_as<T, std::uint64_t>)
  Decimal(const T value) : Decimal{static_cast<std::uint64_t>(value)} {}

  Decimal(std::uint64_t value);

  explicit Decimal(float value);

  explicit Decimal(double value);

  explicit Decimal(const char *const value);

  explicit Decimal(const std::string &value);

  explicit Decimal(const std::string_view value);

  auto operator=(const Decimal &other) -> Decimal &;

  auto operator=(Decimal &&other) noexcept -> Decimal &;

  [[nodiscard]] static auto nan(std::uint64_t payload = 0) -> Decimal;

  [[nodiscard]] static auto snan(std::uint64_t payload = 0) -> Decimal;

  [[nodiscard]] static auto strict_from(double value) -> Decimal;

  [[nodiscard]] static auto infinity() -> Decimal;

  [[nodiscard]] static auto negative_infinity() -> Decimal;

  [[nodiscard]] auto to_scientific_string() const -> std::string;

  [[nodiscard]] auto to_string() const -> std::string;

  [[nodiscard]] auto to_int64() const -> std::int64_t;

  [[nodiscard]] auto to_int32() const -> std::int32_t;

  [[nodiscard]] auto to_uint64() const -> std::uint64_t;

  [[nodiscard]] auto to_uint32() const -> std::uint32_t;

  [[nodiscard]] auto to_float() const -> float;

  [[nodiscard]] auto to_double() const -> double;

  [[nodiscard]] auto is_zero() const -> bool;

  [[nodiscard]] auto is_integral() const -> bool;

  [[nodiscard]] SOURCEMETA_NUMERIC_DECIMAL_FORCE_INLINE inline auto
  is_integer() const -> bool {
    return (this->flags_ & FLAG_INTEGER_LITERAL) != 0;
  }

  [[nodiscard]] SOURCEMETA_NUMERIC_DECIMAL_FORCE_INLINE inline auto
  is_finite() const -> bool {
    return !(this->flags_ & (FLAG_NAN | FLAG_SNAN | FLAG_INFINITE));
  }

  [[nodiscard]] SOURCEMETA_NUMERIC_DECIMAL_FORCE_INLINE inline auto
  is_real() const -> bool {
    return this->is_finite() && !this->is_integral();
  }

  [[nodiscard]] auto is_float() const -> bool;

  [[nodiscard]] auto is_double() const -> bool;

  [[nodiscard]] auto is_int32() const -> bool;

  [[nodiscard]] auto is_int64() const -> bool;

  [[nodiscard]] auto is_uint32() const -> bool;

  [[nodiscard]] auto is_uint64() const -> bool;

  [[nodiscard]] SOURCEMETA_NUMERIC_DECIMAL_FORCE_INLINE inline auto
  is_nan() const -> bool {
    return (this->flags_ & FLAG_NAN) != 0;
  }

  [[nodiscard]] SOURCEMETA_NUMERIC_DECIMAL_FORCE_INLINE inline auto
  is_snan() const -> bool {
    return (this->flags_ & FLAG_SNAN) != 0;
  }

  [[nodiscard]] SOURCEMETA_NUMERIC_DECIMAL_FORCE_INLINE inline auto
  is_qnan() const -> bool {
    return (this->flags_ & FLAG_NAN) != 0 && !(this->flags_ & FLAG_SNAN);
  }

  [[nodiscard]] SOURCEMETA_NUMERIC_DECIMAL_FORCE_INLINE inline auto
  nan_payload() const -> std::uint64_t {
    assert(this->is_nan());
    return static_cast<std::uint64_t>(this->coefficient_);
  }

  [[nodiscard]] SOURCEMETA_NUMERIC_DECIMAL_FORCE_INLINE inline auto
  is_infinite() const -> bool {
    return (this->flags_ & FLAG_INFINITE) != 0;
  }

  [[nodiscard]] SOURCEMETA_NUMERIC_DECIMAL_FORCE_INLINE inline auto
  is_signed() const -> bool {
    return (this->flags_ & FLAG_SIGN) != 0;
  }

  [[nodiscard]] auto to_integral() const -> Decimal;

  [[nodiscard]] auto divisible_by(const Decimal &divisor) const -> bool;

  [[nodiscard]] auto reduce() const -> Decimal;

  [[nodiscard]] auto logb() const -> Decimal;

  [[nodiscard]] auto scale_by(const Decimal &scale) const -> Decimal;

  [[nodiscard]] auto same_quantum(const Decimal &other) const -> bool;

  [[nodiscard]] auto compare_total(const Decimal &other) const -> Decimal;

  [[nodiscard]] auto divide_integer(const Decimal &other) const -> Decimal;

  auto operator+=(const Decimal &other) -> Decimal &;

  auto operator-=(const Decimal &other) -> Decimal &;

  auto operator*=(const Decimal &other) -> Decimal &;

  auto operator/=(const Decimal &other) -> Decimal &;

  auto operator%=(const Decimal &other) -> Decimal &;

  [[nodiscard]] auto operator+(const Decimal &other) const -> Decimal;

  [[nodiscard]] auto operator-(const Decimal &other) const -> Decimal;

  [[nodiscard]] auto operator*(const Decimal &other) const -> Decimal;

  [[nodiscard]] auto operator/(const Decimal &other) const -> Decimal;

  [[nodiscard]] auto operator%(const Decimal &other) const -> Decimal;

  [[nodiscard]] auto operator-() const -> Decimal;

  [[nodiscard]] auto operator+() const -> Decimal;

  auto operator++() -> Decimal &;

  auto operator++(int) -> Decimal;

  auto operator--() -> Decimal &;

  auto operator--(int) -> Decimal;

  [[nodiscard]] auto operator==(const Decimal &other) const -> bool;

  [[nodiscard]] auto operator!=(const Decimal &other) const -> bool;

  [[nodiscard]] auto operator<(const Decimal &other) const -> bool;

  [[nodiscard]] auto operator<=(const Decimal &other) const -> bool;

  [[nodiscard]] auto operator>(const Decimal &other) const -> bool;

  [[nodiscard]] auto operator>=(const Decimal &other) const -> bool;

  [[nodiscard]] auto helper_stub() const noexcept -> int;

private:
  static constexpr std::uint8_t FLAG_SIGN = 0x01;
  static constexpr std::uint8_t FLAG_NAN = 0x02;
  static constexpr std::uint8_t FLAG_SNAN = 0x04;
  static constexpr std::uint8_t FLAG_INFINITE = 0x08;
  static constexpr std::uint8_t FLAG_INTEGER_LITERAL = 0x40;

  std::int64_t coefficient_{0};
  std::uint64_t coefficient_high_{0};
  std::int32_t exponent_{0};
  std::uint8_t flags_{0};
};

template <typename T>
  requires std::integral<T>
inline auto operator+(const T left, const Decimal &right) -> Decimal {
  return Decimal{left} + right;
}

template <typename T>
  requires std::integral<T>
inline auto operator-(const T left, const Decimal &right) -> Decimal {
  return Decimal{left} - right;
}

template <typename T>
  requires std::integral<T>
inline auto operator*(const T left, const Decimal &right) -> Decimal {
  return Decimal{left} * right;
}

template <typename T>
  requires std::integral<T>
inline auto operator/(const T left, const Decimal &right) -> Decimal {
  return Decimal{left} / right;
}

template <typename T>
  requires std::integral<T>
inline auto operator%(const T left, const Decimal &right) -> Decimal {
  return Decimal{left} % right;
}

template <typename T>
  requires std::integral<T>
inline auto operator==(const T left, const Decimal &right) -> bool {
  return Decimal{left} == right;
}

template <typename T>
  requires std::integral<T>
inline auto operator!=(const T left, const Decimal &right) -> bool {
  return Decimal{left} != right;
}

template <typename T>
  requires std::integral<T>
inline auto operator<(const T left, const Decimal &right) -> bool {
  return Decimal{left} < right;
}

template <typename T>
  requires std::integral<T>
inline auto operator<=(const T left, const Decimal &right) -> bool {
  return Decimal{left} <= right;
}

template <typename T>
  requires std::integral<T>
inline auto operator>(const T left, const Decimal &right) -> bool {
  return Decimal{left} > right;
}

template <typename T>
  requires std::integral<T>
inline auto operator>=(const T left, const Decimal &right) -> bool {
  return Decimal{left} >= right;
}

} // namespace sourcemeta::core

#endif
