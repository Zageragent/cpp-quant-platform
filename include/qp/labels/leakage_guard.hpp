#pragma once
#include "qp/core/result.hpp"
#include "qp/core/time.hpp"
namespace qp::labels {
struct LeakagePolicy { Nanoseconds prediction_gap{Nanoseconds{0}}; bool require_strict_future{true}; };
class LeakageGuard { public: explicit LeakageGuard(LeakagePolicy policy={}); [[nodiscard]] Result<void> validate(Timestamp sample_ts, Timestamp label_start_ts, Timestamp label_end_ts) const; private: LeakagePolicy policy_; };
}
