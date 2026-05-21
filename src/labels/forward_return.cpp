#include "qp/labels/forward_return.hpp"
#include <cmath>
namespace qp::labels {
ForwardReturnLabeler::ForwardReturnLabeler(ForwardReturnConfig c): config_(std::move(c)), guard_(LeakagePolicy{config_.prediction_gap,false}){}
Result<std::optional<LabelValue>> ForwardReturnLabeler::label(const data::OhlcvBar& sample, const std::vector<data::OhlcvBar>& future) const {
  auto ok=sample.validate(); if(!ok) return Err<std::optional<LabelValue>>(ok.error());
  Timestamp eligible{sample.end.value + config_.prediction_gap};
  const data::OhlcvBar* start=nullptr; const data::OhlcvBar* end=nullptr; std::optional<Timestamp> prev_end;
  Timestamp target_end{};
  for(const auto& b:future){
    auto bok=b.validate(); if(!bok) return Err<std::optional<LabelValue>>(bok.error());
    if(b.symbol!=sample.symbol) continue;
    if(prev_end && b.start < *prev_end) return Err<std::optional<LabelValue>>(Error{ErrorCode::InvalidData,"future bars must be sorted and non-overlapping"});
    prev_end=b.end;
    if(!start && b.start>=eligible){ start=&b; target_end=Timestamp{b.start.value + config_.horizon}; }
    if(start && b.end>=target_end){ end=&b; break; }
  }
  if(!start || !end) return Ok(std::optional<LabelValue>{});
  auto leak=guard_.validate(sample.end,start->start,end->end); if(!leak) return Err<std::optional<LabelValue>>(leak.error());
  double r=config_.log_return? std::log(end->close.value/sample.close.value): end->close.value/sample.close.value-1.0;
  return Ok(std::optional<LabelValue>{LabelValue{sample.end,start->start,end->end,sample.symbol,config_.name,LabelKind::Regression,r,true}});
}
}
