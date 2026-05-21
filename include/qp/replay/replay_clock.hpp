#pragma once
#include "qp/core/result.hpp"
#include "qp/core/time.hpp"
#include <optional>
namespace qp::replay {
class ReplayClock { public: [[nodiscard]] std::optional<Timestamp> now() const noexcept { return now_; } [[nodiscard]] Result<void> advance_to(Timestamp ts); private: std::optional<Timestamp> now_; };
}
