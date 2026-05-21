#pragma once
#include <chrono>
#include <compare>
#include <cstdint>
namespace qp {
using Nanoseconds = std::chrono::nanoseconds;
struct Timestamp {
  std::chrono::sys_time<std::chrono::nanoseconds> value{};
  constexpr Timestamp() noexcept = default;
  explicit constexpr Timestamp(std::chrono::sys_time<std::chrono::nanoseconds> v) noexcept : value(v) {}
  [[nodiscard]] static constexpr Timestamp from_unix_nanos(std::int64_t ns) noexcept { return Timestamp{std::chrono::sys_time<std::chrono::nanoseconds>{std::chrono::nanoseconds{ns}}}; }
  [[nodiscard]] constexpr std::int64_t unix_nanos() const noexcept { return value.time_since_epoch().count(); }
  friend auto operator<=>(const Timestamp&, const Timestamp&) = default;
};
struct TimeRange {
  Timestamp start{};
  Timestamp end{};
  [[nodiscard]] constexpr bool contains(Timestamp ts) const noexcept { return start <= ts && ts < end; }
  [[nodiscard]] constexpr bool valid() const noexcept { return start < end; }
};
}
