#include "qp/features/feature.hpp"
#include "qp/schema/validation.hpp"
namespace qp::features { Result<void> validate(const FeatureValue& v){ if(v.symbol.empty()) return Err(Error{ErrorCode::InvalidData,"feature symbol empty"}); auto ok=schema::validate_non_empty(v.name,"feature.name"); if(!ok) return ok; return schema::validate_finite(v.value,"feature.value"); } }
