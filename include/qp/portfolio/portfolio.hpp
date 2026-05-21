#pragma once
#include "qp/core/result.hpp"
#include "qp/core/types.hpp"
#include <map>
namespace qp::portfolio {
struct Position { Quantity quantity; Price average_price; };
class Portfolio { public: explicit Portfolio(double initial_cash); [[nodiscard]] double cash() const noexcept { return cash_; } [[nodiscard]] Position position(const Symbol& symbol) const; [[nodiscard]] Result<void> buy(const Symbol& symbol, Quantity quantity, Price price, double fee=0.0); [[nodiscard]] Result<void> sell(const Symbol& symbol, Quantity quantity, Price price, double fee=0.0); [[nodiscard]] double equity(const std::map<Symbol, Price>& marks) const; private: double cash_; std::map<Symbol, Position> positions_; };
}
