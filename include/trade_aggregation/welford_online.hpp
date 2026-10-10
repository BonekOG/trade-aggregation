#pragma once

#include <cmath>
#include <cstdint>

namespace trade_aggregation {

    class WelfordOnline {
        public:
            [[nodiscard]] double variance() const noexcept {
                return count_ > 1 ? sum_squared_deviations_ / (static_cast<double>(count_) - 1.0) : 0.0;
            }

            [[nodiscard]] double std_dev() const noexcept { return std::sqrt(variance()); }

            void reset() noexcept {
                count_ = 0;
                mean_ = 0.0;
                sum_squared_deviations_ = 0.0;
            }

            void add(double value) noexcept {
                ++count_;
                const double old_mean = mean_;
                
                mean_ += (value - old_mean) / static_cast<double>(count_);
                sum_squared_deviations_ += (value - old_mean) * (value - mean_);
            }

            [[nodiscard]] std::uint32_t count() const noexcept { return count_; }
            [[nodiscard]] double mean() const noexcept { return mean_; }

        private:
            std::uint32_t count_{};
            double mean_{};
            double sum_squared_deviations_{};
    };
}
