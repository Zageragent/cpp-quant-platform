#include "qp/backtest/backtest_config.hpp"
#include "qp/backtest/metrics.hpp"
#include "qp/backtest/report.hpp"
#include "qp/data/csv_reader.hpp"
#include "qp/reporting/report_writer.hpp"
#include <iostream>
#include <string>

static void usage(){
  std::cout << "qp " << QP_VERSION << "\n\n"
            << "Usage:\n"
            << "  qp smoke\n"
            << "  qp --help\n"
            << "  qp --version\n"
            << "  qp inspect-csv --data <bars.csv>\n"
            << "  qp validate-config --config <config.json>\n"
            << "  qp backtest --config <config.json> [--output-dir <dir>]\n";
}
static std::string arg_value(int argc,char** argv,const std::string& key){ for(int i=1;i+1<argc;++i) if(argv[i]==key) return argv[i+1]; return {}; }
int main(int argc, char** argv){
  std::string cmd = argc>1 ? argv[1] : "smoke";
  if(cmd=="--help" || cmd=="help"){ usage(); return 0; }
  if(cmd=="--version"){ std::cout << QP_VERSION << "\n"; return 0; }
  if(cmd=="smoke"){
    qp::data::OhlcvBar b{qp::Symbol{"SPY"}, qp::Timestamp::from_unix_nanos(0), qp::Timestamp::from_unix_nanos(1), qp::data::BarInterval::Day1, qp::Price{100}, qp::Price{101}, qp::Price{99}, qp::Price{100.5}, qp::Quantity{1000}};
    auto ok=b.validate(); if(!ok){ std::cerr<<ok.error().message<<"\n"; return 1; }
    std::cout<<"cpp-quant-platform " << QP_VERSION << " smoke ok\n"; return 0;
  }
  if(cmd=="inspect-csv"){
    auto path=arg_value(argc,argv,"--data"); if(path.empty()){ std::cerr << "missing --data\n"; return 2; }
    auto bars=qp::data::read_ohlcv_csv(path); if(!bars){ std::cerr << bars.error().message << " " << bars.error().context << "\n"; return 4; }
    std::cout << "rows: " << bars.value().size() << "\n"; if(!bars.value().empty()) std::cout << "first_symbol: " << bars.value().front().symbol.value << "\n"; return 0;
  }
  if(cmd=="validate-config"){
    auto path=arg_value(argc,argv,"--config"); if(path.empty()){ std::cerr << "missing --config\n"; return 2; }
    auto cfg=qp::backtest::load_backtest_run_config(path); if(!cfg){ std::cerr << cfg.error().message << " " << cfg.error().context << "\n"; return 3; }
    std::cout << "config ok\nrun_id: " << cfg.value().run_id << "\n"; return 0;
  }
  if(cmd=="backtest"){
    auto path=arg_value(argc,argv,"--config"); if(path.empty()){ std::cerr << "missing --config\n"; return 2; }
    auto cfg=qp::backtest::load_backtest_run_config(path); if(!cfg){ std::cerr << cfg.error().message << " " << cfg.error().context << "\n"; return 3; }
    if(cfg.value().data_path.is_relative()) {
      auto config_dir = std::filesystem::path(path).parent_path();
      auto candidate = config_dir / cfg.value().data_path;
      if(!std::filesystem::exists(candidate)) candidate = config_dir.parent_path().parent_path() / cfg.value().data_path;
      cfg.value().data_path = candidate;
    }
    auto out_override=arg_value(argc,argv,"--output-dir"); if(!out_override.empty()) cfg.value().output_dir=out_override;
    auto bars=qp::data::read_ohlcv_csv(cfg.value().data_path); if(!bars){ std::cerr << bars.error().message << " " << bars.error().context << "\n"; return 4; }
    auto report=qp::backtest::run_configured_momentum_backtest(bars.value(), cfg.value()); if(!report){ std::cerr << report.error().message << " " << report.error().context << "\n"; return 1; }
    auto metrics=qp::backtest::compute_metrics(report.value()); if(!metrics){ std::cerr << metrics.error().message << "\n"; return 1; }
    auto artifacts=qp::reporting::write_backtest_artifacts(cfg.value().output_dir, report.value(), metrics.value()); if(!artifacts){ std::cerr << artifacts.error().message << " " << artifacts.error().context << "\n"; return 1; }
    std::cout << "Backtest completed\n"
              << "run_id: " << report.value().run_id << "\n"
              << "bars_seen: " << report.value().bars_seen << "\n"
              << "trades: " << report.value().trades << "\n"
              << "initial_cash: " << report.value().initial_cash << "\n"
              << "final_equity: " << report.value().final_equity << "\n"
              << "return_pct: " << metrics.value().total_return*100.0 << "\n"
              << "report: " << artifacts.value().report_json << "\n"
              << "manifest: " << artifacts.value().manifest_json << "\n";
    return 0;
  }
  std::cerr << "unknown command: " << cmd << "\n"; usage(); return 2;
}
