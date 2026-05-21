#include "qp/core/config.hpp"
#include <algorithm>
#include <cctype>
#include <fstream>
#include <sstream>
#include <cstdint>
#include <iomanip>
#include <utility>
#include <vector>
namespace qp {
void Config::set(std::string key, std::string value){ values_[std::move(key)] = std::move(value); }
std::optional<std::string_view> Config::get(std::string_view key) const { auto it=values_.find(std::string(key)); if(it==values_.end()) return std::nullopt; return std::string_view{it->second}; }
Result<std::string> Config::get_string(std::string_view key) const { auto v=get(key); if(!v) return Err<std::string>(Error{ErrorCode::ConfigurationError,"missing config key",std::string(key)}); return Ok(std::string(*v)); }
Result<int> Config::get_int(std::string_view key) const { auto v=get_string(key); if(!v) return Err<int>(v.error()); try{return Ok(std::stoi(v.value()));}catch(const std::exception& e){return Err<int>(Error{ErrorCode::ConfigurationError,e.what(),std::string(key)});} }
Result<double> Config::get_double(std::string_view key) const { auto v=get_string(key); if(!v) return Err<double>(v.error()); try{return Ok(std::stod(v.value()));}catch(const std::exception& e){return Err<double>(Error{ErrorCode::ConfigurationError,e.what(),std::string(key)});} }
Result<bool> Config::get_bool(std::string_view key) const { auto v=get_string(key); if(!v) return Err<bool>(v.error()); auto s=v.value(); std::transform(s.begin(), s.end(), s.begin(), [](unsigned char c){return static_cast<char>(std::tolower(c));}); if(s=="true"||s=="1") return Ok(true); if(s=="false"||s=="0") return Ok(false); return Err<bool>(Error{ErrorCode::ConfigurationError,"invalid boolean",std::string(key)}); }
static std::string trim(std::string s){ auto not_space=[](unsigned char c){return !std::isspace(c);}; s.erase(s.begin(), std::find_if(s.begin(), s.end(), not_space)); s.erase(std::find_if(s.rbegin(), s.rend(), not_space).base(), s.end()); return s; }
Result<Config> load_flat_json_config(const std::filesystem::path& path){ std::ifstream in(path); if(!in) return Err<Config>(Error{ErrorCode::IoError,"failed to open config",path.string()}); std::string text((std::istreambuf_iterator<char>(in)),{}); Config cfg; std::size_t i=0; while((i=text.find('"', i))!=std::string::npos){ auto j=text.find('"', i+1); if(j==std::string::npos) break; std::string key=text.substr(i+1,j-i-1); auto colon=text.find(':', j+1); if(colon==std::string::npos) break; auto val_start=text.find_first_not_of(" \t\r\n", colon+1); if(val_start==std::string::npos) break; std::string val; if(text[val_start]=='"'){ auto val_end=text.find('"', val_start+1); if(val_end==std::string::npos) return Err<Config>(Error{ErrorCode::ParseError,"unterminated string",key}); val=text.substr(val_start+1,val_end-val_start-1); i=val_end+1; } else { auto val_end=text.find_first_of(",}\n", val_start); val=trim(text.substr(val_start, val_end-val_start)); i=val_end==std::string::npos?text.size():val_end+1; } cfg.set(std::move(key), std::move(val)); } return Ok(std::move(cfg)); }
std::string stable_config_fingerprint(const Config& config){ std::vector<std::pair<std::string,std::string>> entries(config.entries().begin(), config.entries().end()); std::sort(entries.begin(), entries.end()); std::uint64_t h=1469598103934665603ull; auto mix=[&](char c){ h^=static_cast<unsigned char>(c); h*=1099511628211ull; }; for(const auto& [k,v]:entries){ for(char c:k) mix(c); mix('='); for(char c:v) mix(c); mix('\n'); } std::ostringstream out; out<<"fnv1a64:"<<std::hex<<h; return out.str(); }
}
