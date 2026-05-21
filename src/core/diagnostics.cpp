#include "qp/core/diagnostics.hpp"
namespace qp { void Diagnostics::increment(const std::string& name, std::uint64_t n){ counters_[name] += n; } std::uint64_t Diagnostics::get(const std::string& name) const { auto it=counters_.find(name); return it==counters_.end()?0:it->second; } }
