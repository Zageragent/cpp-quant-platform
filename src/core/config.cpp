#include "qp/core/config.hpp"

#include <algorithm>
#include <cctype>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <stdexcept>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

namespace qp {
namespace {

std::string trim(std::string s) {
  const auto not_space = [](unsigned char c) { return !std::isspace(c); };
  s.erase(s.begin(), std::find_if(s.begin(), s.end(), not_space));
  s.erase(std::find_if(s.rbegin(), s.rend(), not_space).base(), s.end());
  return s;
}

void skip_ws(std::string_view text, std::size_t& pos) {
  while (pos < text.size() && std::isspace(static_cast<unsigned char>(text[pos]))) ++pos;
}

Result<std::string> parse_json_string(std::string_view text, std::size_t& pos) {
  if (pos >= text.size() || text[pos] != '"') {
    return Err<std::string>(Error{ErrorCode::ParseError, "expected JSON string"});
  }
  ++pos;

  std::string out;
  while (pos < text.size()) {
    const char c = text[pos++];
    if (c == '"') return Ok(out);
    if (c != '\\') {
      out.push_back(c);
      continue;
    }

    if (pos >= text.size()) return Err<std::string>(Error{ErrorCode::ParseError, "unterminated escape sequence"});
    const char esc = text[pos++];
    switch (esc) {
      case '"': out.push_back('"'); break;
      case '\\': out.push_back('\\'); break;
      case '/': out.push_back('/'); break;
      case 'b': out.push_back('\b'); break;
      case 'f': out.push_back('\f'); break;
      case 'n': out.push_back('\n'); break;
      case 'r': out.push_back('\r'); break;
      case 't': out.push_back('\t'); break;
      default:
        return Err<std::string>(Error{ErrorCode::ParseError, "unsupported JSON escape", std::string{esc}});
    }
  }

  return Err<std::string>(Error{ErrorCode::ParseError, "unterminated string"});
}

Result<std::string> parse_flat_json_value(std::string_view text, std::size_t& pos) {
  skip_ws(text, pos);
  if (pos >= text.size()) return Err<std::string>(Error{ErrorCode::ParseError, "expected config value"});

  if (text[pos] == '"') return parse_json_string(text, pos);

  const auto start = pos;
  while (pos < text.size() && text[pos] != ',' && text[pos] != '}') ++pos;
  auto value = trim(std::string{text.substr(start, pos - start)});
  if (value.empty()) return Err<std::string>(Error{ErrorCode::ParseError, "empty config value"});
  return Ok(std::move(value));
}

}  // namespace

void Config::set(std::string key, std::string value) { values_[std::move(key)] = std::move(value); }

std::optional<std::string_view> Config::get(std::string_view key) const {
  const auto it = values_.find(std::string(key));
  if (it == values_.end()) return std::nullopt;
  return std::string_view{it->second};
}

Result<std::string> Config::get_string(std::string_view key) const {
  auto value = get(key);
  if (!value) return Err<std::string>(Error{ErrorCode::ConfigurationError, "missing config key", std::string(key)});
  return Ok(std::string(*value));
}

Result<int> Config::get_int(std::string_view key) const {
  auto value = get_string(key);
  if (!value) return Err<int>(value.error());

  try {
    std::size_t parsed = 0;
    auto text = value.value();
    const int out = std::stoi(text, &parsed);
    if (parsed != text.size()) {
      return Err<int>(Error{ErrorCode::ConfigurationError, "trailing characters in integer", std::string(key)});
    }
    return Ok(out);
  } catch (const std::exception& e) {
    return Err<int>(Error{ErrorCode::ConfigurationError, e.what(), std::string(key)});
  }
}

Result<double> Config::get_double(std::string_view key) const {
  auto value = get_string(key);
  if (!value) return Err<double>(value.error());

  try {
    std::size_t parsed = 0;
    auto text = value.value();
    const double out = std::stod(text, &parsed);
    if (parsed != text.size()) {
      return Err<double>(Error{ErrorCode::ConfigurationError, "trailing characters in number", std::string(key)});
    }
    return Ok(out);
  } catch (const std::exception& e) {
    return Err<double>(Error{ErrorCode::ConfigurationError, e.what(), std::string(key)});
  }
}

Result<bool> Config::get_bool(std::string_view key) const {
  auto value = get_string(key);
  if (!value) return Err<bool>(value.error());

  auto text = value.value();
  std::transform(text.begin(), text.end(), text.begin(), [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
  if (text == "true" || text == "1") return Ok(true);
  if (text == "false" || text == "0") return Ok(false);
  return Err<bool>(Error{ErrorCode::ConfigurationError, "invalid boolean", std::string(key)});
}

Result<Config> load_flat_json_config(const std::filesystem::path& path) {
  std::ifstream in(path);
  if (!in) return Err<Config>(Error{ErrorCode::IoError, "failed to open config", path.string()});

  const std::string text((std::istreambuf_iterator<char>(in)), {});
  std::size_t pos = 0;
  Config config;

  skip_ws(text, pos);
  if (pos >= text.size() || text[pos] != '{') {
    return Err<Config>(Error{ErrorCode::ParseError, "expected top-level JSON object", path.string()});
  }
  ++pos;

  skip_ws(text, pos);
  if (pos < text.size() && text[pos] == '}') {
    ++pos;
    skip_ws(text, pos);
    if (pos != text.size()) return Err<Config>(Error{ErrorCode::ParseError, "trailing content after config object", path.string()});
    return Ok(std::move(config));
  }

  while (pos < text.size()) {
    skip_ws(text, pos);
    auto key = parse_json_string(text, pos);
    if (!key) return Err<Config>(Error{key.error().code, key.error().message, path.string()});

    skip_ws(text, pos);
    if (pos >= text.size() || text[pos] != ':') {
      return Err<Config>(Error{ErrorCode::ParseError, "expected ':' after config key", key.value()});
    }
    ++pos;

    auto value = parse_flat_json_value(text, pos);
    if (!value) return Err<Config>(Error{value.error().code, value.error().message, key.value()});
    config.set(std::move(key).value(), std::move(value).value());

    skip_ws(text, pos);
    if (pos >= text.size()) return Err<Config>(Error{ErrorCode::ParseError, "unterminated config object", path.string()});
    if (text[pos] == '}') {
      ++pos;
      skip_ws(text, pos);
      if (pos != text.size()) return Err<Config>(Error{ErrorCode::ParseError, "trailing content after config object", path.string()});
      return Ok(std::move(config));
    }
    if (text[pos] != ',') {
      return Err<Config>(Error{ErrorCode::ParseError, "expected ',' or '}' after config value", path.string()});
    }
    ++pos;
  }

  return Err<Config>(Error{ErrorCode::ParseError, "unterminated config object", path.string()});
}

std::string stable_config_fingerprint(const Config& config) {
  std::vector<std::pair<std::string, std::string>> entries(config.entries().begin(), config.entries().end());
  std::sort(entries.begin(), entries.end());

  std::uint64_t h = 1469598103934665603ull;
  const auto mix = [&](char c) {
    h ^= static_cast<unsigned char>(c);
    h *= 1099511628211ull;
  };

  for (const auto& [key, value] : entries) {
    for (char c : key) mix(c);
    mix('=');
    for (char c : value) mix(c);
    mix('\n');
  }

  std::ostringstream out;
  out << "fnv1a64:" << std::hex << h;
  return out.str();
}

}  // namespace qp
