#include "qp/data/trade.hpp"
namespace qp::data { Result<void> Trade::validate() const { if(symbol.empty()) return Err(Error{ErrorCode::InvalidData,"trade symbol empty"}); if(!price.is_positive()) return Err(Error{ErrorCode::InvalidData,"trade price must be positive"}); if(!quantity.is_positive()) return Err(Error{ErrorCode::InvalidData,"trade quantity must be positive"}); return Ok(); } }
