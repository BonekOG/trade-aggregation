#pragma once

#include "trade_aggregation/types.hpp"

namespace trade_aggregation {

    class AveragePrice {
        public:
            [[nodiscard]] double value() const noexcept { return price_sum_ / num_trades_; }
            void reset() noexcept {
                num_trades_ = 0.0;
                price_sum_ = 0.0;
            }

            template <TakerTrade TradeType>
            void update(const TradeType& trade) noexcept {
                num_trades_ += 1.0;
                price_sum_ += trade_price(trade);
            }

        private:
            double num_trades_{};
            double price_sum_{};
    };
}
