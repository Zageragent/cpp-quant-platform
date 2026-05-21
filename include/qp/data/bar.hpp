#pragma once
#include "qp/core/result.hpp"
#include "qp/core/time.hpp"
#include "qp/core/types.hpp"
namespace qp::data {
enum class BarInterval { Tick, Second1, Minute1, Minute5, Hour1, Day1 };
struct OhlcvBar {
  Symbol symbol;
  Timestamp start;
  Timestamp end;
  BarInterval interval{BarInterval::Day1};
  Price open, high, low, close;
  Quantity volume;
  [[nodiscard]] bool is_time_valid() const noexcept { return start < end; }
  [[nodiscard]] bool is_price_valid() const noexcept;
  [[nodiscard]] bool is_volume_valid() const noexcept { return volume.is_non_negative(); }
  [[nodiscard]] Result<void> validate() const;
};
}
