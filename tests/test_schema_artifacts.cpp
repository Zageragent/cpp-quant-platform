#include "qp/data/dataset_writer.hpp"
#include "qp/data/instrument.hpp"
#include "qp/schema/schema_version.hpp"
#include "qp/schema/validation.hpp"
#include "qp/features/feature.hpp"
#include "qp/labels/label.hpp"
#include "qp/signals/momentum.hpp"
#include <cmath>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#define CHECK3(x) do { if(!(x)){ std::cerr << "CHECK failed at " << __FILE__ << ":" << __LINE__ << " : " #x "\n"; return 1; } } while(false)
int test_schema_artifacts(){
  using namespace qp;
  CHECK3(schema::is_known_schema_version(schema::dataset_bars_v1));
  CHECK3(!schema::is_known_schema_version("qp.unknown.v1"));
  CHECK3(schema::validate_time_interval(Timestamp::from_unix_nanos(1), Timestamp::from_unix_nanos(2)).ok());
  CHECK3(!schema::validate_time_interval(Timestamp::from_unix_nanos(2), Timestamp::from_unix_nanos(2)).ok());
  data::Instrument inst{Symbol{"SPY"}, "NYSEARCA", "ETF", "USD"};
  CHECK3(inst.validate().ok());
  features::FeatureValue fv{Timestamp::from_unix_nanos(2), Symbol{"SPY"}, "simple_return", 0.01};
  CHECK3(features::validate(fv).ok());
  features::FeatureValue bad_fv=fv; bad_fv.value=std::numeric_limits<double>::quiet_NaN();
  CHECK3(!features::validate(bad_fv).ok());
  labels::LabelValue lv{Timestamp::from_unix_nanos(1), Timestamp::from_unix_nanos(2), Timestamp::from_unix_nanos(3), Symbol{"SPY"}, "forward_return", labels::LabelKind::Regression, 0.02, true};
  CHECK3(labels::validate(lv).ok());
  labels::LabelValue bad_lv=lv; bad_lv.label_end_ts=bad_lv.label_start_ts;
  CHECK3(!labels::validate(bad_lv).ok()); labels::LabelValue leak_lv=lv; leak_lv.label_start_ts=leak_lv.sample_ts; CHECK3(!labels::validate(leak_lv).ok()); labels::LabelValue bad_kind=lv; bad_kind.kind=static_cast<labels::LabelKind>(999); CHECK3(!labels::validate(bad_kind).ok());
  signals::Signal sig{Timestamp::from_unix_nanos(2), Symbol{"SPY"}, "momentum", signals::SignalSide::Buy, 1.0};
  CHECK3(signals::validate(sig).ok()); signals::Signal bad_sig=sig; bad_sig.side=static_cast<signals::SignalSide>(999); CHECK3(!signals::validate(bad_sig).ok());
  auto dir=std::filesystem::path{"/tmp/qp_schema_artifacts"}; std::filesystem::remove_all(dir); std::filesystem::create_directories(dir);
  data::OhlcvBar bar{Symbol{"SPY"},Timestamp::from_unix_nanos(1),Timestamp::from_unix_nanos(2),data::BarInterval::Day1,Price{100},Price{101},Price{99},Price{100},Quantity{10}};
  auto cwd_bar=std::filesystem::current_path()/"qp_schema_basename_bars.csv"; std::filesystem::remove(cwd_bar); CHECK3(data::write_bars_csv(cwd_bar.filename(), {bar}).ok()); std::filesystem::remove(cwd_bar);
  CHECK3(data::write_bars_csv(dir/"bars.csv", {bar}).ok());
  CHECK3(data::write_features_csv(dir/"features.csv", {fv}).ok());
  CHECK3(data::write_labels_csv(dir/"labels.csv", {lv}).ok());
  CHECK3(data::write_signals_csv(dir/"signals.csv", {sig}).ok());
  std::ifstream in(dir/"features.csv"); std::string header; std::getline(in, header);
  CHECK3(header=="schema_version,ts_ns,symbol,name,value");
  auto fpa=schema::fingerprint_file(dir/"features.csv"); auto fpb=schema::fingerprint_file(dir/"features.csv"); CHECK3(fpa.ok() && fpb.ok() && fpa.value()==fpb.value());
  std::ofstream mutate(dir/"features.csv", std::ios::app); mutate << "#changed\n"; mutate.close(); auto fpc=schema::fingerprint_file(dir/"features.csv"); CHECK3(fpc.ok() && fpc.value()!=fpa.value());
  return 0;
}
