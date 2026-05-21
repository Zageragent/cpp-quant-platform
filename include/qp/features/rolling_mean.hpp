#pragma once
#include "qp/features/feature.hpp"
#include "qp/features/rolling_window.hpp"
#include <map>
namespace qp::features {
class RollingMean final : public Feature { public: RollingMean(std::size_t window, std::string name=""); std::string_view name() const noexcept override; Result<std::optional<FeatureValue>> update(const data::MarketData& data) override; void reset() override; private: std::string name_; std::size_t capacity_; std::map<Symbol, RollingWindow> windows_; };
}
