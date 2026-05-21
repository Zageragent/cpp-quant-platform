#pragma once
#include "qp/data/bar.hpp"
#include "qp/features/feature.hpp"
#include "qp/labels/label.hpp"
#include "qp/signals/momentum.hpp"
#include <filesystem>
#include <vector>
namespace qp::data {
[[nodiscard]] Result<void> write_bars_csv(const std::filesystem::path& path, const std::vector<OhlcvBar>& bars);
[[nodiscard]] Result<void> write_features_csv(const std::filesystem::path& path, const std::vector<features::FeatureValue>& features);
[[nodiscard]] Result<void> write_labels_csv(const std::filesystem::path& path, const std::vector<labels::LabelValue>& labels);
[[nodiscard]] Result<void> write_signals_csv(const std::filesystem::path& path, const std::vector<signals::Signal>& signals);
}
