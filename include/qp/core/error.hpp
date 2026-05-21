#pragma once
#include <string>
#include <string_view>
namespace qp {
enum class ErrorCode { Unknown, InvalidArgument, InvalidData, ParseError, IoError, EndOfStream, NotFound, OutOfOrderTimestamp, LeakageDetected, ConfigurationError };
struct Error {
  ErrorCode code{ErrorCode::Unknown};
  std::string message;
  std::string context;
  Error() = default;
  Error(ErrorCode c, std::string msg);
  Error(ErrorCode c, std::string msg, std::string ctx);
  [[nodiscard]] std::string_view what() const noexcept;
};
}
