#include "qp/schema/validation.hpp"
#include "qp/schema/schema_version.hpp"
#include <cmath>
namespace qp::schema {
Result<void> validate_schema_version(std::string_view version){ if(version.empty()) return Err(Error{ErrorCode::InvalidData,"schema_version is required"}); if(!is_known_schema_version(version)) return Err(Error{ErrorCode::InvalidData,"unknown schema_version",std::string(version)}); return Ok(); }
Result<void> validate_non_empty(std::string_view value, std::string_view field){ if(value.empty()) return Err(Error{ErrorCode::InvalidData,"field must be non-empty",std::string(field)}); return Ok(); }
Result<void> validate_time_interval(Timestamp start, Timestamp end){ if(!(start<end)) return Err(Error{ErrorCode::InvalidData,"time interval must satisfy start < end"}); return Ok(); }
Result<void> validate_finite(double value, std::string_view field){ if(!std::isfinite(value)) return Err(Error{ErrorCode::InvalidData,"field must be finite",std::string(field)}); return Ok(); }
}
