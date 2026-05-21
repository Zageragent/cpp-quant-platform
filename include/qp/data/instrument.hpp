#pragma once
#include "qp/core/result.hpp"
#include "qp/core/types.hpp"
#include "qp/schema/schema_version.hpp"
#include <string>
namespace qp::data {
struct Instrument { Symbol symbol; std::string venue; std::string asset_class; std::string currency{"USD"}; std::string schema_version{std::string(schema::instrument_v1)}; [[nodiscard]] Result<void> validate() const; };
}
