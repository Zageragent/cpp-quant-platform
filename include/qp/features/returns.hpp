#pragma once
#include "qp/features/feature.hpp"
#include <map>
#include <optional>
namespace qp::features {
class SimpleReturn final : public Feature { public: explicit SimpleReturn(std::string name="simple_return"); std::string_view name() const noexcept override; Result<std::optional<FeatureValue>> update(const data::MarketData& data) override; void reset() override; private: std::string name_; std::map<Symbol,double> previous_by_symbol_; };
class LogReturn final : public Feature { public: explicit LogReturn(std::string name="log_return"); std::string_view name() const noexcept override; Result<std::optional<FeatureValue>> update(const data::MarketData& data) override; void reset() override; private: std::string name_; std::map<Symbol,double> previous_by_symbol_; };
}
