#include "qp/backtest/backtest_config.hpp"

#include <cmath>
#include <iomanip>
#include <sstream>
#include <string_view>

namespace qp::backtest {
namespace {

std::string json_escape(std::string_view value) {
  std::ostringstream out;
  for (const char raw : value) {
    const auto c = static_cast<unsigned char>(raw);
    switch (c) {
      case '"': out << "\\\""; break;
      case '\\': out << "\\\\"; break;
      case '\b': out << "\\b"; break;
      case '\f': out << "\\f"; break;
      case '\n': out << "\\n"; break;
      case '\r': out << "\\r"; break;
      case '\t': out << "\\t"; break;
      default:
        if (c < 0x20) {
          out << "\\u" << std::hex << std::setw(4) << std::setfill('0') << static_cast<int>(c)
              << std::dec << std::setfill(' ');
        } else {
          out << static_cast<char>(c);
        }
    }
  }
  return out.str();
}

template <class T>
Result<void> apply_optional_double(const Config& config, std::string_view key, T& target) {
  if (!config.get(key)) return Ok();
  auto value = config.get_double(key);
  if (!value) return Err(value.error());
  target = value.value();
  return Ok();
}

}  // namespace

Result<BacktestRunConfig> backtest_config_from_flat_config(const Config& config) {
  BacktestRunConfig out;

  if (auto value = config.get("schema_version")) {
    out.schema_version = std::string(*value);
    if (out.schema_version != "qp.backtest.config.v1") {
      return Err<BacktestRunConfig>(Error{ErrorCode::ConfigurationError, "unsupported schema_version"});
    }
  }

  if (auto value = config.get("run.id")) out.run_id = std::string(*value);
  if (auto value = config.get("run.output_dir")) out.output_dir = std::string(*value);
  if (auto value = config.get("data.path")) {
    out.data_path = std::string(*value);
  } else {
    return Err<BacktestRunConfig>(Error{ErrorCode::ConfigurationError, "missing data.path"});
  }

  auto ok = apply_optional_double(config, "portfolio.initial_cash", out.initial_cash);
  if (!ok) return Err<BacktestRunConfig>(ok.error());
  ok = apply_optional_double(config, "strategy.buy_threshold", out.buy_threshold);
  if (!ok) return Err<BacktestRunConfig>(ok.error());
  ok = apply_optional_double(config, "strategy.sell_threshold", out.sell_threshold);
  if (!ok) return Err<BacktestRunConfig>(ok.error());
  ok = apply_optional_double(config, "execution.fee_per_trade", out.fee_per_trade);
  if (!ok) return Err<BacktestRunConfig>(ok.error());
  ok = apply_optional_double(config, "execution.slippage_bps", out.slippage_bps);
  if (!ok) return Err<BacktestRunConfig>(ok.error());

  if (auto value = config.get("portfolio.base_currency")) out.base_currency = std::string(*value);

  if (out.run_id.empty()) return Err<BacktestRunConfig>(Error{ErrorCode::ConfigurationError, "run.id cannot be empty"});
  if (out.data_path.empty()) return Err<BacktestRunConfig>(Error{ErrorCode::ConfigurationError, "data.path cannot be empty"});
  if (!std::isfinite(out.initial_cash) || out.initial_cash <= 0.0) {
    return Err<BacktestRunConfig>(Error{ErrorCode::ConfigurationError, "portfolio.initial_cash must be finite and positive"});
  }
  if (!std::isfinite(out.buy_threshold) || !std::isfinite(out.sell_threshold)) {
    return Err<BacktestRunConfig>(Error{ErrorCode::ConfigurationError, "strategy thresholds must be finite"});
  }
  if (!std::isfinite(out.fee_per_trade) || out.fee_per_trade < 0.0) {
    return Err<BacktestRunConfig>(Error{ErrorCode::ConfigurationError, "execution.fee_per_trade must be finite and non-negative"});
  }
  if (!std::isfinite(out.slippage_bps) || out.slippage_bps < 0.0) {
    return Err<BacktestRunConfig>(Error{ErrorCode::ConfigurationError, "execution.slippage_bps must be finite and non-negative"});
  }

  return Ok(out);
}

Result<BacktestRunConfig> load_backtest_run_config(const std::filesystem::path& path) {
  auto config = load_flat_json_config(path);
  if (!config) return Err<BacktestRunConfig>(config.error());
  return backtest_config_from_flat_config(config.value());
}

std::string effective_config_json(const BacktestRunConfig& config) {
  std::ostringstream out;
  out << "{\n"
      << "  \"schema_version\": \"" << json_escape(config.schema_version) << "\",\n"
      << "  \"run.id\": \"" << json_escape(config.run_id) << "\",\n"
      << "  \"run.output_dir\": \"" << json_escape(config.output_dir.string()) << "\",\n"
      << "  \"data.path\": \"" << json_escape(config.data_path.string()) << "\",\n"
      << "  \"portfolio.initial_cash\": " << config.initial_cash << ",\n"
      << "  \"strategy.buy_threshold\": " << config.buy_threshold << ",\n"
      << "  \"strategy.sell_threshold\": " << config.sell_threshold << ",\n"
      << "  \"execution.fee_per_trade\": " << config.fee_per_trade << ",\n"
      << "  \"execution.slippage_bps\": " << config.slippage_bps << "\n"
      << "}\n";
  return out.str();
}

}  // namespace qp::backtest
