#include "qp/data/bar.hpp"
#include <algorithm>
namespace qp::data {
bool OhlcvBar::is_price_valid() const noexcept { return open.is_positive() && high.is_positive() && low.is_positive() && close.is_positive() && high.value >= std::max({open.value, low.value, close.value}) && low.value <= std::min({open.value, high.value, close.value}); }
Result<void> OhlcvBar::validate() const { if(symbol.empty()) return Err(Error{ErrorCode::InvalidData,"bar symbol empty"}); if(!is_time_valid()) return Err(Error{ErrorCode::InvalidData,"bar end must be after start"}); if(!is_price_valid()) return Err(Error{ErrorCode::InvalidData,"invalid OHLC price relationship"}); if(!is_volume_valid()) return Err(Error{ErrorCode::InvalidData,"invalid volume"}); return Ok(); }
}
