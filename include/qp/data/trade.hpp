#pragma once
#include "qp/core/result.hpp"
#include "qp/core/time.hpp"
#include "qp/core/types.hpp"
#include <optional>
#include <string>
namespace qp::data {
enum class TradeSide { Unknown, BuyerInitiated, SellerInitiated };
struct Trade { Symbol symbol; Timestamp ts; Price price; Quantity quantity; TradeSide side{TradeSide::Unknown}; std::optional<std::string> trade_id; [[nodiscard]] Result<void> validate() const; };
}
