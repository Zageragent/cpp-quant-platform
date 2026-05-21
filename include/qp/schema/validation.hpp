#pragma once
#include "qp/core/result.hpp"
#include "qp/core/time.hpp"
#include <string_view>
namespace qp::schema {
[[nodiscard]] Result<void> validate_schema_version(std::string_view version);
[[nodiscard]] Result<void> validate_non_empty(std::string_view value, std::string_view field_name);
[[nodiscard]] Result<void> validate_time_interval(Timestamp start, Timestamp end);
[[nodiscard]] Result<void> validate_finite(double value, std::string_view field_name);
}
