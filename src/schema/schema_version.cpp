#include "qp/schema/schema_version.hpp"
#include <fstream>
#include <iomanip>
#include <sstream>
namespace qp::schema {
std::vector<std::string_view> known_schema_versions(){ return {backtest_config_v1,backtest_report_v1,run_manifest_v1,dataset_bars_v1,dataset_features_v1,dataset_labels_v1,dataset_signals_v1,instrument_v1}; }
bool is_known_schema_version(std::string_view v){ for(auto k: known_schema_versions()) if(k==v) return true; return false; }
std::string fnv1a64_hex(std::string_view text){ std::uint64_t h=1469598103934665603ull; for(char c: text){ h^=static_cast<unsigned char>(c); h*=1099511628211ull; } std::ostringstream o; o << "fnv1a64:" << std::hex << h; return o.str(); }
Result<std::string> fingerprint_file(const std::filesystem::path& path){ std::ifstream in(path, std::ios::binary); if(!in) return Err<std::string>(Error{ErrorCode::IoError,"failed to open file for fingerprint",path.string()}); std::uint64_t h=1469598103934665603ull; char ch{}; while(in.get(ch)){ h^=static_cast<unsigned char>(ch); h*=1099511628211ull; } std::ostringstream o; o << "fnv1a64:" << std::hex << h; return Ok(o.str()); }
}
