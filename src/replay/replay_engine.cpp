#include "qp/replay/replay_engine.hpp"
#include <optional>
namespace qp::replay {
ReplayEngine::ReplayEngine(ReplayConfig config): config_(config) {}
void ReplayEngine::add_sink(ReplaySink& sink){ sinks_.push_back(&sink); }
Result<ReplayStats> ReplayEngine::run(EventSource& source){
  stats_={};
  clock_=ReplayClock{};
  std::optional<Timestamp> last_ts;
  std::optional<SequenceNumber> last_seq;
  while(true){
    if(config_.max_events && stats_.events_dispatched>=config_.max_events) break;
    auto nxt=source.next();
    if(!nxt) return Err<ReplayStats>(nxt.error());
    auto opt=std::move(nxt).value();
    if(!opt) break;
    auto ev=*opt;
    ++stats_.events_seen;
    auto ts=event_time(ev);
    auto seq=sequence_number(ev);
    if(last_ts && (ts<*last_ts || (ts==*last_ts && seq<=*last_seq))) {
      return Err<ReplayStats>(Error{ErrorCode::OutOfOrderTimestamp,"replay events must be strictly ordered by (time, sequence)"});
    }
    last_ts=ts;
    last_seq=seq;
    if(config_.time_range && !config_.time_range->contains(ts)){
      ++stats_.events_filtered;
      continue;
    }
    if(config_.require_monotonic_time){
      auto clk=clock_.advance_to(ts);
      if(!clk) return Err<ReplayStats>(clk.error());
    }
    if(config_.validate_market_data && std::holds_alternative<MarketDataEvent>(ev)){
      const auto& m=std::get<MarketDataEvent>(ev);
      if(m.receive_ts < m.exchange_ts) return Err<ReplayStats>(Error{ErrorCode::InvalidData,"receive timestamp before exchange timestamp"});
      auto ok=data::validate(m.data);
      if(!ok){ ++stats_.validation_errors; return Err<ReplayStats>(ok.error()); }
    }
    for(auto* sink:sinks_){
      auto ok=sink->on_event(ev);
      if(!ok) return Err<ReplayStats>(ok.error());
    }
    ++stats_.events_dispatched;
  }
  for(auto* sink:sinks_){
    auto ok=sink->on_end();
    if(!ok) return Err<ReplayStats>(ok.error());
  }
  return Ok(stats_);
}
}
