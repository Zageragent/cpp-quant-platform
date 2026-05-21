#include "qp/backtest/backtest_config.hpp"
#include "qp/backtest/metrics.hpp"
#include "qp/backtest/report.hpp"
#include "qp/execution/commission_model.hpp"
#include "qp/execution/execution_simulator.hpp"
#include "qp/execution/slippage_model.hpp"
#include "qp/reporting/report_writer.hpp"
#include <filesystem>
#include <fstream>
#include <iostream>
#include <cmath>
#include <string>
#include <vector>

#define CHECK2(x) do { if(!(x)){ std::cerr << "CHECK failed at " << __FILE__ << ":" << __LINE__ << " : " #x "\n"; return 1; } } while(false)
static bool near2(double a,double b,double eps=1e-9){ return std::abs(a-b)<eps; }

int test_execution_reporting(){
  using namespace qp;
  using namespace qp::execution;
  auto t0=Timestamp::from_unix_nanos(0), t1=Timestamp::from_unix_nanos(1), t2=Timestamp::from_unix_nanos(2), t3=Timestamp::from_unix_nanos(3);
  data::OhlcvBar b{Symbol{"SPY"},t0,t1,data::BarInterval::Day1,Price{100},Price{101},Price{99},Price{100},Quantity{1000}};
  Order buy{OrderId{1}, Symbol{"SPY"}, OrderSide::Buy, OrderType::Market, Quantity{2}, std::nullopt, TimeInForce::Day, t1, "unit"};
  CHECK2(buy.validate().ok());
  BpsCommissionModel commission(BasisPoints{10.0});
  FixedBpsSlippageModel slippage(BasisPoints{5.0});
  ExecutionSimulator sim(commission, slippage);
  auto causal_bad=sim.execute(Order{OrderId{99}, Symbol{"SPY"}, OrderSide::Buy, OrderType::Market, Quantity{1}, std::nullopt, TimeInForce::Day, t2, "unit"}, ExecutionContext{t1, data::MarketData{b}}); CHECK2(!causal_bad.ok());
  auto fill=sim.execute(buy, ExecutionContext{t1, data::MarketData{b}});
  CHECK2(fill.ok() && fill.value());
  CHECK2(fill.value()->side==OrderSide::Buy);
  CHECK2(near2(fill.value()->price.value, 100.05));
  CHECK2(near2(fill.value()->commission, 0.2001, 1e-6));

  portfolio::Portfolio pf(1000.0);
  auto ledger=pf.apply_fill(*fill.value());
  CHECK2(ledger.ok());
  CHECK2(pf.ledger().size()==1);
  CHECK2(near2(pf.cash(), 799.6999, 1e-6));
  auto snap=pf.snapshot(t1, {{Symbol{"SPY"}, Price{101.0}}});
  CHECK2(snap.ok());
  CHECK2(near2(snap.value().equity, 1001.6999, 1e-6));

  data::OhlcvBar b2{Symbol{"SPY"},t1,t2,data::BarInterval::Day1,Price{100},Price{103},Price{99},Price{102},Quantity{1000}};
  data::OhlcvBar b3{Symbol{"SPY"},t2,t3,data::BarInterval::Day1,Price{102},Price{104},Price{101},Price{103},Quantity{1000}};
  qp::Config bad_cfg; bad_cfg.set("data.path","examples/data/sample_bars.csv"); bad_cfg.set("portfolio.initial_cash","-1"); CHECK2(!backtest::backtest_config_from_flat_config(bad_cfg).ok()); qp::Config bad_numeric; bad_numeric.set("data.path","examples/data/sample_bars.csv"); bad_numeric.set("portfolio.initial_cash","1abc"); CHECK2(!backtest::backtest_config_from_flat_config(bad_numeric).ok());
  auto cfg = backtest::BacktestRunConfig{};
  cfg.run_id="unit_\"run\nwith_escape"; cfg.data_path="examples/data/sample_bars.csv"; cfg.output_dir="/tmp/qp_unit_report"; cfg.initial_cash=1000.0; cfg.buy_threshold=0.01; cfg.sell_threshold=-0.01;
  auto result=backtest::run_configured_momentum_backtest({b,b2,b3}, cfg);
  CHECK2(result.ok());
  CHECK2(result.value().bars_seen==3);
  CHECK2(result.value().equity_curve.size()==3);
  auto metrics=backtest::compute_metrics(result.value());
  CHECK2(metrics.ok());
  CHECK2(metrics.value().trades==1);
  CHECK2(metrics.value().total_return>0.0);

  std::filesystem::remove_all("/tmp/qp_unit_report");
  auto report=reporting::write_backtest_artifacts("/tmp/qp_unit_report", result.value(), metrics.value());
  CHECK2(report.ok());
  CHECK2(std::filesystem::exists("/tmp/qp_unit_report/report.json"));
  CHECK2(std::filesystem::exists("/tmp/qp_unit_report/manifest.json"));
  CHECK2(std::filesystem::exists("/tmp/qp_unit_report/summary.txt"));
  std::ifstream in("/tmp/qp_unit_report/report.json"); std::string body((std::istreambuf_iterator<char>(in)),{});
  CHECK2(body.find("qp.backtest.report.v1") != std::string::npos);
  CHECK2(body.find("unit_\\\"run\\nwith_escape") != std::string::npos);
  std::ifstream manifest_in("/tmp/qp_unit_report/manifest.json"); std::string manifest((std::istreambuf_iterator<char>(manifest_in)),{});
  CHECK2(manifest.find("qp.run_manifest.v1") != std::string::npos);
  CHECK2(manifest.find("unit_\\\"run\\nwith_escape") != std::string::npos);
  CHECK2(manifest.find("equity.csv") != std::string::npos);
  CHECK2(manifest.find("fills.csv") != std::string::npos);
  CHECK2(manifest.find("input_data_fingerprint") != std::string::npos);
  CHECK2(manifest.find("config_fingerprint") != std::string::npos);
  return 0;
}
