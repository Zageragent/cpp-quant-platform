#pragma once
#include "qp/data/bar.hpp"
#include "qp/labels/label.hpp"
#include "qp/labels/leakage_guard.hpp"
#include <optional>
#include <vector>
namespace qp::labels {
struct ForwardReturnConfig { Nanoseconds horizon; Nanoseconds prediction_gap{Nanoseconds{0}}; bool log_return{false}; std::string name{"forward_return"}; };
class ForwardReturnLabeler { public: explicit ForwardReturnLabeler(ForwardReturnConfig config); [[nodiscard]] Result<std::optional<LabelValue>> label(const data::OhlcvBar& sample_bar, const std::vector<data::OhlcvBar>& future_bars) const; private: ForwardReturnConfig config_; LeakageGuard guard_; };
}
