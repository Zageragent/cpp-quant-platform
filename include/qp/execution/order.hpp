#pragma once
#include "qp/core/result.hpp"
#include "qp/core/time.hpp"
#include "qp/core/types.hpp"
#include <cstdint>
#include <optional>
#include <string>
namespace qp::execution {
enum class OrderSide { Buy, Sell };
enum class OrderType { Market, Limit };
enum class TimeInForce { Day, Gtc, Ioc };
enum class OrderStatus { New, Accepted, PartiallyFilled, Filled, Cancelled, Rejected };
struct OrderId { std::uint64_t value{0}; friend auto operator<=>(const OrderId&, const OrderId&) = default; };
struct Order { OrderId id; Symbol symbol; OrderSide side{OrderSide::Buy}; OrderType type{OrderType::Market}; Quantity quantity; std::optional<Price> limit_price; TimeInForce tif{TimeInForce::Day}; Timestamp created_ts; std::string strategy_id; [[nodiscard]] Result<void> validate() const; };
}
