#include "qp/core/result.hpp"
#include "qp/core/config.hpp"
#include "qp/data/bar.hpp"
#include "qp/data/quote.hpp"
#include "qp/data/csv_reader.hpp"
#include "qp/backtest/simple_backtest.hpp"
#include "qp/replay/event_source.hpp"
#include "qp/replay/replay_engine.hpp"
#include "qp/features/rolling_window.hpp"
#include "qp/features/returns.hpp"
#include "qp/features/rolling_mean.hpp"
#include "qp/features/rolling_variance.hpp"
#include "qp/labels/leakage_guard.hpp"
#include "qp/labels/forward_return.hpp"
#include "qp/labels/triple_barrier.hpp"
#include "qp/signals/momentum.hpp"
#include "qp/portfolio/portfolio.hpp"
#include <cmath>
#include <fstream>
#include <iostream>
#include <vector>
#define CHECK(x) do { if(!(x)){ std::cerr << "CHECK failed at " << __FILE__ << ":" << __LINE__ << " : " #x "\n"; return 1; } } while(false)
static bool near(double a,double b,double eps=1e-9){ return std::fabs(a-b)<eps; }
using namespace qp;
int test_execution_reporting();
int test_schema_artifacts();
int main(){
  auto t0=Timestamp::from_unix_nanos(0), t1=Timestamp::from_unix_nanos(1), t2=Timestamp::from_unix_nanos(2), t3=Timestamp::from_unix_nanos(3);
  data::OhlcvBar b{Symbol{"spy"},t0,t1,data::BarInterval::Day1,Price{100},Price{101},Price{99},Price{100},Quantity{10}}; CHECK(b.symbol.value=="SPY"); CHECK(b.validate().ok());
  data::OhlcvBar bad=b; bad.high=Price{90}; CHECK(!bad.validate().ok()); features::SimpleReturn invalid_feature; CHECK(!invalid_feature.update(data::MarketData{bad}).ok());
  data::Quote q{Symbol{"SPY"},t1,Price{100},Quantity{5},Price{101},Quantity{7}}; CHECK(q.validate().ok()); CHECK(near(q.mid().value,100.5)); CHECK(q.spread()>0);
  data::Quote crossed=q; crossed.bid_price=Price{102}; CHECK(!crossed.validate().ok());
  features::RollingWindow w(3); w.push(1); w.push(2); CHECK(!w.full()); w.push(3); CHECK(w.full()); CHECK(near(w.mean(),2)); w.push(4); CHECK(near(w.mean(),3));
  data::OhlcvBar b2{Symbol{"SPY"},t1,t2,data::BarInterval::Day1,Price{100},Price{103},Price{99},Price{102},Quantity{10}};
  data::OhlcvBar b3{Symbol{"SPY"},t2,t3,data::BarInterval::Day1,Price{102},Price{104},Price{101},Price{103},Quantity{10}};
  features::SimpleReturn sr; auto r0=sr.update(data::MarketData{b}); CHECK(r0.ok() && !r0.value()); auto r1=sr.update(data::MarketData{b2}); CHECK(r1.ok() && r1.value()); CHECK(near(r1.value()->value,0.02)); CHECK(r1.value()->ts==t2);
  features::RollingMean rm(2); CHECK(!rm.update(data::MarketData{b}).value()); auto mv=rm.update(data::MarketData{b2}).value(); CHECK(mv && near(mv->value,101));
  features::RollingVariance rv(3,false); rv.update(data::MarketData{b}); rv.update(data::MarketData{b2}); auto vv=rv.update(data::MarketData{b3}).value(); CHECK(vv && near(vv->value, ( (100-101.6666666667)*(100-101.6666666667)+(102-101.6666666667)*(102-101.6666666667)+(103-101.6666666667)*(103-101.6666666667) )/3.0, 1e-6));
  labels::LeakageGuard guard; CHECK(!guard.validate(t1,t1,t2).ok()); CHECK(guard.validate(t1,t2,t3).ok());
  labels::ForwardReturnLabeler fl(labels::ForwardReturnConfig{std::chrono::nanoseconds{1}}); auto lab=fl.label(b,{b2,b3}); CHECK(lab.ok() && lab.value()); CHECK(near(lab.value()->value,0.02));
  labels::TripleBarrierLabeler tb(labels::TripleBarrierConfig{std::chrono::nanoseconds{2},std::chrono::nanoseconds{0},0.02,-0.02}); auto tl=tb.label(b,{b2,b3}); CHECK(tl.ok() && tl.value()); CHECK(tl.value()->value==1.0);
  signals::MomentumSignal ms(signals::MomentumConfig{"simple_return",0.01,-0.01,"mom"}); auto sig=ms.update(*r1.value()); CHECK(sig && sig->side==signals::SignalSide::Buy);
  replay::ReplayEvent e2=replay::MarketDataEvent{replay::SequenceNumber{2},t2,t2,data::MarketData{b2}}; replay::ReplayEvent e1=replay::MarketDataEvent{replay::SequenceNumber{1},t1,t1,data::MarketData{b}}; replay::VectorEventSource src({e2,e1}); struct Sink: replay::ReplaySink { int n=0; Result<void> on_event(const replay::ReplayEvent&) override { ++n; return Ok(); } } sink; replay::ReplayEngine eng; eng.add_sink(sink); auto stats=eng.run(src); CHECK(stats.ok()); CHECK(stats.value().events_dispatched==2); CHECK(sink.n==2); CHECK(eng.clock().now()->unix_nanos()==2);
  replay::VectorEventSource dup_src({e1,e1}, false); replay::ReplayEngine dup_eng; auto dup_stats=dup_eng.run(dup_src); CHECK(!dup_stats.ok());
  replay::VectorEventSource limited_src({e1,e2}, false); replay::ReplayEngine limited_eng(replay::ReplayConfig{true,true,std::nullopt,1}); auto limited=limited_eng.run(limited_src); CHECK(limited.ok()); CHECK(limited.value().events_seen==1); CHECK(limited.value().events_dispatched==1);
  portfolio::Portfolio pf(1000); CHECK(pf.buy(Symbol{"SPY"},Quantity{2},Price{100},1).ok()); CHECK(near(pf.cash(),799)); CHECK(near(pf.equity({{Symbol{"SPY"},Price{110}}}),1019)); CHECK(pf.sell(Symbol{"SPY"},Quantity{1},Price{120},1).ok()); CHECK(near(pf.cash(),918));
  std::ofstream csv("/tmp/qp_test_bars.csv"); csv << "symbol,start_ns,end_ns,open,high,low,close,volume\nSPY,0,1,100,101,99,100,10\n"; csv.close(); auto bars=data::read_ohlcv_csv("/tmp/qp_test_bars.csv"); CHECK(bars.ok()); CHECK(bars.value().size()==1); CHECK(bars.value()[0].symbol.value=="SPY");
  std::ofstream cfg_file("/tmp/qp_config.json"); cfg_file << "{\"seed\":42,\"threshold\":0.01,\"enabled\":true}"; cfg_file.close(); auto cfg=load_flat_json_config("/tmp/qp_config.json"); CHECK(cfg.ok()); CHECK(cfg.value().get_int("seed").value()==42); CHECK(near(cfg.value().get_double("threshold").value(),0.01)); CHECK(cfg.value().get_bool("enabled").value()); Config cfg2; cfg2.set("seed","43"); cfg2.set("threshold","0.01"); cfg2.set("enabled","true"); CHECK(stable_config_fingerprint(cfg.value()) != stable_config_fingerprint(cfg2));
  features::SimpleReturn cross_symbol; CHECK(!cross_symbol.update(data::MarketData{b}).value()); data::OhlcvBar other{Symbol{"QQQ"},t1,t2,data::BarInterval::Day1,Price{10},Price{11},Price{9},Price{10},Quantity{1}}; CHECK(!cross_symbol.update(data::MarketData{other}).value()); auto spy_again=cross_symbol.update(data::MarketData{b2}); CHECK(spy_again.value() && near(spy_again.value()->value,0.02));
  auto wrong_symbol_label=fl.label(b,{other}); CHECK(wrong_symbol_label.ok() && !wrong_symbol_label.value());
  auto bt=backtest::run_simple_momentum_backtest({b,b2,b3}, backtest::SimpleBacktestConfig{1000.0,0.01,-0.01,0.0}); CHECK(bt.ok()); CHECK(bt.value().bars_seen==3); CHECK(bt.value().trades==1); CHECK(bt.value().position_quantity==1.0); CHECK(near(bt.value().final_equity,1001.0));
  CHECK(test_execution_reporting()==0);
  CHECK(test_schema_artifacts()==0);
  std::cout << "all qp tests passed\n"; return 0;
}
