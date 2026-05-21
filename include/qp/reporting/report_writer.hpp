#pragma once
#include "qp/backtest/metrics.hpp"
#include <filesystem>
namespace qp::reporting { struct ArtifactPaths { std::filesystem::path report_json; std::filesystem::path manifest_json; std::filesystem::path summary_txt; std::filesystem::path equity_csv; std::filesystem::path fills_csv; }; [[nodiscard]] Result<ArtifactPaths> write_backtest_artifacts(const std::filesystem::path& dir, const backtest::BacktestReport& report, const backtest::BacktestMetrics& metrics); }
