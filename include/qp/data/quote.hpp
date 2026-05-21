#pragma once
#include "qp/core/result.hpp"
#include "qp/core/time.hpp"
#include "qp/core/types.hpp"
namespace qp::data {
struct Quote { Symbol symbol; Timestamp ts; Price bid_price; Quantity bid_quantity; Price ask_price; Quantity ask_quantity; [[nodiscard]] Price mid() const noexcept; [[nodiscard]] double spread() const noexcept; [[nodiscard]] double spread_bps() const noexcept; [[nodiscard]] Result<void> validate() const; };
}
