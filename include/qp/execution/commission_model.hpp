#pragma once
#include "qp/execution/order.hpp"
namespace qp::execution {
struct CommissionInput { Symbol symbol; OrderSide side{OrderSide::Buy}; Quantity quantity; Price price; };
class CommissionModel { public: virtual ~CommissionModel()=default; [[nodiscard]] virtual double calculate(const CommissionInput& input) const=0; };
class NoCommissionModel final : public CommissionModel { public: [[nodiscard]] double calculate(const CommissionInput&) const override { return 0.0; } };
class PerShareCommissionModel final : public CommissionModel { public: explicit PerShareCommissionModel(double per_share, double minimum=0.0); [[nodiscard]] double calculate(const CommissionInput& input) const override; private: double per_share_; double minimum_; };
class BpsCommissionModel final : public CommissionModel { public: explicit BpsCommissionModel(BasisPoints bps, double minimum=0.0); [[nodiscard]] double calculate(const CommissionInput& input) const override; private: BasisPoints bps_; double minimum_; };
}
