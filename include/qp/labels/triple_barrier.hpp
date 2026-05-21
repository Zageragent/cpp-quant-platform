#pragma once
#include "qp/data/bar.hpp"
#include "qp/labels/label.hpp"
#include "qp/labels/leakage_guard.hpp"
#include <optional>
#include <vector>
namespace qp::labels {
struct TripleBarrierConfig { Nanoseconds horizon; Nanoseconds prediction_gap{Nanoseconds{0}}; double take_profit_return{0.01}; double stop_loss_return{-0.01}; std::string name{"triple_barrier"}; };
enum class TripleBarrierOutcome { StopLoss=-1, NoTouch=0, TakeProfit=1 };
class TripleBarrierLabeler { public: explicit TripleBarrierLabeler(TripleBarrierConfig config); [[nodiscard]] Result<std::optional<LabelValue>> label(const data::OhlcvBar& sample_bar, const std::vector<data::OhlcvBar>& future_bars) const; private: TripleBarrierConfig config_; LeakageGuard guard_; };
}
