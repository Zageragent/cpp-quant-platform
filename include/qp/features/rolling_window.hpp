#pragma once
#include <cstddef>
#include <deque>
namespace qp::features {
class RollingWindow { public: explicit RollingWindow(std::size_t capacity); void push(double value); [[nodiscard]] bool full() const noexcept { return values_.size()==capacity_; } [[nodiscard]] bool empty() const noexcept { return values_.empty(); } [[nodiscard]] std::size_t size() const noexcept { return values_.size(); } [[nodiscard]] std::size_t capacity() const noexcept { return capacity_; } [[nodiscard]] double sum() const noexcept { return sum_; } [[nodiscard]] double mean() const noexcept; [[nodiscard]] double oldest() const; [[nodiscard]] double newest() const; void clear(); const std::deque<double>& values() const noexcept { return values_; } private: std::size_t capacity_; std::deque<double> values_; double sum_{0.0}; };
}
