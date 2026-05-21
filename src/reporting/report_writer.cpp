#include "qp/reporting/report_writer.hpp"
#include "qp/core/config.hpp"
#include "qp/schema/schema_version.hpp"
#include <filesystem>
#include <fstream>
#include <string>

namespace qp::reporting {
namespace {
std::string esc(const std::string& x){
  std::string o;
  for(char c:x){
    if(c=='"'||c=='\\') { o.push_back('\\'); o.push_back(c); }
    else if(c=='\n') o += "\\n";
    else o.push_back(c);
  }
  return o;
}
Result<void> write_file(const std::filesystem::path& p,const std::string& s){
  std::ofstream o(p);
  if(!o) return Err(Error{ErrorCode::IoError,"failed to write file",p.string()});
  o<<s;
  return Ok();
}
}

Result<ArtifactPaths> write_backtest_artifacts(const std::filesystem::path& dir,const backtest::BacktestReport& r,const backtest::BacktestMetrics& m){
  std::error_code ec;
  std::filesystem::create_directories(dir,ec);
  if(ec) return Err<ArtifactPaths>(Error{ErrorCode::IoError,ec.message(),dir.string()});
  ArtifactPaths paths{dir/"report.json",dir/"manifest.json",dir/"summary.txt",dir/"equity.csv",dir/"fills.csv"};
  std::string report="{\n"
    "  \"schema_version\": \""+std::string(schema::backtest_report_v1)+"\",\n"
    "  \"run_id\": \""+esc(r.run_id)+"\",\n"
    "  \"bars_seen\": "+std::to_string(r.bars_seen)+",\n"
    "  \"trades\": "+std::to_string(r.trades)+",\n"
    "  \"initial_cash\": "+std::to_string(r.initial_cash)+",\n"
    "  \"final_equity\": "+std::to_string(r.final_equity)+",\n"
    "  \"total_return\": "+std::to_string(m.total_return)+"\n"
    "}\n";
  auto ok=write_file(paths.report_json,report);
  if(!ok) return Err<ArtifactPaths>(ok.error());

  auto config_fingerprint=schema::fnv1a64_hex(backtest::effective_config_json(r.config));
  auto input_fingerprint=schema::fingerprint_file(r.config.data_path);
  std::string input_fp=input_fingerprint ? input_fingerprint.value() : "unavailable";
  std::string manifest="{\n"
    "  \"schema_version\": \""+std::string(schema::run_manifest_v1)+"\",\n"
    "  \"run_id\": \""+esc(r.run_id)+"\",\n"
    "  \"status\": \"success\",\n"
    "  \"qp_version\": \""+std::string(QP_VERSION)+"\",\n"
    "  \"git_sha\": \"unknown\",\n"
    "  \"compiler\": \""+esc(std::string(__VERSION__))+"\",\n"
    "  \"config_schema_version\": \""+std::string(schema::backtest_config_v1)+"\",\n"
    "  \"report_schema_version\": \""+std::string(schema::backtest_report_v1)+"\",\n"
    "  \"config_fingerprint\": \""+config_fingerprint+"\",\n"
    "  \"input_data_path\": \""+esc(r.config.data_path.string())+"\",\n"
    "  \"input_data_fingerprint\": \""+input_fp+"\",\n"
    "  \"artifacts\": {\n"
    "    \"report\": \"report.json\",\n"
    "    \"summary\": \"summary.txt\",\n"
    "    \"equity\": \"equity.csv\",\n"
    "    \"fills\": \"fills.csv\"\n"
    "  }\n"
    "}\n";
  ok=write_file(paths.manifest_json,manifest);
  if(!ok) return Err<ArtifactPaths>(ok.error());

  std::string summary="run_id: "+esc(r.run_id)+"\nbars_seen: "+std::to_string(r.bars_seen)+"\ntrades: "+std::to_string(r.trades)+"\ninitial_cash: "+std::to_string(r.initial_cash)+"\nfinal_equity: "+std::to_string(r.final_equity)+"\ntotal_return: "+std::to_string(m.total_return)+"\n";
  ok=write_file(paths.summary_txt,summary);
  if(!ok) return Err<ArtifactPaths>(ok.error());

  std::ofstream eq(paths.equity_csv);
  if(!eq) return Err<ArtifactPaths>(Error{ErrorCode::IoError,"failed to write equity csv"});
  eq<<"ts_ns,equity,cash,gross_exposure,net_exposure\n";
  for(const auto& point:r.equity_curve) eq<<point.ts.unix_nanos()<<","<<point.equity<<","<<point.cash<<","<<point.gross_exposure<<","<<point.net_exposure<<"\n";

  std::ofstream fills(paths.fills_csv);
  if(!fills) return Err<ArtifactPaths>(Error{ErrorCode::IoError,"failed to write fills csv"});
  fills<<"order_id,symbol,side,ts_ns,quantity,price,commission,slippage\n";
  for(const auto& f:r.fills) fills<<f.order_id.value<<","<<f.symbol.value<<","<<(f.side==execution::OrderSide::Buy?"buy":"sell")<<","<<f.ts.unix_nanos()<<","<<f.quantity.value<<","<<f.price.value<<","<<f.commission<<","<<f.slippage<<"\n";
  return Ok(paths);
}
}
