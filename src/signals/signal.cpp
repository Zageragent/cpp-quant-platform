#include "qp/signals/momentum.hpp"
#include "qp/schema/validation.hpp"
namespace qp::signals { Result<void> validate(const Signal& s){ if(s.symbol.empty()) return Err(Error{ErrorCode::InvalidData,"signal symbol empty"}); auto ok=schema::validate_non_empty(s.name,"signal.name"); if(!ok) return ok; if(s.side!=SignalSide::Sell && s.side!=SignalSide::Flat && s.side!=SignalSide::Buy) return Err(Error{ErrorCode::InvalidData,"invalid signal side"}); return schema::validate_finite(s.strength,"signal.strength"); } }
