#pragma once
#include "qp/backtest/backtest_config.hpp"
#include "qp/data/bar.hpp"
#include "qp/execution/fill.hpp"
#include "qp/portfolio/portfolio.hpp"
#include <map>
#include <vector>
namespace qp::backtest {
struct EquityPoint { Timestamp ts; double equity{0.0}; double cash{0.0}; double gross_exposure{0.0}; double net_exposure{0.0}; };
struct BacktestReport { std::string schema_version{"qp.backtest.report.v1"}; std::string run_id; BacktestRunConfig config; double initial_cash{0.0}; double final_equity{0.0}; double cash{0.0}; std::size_t bars_seen{0}; std::size_t trades{0}; std::map<Symbol, portfolio::Position> positions; std::vector<EquityPoint> equity_curve; std::vector<execution::Fill> fills; std::vector<portfolio::TradeLedgerEntry> ledger; std::vector<std::string> warnings; };
[[nodiscard]] Result<BacktestReport> run_configured_momentum_backtest(const std::vector<data::OhlcvBar>& bars, const BacktestRunConfig& config);
}
