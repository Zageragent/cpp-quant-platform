#include "qp/data/market_data.hpp"
#include <variant>
namespace qp::data {
Timestamp timestamp(const MarketData& d){ return std::visit([](const auto& x){ if constexpr (requires { x.end; }) return x.end; else return x.ts; }, d); }
const Symbol& symbol(const MarketData& d){ return std::visit([](const auto& x)->const Symbol&{ return x.symbol; }, d); }
Result<void> validate(const MarketData& d){ return std::visit([](const auto& x){ return x.validate(); }, d); }
double reference_price(const MarketData& d){ return std::visit([](const auto& x)->double{ if constexpr (requires { x.close; }) return x.close.value; else if constexpr (requires { x.price; }) return x.price.value; else return x.mid().value; }, d); }
}
