#pragma once
#include "qp/execution/order.hpp"
namespace qp::execution {
struct Fill { OrderId order_id; Symbol symbol; OrderSide side{OrderSide::Buy}; Timestamp ts; Quantity quantity; Price price; double commission{0.0}; double slippage{0.0}; [[nodiscard]] Result<void> validate() const; };
}
