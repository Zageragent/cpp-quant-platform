#pragma once
#include "qp/data/bar.hpp"
#include "qp/core/result.hpp"
#include <vector>
namespace qp::backtest {
struct SimpleBacktestConfig { double initial_cash{100000.0}; double buy_threshold{0.0}; double sell_threshold{0.0}; double fee_per_trade{0.0}; };
struct SimpleBacktestResult { double final_equity{0.0}; double cash{0.0}; double position_quantity{0.0}; std::size_t bars_seen{0}; std::size_t trades{0}; };
[[nodiscard]] Result<SimpleBacktestResult> run_simple_momentum_backtest(const std::vector<data::OhlcvBar>& bars, const SimpleBacktestConfig& config = {});
}
