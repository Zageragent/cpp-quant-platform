#pragma once
#include "qp/core/result.hpp"
#include "qp/core/time.hpp"
#include "qp/replay/event_source.hpp"
#include "qp/replay/replay_clock.hpp"
#include <optional>
#include <vector>
namespace qp::replay {
class ReplaySink { public: virtual ~ReplaySink()=default; [[nodiscard]] virtual Result<void> on_event(const ReplayEvent& event)=0; [[nodiscard]] virtual Result<void> on_end(){ return Ok(); } };
struct ReplayConfig { bool require_monotonic_time{true}; bool validate_market_data{true}; std::optional<TimeRange> time_range; std::uint64_t max_events{0}; };
struct ReplayStats { std::uint64_t events_seen{0}; std::uint64_t events_dispatched{0}; std::uint64_t events_filtered{0}; std::uint64_t validation_errors{0}; };
class ReplayEngine { public: explicit ReplayEngine(ReplayConfig config={}); void add_sink(ReplaySink& sink); [[nodiscard]] const ReplayClock& clock() const noexcept { return clock_; } [[nodiscard]] ReplayStats stats() const noexcept { return stats_; } [[nodiscard]] Result<ReplayStats> run(EventSource& source); private: ReplayConfig config_; ReplayClock clock_; ReplayStats stats_; std::vector<ReplaySink*> sinks_; };
}
