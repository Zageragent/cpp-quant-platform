#include "qp/backtest/backtest_config.hpp"
#include "qp/backtest/metrics.hpp"
#include "qp/backtest/report.hpp"
#include "qp/data/csv_reader.hpp"
#include "qp/reporting/report_writer.hpp"

#include <filesystem>
#include <iostream>
#include <string>

namespace {

void usage() {
  std::cout << "qp " << QP_VERSION << "\n\n"
            << "Usage:\n"
            << "  qp smoke\n"
            << "  qp --help\n"
            << "  qp --version\n"
            << "  qp inspect-csv --data <bars.csv>\n"
            << "  qp validate-config --config <config.json>\n"
            << "  qp backtest --config <config.json> [--output-dir <dir>]\n";
}

std::string arg_value(int argc, char** argv, const std::string& key) {
  for (int i = 1; i + 1 < argc; ++i) {
    if (argv[i] == key) return argv[i + 1];
  }
  return {};
}

void print_error(const qp::Error& error) {
  std::cerr << error.message;
  if (!error.context.empty()) std::cerr << ' ' << error.context;
  std::cerr << '\n';
}

int smoke() {
  qp::data::OhlcvBar bar{qp::Symbol{"SPY"},
                         qp::Timestamp::from_unix_nanos(0),
                         qp::Timestamp::from_unix_nanos(1),
                         qp::data::BarInterval::Day1,
                         qp::Price{100},
                         qp::Price{101},
                         qp::Price{99},
                         qp::Price{100.5},
                         qp::Quantity{1000}};

  auto ok = bar.validate();
  if (!ok) {
    print_error(ok.error());
    return 1;
  }

  std::cout << "cpp-quant-platform " << QP_VERSION << " smoke ok\n";
  return 0;
}

int inspect_csv(int argc, char** argv) {
  const auto path = arg_value(argc, argv, "--data");
  if (path.empty()) {
    std::cerr << "missing --data\n";
    return 2;
  }

  auto bars = qp::data::read_ohlcv_csv(path);
  if (!bars) {
    print_error(bars.error());
    return 4;
  }

  std::cout << "rows: " << bars.value().size() << "\n";
  if (!bars.value().empty()) std::cout << "first_symbol: " << bars.value().front().symbol.value << "\n";
  return 0;
}

int validate_config(int argc, char** argv) {
  const auto path = arg_value(argc, argv, "--config");
  if (path.empty()) {
    std::cerr << "missing --config\n";
    return 2;
  }

  auto config = qp::backtest::load_backtest_run_config(path);
  if (!config) {
    print_error(config.error());
    return 3;
  }

  std::cout << "config ok\nrun_id: " << config.value().run_id << "\n";
  return 0;
}

void resolve_config_relative_data_path(const std::filesystem::path& config_path, qp::backtest::BacktestRunConfig& config) {
  if (!config.data_path.is_relative()) return;

  const auto config_dir = config_path.parent_path();
  auto candidate = config_dir / config.data_path;
  if (!std::filesystem::exists(candidate)) candidate = config_dir.parent_path().parent_path() / config.data_path;
  config.data_path = candidate;
}

int backtest(int argc, char** argv) {
  const auto path = arg_value(argc, argv, "--config");
  if (path.empty()) {
    std::cerr << "missing --config\n";
    return 2;
  }

  auto loaded_config = qp::backtest::load_backtest_run_config(path);
  if (!loaded_config) {
    print_error(loaded_config.error());
    return 3;
  }
  auto config = loaded_config.value();
  resolve_config_relative_data_path(path, config);

  const auto out_override = arg_value(argc, argv, "--output-dir");
  if (!out_override.empty()) config.output_dir = out_override;

  auto bars = qp::data::read_ohlcv_csv(config.data_path);
  if (!bars) {
    print_error(bars.error());
    return 4;
  }

  auto report = qp::backtest::run_configured_momentum_backtest(bars.value(), config);
  if (!report) {
    print_error(report.error());
    return 1;
  }

  auto metrics = qp::backtest::compute_metrics(report.value());
  if (!metrics) {
    print_error(metrics.error());
    return 1;
  }

  auto artifacts = qp::reporting::write_backtest_artifacts(config.output_dir, report.value(), metrics.value());
  if (!artifacts) {
    print_error(artifacts.error());
    return 1;
  }

  std::cout << "Backtest completed\n"
            << "run_id: " << report.value().run_id << "\n"
            << "bars_seen: " << report.value().bars_seen << "\n"
            << "trades: " << report.value().trades << "\n"
            << "initial_cash: " << report.value().initial_cash << "\n"
            << "final_equity: " << report.value().final_equity << "\n"
            << "return_pct: " << metrics.value().total_return * 100.0 << "\n"
            << "report: " << artifacts.value().report_json << "\n"
            << "manifest: " << artifacts.value().manifest_json << "\n";
  return 0;
}

}  // namespace

int main(int argc, char** argv) {
  const std::string command = argc > 1 ? argv[1] : "smoke";

  if (command == "--help" || command == "help") {
    usage();
    return 0;
  }
  if (command == "--version") {
    std::cout << QP_VERSION << "\n";
    return 0;
  }
  if (command == "smoke") return smoke();
  if (command == "inspect-csv") return inspect_csv(argc, argv);
  if (command == "validate-config") return validate_config(argc, argv);
  if (command == "backtest") return backtest(argc, argv);

  std::cerr << "unknown command: " << command << "\n";
  usage();
  return 2;
}
