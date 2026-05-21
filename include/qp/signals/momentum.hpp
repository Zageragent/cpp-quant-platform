#pragma once
#include "qp/features/feature.hpp"
#include <optional>
#include <string>
#include <string_view>
namespace qp::signals {
enum class SignalSide { Sell=-1, Flat=0, Buy=1 };
struct Signal { Timestamp ts; Symbol symbol; std::string name; SignalSide side{SignalSide::Flat}; double strength{0.0}; };
struct MomentumConfig { std::string feature_name{"simple_return"}; double buy_threshold{0.0}; double sell_threshold{0.0}; std::string name{"momentum"}; };
class MomentumSignal { public: explicit MomentumSignal(MomentumConfig config={}); [[nodiscard]] std::string_view name() const noexcept; [[nodiscard]] std::optional<Signal> update(const features::FeatureValue& feature); void reset() {} private: MomentumConfig config_; };
}
