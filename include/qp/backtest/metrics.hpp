#pragma once
#include "qp/backtest/report.hpp"
namespace qp::backtest {
struct BacktestMetrics { double total_return{0.0}; double max_drawdown{0.0}; double max_drawdown_pct{0.0}; double win_rate{0.0}; double profit_factor{0.0}; std::size_t trades{0}; std::size_t winning_trades{0}; std::size_t losing_trades{0}; double total_commission{0.0}; double total_slippage{0.0}; };
[[nodiscard]] Result<BacktestMetrics> compute_metrics(const BacktestReport& report);
}
