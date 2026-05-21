#include "qp/replay/event_source.hpp"
#include <algorithm>
namespace qp::replay { VectorEventSource::VectorEventSource(std::vector<ReplayEvent> events,bool sort_events): events_(std::move(events)){ if(sort_events) std::sort(events_.begin(), events_.end(), event_less); } Result<std::optional<ReplayEvent>> VectorEventSource::next(){ if(index_>=events_.size()) return Ok(std::optional<ReplayEvent>{}); return Ok(std::optional<ReplayEvent>{events_[index_++]}); } }
