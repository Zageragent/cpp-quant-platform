#pragma once
#include "qp/core/result.hpp"
#include "qp/core/types.hpp"
#include "qp/execution/fill.hpp"
#include <map>
#include <vector>
namespace qp::portfolio {
struct Position { Quantity quantity; Price average_price; };
struct TradeLedgerEntry { execution::Fill fill; double cash_after{0.0}; Quantity position_after; Price average_price_after; double realized_pnl{0.0}; };
struct PortfolioSnapshot { Timestamp ts; double cash{0.0}; double gross_exposure{0.0}; double net_exposure{0.0}; double equity{0.0}; double realized_pnl{0.0}; double unrealized_pnl{0.0}; };
class Portfolio { public: explicit Portfolio(double initial_cash); [[nodiscard]] double cash() const noexcept { return cash_; } [[nodiscard]] double realized_pnl() const noexcept { return realized_pnl_; } [[nodiscard]] Position position(const Symbol& symbol) const; [[nodiscard]] Result<void> buy(const Symbol& symbol, Quantity quantity, Price price, double fee=0.0); [[nodiscard]] Result<void> sell(const Symbol& symbol, Quantity quantity, Price price, double fee=0.0); [[nodiscard]] double equity(const std::map<Symbol, Price>& marks) const; [[nodiscard]] Result<TradeLedgerEntry> apply_fill(const execution::Fill& fill); [[nodiscard]] const std::vector<TradeLedgerEntry>& ledger() const noexcept { return ledger_; } [[nodiscard]] Result<PortfolioSnapshot> snapshot(Timestamp ts, const std::map<Symbol, Price>& marks) const; private: double cash_; double realized_pnl_{0.0}; std::map<Symbol, Position> positions_; std::vector<TradeLedgerEntry> ledger_; };
}
