#include "qp/reporting/report_writer.hpp"

#include "qp/core/config.hpp"
#include "qp/schema/schema_version.hpp"

#include <filesystem>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <string>
#include <string_view>

namespace qp::reporting {
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

std::string csv_escape(std::string_view value) {
  const bool must_quote = value.find_first_of(",\r\n\"") != std::string_view::npos;
  if (!must_quote) return std::string(value);

  std::string out;
  out.reserve(value.size() + 2);
  out.push_back('"');
  for (const char c : value) {
    if (c == '"') out.push_back('"');
    out.push_back(c);
  }
  out.push_back('"');
  return out;
}

Result<void> write_text_file(const std::filesystem::path& path, const std::string& contents) {
  std::ofstream out(path);
  if (!out) return Err(Error{ErrorCode::IoError, "failed to write file", path.string()});
  out << contents;
  if (!out) return Err(Error{ErrorCode::IoError, "failed while writing file", path.string()});
  return Ok();
}

std::string build_report_json(const backtest::BacktestReport& report, const backtest::BacktestMetrics& metrics) {
  std::ostringstream out;
  out << "{\n"
      << "  \"schema_version\": \"" << schema::backtest_report_v1 << "\",\n"
      << "  \"run_id\": \"" << json_escape(report.run_id) << "\",\n"
      << "  \"bars_seen\": " << report.bars_seen << ",\n"
      << "  \"trades\": " << report.trades << ",\n"
      << "  \"initial_cash\": " << report.initial_cash << ",\n"
      << "  \"final_equity\": " << report.final_equity << ",\n"
      << "  \"total_return\": " << metrics.total_return << "\n"
      << "}\n";
  return out.str();
}

std::string build_manifest_json(const backtest::BacktestReport& report) {
  const auto config_fingerprint = schema::fnv1a64_hex(backtest::effective_config_json(report.config));
  const auto input_fingerprint = schema::fingerprint_file(report.config.data_path);

  std::ostringstream out;
  out << "{\n"
      << "  \"schema_version\": \"" << schema::run_manifest_v1 << "\",\n"
      << "  \"run_id\": \"" << json_escape(report.run_id) << "\",\n"
      << "  \"status\": \"success\",\n"
      << "  \"qp_version\": \"" << QP_VERSION << "\",\n"
      << "  \"git_sha\": \"unknown\",\n"
      << "  \"compiler\": \"" << json_escape(__VERSION__) << "\",\n"
      << "  \"config_schema_version\": \"" << schema::backtest_config_v1 << "\",\n"
      << "  \"report_schema_version\": \"" << schema::backtest_report_v1 << "\",\n"
      << "  \"config_fingerprint\": \"" << config_fingerprint << "\",\n"
      << "  \"input_data_path\": \"" << json_escape(report.config.data_path.string()) << "\",\n"
      << "  \"input_data_fingerprint\": \""
      << json_escape(input_fingerprint ? input_fingerprint.value() : "unavailable") << "\",\n"
      << "  \"artifacts\": {\n"
      << "    \"report\": \"report.json\",\n"
      << "    \"summary\": \"summary.txt\",\n"
      << "    \"equity\": \"equity.csv\",\n"
      << "    \"fills\": \"fills.csv\"\n"
      << "  }\n"
      << "}\n";
  return out.str();
}

std::string build_summary(const backtest::BacktestReport& report, const backtest::BacktestMetrics& metrics) {
  std::ostringstream out;
  out << "run_id: " << report.run_id << "\n"
      << "bars_seen: " << report.bars_seen << "\n"
      << "trades: " << report.trades << "\n"
      << "initial_cash: " << report.initial_cash << "\n"
      << "final_equity: " << report.final_equity << "\n"
      << "total_return: " << metrics.total_return << "\n";
  return out.str();
}

Result<void> write_equity_csv(const std::filesystem::path& path, const backtest::BacktestReport& report) {
  std::ofstream out(path);
  if (!out) return Err(Error{ErrorCode::IoError, "failed to write equity csv", path.string()});

  out << "ts_ns,equity,cash,gross_exposure,net_exposure\n";
  for (const auto& point : report.equity_curve) {
    out << point.ts.unix_nanos() << ',' << point.equity << ',' << point.cash << ',' << point.gross_exposure << ','
        << point.net_exposure << '\n';
  }
  if (!out) return Err(Error{ErrorCode::IoError, "failed while writing equity csv", path.string()});
  return Ok();
}

Result<void> write_fills_csv(const std::filesystem::path& path, const backtest::BacktestReport& report) {
  std::ofstream out(path);
  if (!out) return Err(Error{ErrorCode::IoError, "failed to write fills csv", path.string()});

  out << "order_id,symbol,side,ts_ns,quantity,price,commission,slippage\n";
  for (const auto& fill : report.fills) {
    out << fill.order_id.value << ',' << csv_escape(fill.symbol.value) << ','
        << (fill.side == execution::OrderSide::Buy ? "buy" : "sell") << ',' << fill.ts.unix_nanos() << ','
        << fill.quantity.value << ',' << fill.price.value << ',' << fill.commission << ',' << fill.slippage << '\n';
  }
  if (!out) return Err(Error{ErrorCode::IoError, "failed while writing fills csv", path.string()});
  return Ok();
}

}  // namespace

Result<ArtifactPaths> write_backtest_artifacts(const std::filesystem::path& dir,
                                               const backtest::BacktestReport& report,
                                               const backtest::BacktestMetrics& metrics) {
  std::error_code ec;
  std::filesystem::create_directories(dir, ec);
  if (ec) return Err<ArtifactPaths>(Error{ErrorCode::IoError, ec.message(), dir.string()});

  ArtifactPaths paths{dir / "report.json", dir / "manifest.json", dir / "summary.txt", dir / "equity.csv", dir / "fills.csv"};

  auto ok = write_text_file(paths.report_json, build_report_json(report, metrics));
  if (!ok) return Err<ArtifactPaths>(ok.error());

  ok = write_text_file(paths.manifest_json, build_manifest_json(report));
  if (!ok) return Err<ArtifactPaths>(ok.error());

  ok = write_text_file(paths.summary_txt, build_summary(report, metrics));
  if (!ok) return Err<ArtifactPaths>(ok.error());

  ok = write_equity_csv(paths.equity_csv, report);
  if (!ok) return Err<ArtifactPaths>(ok.error());

  ok = write_fills_csv(paths.fills_csv, report);
  if (!ok) return Err<ArtifactPaths>(ok.error());

  return Ok(paths);
}

}  // namespace qp::reporting
