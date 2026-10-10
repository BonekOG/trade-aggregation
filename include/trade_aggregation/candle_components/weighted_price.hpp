#pragma once

#include "trade_aggregation/types.hpp"

#include <cmath>

namespace trade_aggregation {

    class WeightedPrice {
        public:
            [[nodiscard]] double value() const noexcept { return weighted_sum_ / total_weights_; }
            void reset() noexcept {
                total_weights_ = 0.0;
                weighted_sum_ = 0.0;
            }

            template <TakerTrade TradeType>
            void update(const TradeType& trade) noexcept {
                const double weight = std::abs(trade_size(trade));
                total_weights_ += weight;
                weighted_sum_ += trade_price(trade) * weight;
            }

        private:
            double total_weights_{};
            double weighted_sum_{};
    };
}
