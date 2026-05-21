#include "qp/backtest/simple_backtest.hpp"
#include "qp/features/returns.hpp"
#include "qp/portfolio/portfolio.hpp"
#include "qp/signals/momentum.hpp"
namespace qp::backtest {
Result<SimpleBacktestResult> run_simple_momentum_backtest(const std::vector<data::OhlcvBar>& bars, const SimpleBacktestConfig& config){ if(bars.empty()) return Err<SimpleBacktestResult>(Error{ErrorCode::InvalidArgument,"backtest requires at least one bar"}); portfolio::Portfolio pf(config.initial_cash); features::SimpleReturn ret; signals::MomentumSignal model(signals::MomentumConfig{"simple_return",config.buy_threshold,config.sell_threshold,"momentum"}); std::size_t trades=0; for(const auto& bar:bars){ auto ok=bar.validate(); if(!ok) return Err<SimpleBacktestResult>(ok.error()); auto fv=ret.update(data::MarketData{bar}); if(!fv) return Err<SimpleBacktestResult>(fv.error()); if(!fv.value()) continue; auto sig=model.update(*fv.value()); if(!sig) continue; auto pos=pf.position(bar.symbol); if(sig->side==signals::SignalSide::Buy && pos.quantity.value==0.0){ auto buy=pf.buy(bar.symbol, Quantity{1.0}, bar.close, config.fee_per_trade); if(buy){ ++trades; } }
    if(sig->side==signals::SignalSide::Sell && pos.quantity.value>0.0){ auto sell=pf.sell(bar.symbol, pos.quantity, bar.close, config.fee_per_trade); if(sell){ ++trades; } }
  }
  const auto& last=bars.back(); auto pos=pf.position(last.symbol); double final_equity=pf.equity({{last.symbol,last.close}}); return Ok(SimpleBacktestResult{final_equity,pf.cash(),pos.quantity.value,bars.size(),trades}); }
}
