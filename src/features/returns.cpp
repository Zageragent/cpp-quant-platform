#include "qp/features/returns.hpp"
#include <cmath>
namespace qp::features {
SimpleReturn::SimpleReturn(std::string n): name_(std::move(n)){} std::string_view SimpleReturn::name() const noexcept{return name_;}
Result<std::optional<FeatureValue>> SimpleReturn::update(const data::MarketData& d){ double p=data::reference_price(d); if(p<=0.0) return Err<std::optional<FeatureValue>>(Error{ErrorCode::InvalidData,"price must be positive"}); const auto& sym=data::symbol(d); auto it=previous_by_symbol_.find(sym); if(it==previous_by_symbol_.end()){ previous_by_symbol_[sym]=p; return Ok(std::optional<FeatureValue>{}); } double old=it->second; it->second=p; return Ok(std::optional<FeatureValue>{FeatureValue{data::timestamp(d), sym, name_, p/old-1.0}}); }
void SimpleReturn::reset(){ previous_by_symbol_.clear(); }
LogReturn::LogReturn(std::string n): name_(std::move(n)){} std::string_view LogReturn::name() const noexcept{return name_;}
Result<std::optional<FeatureValue>> LogReturn::update(const data::MarketData& d){ double p=data::reference_price(d); if(p<=0.0) return Err<std::optional<FeatureValue>>(Error{ErrorCode::InvalidData,"price must be positive"}); const auto& sym=data::symbol(d); auto it=previous_by_symbol_.find(sym); if(it==previous_by_symbol_.end()){ previous_by_symbol_[sym]=p; return Ok(std::optional<FeatureValue>{}); } double old=it->second; it->second=p; return Ok(std::optional<FeatureValue>{FeatureValue{data::timestamp(d), sym, name_, std::log(p/old)}}); }
void LogReturn::reset(){ previous_by_symbol_.clear(); }
}
