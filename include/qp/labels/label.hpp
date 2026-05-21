#pragma once
#include "qp/core/time.hpp"
#include "qp/core/result.hpp"
#include "qp/core/types.hpp"
#include <string>
namespace qp::labels {
enum class LabelKind { Regression, BinaryClassification, MultiClassClassification };
struct LabelValue { Timestamp sample_ts; Timestamp label_start_ts; Timestamp label_end_ts; Symbol symbol; std::string name; LabelKind kind{LabelKind::Regression}; double value{0.0}; bool leakage_safe{true}; };
[[nodiscard]] Result<void> validate(const LabelValue& value);
}
