#pragma once
#include "qp/core/result.hpp"
#include <filesystem>
#include <optional>
#include <string>
#include <string_view>
#include <unordered_map>
namespace qp {
class Config {
 public:
  void set(std::string key, std::string value);
  [[nodiscard]] std::optional<std::string_view> get(std::string_view key) const;
  [[nodiscard]] Result<std::string> get_string(std::string_view key) const;
  [[nodiscard]] Result<int> get_int(std::string_view key) const;
  [[nodiscard]] Result<double> get_double(std::string_view key) const;
  [[nodiscard]] Result<bool> get_bool(std::string_view key) const;
  [[nodiscard]] std::size_t size() const noexcept { return values_.size(); }
  [[nodiscard]] const std::unordered_map<std::string, std::string>& entries() const noexcept { return values_; }
 private:
  std::unordered_map<std::string, std::string> values_;
};
[[nodiscard]] Result<Config> load_flat_json_config(const std::filesystem::path& path);
[[nodiscard]] std::string stable_config_fingerprint(const Config& config);
}
