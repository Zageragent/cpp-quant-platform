#pragma once
#include "qp/core/time.hpp"
#include "qp/data/market_data.hpp"
#include <compare>
#include <cstdint>
#include <variant>
namespace qp::replay {
struct SequenceNumber { std::uint64_t value{0}; friend auto operator<=>(const SequenceNumber&, const SequenceNumber&) = default; };
struct MarketDataEvent { SequenceNumber sequence; Timestamp receive_ts; Timestamp exchange_ts; data::MarketData data; };
struct TimerEvent { SequenceNumber sequence; Timestamp ts; };
using ReplayEvent = std::variant<MarketDataEvent, TimerEvent>;
[[nodiscard]] Timestamp event_time(const ReplayEvent& event);
[[nodiscard]] SequenceNumber sequence_number(const ReplayEvent& event);
[[nodiscard]] bool event_less(const ReplayEvent& a, const ReplayEvent& b);
}
