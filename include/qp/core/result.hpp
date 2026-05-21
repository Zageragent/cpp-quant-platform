#pragma once
#include "qp/core/error.hpp"
#include <optional>
#include <stdexcept>
#include <type_traits>
#include <utility>
#include <variant>
namespace qp {
template<class T> class Result {
 public:
  Result(T value) : storage_(std::move(value)) {}
  Result(Error error) : storage_(std::move(error)) {}
  [[nodiscard]] bool ok() const noexcept { return std::holds_alternative<T>(storage_); }
  [[nodiscard]] explicit operator bool() const noexcept { return ok(); }
  [[nodiscard]] T& value() & { if(!ok()) throw std::logic_error("Result has no value"); return std::get<T>(storage_); }
  [[nodiscard]] const T& value() const& { if(!ok()) throw std::logic_error("Result has no value"); return std::get<T>(storage_); }
  [[nodiscard]] T&& value() && { if(!ok()) throw std::logic_error("Result has no value"); return std::move(std::get<T>(storage_)); }
  [[nodiscard]] Error& error() & { if(ok()) throw std::logic_error("Result has no error"); return std::get<Error>(storage_); }
  [[nodiscard]] const Error& error() const& { if(ok()) throw std::logic_error("Result has no error"); return std::get<Error>(storage_); }
 private:
  std::variant<T, Error> storage_;
};
template<> class Result<void> {
 public:
  Result() = default;
  Result(Error error) : error_(std::move(error)) {}
  [[nodiscard]] bool ok() const noexcept { return !error_.has_value(); }
  [[nodiscard]] explicit operator bool() const noexcept { return ok(); }
  [[nodiscard]] Error& error() & { if(ok()) throw std::logic_error("Result has no error"); return *error_; }
  [[nodiscard]] const Error& error() const& { if(ok()) throw std::logic_error("Result has no error"); return *error_; }
 private:
  std::optional<Error> error_;
};
template<class T> [[nodiscard]] Result<T> Ok(T value) { return Result<T>(std::move(value)); }
[[nodiscard]] inline Result<void> Ok() { return Result<void>(); }
template<class T=void> [[nodiscard]] Result<T> Err(Error error) { return Result<T>(std::move(error)); }
}
