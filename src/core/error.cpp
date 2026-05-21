#include "qp/core/error.hpp"
namespace qp { Error::Error(ErrorCode c, std::string msg): code(c), message(std::move(msg)) {} Error::Error(ErrorCode c, std::string msg, std::string ctx): code(c), message(std::move(msg)), context(std::move(ctx)) {} std::string_view Error::what() const noexcept { return message; } }
