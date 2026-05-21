#include "qp/data/instrument.hpp"
#include "qp/schema/validation.hpp"
namespace qp::data { Result<void> Instrument::validate() const { if(symbol.empty()) return Err(Error{ErrorCode::InvalidData,"instrument symbol empty"}); auto ok=schema::validate_non_empty(venue,"venue"); if(!ok) return ok; ok=schema::validate_non_empty(asset_class,"asset_class"); if(!ok) return ok; ok=schema::validate_non_empty(currency,"currency"); if(!ok) return ok; return schema::validate_schema_version(schema_version); } }
