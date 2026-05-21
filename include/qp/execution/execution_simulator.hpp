#pragma once
#include "qp/data/market_data.hpp"
#include "qp/execution/commission_model.hpp"
#include "qp/execution/fill.hpp"
#include "qp/execution/slippage_model.hpp"
#include <optional>
namespace qp::execution {
struct ExecutionSimulatorConfig { bool allow_partial_fills{false}; double max_participation_rate{1.0}; bool reject_if_insufficient_bar_volume{false}; };
struct ExecutionContext { Timestamp ts; data::MarketData market_data; };
class ExecutionSimulator { public: ExecutionSimulator(const CommissionModel& commission_model, const SlippageModel& slippage_model, ExecutionSimulatorConfig config={}); [[nodiscard]] Result<std::optional<Fill>> execute(const Order& order, const ExecutionContext& context) const; private: const CommissionModel& commission_model_; const SlippageModel& slippage_model_; ExecutionSimulatorConfig config_; };
}
