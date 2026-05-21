#include "qp/replay/replay_clock.hpp"
namespace qp::replay { Result<void> ReplayClock::advance_to(Timestamp ts){ if(now_ && ts < *now_) return Err(Error{ErrorCode::OutOfOrderTimestamp,"replay clock cannot move backwards"}); now_=ts; return Ok(); } }
