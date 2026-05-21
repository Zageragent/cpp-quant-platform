#pragma once
#include "qp/core/result.hpp"
#include <filesystem>
#include <string>
#include <string_view>
#include <vector>
namespace qp::schema {
inline constexpr std::string_view backtest_config_v1 = "qp.backtest.config.v1";
inline constexpr std::string_view backtest_report_v1 = "qp.backtest.report.v1";
inline constexpr std::string_view run_manifest_v1 = "qp.run_manifest.v1";
inline constexpr std::string_view dataset_bars_v1 = "qp.dataset.bars.v1";
inline constexpr std::string_view dataset_features_v1 = "qp.dataset.features.v1";
inline constexpr std::string_view dataset_labels_v1 = "qp.dataset.labels.v1";
inline constexpr std::string_view dataset_signals_v1 = "qp.dataset.signals.v1";
inline constexpr std::string_view instrument_v1 = "qp.instrument.v1";
[[nodiscard]] std::vector<std::string_view> known_schema_versions();
[[nodiscard]] bool is_known_schema_version(std::string_view version);
[[nodiscard]] Result<std::string> fingerprint_file(const std::filesystem::path& path);
[[nodiscard]] std::string fnv1a64_hex(std::string_view text);
}
