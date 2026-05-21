#pragma once
#include "qp/core/config.hpp"
#include <filesystem>
#include <string>
namespace qp::backtest {
struct BacktestRunConfig { std::string schema_version{"qp.backtest.config.v1"}; std::string run_id{"run"}; std::filesystem::path data_path; std::filesystem::path output_dir{"out/run"}; double initial_cash{100000.0}; double buy_threshold{0.01}; double sell_threshold{-0.01}; double fee_per_trade{0.0}; double slippage_bps{0.0}; std::string base_currency{"USD"}; };
[[nodiscard]] Result<BacktestRunConfig> backtest_config_from_flat_config(const Config& config);
[[nodiscard]] Result<BacktestRunConfig> load_backtest_run_config(const std::filesystem::path& path);
[[nodiscard]] std::string effective_config_json(const BacktestRunConfig& config);
}
