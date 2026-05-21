#include "qp/execution/commission_model.hpp"
#include <algorithm>
#include <cmath>
#include <stdexcept>
namespace qp::execution { PerShareCommissionModel::PerShareCommissionModel(double p,double m): per_share_(p), minimum_(m){ if(!std::isfinite(p)||p<0.0||!std::isfinite(m)||m<0.0) throw std::invalid_argument("invalid commission model"); } double PerShareCommissionModel::calculate(const CommissionInput& i) const { return std::max(minimum_, per_share_*i.quantity.value); } BpsCommissionModel::BpsCommissionModel(BasisPoints bps,double m): bps_(bps), minimum_(m){ if(!std::isfinite(bps.value)||bps.value<0.0||!std::isfinite(m)||m<0.0) throw std::invalid_argument("invalid bps commission model"); } double BpsCommissionModel::calculate(const CommissionInput& i) const { return std::max(minimum_, i.quantity.value*i.price.value*bps_.value/10000.0); } }
