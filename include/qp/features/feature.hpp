#pragma once
#include "qp/core/result.hpp"
#include "qp/core/time.hpp"
#include "qp/core/types.hpp"
#include "qp/data/market_data.hpp"
#include <optional>
#include <string>
#include <string_view>
namespace qp::features {
struct FeatureValue { Timestamp ts; Symbol symbol; std::string name; double value{0.0}; };
class Feature { public: virtual ~Feature()=default; [[nodiscard]] virtual std::string_view name() const noexcept=0; [[nodiscard]] virtual Result<std::optional<FeatureValue>> update(const data::MarketData& data)=0; virtual void reset()=0; };
}
