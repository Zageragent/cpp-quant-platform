#pragma once
#include "qp/data/bar.hpp"
#include "qp/data/trade.hpp"
#include "qp/data/quote.hpp"
#include <variant>
namespace qp::data {
using MarketData = std::variant<OhlcvBar, Trade, Quote>;
[[nodiscard]] Timestamp timestamp(const MarketData& data);
[[nodiscard]] const Symbol& symbol(const MarketData& data);
[[nodiscard]] Result<void> validate(const MarketData& data);
[[nodiscard]] double reference_price(const MarketData& data);
}
