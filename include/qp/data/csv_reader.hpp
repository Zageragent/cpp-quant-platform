#pragma once
#include "qp/core/result.hpp"
#include "qp/data/bar.hpp"
#include <filesystem>
#include <vector>
namespace qp::data {
struct CsvOptions { bool has_header{true}; char delimiter{','}; bool strict{true}; };
[[nodiscard]] Result<std::vector<OhlcvBar>> read_ohlcv_csv(const std::filesystem::path& path, CsvOptions options = {});
}
