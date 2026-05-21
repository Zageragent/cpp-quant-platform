#pragma once
#include "qp/data/market_data.hpp"
#include "qp/execution/order.hpp"
#include <optional>
namespace qp::execution {
struct SlippageInput { Symbol symbol; OrderSide side{OrderSide::Buy}; Quantity quantity; Price reference_price; std::optional<data::Quote> quote; std::optional<data::Trade> last_trade; };
struct SlippageResult { Price fill_price; double slippage_amount{0.0}; };
class SlippageModel { public: virtual ~SlippageModel()=default; [[nodiscard]] virtual Result<SlippageResult> apply(const SlippageInput& input) const=0; };
class NoSlippageModel final : public SlippageModel { public: [[nodiscard]] Result<SlippageResult> apply(const SlippageInput& input) const override; };
class FixedBpsSlippageModel final : public SlippageModel { public: explicit FixedBpsSlippageModel(BasisPoints bps); [[nodiscard]] Result<SlippageResult> apply(const SlippageInput& input) const override; private: BasisPoints bps_; };
class QuoteTouchSlippageModel final : public SlippageModel { public: [[nodiscard]] Result<SlippageResult> apply(const SlippageInput& input) const override; };
}
