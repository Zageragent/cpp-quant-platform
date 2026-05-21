#pragma once
#include <cmath>
#include <compare>
#include <stdexcept>
#include <string>
#include <string_view>
namespace qp {
struct Symbol {
  std::string value;
  explicit Symbol(std::string v="");
  [[nodiscard]] std::string_view str() const noexcept { return value; }
  [[nodiscard]] bool empty() const noexcept { return value.empty(); }
  friend auto operator<=>(const Symbol&, const Symbol&) = default;
};
struct Exchange { std::string value; explicit Exchange(std::string v=""); [[nodiscard]] std::string_view str() const noexcept { return value; } friend auto operator<=>(const Exchange&, const Exchange&) = default; };
struct Price { double value{0.0}; explicit constexpr Price(double v=0.0) noexcept : value(v) {} [[nodiscard]] bool is_positive() const noexcept { return std::isfinite(value) && value > 0.0; } friend auto operator<=>(const Price&, const Price&) = default; };
struct Quantity { double value{0.0}; explicit constexpr Quantity(double v=0.0) noexcept : value(v) {} [[nodiscard]] bool is_non_negative() const noexcept { return std::isfinite(value) && value >= 0.0; } [[nodiscard]] bool is_positive() const noexcept { return std::isfinite(value) && value > 0.0; } friend auto operator<=>(const Quantity&, const Quantity&) = default; };
struct Notional { double value{0.0}; explicit constexpr Notional(double v=0.0) noexcept : value(v) {} friend auto operator<=>(const Notional&, const Notional&) = default; };
struct BasisPoints { double value{0.0}; explicit constexpr BasisPoints(double v=0.0) noexcept : value(v) {} };
[[nodiscard]] constexpr Notional operator*(Price p, Quantity q) noexcept { return Notional{p.value*q.value}; }
}
