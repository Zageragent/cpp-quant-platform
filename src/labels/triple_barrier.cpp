#include "qp/labels/triple_barrier.hpp"
namespace qp::labels {
TripleBarrierLabeler::TripleBarrierLabeler(TripleBarrierConfig c): config_(std::move(c)), guard_(LeakagePolicy{config_.prediction_gap,false}){}
Result<std::optional<LabelValue>> TripleBarrierLabeler::label(const data::OhlcvBar& sample, const std::vector<data::OhlcvBar>& future) const {
  auto ok=sample.validate(); if(!ok) return Err<std::optional<LabelValue>>(ok.error());
  Timestamp eligible{sample.end.value + config_.prediction_gap}; Timestamp horizon_end{eligible.value + config_.horizon};
  bool saw=false; bool horizon_complete=false; double outcome=0.0; Timestamp first_start{}; Timestamp last_end{}; std::optional<Timestamp> prev_end;
  for(const auto& b:future){
    auto bok=b.validate(); if(!bok) return Err<std::optional<LabelValue>>(bok.error());
    if(b.symbol!=sample.symbol) continue;
    if(prev_end && b.start < *prev_end) return Err<std::optional<LabelValue>>(Error{ErrorCode::InvalidData,"future bars must be sorted and non-overlapping"});
    prev_end=b.end;
    if(b.start<eligible) continue;
    if(!saw){ first_start=b.start; saw=true; }
    if(b.start>=horizon_end) break;
    last_end=b.end;
    double up=sample.close.value*(1.0+config_.take_profit_return); double down=sample.close.value*(1.0+config_.stop_loss_return);
    if(b.low.value<=down){ outcome=static_cast<double>(TripleBarrierOutcome::StopLoss); horizon_complete=true; break; }
    if(b.high.value>=up){ outcome=static_cast<double>(TripleBarrierOutcome::TakeProfit); horizon_complete=true; break; }
    if(b.end>=horizon_end) horizon_complete=true;
  }
  if(!saw || !horizon_complete) return Ok(std::optional<LabelValue>{});
  auto leak=guard_.validate(sample.end,first_start,last_end); if(!leak) return Err<std::optional<LabelValue>>(leak.error());
  return Ok(std::optional<LabelValue>{LabelValue{sample.end,first_start,last_end,sample.symbol,config_.name,LabelKind::MultiClassClassification,outcome,true}});
}
}
