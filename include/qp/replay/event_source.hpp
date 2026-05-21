#pragma once
#include "qp/core/result.hpp"
#include "qp/replay/event.hpp"
#include <optional>
#include <vector>
namespace qp::replay {
class EventSource { public: virtual ~EventSource() = default; [[nodiscard]] virtual Result<std::optional<ReplayEvent>> next() = 0; };
class VectorEventSource final : public EventSource {
 public:
  explicit VectorEventSource(std::vector<ReplayEvent> events, bool sort_events=true);
  [[nodiscard]] Result<std::optional<ReplayEvent>> next() override;
 private:
  std::vector<ReplayEvent> events_;
  std::size_t index_{0};
};
}
