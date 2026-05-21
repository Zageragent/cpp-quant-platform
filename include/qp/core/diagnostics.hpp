#pragma once
#include <cstdint>
#include <string>
#include <unordered_map>
namespace qp {
struct Counter { std::uint64_t value{0}; void increment(std::uint64_t n=1) noexcept { value += n; } };
class Diagnostics {
 public:
  void increment(const std::string& name, std::uint64_t n=1);
  [[nodiscard]] std::uint64_t get(const std::string& name) const;
  [[nodiscard]] std::unordered_map<std::string, std::uint64_t> snapshot() const { return counters_; }
 private:
  std::unordered_map<std::string, std::uint64_t> counters_;
};
}
